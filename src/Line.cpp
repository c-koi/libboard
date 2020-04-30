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
#include "board/Line.h"
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

const std::string Line::_name("Line");

const std::string & Line::name() const
{
  return _name;
}

Line & Line::rotate(double angle, const Point & center)
{
  Point(_x1, _y1).rotate(angle, center).get(_x1, _y1);
  Point(_x2, _y2).rotate(angle, center).get(_x2, _y2);
  return *this;
}

Line & Line::rotate(double angle)
{
  return Line::rotate(angle, center());
}

Line Line::rotated(double angle, const Point & center) const
{
  Line res(*this);
  Point(_x1, _y1).rotate(angle, center).get(res._x1, res._y1);
  Point(_x2, _y2).rotate(angle, center).get(res._x2, res._y2);
  return res;
}

Line Line::rotated(double angle) const
{
  Line res(*this);
  Point c = center();
  Point(_x1, _y1).rotate(angle, c).get(res._x1, res._y1);
  Point(_x2, _y2).rotate(angle, c).get(res._x2, res._y2);
  return res;
}

Line & Line::translate(double dx, double dy)
{
  _x1 += dx;
  _x2 += dx;
  _y1 += dy;
  _y2 += dy;
  return *this;
}

Line Line::translated(double dx, double dy) const
{
  Line res(*this);
  res._x1 += dx;
  res._x2 += dx;
  res._y1 += dy;
  res._y2 += dy;
  return *this;
}

Line & Line::scale(double sx, double sy)
{
  Point c = center();
  _x1 *= sx;
  _x2 *= sx;
  _y1 *= sy;
  _y2 *= sy;
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
  _x1 *= s;
  _y1 *= s;
  _x2 *= s;
  _y2 *= s;
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
  stream << "\n% Line\n";
  stream << postscriptProperties(transform) << " "
         << "n " << transform.mapX(_x1) << " " << transform.mapY(_y1) << " "
         << "m " << transform.mapX(_x2) << " " << transform.mapY(_y2) << " "
         << "l " << _penColor.postscript() << " srgb stroke" << std::endl;
}

void Line::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  stream << "2 1 ";
  // Line style
  stream << _lineStyle << " ";
  // Thickness
  stream << (_penColor.valid() ? transform.mapWidth(_lineWidth) : 0) << " ";
  // Pen color
  stream << colormap[_penColor] << " ";
  // Fill color
  stream << "0 ";
  // Depth
  stream << transform.mapDepth(_depth) << " ";
  // Pen style
  stream << "-1 ";
  // Area fill, style val, join style, cap style, radius, f_arrow, b_arrow
  stream << "-1 " << (_lineStyle ? "4.000 " : "0.000 ") << _lineJoin << " " << _lineCap << " -1 0 0 ";
  // Number of points
  stream << "2\n";
  stream << "         ";
  stream << static_cast<int>(transform.mapX(_x1)) << " " << static_cast<int>(transform.mapY(_y1)) << " " << static_cast<int>(transform.mapX(_x2)) << " " << static_cast<int>(transform.mapY(_y2))
         << std::endl;
}

void Line::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  stream << "<line x1=\"" << transform.mapX(_x1) << "\""
         << " y1=\"" << transform.mapY(_y1) << "\""
         << " x2=\"" << transform.mapX(_x2) << "\""
         << " y2=\"" << transform.mapY(_y2) << "\"" << svgProperties(transform) << " />" << std::endl;
}

void Line::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  stream << "\\path[" << tikzProperties(transform) << "] (" << transform.mapX(_x1) << ',' << transform.mapY(_y1) << ") -- (" << transform.mapX(_x2) << ',' << transform.mapY(_y2) << ");" << std::endl;
}

Rect Line::boundingBox(LineWidthFlag lineWidthFlag) const
{
  Path p;
  p << Point(_x1, _y1) << Point(_x2, _y2);
  switch (lineWidthFlag) {
  case UseLineWidth:
    return Tools::pathBoundingBox(p, _lineWidth, _lineCap, _lineJoin);
    break;
  case IgnoreLineWidth:
    return p.boundingBox();
    break;
  default:
    Tools::error << "LineWidthFlag incorrect value (" << lineWidthFlag << ")\n";
    return Rect();
    break;
  }
}

} // namespace LibBoard
