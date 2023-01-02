/* -*- mode: c++ -*- */
/**
 * @file   Ellipse.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
 *
 * @brief
 * \@copyright
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://foureys.users.greyc.fr/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <board/Ellipse.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <sstream>
#include <vector>
#include <BoardConfig.h>
#include <board/PSFonts.h>
#include <board/PathBoundaries.h>
#include <board/Rect.h>
#include <board/ShapeVisitor.h>
#include <board/Tools.h>
#include <board/Transforms.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

namespace LibBoard
{

const std::string Ellipse::_name("Ellipse");

const std::string & Ellipse::name() const
{
  return _name;
}

Point Ellipse::center(LineWidthFlag) const
{
  return _center;
}

Ellipse & Ellipse::rotate(double angle, const Point & center)
{
  Point c(_center);
  Point e = (c + Point(_xRadius, 0)).rotate(_angle, c);
  Point rc = c.rotated(angle, center);
  Point re = e.rotated(angle, center);
  Point axis = re - rc;
  _angle = atan(axis.y / axis.x);
  _center = rc;
  return *this;
}

Ellipse Ellipse::rotated(double angle, const Point & center) const
{
  return Ellipse(*this).rotate(angle, center);
}

Ellipse & Ellipse::rotate(double angle)
{
  return Ellipse::rotate(angle, center());
}

Ellipse Ellipse::rotated(double angle) const
{
  return Ellipse(*this).rotate(angle, center());
}

Ellipse & Ellipse::translate(double dx, double dy)
{
  _center += Point(dx, dy);
  return *this;
}

Ellipse Ellipse::translated(double dx, double dy) const
{
  return Ellipse(*this).translate(dx, dy);
}

Ellipse & Ellipse::scale(double sx, double sy)
{
  // Thanks to Freddie Exall for pointing an error with the first version
  // of this function, and for pointing to a fix as well!
  if (!Tools::almostEqual(_angle, 0.0)) {
    const double co = cos(_angle);
    const double si = sin(_angle);

    // current transformation matrix
    const double m00 = (1 / _xRadius) * co;
    const double m01 = (1 / _xRadius) * si;
    const double m10 = -(1 / _yRadius) * si;
    const double m11 = (1 / _yRadius) * co;

    // Implicit function of ellipse at current
    // ax^2 + bxy + cy^2 = 1
    double a = (m00 * m00) + (m10 * m10);
    double b = 2 * ((m00 * m01) + (m10 * m11));
    double c = (m01 * m01) + (m11 * m11);

    // Scale coefficients ( x_new = sx * x, y_new = sy * y )
    a = a / (sx * sx);
    b = b / (sx * sy);
    c = c / (sy * sy);

    if (Tools::almostEqual(b, 0.0)) {
      _angle = 0;
    } else if (Tools::almostEqual(a, c)) {
      _angle = M_PI / 4;
      a += (b / 2);
      c -= (b / 2);
    } else {
      _angle = 0.5 * atan(b / (a - c));
      double k = 1 + ((b * b) / ((a - c) * (a - c)));
      k = sqrt(k);
      k *= (a - c);
      c += a;
      a = 0.5 * (c + k);
      c = 0.5 * (c - k);
    }
    _xRadius = 1 / sqrt(a);
    _yRadius = 1 / sqrt(c);
  } else {
    _xRadius = _xRadius * sx;
    _yRadius = _yRadius * sy;
  }
  updateLineWidth(std::max(sx, sy));
  return *this;
}

Ellipse & Ellipse::scale(double s)
{
  return Ellipse::scale(s, s);
}

Ellipse Ellipse::scaled(double sx, double sy) const
{
  return Ellipse(*this).scale(sx, sy);
}

Ellipse Ellipse::scaled(double s) const
{
  return Ellipse(*this).scale(s, s);
}

void Ellipse::scaleAll(double s)
{
  _xRadius *= s;
  _yRadius *= s;
  _center *= s;
}

Ellipse Ellipse::resized(double w, double h, LineWidthFlag lineWidthFlag) const
{
  return static_cast<Ellipse &>(Ellipse(*this).resize(w, h, lineWidthFlag));
}

Ellipse * Ellipse::clone() const
{
  return new Ellipse(*this);
}

void Ellipse::setCircleFlag()
{
  _isCreatedAsCircle = true;
}

double Ellipse::perimeter() const
{
  double theta = 0.0;
  double dTheta = 0.0001;
  double numIntegrals = std::round(2.0 * M_PI / dTheta);
  double result = 0.0;
  for (int i = 0; i < numIntegrals; ++i) {
    theta += i * dTheta;
    result += std::hypot(_xRadius * sin(theta), _yRadius * cos(theta)); // result += dpt
  }
  return result * dTheta;
}

Path Ellipse::sampledPath(int n, SamplingStart start) const
{
  Path path(Path::Closed);
  double theta = 0.0;
  double dTheta = 0.0001;
  double numIntegrals = std::round(2.0 * M_PI / dTheta);
  double circ = perimeter();
  int nextPoint = 0;
  double run = 0.0;
  switch (start) {
  case SamplingFromRight:
    theta = 0.0;
    break;
  case SamplingFromTop:
    theta = M_PI / 2.0;
    break;
  case SamplingFromLeft:
    theta = M_PI;
    break;
  case SamplingFromBottom:
    theta = -M_PI / 2.0;
    break;
  }
  for (int i = 0; i < numIntegrals; i++) {
    theta += dTheta;
    double subIntegral = n * run / circ;
    if (static_cast<int>(subIntegral) >= nextPoint) {
      double x = _xRadius * cos(theta);
      double y = _yRadius * sin(theta);
      path.push_back(Point(x, y).rotated(_angle, _center));
      ++nextPoint;
    }
    run += std::hypot(_xRadius * sin(theta), _yRadius * cos(theta)) * dTheta; // run += dpt
  }
  path.pop_back();
  path.moveCenter(_center);
  return path;
}

bool Ellipse::isACircle() const
{
  return _isCreatedAsCircle && Tools::almostEqual(_xRadius, _yRadius);
}

void Ellipse::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  double yScale = _yRadius / _xRadius;
  stream << "\n% Ellipse\n";
  if (filled()) {
    stream << "gs " << transform.mapX(_center.x) << " " << transform.mapY(_center.y) << " tr";
    if (_angle != 0.0) {
      stream << " " << (_angle * 180 / M_PI) << " rot ";
    }
    if (!isACircle()) {
      stream << " " << 1.0 << " " << yScale << " sc";
    }
    stream << " n " << transform.scale(_xRadius) << " 0 m "
           << " 0 0 " << transform.scale(_xRadius) << " 0.0 360.0 arc ";
    stream << " " << fillColor().postscript() << " srgb";
    stream << " fill gr" << std::endl;
  }

  if (penColor() != Color::Null && lineWidth() != 0.0) {
    stream << _style.postscriptProperties(transform) << "\n";
    stream << "gs " << transform.mapX(_center.x) << " " << transform.mapY(_center.y) << " tr";
    if (_angle != 0.0) {
      stream << " " << (_angle * 180 / M_PI) << " rot ";
    }
    if (!isACircle()) {
      stream << " " << 1.0 << " " << yScale << " sc";
    }
    stream << " n " << transform.scale(_xRadius) << " 0 m "
           << " 0 0 " << transform.scale(_xRadius) << " 0.0 360.0 arc ";
    stream << " " << penColor().postscript() << " srgb";
    stream << " stroke gr" << std::endl;
  }
}

void Ellipse::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  // Ellipse, Sub type, Line style, Thickness
  if (isACircle()) {
    stream << "1 3 " << lineStyle() << " ";
  } else {
    stream << "1 1 " << lineStyle() << " ";
  }
  stream << (penColor().valid() ? transform.mapWidth(lineWidth()) : 0) << " ";
  // Pen color, Fill color
  stream << colormap[penColor()] << " " << colormap[fillColor()] << " ";
  // Depth, Pen style, Area fill, Style val, Direction, angle
  if (filled()) {
    stream << transform.shapeDepth(this) << " -1 20 " << (lineStyle() ? "4.000 " : "0.000 ") << "  1 " << _angle << " ";
  } else {
    stream << transform.shapeDepth(this) << " -1 -1 " << (lineStyle() ? "4.000 " : "0.000 ") << " 1 " << _angle << " ";
  }
  stream << static_cast<int>(transform.mapX(_center.x)) << " " << static_cast<int>(transform.mapY(_center.y)) << " " << static_cast<int>(transform.scale(_xRadius)) << " "
         << static_cast<int>(transform.scale(_yRadius)) << " " << static_cast<int>(transform.mapX(_center.x)) << " " << static_cast<int>(transform.mapY(_center.y)) << " "
         << static_cast<int>(transform.mapX(_center.x) + transform.scale(_xRadius)) << " " << static_cast<int>(transform.mapY(_center.y)) << "\n";
}

void Ellipse::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  stream << "<ellipse cx=\"" << transform.mapX(_center.x) << '"' << " cy=\"" << transform.mapY(_center.y) << '"' << " rx=\"" << transform.scale(_xRadius) << '"' << " ry=\""
         << transform.scale(_yRadius) << '"' << _style.svgProperties(transform);
  if (_angle != 0.0) {
    stream << " transform=\"rotate( " << -(_angle * 180 / M_PI) << ", " << transform.mapX(_center.x) << ", " << transform.mapY(_center.y) << " )\" ";
  }
  stream << " />" << std::endl;
}

void Ellipse::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  // FIXME: unimplemented
  stream << "% FIXME: Ellipse::flushTikZ unimplemented" << std::endl;
  stream << "\\path[" << _style.tikzProperties(transform) << "] (" << transform.mapX(_center.x) << ',' << transform.mapY(_center.y) << ')' << " circle [x radius=" << transform.scale(_xRadius) << ','
         << "y radius=" << transform.scale(_yRadius) << ',' << "rotate=" << -(_angle * 180 / M_PI) << "];" << std::endl;
}

void Ellipse::accept(ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Ellipse::accept(const ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Ellipse::accept(ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

void Ellipse::accept(const ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

Shape * Ellipse::accept(CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Shape * Ellipse::accept(const CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Rect Ellipse::boundingBox(LineWidthFlag lineWidthFlag) const
{
  Rect box;
  if (_angle == 0.0) {
    box = Rect(_center.x - _xRadius, _center.y + _yRadius, 2 * _xRadius, 2 * _yRadius);
  } else {
    double angleXmax = -atan((_yRadius / _xRadius) * (tan(_angle)));
    double angleXmin = angleXmax + M_PI;
    double angleYmax = atan((_yRadius / _xRadius) * (1 / tan(_angle)));
    double angleYmin = angleYmax + M_PI;
    if (_angle < 0.0) {
      angleYmax += M_PI;
      angleYmin -= M_PI;
    }
    box = Rect(_center.x + _xRadius * cos(angleXmin) * cos(_angle) - _yRadius * sin(angleXmin) * sin(_angle),
               _center.y + _xRadius * cos(angleYmax) * sin(_angle) + _yRadius * sin(angleYmax) * cos(_angle),
               (_xRadius * cos(angleXmax) * cos(_angle) - _yRadius * sin(angleXmax) * sin(_angle)) - (_xRadius * cos(angleXmin) * cos(_angle) - _yRadius * sin(angleXmin) * sin(_angle)),
               (_xRadius * cos(angleYmax) * sin(_angle) + _yRadius * sin(angleYmax) * cos(_angle)) - (_xRadius * cos(angleYmin) * sin(_angle) + _yRadius * sin(angleYmin) * cos(_angle)));
  }
  if (lineWidthFlag == UseLineWidth) {
    box.grow(0.5 * lineWidth());
  }
  return box;
}

Ellipse circle(double x, double y, double radius, Color penColor, Color fillColor, double lineWidth, const LineStyle lineStyle)
{
  Ellipse ellipse(x, y, radius, radius, penColor, fillColor, lineWidth, lineStyle);
  ellipse.setCircleFlag();
  return ellipse;
}

Ellipse circle(Point center, double radius, Color penColor, Color fillColor, double lineWidth, const LineStyle lineStyle)
{
  Ellipse ellipse(center, radius, radius, penColor, fillColor, lineWidth, lineStyle);
  ellipse.setCircleFlag();
  return ellipse;
}

Ellipse circle(Point center, double radius, Style style)
{
  Ellipse ellipse(center, radius, radius, style);
  ellipse.setCircleFlag();
  return ellipse;
}

} // namespace LibBoard
