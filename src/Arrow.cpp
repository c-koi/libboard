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
#include <board/Arrow.h>
#include <board/PSFonts.h>
#include <board/PathBoundaries.h>
#include <board/Polyline.h>
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

const std::string Arrow::_name("Arrow");

const std::string & Arrow::name() const
{
  return _name;
}

Arrow Arrow::rotated(double angle, const Point & center) const
{
  return static_cast<Arrow &>(Arrow(*this).rotate(angle, center));
}

Arrow Arrow::rotated(double angle) const
{
  return static_cast<Arrow &>(Arrow(*this).rotate(angle));
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

Arrow Arrow::resized(double w, double h, LineWidthFlag lineWidthFlag) const
{
  return static_cast<Arrow &>(Arrow(*this).resize(w, h, lineWidthFlag));
}

Rect Arrow::boundingBox(LineWidthFlag) const
{
  const Path pArrow = extremity();
  Path pLine;
  pLine << _a << ((pArrow[0] + pArrow[2]) / 2.0);
  Rect bbArrow = Tools::pathBoundingBox(pArrow, lineWidth(), lineCap(), lineJoin());
  return Tools::pathBoundingBox(pLine, lineWidth(), lineCap(), lineJoin()) || bbArrow;
}

Arrow * Arrow::clone() const
{
  return new Arrow(*this);
}

Path Arrow::extremity() const
{
  const double angle = 0.3;
  Path result(Path::Closed);
  Point d = (_a - _b).normalised();
  d *= 10 * lineWidth();
  const Point nd1 = d.rotated(angle);
  const Point nd2 = d.rotated(-angle);
  result << (_b + nd1) << _b << (_b + nd2);
  return result;
}

void Arrow::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  const Point ma = transform.map(_a);
  const Path triangle = extremity();
  const Point stop = (_type == ExtremityType::Stick) ? transform.map(_b) : transform.map((triangle[0] + triangle[2]) / 2.0);

  stream << "\n% Arrow\n";
  stream << penColor().postscript() << " srgb " << _style.postscriptProperties(transform) << " "
         << "n " << ma.x << " " << ma.y << " "
         << "m " << stop.x << " " << stop.y << " "
         << "l stroke" << std::endl;

  Style style = _style;
  style.lineStyle = SolidStyle;
  switch (_type) {
  case ExtremityType::Plain: {
    stream << "n ";
    triangle.flushPostscript(stream, transform);
    stream << " " << penColor().postscript() << " srgb " << style.postscriptProperties(transform) << " fill" << std::endl;
    stream << "n ";
    triangle.flushPostscript(stream, transform);
    stream << " " << penColor().postscript() << " srgb " << style.postscriptProperties(transform) << " stroke" << std::endl;
  } break;
  case ExtremityType::Closed: {
    stream << "n ";
    triangle.flushPostscript(stream, transform);
    stream << " " << Color::White.postscript() << " srgb fill" << std::endl;
    stream << "n ";
    triangle.flushPostscript(stream, transform);
    stream << " " << penColor().postscript() << " srgb stroke" << std::endl;
  } break;
  case ExtremityType::Stick: {
    // FIXME : Fixe line length to the end...
    Path p = triangle;
    p.open();
    stream << "n ";
    p.flushPostscript(stream, transform);
    stream << " " << penColor().postscript() << " srgb stroke" << std::endl;
  } break;
  }
}

void Arrow::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
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
  stream << colormap[fillColor()] << " ";
  // Depth
  stream << transform.shapeDepth(this) << " ";
  // Pen style
  stream << "-1 ";
  // Area fill, style val, join style, cap style, radius, f_arrow, b_arrow
  stream << "-1 " << (lineStyle() ? "4.000 " : "0.000 ") << lineJoin() << " " << lineCap() << " -1 1 0 ";
  // Number of points
  stream << "2\n";

  // Arrow type (0 stick, 1 closed)
  // Arrow style (0 white, 1 filled with pen color)
  switch (_type) {
  case ExtremityType::Stick:
    stream << "       0 0";
    break;
  case ExtremityType::Plain:
    stream << "       1 1";
    break;
  case ExtremityType::Closed:
    stream << "       1 0";
    break;
  }

  stream << " 1.00 60.00 120.00\n";
  stream << "         ";
  stream << static_cast<int>(a.x) << " " << static_cast<int>(a.y) //
         << " " << static_cast<int>(b.x) << " " << static_cast<int>(b.y) << std::endl;
}

void Arrow::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  Path triangle = extremity();
  const Point stop = (_type == ExtremityType::Stick) ? _b : (triangle[0] + triangle[2]) / 2.0;
  stream << "<g>" << std::endl;
  // The line
  Line(_a, stop, _style).flushSVG(stream, transform);

  // FIXME : Handle type

  Style style = _style;
  style.lineStyle = SolidStyle;
  switch (_type) {
  case ExtremityType::Plain:
    style.fillColor = style.penColor;
    break;
  case ExtremityType::Closed:
    style.fillColor = Color::White;
    break;
  case ExtremityType::Stick:
    triangle.open();
    style.fillColor = Color::Null;
    break;
  }

  // The arrow

  Polyline(triangle, style).flushSVG(stream, transform);

  //  stream << " <polygon";
  //  stream << " fill=\"" << fillColor().svg() << "\"";
  //  stream << " stroke=\"none\""
  //         << " stroke-width=\"0mm\""
  //         << " style=\"stroke-linecap:butt;stroke-linejoin:miter\"" << fill.svgAlpha(" fill") << penColor().svgAlpha(" stroke") //
  //         << " points=\"";
  //  triangle.flushSVGPoints(stream, transform);
  //  stream << " z\" />" << std::endl;
  stream << "</g>" << std::endl;
}

void Arrow::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  const Point a = transform.map(_a);
  const Point b = transform.map(_b);
  stream << "\\path[-latex," << _style.tikzProperties(transform) << "] (" << a.x << ',' << a.y //
         << ") -- (" << b.x << ',' << b.y << ");" << std::endl;
}

void Arrow::accept(ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Arrow::accept(const ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Arrow::accept(ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

void Arrow::accept(const ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

Shape * Arrow::accept(CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Shape * Arrow::accept(const CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

} // namespace LibBoard
