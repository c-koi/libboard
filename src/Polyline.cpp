/* -*- mode: c++ -*- */
/**
 * @file   Polyline.cpp
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
#include <board/Polyline.h>
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

const std::string Polyline::_name("Polyline");

const std::string & Polyline::name() const
{
  return _name;
}

Polyline & Polyline::operator<<(const Point & p)
{
  _path << p;
  return *this;
}

Polyline & Polyline::rotate(double angle, const Point & center)
{
  _path.rotate(angle, center);
  std::vector<Path>::size_type count = _holes.size();
  for (std::vector<Path>::size_type i = 0; i < count; ++i) {
    Path & hole = _holes[i];
    hole.rotate(angle, center);
  }
  return *this;
}

Polyline Polyline::rotated(double angle, const Point & center) const
{
  return Polyline(*this).rotate(angle, center);
}

Polyline & Polyline::rotate(double angle)
{
  rotate(angle, center());
  return *this;
}

Polyline Polyline::rotated(double angle) const
{
  return Polyline(*this).rotate(angle, center());
}

Polyline & Polyline::translate(double dx, double dy)
{
  _path.translate(dx, dy);
  std::vector<Path>::size_type count = _holes.size();
  for (std::vector<Path>::size_type i = 0; i < count; ++i) {
    Path & hole = _holes[i];
    hole.translate(dx, dy);
  }
  return *this;
}

Polyline Polyline::translated(double dx, double dy) const
{
  return Polyline(*this).translate(dx, dy);
}

Polyline & Polyline::scale(double sx, double sy)
{
  _path.scale(sx, sy);
  Point pathCenter = _path.center();
  std::vector<Path>::size_type count = _holes.size();
  for (std::vector<Path>::size_type i = 0; i < count; ++i) {
    Path & hole = _holes[i];
    Point holeShift = hole.center() - pathCenter;
    hole.scale(sx, sy);
    holeShift.x *= sx;
    holeShift.y *= sy;
    hole.moveCenter(pathCenter + holeShift);
  }
  updateLineWidth(std::max(sx, sy));
  return *this;
}

Polyline & Polyline::scale(double s)
{
  Polyline::scale(s, s);
  return *this;
}

Polyline Polyline::scaled(double sx, double sy) const
{
  return Polyline(*this).scale(sx, sy);
}

Polyline Polyline::scaled(double s) const
{
  return Polyline(*this).scale(s, s);
}

void Polyline::scaleAll(double s)
{
  _path.scaleAll(s);
  std::vector<Path>::size_type count = _holes.size();
  for (std::vector<Path>::size_type i = 0; i < count; ++i) {
    Path & hole = _holes[i];
    hole.scaleAll(s);
  }
}

Polyline Polyline::resized(double w, double h, LineWidthFlag lineWidthFlag) const
{
  return static_cast<Polyline &>(Polyline(*this).resize(w, h, lineWidthFlag));
}

void Polyline::addHole(const Path & path)
{
  _holes.push_back(path);
  _holes.back().close();
}

Polyline * Polyline::clone() const
{
  return new Polyline(*this);
}

void Polyline::setRectangleFlag()
{
  _isCreatedRectangle = true;
}

void Polyline::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  if (_path.empty()) {
    return;
  }
  stream << "\n% Polyline\n";
  if (filled()) {
    stream << "n ";
    _path.getClockwise().flushPostscript(stream, transform);
    if (_holes.size()) {
      for (unsigned int h = 0; h < _holes.size(); ++h) {
        _holes[h].getCounterclockwise().flushPostscript(stream, transform);
      }
    }
    stream << " ";
    fillColor().flushPostscript(stream);
    stream << " " << _style.postscriptProperties(transform);
    stream << " fill" << std::endl;
  }
  if (penColor() != Color::Null && lineWidth() != 0.0) {
    stream << " " << _style.postscriptProperties(transform) << "\n";
    stream << "n ";
    _path.flushPostscript(stream, transform);
    if (_holes.size()) {
      for (unsigned int h = 0; h < _holes.size(); ++h) {
        _holes[h].flushPostscript(stream, transform);
      }
    }
    stream << " ";
    penColor().flushPostscript(stream);
    stream << " stroke" << std::endl;
  }
}

void Polyline::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  if (_path.empty()) {
    return;
  }
  if (_isCreatedRectangle && (_path.size() == 4)) {
    // Check if this is an horizontal rectangle for according to XFig file format
    Point u = _path[1] - _path[0];
    Point v = _path[3] - _path[0];
    if (Tools::almostEqual(u.y, 0.0) && Tools::almostEqual(v.x, 0.0) && (std::abs(u * v) <= 0.01)) {
      flushRectangleFIG(stream, transform, colormap);
      return;
    }
  }
  if (_path.isClosed()) {
    stream << "2 3 " << lineStyle() << " ";
  } else {
    stream << "2 1 " << lineStyle() << " ";
  }
  // Thickness
  stream << (penColor().valid() ? transform.mapWidth(lineWidth()) : 0) << " ";
  // Pen color
  stream << colormap[penColor()] << " ";
  // Fill color
  stream << colormap[fillColor()] << " ";
  // Depth
  stream << transform.shapeDepth(this) << " ";
  // Pen style
  stream << "-1 ";
  // Area fill, style val, join style, cap style, radius, f_arrow, b_arrow
  if (filled()) {
    stream << "20 " << (lineStyle() ? "4.000 " : "0.000 ") << lineJoin() << " " << lineCap() << " -1 0 0 ";
  } else {
    stream << "-1 " << (lineStyle() ? "4.000 " : "0.000 ") << lineJoin() << " " << lineCap() << " -1 0 0 ";
  }
  // Number of points
  stream << _path.size() + _path.isClosed() << std::endl;
  _path.flushFIG(stream << "         ", transform);
  stream << std::endl;
}

void Polyline::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  if (_path.empty()) {
    return;
  }

  if (_isCreatedRectangle && (_path.size() == 4)) {
    const Point ab = _path[1] - _path[0];
    const Point ad = _path[3] - _path[0];
    const Point cb = _path[1] - _path[2];
    const Point cd = _path[3] - _path[2];
    if (orthogonal(ab, ad) && orthogonal(cb, cd)) {
      flushRectangleSVG(stream, transform);
      return;
    }
  }

  if (_holes.size()) {
    stream << "<path";
    stream << _style.svgProperties(transform) << std::endl;
    stream << "          d=\"";
    Path p = _path.getClockwise();
    p.close();
    p.flushSVGCommands(stream, transform);
    for (unsigned int h = 0; h < _holes.size(); ++h) {
      p = _holes[h];
      p.setCounterclockwise();
      p.close();
      p.flushSVGCommands(stream, transform);
    }
  } else {
    if (_path.isClosed()) {
      stream << "<polygon";
    } else {
      stream << "<polyline";
    }
    stream << _style.svgProperties(transform) << std::endl;
    stream << "          points=\"";
    _path.flushSVGPoints(stream, transform);
  }

  stream << "\" />" << std::endl;
}

void Polyline::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  if (_path.empty())
    return;

  stream << "\\path[" << _style.tikzProperties(transform) << "] ";
  _path.flushTikZPoints(stream, transform);
  if (_path.isClosed())
    stream << " -- cycle";
  stream << ";" << std::endl;
}

void Polyline::accept(ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Polyline::accept(const ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Polyline::accept(ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

void Polyline::accept(const ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

Shape * Polyline::accept(CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Shape * Polyline::accept(const CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}
Rect Polyline::boundingBox(LineWidthFlag lineWidthFlag) const
{
  switch (lineWidthFlag) {
  case UseLineWidth:
    return Tools::pathBoundingBox(_path, lineWidth(), lineCap(), lineJoin());
  case IgnoreLineWidth:
    return _path.boundingBox();
  }
  return Rect();
}

void Polyline::flushRectangleFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  stream << "2 2 " << lineStyle() << " ";
  // Thickness
  stream << (penColor().valid() ? transform.mapWidth(lineWidth()) : 0) << " ";
  // Pen color
  stream << colormap[penColor()] << " ";
  // Fill color
  stream << colormap[fillColor()] << " ";
  // Depth
  stream << transform.shapeDepth(this) << " ";
  // Pen style
  stream << "-1 ";
  // Area fill, style val, join style, cap style, radius, f_arrow, b_arrow, number of points
  if (filled()) {
    stream << "20 " << (lineStyle() ? "4.000 " : "0.000 ") << lineJoin() << " " << lineCap() << " -1 0 0 5\n";
  } else {
    stream << "-1 " << (lineStyle() ? "4.000 " : "0.000 ") << lineJoin() << " " << lineCap() << " -1 0 0 5\n";
  }
  stream << "         ";
  _path.flushFIG(stream, transform);
  stream << std::endl;
}

void Polyline::flushRectangleSVG(std::ostream & stream, const TransformSVG & transform) const
{
  if (Tools::almostEqual(_path[0].y, _path[1].y) && (_path[1].y > _path[2].y)) {
    stream << "<rect x=\"" << transform.mapX(_path[0].x) << '"'               //
           << " y=\"" << transform.mapY(_path[0].y) << '"'                    //
           << " width=\"" << transform.scale(_path[1].x - _path[0].x) << '"'  //
           << " height=\"" << transform.scale(_path[0].y - _path[3].y) << '"' //
           << _style.svgProperties(transform) << " />" << std::endl;
  } else {
    Point v = _path[1] - _path[0];
    v /= v.norm();
    const double angle = Tools::rad2deg(v.argument());
    stream << "<rect x=\"" << transform.mapX(_path[0].x) << '"'                    //
           << " y=\"" << transform.mapY(_path[0].y) << '"'                         //
           << " width=\"" << transform.scale((_path[1] - _path[0]).norm()) << '"'  //
           << " height=\"" << transform.scale((_path[0] - _path[3]).norm()) << '"' //
           << _style.svgProperties(transform) << ' '                               //
           << " transform=\"rotate(" << -angle << ", " << transform.mapX(_path[0].x) << ", " << transform.mapY(_path[0].y) << ") \" "
           << " />" << std::endl;
  }
}

/*
 * GouraudTriangle
 */

