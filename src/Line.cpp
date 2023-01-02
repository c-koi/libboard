/* -*- mode: c++ -*- */
/**
 * @file   Line.cpp
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
#include <board/Line.h>
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

const std::string Line::_name("Line");

const std::string & Line::name() const
{
  return _name;
}

Line & Line::rotate(double angle, const Point & center)
{
  _a = _a.rotated(angle, center);
  _b = _b.rotated(angle, center);
  return *this;
}

Line & Line::rotate(double angle)
{
  return Line::rotate(angle, center());
}

Line Line::rotated(double angle, const Point & center) const
{
  Line res(*this);
  res._a = _a.rotated(angle, center);
  res._b = _b.rotated(angle, center);
  return res;
}

Line Line::rotated(double angle) const
{
  Line res(*this);
  const Point c = center();
  res._a = _a.rotated(angle, c);
  res._b = _b.rotated(angle, c);
  return res;
}

Line & Line::translate(double dx, double dy)
{
  _a.translate(dx, dy);
  _b.translate(dx, dy);
  return *this;
}

Line Line::translated(double dx, double dy) const
{
  Line res(*this);
  res._a.translate(dx, dy);
  res._b.translate(dx, dy);
  return *this;
}

Line & Line::scale(double sx, double sy)
{
  Point c = center();
  _a.x *= sx;
  _a.y *= sy;
  _b.x *= sx;
  _b.y *= sy;
  Point delta = c - center();
  translate(delta.x, delta.y);
  updateLineWidth(std::max(sx, sy));
  return *this;
}

Line & Line::scale(double s)
{
  scale(s, s);
  return (*this);
}

Line Line::scaled(double sx, double sy) const
{
  return Line(*this).scale(sx, sy);
}

Line Line::scaled(double s) const
{
  return Line::scaled(s, s);
}

void Line::scaleAll(double s)
{
  _a.x *= s;
  _a.y *= s;
  _b.x *= s;
  _b.y *= s;
}

Line Line::resized(double w, double h, LineWidthFlag lineWidthFlag) const
{
  return static_cast<Line &>(Line(*this).resize(w, h, lineWidthFlag));
}

Line * Line::clone() const
{
  return new Line(*this);
}

void Line::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  const Point a = transform.map(_a);
  const Point b = transform.map(_b);
  stream << "\n% Line\n";
  stream << _style.postscriptProperties(transform) << " "
         << "n " << a.x << " " << a.y << " "
         << "m " << b.x << " " << b.y << " "
         << "l " << penColor().postscript() << " srgb stroke" << std::endl;
}

void Line::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  const Point a = transform.map(_a);
  const Point b = transform.map(_b);
  stream << "2 1 ";
  // Line style
  stream << lineStyle() << " ";
  // Thickness
  stream << (penColor().valid() ? transform.mapWidth(lineWidth()) : 0) << " ";
  // Pen color
  stream << colormap[penColor()] << " ";
  // Fill color
  stream << "0 ";
  // Depth
  stream << transform.shapeDepth(this) << " ";
  // Pen style
  stream << "-1 ";
  // Area fill, style val, join style, cap style, radius, f_arrow, b_arrow
  stream << "-1 " << (lineStyle() ? "4.000 " : "0.000 ") << lineJoin() << " " << lineCap() << " -1 0 0 ";
  // Number of points
  stream << "2\n";
  stream << "         ";
  stream << static_cast<int>(a.x) << " " << static_cast<int>(a.y) << " " //
         << static_cast<int>(b.x) << " " << static_cast<int>(b.y) << std::endl;
}

void Line::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  const Point a = transform.map(_a);
  const Point b = transform.map(_b);
  stream << "<line x1=\"" << a.x << "\""
         << " y1=\"" << a.y << "\""
         << " x2=\"" << b.x << "\""
         << " y2=\"" << b.y << "\"" << _style.svgProperties(transform) << " />" << std::endl;
}

void Line::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  const Point a = transform.map(_a);
  const Point b = transform.map(_b);
  stream << "\\path[" << _style.tikzProperties(transform) << "] (" << a.x << ',' << a.y << ") -- (" << b.x << ',' << b.y << ");" << std::endl;
}

void Line::accept(ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Line::accept(const ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Line::accept(ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

void Line::accept(const ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

Shape * Line::accept(CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Shape * Line::accept(const CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Line::Line(const Line & other) //
    : ShapeWithStyle(other), _a(other._a), _b(other._b)
{
}

Line & Line::operator=(const Line & other)
{
  _a = other._a;
  _b = other._b;
  return *this;
}

Rect Line::boundingBox(LineWidthFlag lineWidthFlag) const
{
  Path p;
  p << _a << _b;
  switch (lineWidthFlag) {
  case UseLineWidth:
    return Tools::pathBoundingBox(p, lineWidth(), lineCap(), lineJoin());
  case IgnoreLineWidth:
    return p.boundingBox();
  }
  return p.boundingBox();
}

} // namespace LibBoard
