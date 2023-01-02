/* -*- mode: c++ -*- */
/**
 * @file   Dot.cpp
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
#include <BoardConfig.h>
#include <algorithm>
#include <board/Dot.h>
#include <board/PSFonts.h>
#include <board/PathBoundaries.h>
#include <board/Rect.h>
#include <board/ShapeVisitor.h>
#include <board/Tools.h>
#include <board/Transforms.h>
#include <cmath>
#include <cstring>
#include <limits>
#include <sstream>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

namespace LibBoard
{

const std::string Dot::_name("Dot");

const std::string & Dot::name() const
{
  return _name;
}

Point Dot::center(LineWidthFlag) const
{
  return Point(_x, _y);
}

Dot & Dot::rotate(double angle, const Point & center)
{
  Point(_x, _y).rotate(angle, center).get(_x, _y);
  return *this;
}

Dot Dot::rotated(double angle, const Point & center) const
{
  return Dot(*this).rotate(angle, center);
}

Dot & Dot::rotate(double)
{
  return *this;
}

Dot Dot::rotated(double) const
{
  return *this;
}

Dot & Dot::translate(double dx, double dy)
{
  _x += dx;
  _y += dy;
  return *this;
}

Dot Dot::translated(double dx, double dy) const
{
  return Dot(*this).translate(dx, dy);
}

Dot & Dot::scale(double, double)
{
  return *this;
}

Dot & Dot::scale(double)
{
  return *this;
}

Dot Dot::scaled(double, double) const
{
  return *this;
}

Dot Dot::scaled(double) const
{
  return *this;
}

void Dot::scaleAll(double s)
{
  _x *= s;
  _y *= s;
}

void Dot::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  stream << "\n% Dot\n";
  stream << _style.postscriptProperties(transform) << " "
         << "n " << transform.mapX(_x) << " " << transform.mapY(_y) << " "
         << "m " << transform.mapX(_x) << " " << transform.mapY(_y) << " "
         << "l " << penColor().postscript() << " srgb stroke" << std::endl;
}

void Dot::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  stream << "2 1 0 ";
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
  stream << "-1 0.000 " << lineJoin() << " " << lineCap() << " -1 0 0 ";
  // Number of points
  stream << "2\n";
  stream << "         ";
  stream << static_cast<int>(transform.mapX(_x)) << " " << static_cast<int>(transform.mapY(_y)) << " " << static_cast<int>(transform.mapX(_x)) << " " << static_cast<int>(transform.mapY(_y))
         << std::endl;
}

void Dot::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  stream << "<line x1=\"" << transform.mapX(_x) << "\""
         << " y1=\"" << transform.mapY(_y) << "\""
         << " x2=\"" << transform.mapX(_x) << "\""
         << " y2=\"" << transform.mapY(_y) << "\"" << _style.svgProperties(transform) << " />" << std::endl;
}

void Dot::flushTikZ(std::ostream & stream, const TransformTikZ & /*transform*/) const

{
  // FIXME: unimplemented
  stream << "% FIXME: Dot::flushTikZ unimplemented" << std::endl;
}

void Dot::accept(ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Dot::accept(const ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Dot::accept(ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

void Dot::accept(const ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

Shape * Dot::accept(CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Shape * Dot::accept(const CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Rect Dot::boundingBox(LineWidthFlag lineWidthFlag) const
{
  switch (lineWidthFlag) {
  case IgnoreLineWidth:
    return Rect(_x, _y, 0.0, 0.0);
  case UseLineWidth:
    return Rect(_x - 0.5 * lineWidth(), _y + 0.5 * lineWidth(), lineWidth(), lineWidth());
  }
  return Rect();
}

Dot * Dot::clone() const
{
  return new Dot(*this);
}

} // namespace LibBoard