const std::string GouraudTriangle::_name("GouraudTriangle");

const std::string & GouraudTriangle::name() const
{
  return _name;
}

GouraudTriangle::GouraudTriangle(const Point & p0, const Color & color0, //
                                 const Point & p1, const Color & color1, //
                                 const Point & p2, const Color & color2, //
                                 int subdivisions)
    : Polyline(std::vector<Point>(), Path::Closed, Color::Null, Color::Null, 0.0, SolidStyle, ButtCap, MiterJoin), //
      _color0(color0), _color1(color1), _color2(color2), _subdivisions(subdivisions)
{
  _path << p0;
  _path << p1;
  _path << p2;

  _style.fillColor.red((color0.red() + color1.red() + color2.red()) / 3);
  _style.fillColor.green((color0.green() + color1.green() + color2.green()) / 3);
  _style.fillColor.blue((color0.blue() + color1.blue() + color2.blue()) / 3);
}

GouraudTriangle::GouraudTriangle(const Point & p0, float brightness0, //
                                 const Point & p1, float brightness1, //
                                 const Point & p2, float brightness2, //
                                 const Color & fillColor, int subdivisions)
    : Polyline(std::vector<Point>(), Path::Closed, Color::Null, Color::Null, 0.0, SolidStyle, ButtCap, MiterJoin), //
      _color0(fillColor), _color1(fillColor), _color2(fillColor), _subdivisions(subdivisions)
{
  _path << p0;
  _path << p1;
  _path << p2;
  _color0.red(static_cast<unsigned char>(std::min(255.0f, _color0.red() * brightness0)));
  _color0.green(static_cast<unsigned char>(std::min(255.0f, _color0.green() * brightness0)));
  _color0.blue(static_cast<unsigned char>(std::min(255.0f, _color0.blue() * brightness0)));
  _color1.red(static_cast<unsigned char>(std::min(255.0f, _color1.red() * brightness1)));
  _color1.green(static_cast<unsigned char>(std::min(255.0f, _color1.green() * brightness1)));
  _color1.blue(static_cast<unsigned char>(std::min(255.0f, _color1.blue() * brightness1)));
  _color2.red(static_cast<unsigned char>(std::min(255.0f, _color2.red() * brightness2)));
  _color2.green(static_cast<unsigned char>(std::min(255.0f, _color2.green() * brightness2)));
  _color2.blue(static_cast<unsigned char>(std::min(255.0f, _color2.blue() * brightness2)));

  _style.fillColor.red((_color0.red() + _color1.red() + _color2.red()) / 3);
  _style.fillColor.green((_color0.green() + _color1.green() + _color2.green()) / 3);
  _style.fillColor.blue((_color0.blue() + _color1.blue() + _color2.blue()) / 3);
}

