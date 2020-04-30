/* -*- mode: c++ -*- */
/**
 * @file   Bezier.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
 *
 * @brief
 * \@copyright
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr/>
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
#include "board/Bezier.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <sstream>
#include <vector>
#include "BoardConfig.h"
#include "board/PSFonts.h"
#include "board/PathBoundaries.h"
#include "board/Rect.h"
#include "board/ShapeVisitor.h"
#include "board/Tools.h"
#include "board/Transforms.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

namespace LibBoard
{

const std::string Bezier::_name("Bezier");

Bezier::Bezier(const std::vector<Point> & points, const std::vector<Point> & controls, Color penColor, Color fillColor, double lineWidth, const Shape::LineStyle lineStyle, const Shape::LineCap cap,
               const Shape::LineJoin join, int depth)
    : Shape(penColor, fillColor, lineWidth, lineStyle, cap, join, depth), _path(points, false), _controls(controls, false)
{
}

Bezier::Bezier(Point p0, Point control0, Point p1, Point control1, Color penColor, Color fillColor, double lineWidth, const LineStyle lineStyle, const LineCap cap, const LineJoin join, int depth)
    : Shape(penColor, fillColor, lineWidth, lineStyle, cap, join, depth)
{
  _path << p0 << p1;
  _controls << control0 << control1;
}

Bezier::Bezier(double x0, double y0, double xc0, double yc0, double x1, double y1, double xc1, double yc1, Color penColor, Color fillColor, double lineWidth, const LineStyle lineStyle,
               const LineCap cap, const LineJoin join, int depth)
    : Shape(penColor, fillColor, lineWidth, lineStyle, cap, join, depth)
{
  _path << Point(x0, y0) << Point(x1, y1);
  _controls << Point(xc0, yc0) << Point(xc1, yc1);
}

const std::string & Bezier::name() const
{
  return _name;
}

Bezier & Bezier::rotate(double angle, const Point & center)
{
  _path.rotate(angle, center);
  _controls.rotate(angle, center);
  return *this;
}

Bezier Bezier::rotated(double angle, const Point & center) const
{
  return Bezier(*this).rotate(angle, center);
}

Bezier & Bezier::rotate(double angle)
{
  _path.rotate(angle, center());
  _controls.rotate(angle, center());
  return *this;
}

Bezier Bezier::rotated(double angle) const
{
  return Bezier(*this).rotate(angle, center());
}

Bezier & Bezier::translate(double dx, double dy)
{
  _path.translate(dx, dy);
  _controls.translate(dx, dy);
  return *this;
}

Bezier Bezier::translated(double dx, double dy) const
{
  return Bezier(*this).translate(dx, dy);
}

Bezier & Bezier::scale(double sx, double sy)
{
  _path.scale(sx, sy);
  _controls.scale(sx, sy);
  updateLineWidth(std::max(sx, sy));
  return *this;
}

Bezier & Bezier::scale(double s)
{
  Bezier::scale(s, s);
  return *this;
}

Bezier Bezier::scaled(double sx, double sy) const
{
  return Bezier(*this).scale(sx, sy);
}

Bezier Bezier::scaled(double s) const
{
  return Bezier(*this).scale(s, s);
}

void Bezier::scaleAll(double s)
{
  _path.scaleAll(s);
  _controls.scaleAll(s);
}

Bezier Bezier::resized(double w, double h, Shape::LineWidthFlag lineWidthFlag) const
{
  return static_cast<Bezier &>(Bezier(*this).resize(w, h, lineWidthFlag));
}

Bezier * Bezier::clone() const
{
  return new Bezier(*this);
}

Path Bezier::discretizedPath() const
{
  Path path;

  std::vector<Point>::const_iterator iPoint = _path.points().begin();
  std::vector<Point>::const_iterator endPoint = _path.points().end();
  std::vector<Point>::const_iterator iControl = _controls.points().begin();
  Point p0(iPoint->x, iPoint->y);
  Point p1;
  ++iPoint;
  while (iPoint != endPoint) {
    p1 = Point(iPoint->x, iPoint->y);
    Point t0 = 3 * (Point(iControl[0].x, iControl[0].y) - p0);
    Point t1 = 3 * (p1 - Point(iControl[1].x, iControl[1].y));

    //
    // Discretize a Bezier segment
    //
    const double a0 = 2 * p0.x - 2 * p1.x + t0.x + t1.x;
    const double b0 = -3 * p0.x + 3 * p1.x - 2 * t0.x - t1.x;
    const double c0 = t0.x;
    const double d0 = p0.x;
    const double a1 = 2 * p0.y - 2 * p1.y + t0.y + t1.y;
    const double b1 = -3 * p0.y + 3 * p1.y - 2 * t0.y - t1.y;
    const double c1 = t0.y;
    const double d1 = p0.y;
    // Compute optimal step
    double previousLength = 0.0;
    double length = std::numeric_limits<double>::max();
    double step = 1.0;
    while (step > 1 / 512.0f && fabs(length - previousLength) > 0.00002) {
      step /= 2.0;
      previousLength = length;
      // Compute length
      Point previous(d0, d1);
      length = 0.0;
      for (double t = step; t <= 1.0; t += step) {
        Point current(a0 * t * t * t + b0 * t * t + c0 * t + d0, a1 * t * t * t + b1 * t * t + c1 * t + d1);
        length += (current - previous).norm();
        previous = current;
      }
    }
    // Add points
    for (float t = 0; t <= 1.0; t += step) {
      //      std::cout << Point( (a0 * t * t * t + b0 * t * t + c0 * t + d0),
      //                     (a1 * t * t * t + b1 * t * t + c1 * t + d1) ) << std::endl;
      path << Point((a0 * t * t * t + b0 * t * t + c0 * t + d0), (a1 * t * t * t + b1 * t * t + c1 * t + d1));
    }

    ++iPoint;
    iControl += 2;
    p0 = p1;
  }
  return path;
}

void Bezier::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  if (_path.empty()) {
    return;
  }
  if (_controls.size() != 2 * (_path.size() - 1)) {
    Tools::error << "Bezier::flushPostscript: wrong number of control points\n";
    return;
  }

  stream << "\n% Bezier\n";
  if (filled()) {
    stream << "n ";

    // Fill
    std::vector<Point>::const_iterator iPoint = _path.points().begin();
    std::vector<Point>::const_iterator endPoint = _path.points().end();
    std::vector<Point>::const_iterator iControl = _controls.points().begin();
    int count = 0;
    stream << transform.mapX(iPoint->x) << " " << transform.mapY(iPoint->y) << " m";
    ++iPoint;
    while (iPoint != endPoint) {
      stream << " " << transform.mapX(iControl[0].x) << " " << transform.mapY(iControl[0].y) << " " << transform.mapX(iControl[1].x) << " " << transform.mapY(iControl[1].y) << " "
             << transform.mapX(iPoint->x) << " " << transform.mapY(iPoint->y) << " ct";
      ++iPoint;
      iControl += 2;
      count = (count + 1) % 6;
      if (!count)
        stream << "\n                  ";
    }
    stream << " ";

    _fillColor.flushPostscript(stream);
    stream << " " << postscriptProperties(transform);
    stream << " fill" << std::endl;
  }
  if (_penColor != Color::Null && _lineWidth != 0.0) {
    stream << " " << postscriptProperties(transform) << "\n";
    stream << "n ";

    // Stroke
    std::vector<Point>::const_iterator iPoint = _path.points().begin();
    std::vector<Point>::const_iterator endPoint = _path.points().end();
    std::vector<Point>::const_iterator iControl = _controls.points().begin();
    int count = 0;
    stream << transform.mapX(iPoint->x) << " " << transform.mapY(iPoint->y) << " m";
    ++iPoint;
    while (iPoint != endPoint) {
      stream << " " << transform.mapX(iControl[0].x) << " " << transform.mapY(iControl[0].y) << " " << transform.mapX(iControl[1].x) << " " << transform.mapY(iControl[1].y) << " "
             << transform.mapX(iPoint->x) << " " << transform.mapY(iPoint->y) << " ct";
      ++iPoint;
      iControl += 2;
      count = (count + 1) % 6;
      if (!count)
        stream << "\n                  ";
    }

    stream << " ";
    _penColor.flushPostscript(stream);
    stream << " stroke" << std::endl;
  }
}

void Bezier::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  Tools::unused(stream);
  Tools::unused(transform);
  Tools::unused(colormap);
  Tools::warning << "Bezier::flushFIG not supported yet\n";
}

void Bezier::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  if (_path.empty()) {
    return;
  }
  if (_controls.size() != 2 * (_path.size() - 1)) {
    Tools::error << "Bezier::flushSVG: wrong number of control points\n";
    return;
  }
  stream << "<path";
  stream << svgProperties(transform) << std::endl;
  stream << "          d=\"";
  std::vector<Point>::const_iterator iPoint = _path.points().begin();
  std::vector<Point>::const_iterator endPoint = _path.points().end();
  std::vector<Point>::const_iterator iControl = _controls.points().begin();
  int count = 0;
  stream << "M" << transform.mapX(iPoint->x) << "," << transform.mapY(iPoint->y);
  ++iPoint;
  while (iPoint != endPoint) {
    stream << " C" << transform.mapX(iControl[0].x) << "," << transform.mapY(iControl[0].y) << " " << transform.mapX(iControl[1].x) << "," << transform.mapY(iControl[1].y) << " "
           << transform.mapX(iPoint->x) << "," << transform.mapY(iPoint->y);
    ++iPoint;
    iControl += 2;
    count = (count + 1) % 6;
    if (!count)
      stream << "\n                  ";
  }
  stream << "\" />" << std::endl;
}

void Bezier::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  Tools::unused(stream);
  Tools::unused(transform);
  // Not supported yet
  Tools::warning << "Bezier::flushTikZ not supported yet\n";
}

Rect Bezier::boundingBox(LineWidthFlag lineWidthFlag) const
{
  Path path = discretizedPath();
  switch (lineWidthFlag) {
  case UseLineWidth:
    return Tools::pathBoundingBox(path, _lineWidth, _lineCap, Shape::RoundJoin);
    break;
  case IgnoreLineWidth:
    return path.boundingBox();
    break;
  default:
    Tools::error << "LineWidthFlag incorrect value (" << lineWidthFlag << ")\n";
    return Rect();
    break;
  }
}

} // namespace LibBoard
