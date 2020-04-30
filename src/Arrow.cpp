/* -*- mode: c++ -*- */
/**
 * @file   Arrow.cpp
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
#include "board/Arrow.h"
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

const std::string Arrow::_name("Arrow");

const std::string & Arrow::name() const
{
  return _name;
}

Arrow Arrow::rotated(double angle, const Point & center) const
{
  Arrow res(*this);
  Point(_x1, _y1).rotate(angle, center).get(res._x1, res._y1);
  Point(_x2, _y2).rotate(angle, center).get(res._x2, res._y2);
  return res;
}

Arrow Arrow::rotated(double angle) const
{
  Arrow res(*this);
  Point c = center();
  Point(_x1, _y1).rotate(angle, c).get(res._x1, res._y1);
  Point(_x2, _y2).rotate(angle, c).get(res._x2, res._y2);
  return res;
}

Arrow Arrow::translated(double dx, double dy) const
{
  return static_cast<Arrow &>(Arrow(*this).translate(dx, dy));
}

Arrow Arrow::scaled(double sx, double sy) const
{
  return static_cast<Arrow &>(Arrow(*this).scale(sx, sy));
}

Arrow Arrow::scaled(double s) const
{
  return Arrow::scaled(s, s);
}

Arrow Arrow::resized(double w, double h, Shape::LineWidthFlag lineWidthFlag) const
{
  return static_cast<Arrow &>(Arrow(*this).resize(w, h, lineWidthFlag));
}

Rect Arrow::boundingBox(Shape::LineWidthFlag) const
{
  double dx = _x1 - _x2;
  double dy = _y1 - _y2;
  double norm = sqrt(dx * dx + dy * dy);
  dx /= norm;
  dy /= norm;
  dx *= 10 * _lineWidth;
  dy *= 10 * _lineWidth;

  double ndx1 = dx * cos(0.3) - dy * sin(0.3);
  double ndy1 = dx * sin(0.3) + dy * cos(0.3);
  double ndx2 = dx * cos(-0.3) - dy * sin(-0.3);
  double ndy2 = dx * sin(-0.3) + dy * cos(-0.3);

  Path pLine;
  pLine << Point(_x1, _y1);
  pLine << Point(_x2 + (dx * cos(0.3)), _y2 + (dy * cos(0.3)));

  Path pArrow;
  pArrow << Point(_x2 + ndx1, _y2 + ndy1) << Point(_x2, _y2) << Point(_x2 + ndx2, _y2 + ndy2);

  return Tools::pathBoundingBox(pLine, _lineWidth, _lineCap, _lineJoin) || pArrow.boundingBox();
}

Arrow * Arrow::clone() const
{
  return new Arrow(*this);
}

void Arrow::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  double dx = _x1 - _x2;
  double dy = _y1 - _y2;
  double norm = sqrt(dx * dx + dy * dy);
  dx /= norm;
  dy /= norm;
  dx *= 10 * _lineWidth;
  dy *= 10 * _lineWidth;
  double ndx1 = dx * cos(0.3) - dy * sin(0.3);
  double ndy1 = dx * sin(0.3) + dy * cos(0.3);
  double ndx2 = dx * cos(-0.3) - dy * sin(-0.3);
  double ndy2 = dx * sin(-0.3) + dy * cos(-0.3);

  stream << "\n% Arrow\n";
  stream << _penColor.postscript() << " srgb " << postscriptProperties(transform) << " "
         << "n " << transform.mapX(_x1) << " " << transform.mapY(_y1) << " "
         << "m " << transform.mapX(_x2 + (dx * cos(0.3))) << " " << transform.mapY(_y2 + (dy * cos(0.3))) << " "
         << "l stroke" << std::endl;

  if (filled()) {
    stream << "n " << transform.mapX(_x2) + transform.scale(ndx1) << " " << transform.mapY(_y2) + transform.scale(ndy1) << " "
           << "m " << transform.mapX(_x2) << " " << transform.mapY(_y2) << " l " << transform.mapX(_x2) + transform.scale(ndx2) << " " << transform.mapY(_y2) + transform.scale(ndy2) << " ";
    stream << "l cp " << _fillColor.postscript() << " srgb  fill" << std::endl;
  }
}

void Arrow::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  stream << "2 1 ";
  // Line style
  stream << _lineStyle << " ";
  // Thickness
  stream << (_penColor.valid() ? transform.mapWidth(_lineWidth) : 0) << " ";
  // Pen color
  stream << colormap[_penColor] << " ";
  // Fill color
  stream << colormap[_fillColor] << " ";
  // Depth
  stream << transform.mapDepth(_depth) << " ";
  // Pen style
  stream << "-1 ";
  // Area fill, style val, join style, cap style, radius, f_arrow, b_arrow
  stream << "-1 " << (_lineStyle ? "4.000 " : "0.000 ") << _lineJoin << " " << _lineCap << " -1 1 0 ";
  // Number of points
  stream << "2\n";
  if (filled())
    stream << "       1 1 1.00 60.00 120.00\n";
  else
    stream << "       1 0 1.00 60.00 120.00\n";
  stream << "         ";
  stream << static_cast<int>(transform.mapX(_x1)) << " " << static_cast<int>(transform.mapY(_y1)) << " " << static_cast<int>(transform.mapX(_x2)) << " " << static_cast<int>(transform.mapY(_y2))
         << std::endl;
}

void Arrow::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  double dx = _x1 - _x2;
  double dy = _y1 - _y2;
  double norm = sqrt(dx * dx + dy * dy);
  dx /= norm;
  dy /= norm;
  dx *= 10 * _lineWidth;
  dy *= 10 * _lineWidth;
  double ndx1 = dx * cos(0.3) - dy * sin(0.3);
  double ndy1 = dx * sin(0.3) + dy * cos(0.3);
  double ndx2 = dx * cos(-0.3) - dy * sin(-0.3);
  double ndy2 = dx * sin(-0.3) + dy * cos(-0.3);

  stream << "<g>" << std::endl;
  // The line
  stream << " <path "
         << "d=\"M " << transform.mapX(_x1) << " " << transform.mapY(_y1) << " L " << transform.mapX(_x2 + (dx * cos(0.3))) << " " << transform.mapY(_y2 + (dy * cos(0.3))) << " z\""
         << " fill=\"none\" stroke=\"" << _penColor.svg() << "\"" << _penColor.svgAlpha(" stroke");

  if (_lineStyle != SolidStyle) {
    stream << " style=\"" << xFigDashStylesSVG[_lineStyle] << '"';
  }
  stream << " stroke-width=\"" << transform.mapWidth(_lineWidth) << "mm\" />";

  // The arrow
  stream << " <polygon";
  stream << " fill=\"" << _fillColor.svg() << "\"";
  stream << " stroke=\"none\""
         << " stroke-width=\"0mm\""
         << " style=\"stroke-linecap:butt;stroke-linejoin:miter\"" << _fillColor.svgAlpha(" fill") << _penColor.svgAlpha(" stroke") << " points=\"" << transform.mapX(_x2) + transform.scale(ndx1)
         << "," << transform.mapY(_y2) - transform.scale(ndy1) << " " << transform.mapX(_x2) << "," << transform.mapY(_y2) << " " << transform.mapX(_x2) + transform.scale(ndx2) << ","
         << transform.mapY(_y2) - transform.scale(ndy2) << " " << transform.mapX(_x2) + transform.scale(ndx1) << "," << transform.mapY(_y2) - transform.scale(ndy1) << "\" />" << std::endl;
  stream << "</g>" << std::endl;
}

void Arrow::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  stream << "\\path[-latex," << tikzProperties(transform) << "] (" << transform.mapX(_x1) << ',' << transform.mapY(_y1) << ") -- (" << transform.mapX(_x2) << ',' << transform.mapY(_y2) << ");"
         << std::endl;
}

} // namespace LibBoard