GouraudTriangle & GouraudTriangle::rotate(double angle, const Point & center)
{
  _path.rotate(angle, center);
  return *this;
}

GouraudTriangle GouraudTriangle::rotated(double angle, const Point & center) const
{
  return GouraudTriangle(*this).rotate(angle, center);
}

GouraudTriangle & GouraudTriangle::rotate(double angle)
{
  return GouraudTriangle::rotate(angle, center());
}

GouraudTriangle GouraudTriangle::rotated(double angle) const
{
  return GouraudTriangle(*this).rotate(angle, center());
}

GouraudTriangle GouraudTriangle::translated(double dx, double dy) const
{
  return static_cast<GouraudTriangle &>(GouraudTriangle(*this).translate(dx, dy));
}

GouraudTriangle GouraudTriangle::scaled(double sx, double sy) const
{
  return static_cast<GouraudTriangle &>(GouraudTriangle(*this).scale(sx, sy));
}

GouraudTriangle GouraudTriangle::scaled(double s) const
{
  return static_cast<GouraudTriangle &>(GouraudTriangle(*this).scale(s, s));
}

void GouraudTriangle::scaleAll(double s)
{
  _path.scaleAll(s);
}

GouraudTriangle GouraudTriangle::resized(double w, double h, LineWidthFlag lineWidthFlag) const
{
  return static_cast<GouraudTriangle &>(GouraudTriangle(*this).resize(w, h, lineWidthFlag));
}

GouraudTriangle * GouraudTriangle::clone() const
{
  return new GouraudTriangle(*this);
}

void GouraudTriangle::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  if (!_subdivisions) {
    Polyline::flushPostscript(stream, transform);
    return;
  }
  const Point & p0 = _path[0];
  const Point & p1 = _path[1];
  const Point & p2 = _path[2];
  Point p01(0.5 * (p0.x + p1.x), 0.5 * (p0.y + p1.y));
  Color c01((_color0.red() + _color1.red()) / 2, (_color0.green() + _color1.green()) / 2, (_color0.blue() + _color1.blue()) / 2);
  Point p12(0.5 * (p1.x + p2.x), 0.5 * (p1.y + p2.y));
  Color c12((_color1.red() + _color2.red()) / 2, (_color1.green() + _color2.green()) / 2, (_color1.blue() + _color2.blue()) / 2);
  Point p20(0.5 * (p2.x + p0.x), 0.5 * (p2.y + p0.y));
  Color c20((_color2.red() + _color0.red()) / 2, (_color2.green() + _color0.green()) / 2, (_color2.blue() + _color0.blue()) / 2);
  GouraudTriangle(p0, _color0, p20, c20, p01, c01, _subdivisions - 1).flushPostscript(stream, transform);
  GouraudTriangle(p1, _color1, p01, c01, p12, c12, _subdivisions - 1).flushPostscript(stream, transform);
  GouraudTriangle(p2, _color2, p20, c20, p12, c12, _subdivisions - 1).flushPostscript(stream, transform);
  GouraudTriangle(p01, c01, p12, c12, p20, c20, _subdivisions - 1).flushPostscript(stream, transform);
}

void GouraudTriangle::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{

  Color c(static_cast<unsigned char>((_color0.red() + _color1.red() + _color2.red()) / 3.0), static_cast<unsigned char>((_color0.green() + _color1.green() + _color2.green()) / 3.0),
          static_cast<unsigned char>((_color0.blue() + _color1.blue() + _color2.blue()) / 3.0));
  Polyline(_path, Color::Null, c, 0.0).flushFIG(stream, transform, colormap);

  // if ( ! _subdivisions ) {
  // Polyline::flushFIG( stream, transform, colormap );
  // return;
  // }
  // TODO : Handle extended colormap through clustering...
  //   const Point & p0 = _points[0];
  //   const Point & p1 = _points[1];
  //   const Point & p2 = _points[2];
  //   Point p01( 0.5*(p0.x+p1.x), 0.5*(p0.y+p1.y) );
  //   Color c01( (_color0.red() + _color1.red())/2,
  // 	     (_color0.green() + _color1.green())/2,
  // 	     (_color0.blue() + _color1.blue())/2 );
  //   Point p12( 0.5*(p1.x+p2.x), 0.5*(p1.y+p2.y) );
  //   Color c12( (_color1.red() + _color2.red())/2,
  // 	     (_color1.green() + _color2.green())/2,
  // 	     (_color1.blue() + _color2.blue())/2 );
  //   Point p20( 0.5*(p2.x+p0.x), 0.5*(p2.y+p0.y) );
  //   Color c20( (_color2.red() + _color0.red())/2,
  // 	     (_color2.green() + _color0.green())/2,
  // 	     (_color2.blue() + _color0.blue())/2 );
  //   GouraudTriangle( p0, _color0, p20, c20, p01, c01, _subdivisions - 1, _depth ).flushFIG( stream, transform, colormap );
  //   GouraudTriangle( p1, _color1, p01, c01, p12, c12, _subdivisions - 1, _depth ).flushFIG( stream, transform, colormap );
  //   GouraudTriangle( p2, _color2, p20, c20, p12, c12, _subdivisions - 1, _depth ).flushFIG( stream, transform, colormap );
  //   GouraudTriangle( p01, c01, p12, c12, p20, c20,  _subdivisions - 1, _depth ).flushFIG( stream, transform, colormap );
}

void GouraudTriangle::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  if (!_subdivisions) {
    Polyline::flushSVG(stream, transform);
    return;
  }
  const Point & p0 = _path[0];
  const Point & p1 = _path[1];
  const Point & p2 = _path[2];
  Point p01(0.5 * (p0.x + p1.x), 0.5 * (p0.y + p1.y));
  Color c01((_color0.red() + _color1.red()) / 2, (_color0.green() + _color1.green()) / 2, (_color0.blue() + _color1.blue()) / 2);
  Point p12(0.5 * (p1.x + p2.x), 0.5 * (p1.y + p2.y));
  Color c12((_color1.red() + _color2.red()) / 2, (_color1.green() + _color2.green()) / 2, (_color1.blue() + _color2.blue()) / 2);
  Point p20(0.5 * (p2.x + p0.x), 0.5 * (p2.y + p0.y));
  Color c20((_color2.red() + _color0.red()) / 2, (_color2.green() + _color0.green()) / 2, (_color2.blue() + _color0.blue()) / 2);
  GouraudTriangle(p0, _color0, p20, c20, p01, c01, _subdivisions - 1).flushSVG(stream, transform);
  GouraudTriangle(p1, _color1, p01, c01, p12, c12, _subdivisions - 1).flushSVG(stream, transform);
  GouraudTriangle(p2, _color2, p20, c20, p12, c12, _subdivisions - 1).flushSVG(stream, transform);
  GouraudTriangle(p01, c01, p12, c12, p20, c20, _subdivisions - 1).flushSVG(stream, transform);
}

void GouraudTriangle::flushTikZ(std::ostream & stream, const TransformTikZ & /*transform*/) const
{
  // FIXME: unimplemented
  stream << "% FIXME: GouraudTriangle::flushTikZ unimplemented" << std::endl;
}

Polyline rectangle(double left, double top, double width, double height, //
                   Color penColor, Color fillColor, double lineWidth,    //
                   const LineStyle lineStyle, const LineCap cap, const LineJoin join)
{
  Path path(Path::Closed);
  width = std::abs(width);
  height = std::abs(height);
  path << Point(left, top);
  path << Point(left + width, top);
  path << Point(left + width, top - height);
  path << Point(left, top - height);
  Polyline polyline(path, penColor, fillColor, lineWidth, lineStyle, cap, join);
  polyline.setRectangleFlag();
  return polyline;
}

Polyline rectangle(const Rect & rect, Color penColor, Color fillColor, double lineWidth, //
                   const LineStyle lineStyle, const LineCap cap, const LineJoin join)
{
  return rectangle(rect.left, rect.top, rect.width, rect.height, penColor, fillColor, lineWidth, lineStyle, cap, join);
}

Polyline rectangle(double left, double top, double width, double height, const Style & style)
{
  return rectangle(left, top, width, height, style.penColor, style.fillColor, style.lineWidth, style.lineStyle, style.lineCap, style.lineJoin);
}

Polyline rectangle(const Rect & rect, const Style & style)
{
  return rectangle(rect.left, rect.top, rect.width, rect.height, style.penColor, style.fillColor, style.lineWidth, style.lineStyle, style.lineCap, style.lineJoin);
}

Polyline triangle(const double x1, const double y1, const double x2, const double y2, const double x3, const double y3, //
                  Color penColor, Color fillColor, double lineWidth,                                                    //
                  const LineStyle lineStyle, const LineCap cap, const LineJoin join)
{
  Path path(Path::Closed);
  path << Point(x1, y1) << Point(x2, y2) << Point(x3, y3);
  return Polyline(path, penColor, fillColor, lineWidth, lineStyle, cap, join);
}

Polyline triangle(const Point & p1, const Point & p2, const Point & p3, //
                  Color penColor, Color fillColor, double lineWidth,    //
                  const LineStyle lineStyle, const LineCap cap, const LineJoin join)
{
  Path path(Path::Closed);
  path << p1 << p2 << p3;
  return Polyline(path, penColor, fillColor, lineWidth, lineStyle, cap, join);
}

Polyline triangle(const Point & p1, const Point & p2, const Point & p3, const Style & style)
{
  Path path(Path::Closed);
  path << p1 << p2 << p3;
  return Polyline(path, style.penColor, style.fillColor, style.lineWidth, style.lineStyle, style.lineCap, style.lineJoin);
}

Polyline mix(const Polyline & a, const Polyline & b, double time)
{
  Path path = mix(a.path(), b.path(), time);
  if (a.lineStyle() != b.lineStyle() || a.lineCap() != b.lineCap() || a.lineJoin() != b.lineJoin()) {
    Tools::warning << "Polyline::mid(): lines have different style/join/cap attributes" << std::endl;
  }
  Color pen = a.penColor();
  if (pen != b.penColor()) {
    pen = Color::mixRGB(a.penColor(), b.penColor(), static_cast<float>(time));
  }
  Color fill = a.fillColor();
  if (fill != b.fillColor()) {
    fill = Color::mixRGB(a.fillColor(), b.fillColor(), static_cast<float>(time));
  }
  double width = a.lineWidth();
  if (width != b.lineWidth()) {
    width = Tools::mix(a.lineWidth(), b.lineWidth(), time);
  }
  return Polyline(path, pen, fill, width, a.lineStyle(), a.lineCap(), a.lineJoin());
}

} // namespace LibBoard
