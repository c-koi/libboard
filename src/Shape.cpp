/* -*- mode: c++ -*- */
/**
 * @file   Shape.cpp
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
#include "board/Shape.h"
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
const char * xFigDashStylesPS[] = {
    " [] 0 sd ",                                // SolidStyle
    " [1 1] 0 sd ",                             // DashStyle,
    " [1.5 4.5] 45 sd ",                        // DotStyle
    " [4.5 2.3 1.5 2.3] 0 sd ",                 // DashDotStyle:
    " [4.5 2.0 1.5 1.5 1.5 2.0] 0 sd ",         // DashDotDotStyle:
    " [4.5 1.8 1.5 1.4 1.5 1.4 1.5 1.8 ] 0 sd " // DashDotDotDotStyle
};

const char * xFigDashStylesSVG[] = {
    "",                                                                    // SolidStyle
    "stroke-dasharray:1,1;stroke-dashoffset:0",                            // DashStyle,
    "stroke-dasharray:1.5,4.5;stroke-dashoffset:45",                       // DotStyle
    "stroke-dasharray:4.5,2.3,1.5,2.3;stroke-dashoffset:0",                // DashDotStyle:
    "stroke-dasharray:4.5,2.0,1.5,1.5,1.5,2.0;stroke-dashoffset;0",        // DashDotDotStyle:
    "stroke-dasharray:4.5,1.8,1.5,1.4,1.5,1.4,1.5,1.8;stroke-dashoffset:0" // DashDotDotDotStyle
};

const char * xFigDashStylesTikZ[] = {
    "",                                           // SolidStyle
    "dash pattern=on 1pt off 1pt,",               // DashStyle
    "dotted,",                                    // DotStyle
    "dashdotted,",                                // DashDotStyle
    "dashdotdotted,",                             // DashDotDotStyle
    "dash pattern=on 2pt off 3pt on 4pt off 4pt," // DashDotDotDotStyle
};
}

namespace LibBoard
{

extern const char * XFigPostscriptFontnames[];

bool Shape::_lineWidthScaling = true;
double Shape::_defaultLineWidth = 1.0;
Color Shape::_defaultPenColor = Color::Black;
Color Shape::_defaultFillColor = Color::Null;
Shape::LineStyle Shape::_defaultLineStyle = Shape::SolidStyle;
Shape::LineCap Shape::_defaultLineCap = Shape::ButtCap;
Shape::LineJoin Shape::_defaultLineJoin = Shape::MiterJoin;

bool shapeGreaterDepth(const Shape * s1, const Shape * s2)
{
  return s1->depth() > s2->depth();
}

const std::string Shape::_name("AbstractShape");

const std::string & Shape::name() const
{
  return _name;
}

Point Shape::center(LineWidthFlag lineWidthFlag) const
{
  return boundingBox(lineWidthFlag).center();
}

Shape & Shape::moveCenter(double x, double y, LineWidthFlag lineWidthFlag)
{
  Point c = center(lineWidthFlag);
  translate(x - c.x, y - c.y);
  return *this;
}

Shape & Shape::moveCenter(Point p, LineWidthFlag lineWidthFlag)
{
  Point c = center(lineWidthFlag);
  translate(p.x - c.x, p.y - c.y);
  return *this;
}

Shape & Shape::resize(double width, double height, Shape::LineWidthFlag lineWidthFlag)
{
  Rect bbox = boundingBox(lineWidthFlag);
  scale(width / bbox.width, height / bbox.height);
  return *this;
}

Shape & Shape::scaleToWidth(double w, Shape::LineWidthFlag lineWidthFlag)
{
  double factor = w / boundingBox(lineWidthFlag).width;
  scale(factor);
  return *this;
}

Shape & Shape::scaleToHeight(double h, Shape::LineWidthFlag lineWidthFlag)
{
  double factor = h / boundingBox(lineWidthFlag).height;
  scale(factor);
  return *this;
}

void Shape::enableLineWidthScaling()
{
  _lineWidthScaling = true;
}

void Shape::disableLineWidthScaling()
{
  _lineWidthScaling = false;
}

void Shape::setLineWidthScaling(bool on)
{
  _lineWidthScaling = on;
}

std::string Shape::svgProperties(const TransformSVG & transform) const
{
  static const char * capStrings[3] = {"butt", "round", "square"};
  static const char * joinStrings[3] = {"miter", "round", "bevel"};
  std::stringstream str;
  if (_penColor != Color::Null) {
    str << " fill=\"" << _fillColor.svg() << '"' << " stroke=\"" << _penColor.svg() << '"' << " stroke-width=\"" << transform.mapWidth(_lineWidth) << "mm\""
        << " style=\"stroke-linecap:" << capStrings[_lineCap] << ";stroke-linejoin:" << joinStrings[_lineJoin];
    if (_lineStyle != SolidStyle)
      str << ";" << xFigDashStylesSVG[_lineStyle];
    str << '"' << _fillColor.svgAlpha(" fill") << _penColor.svgAlpha(" stroke");
  } else {
    str << " fill=\"" << _fillColor.svg()
        << '"'
        // 	<< " stroke=\"" << _fillColor.svg() << '"'
        // 	<< " stroke-width=\"0.5px\""
        << " stroke=\"none\""
        << " stroke-width=\"0\""
        << " style=\"stroke-linecap:round;stroke-linejoin:round;" << '"' << _fillColor.svgAlpha(" fill") << _fillColor.svgAlpha(" stroke");
  }
  return str.str();
}

std::string Shape::postscriptProperties(const TransformEPS & transform) const
{
  std::stringstream str;
  str << transform.mapWidth(_lineWidth) << " slw ";
  str << _lineCap << " slc ";
  str << _lineJoin << " slj";
  str << xFigDashStylesPS[_lineStyle];

  return str.str();
}

std::string Shape::tikzProperties(const TransformTikZ & transform) const
{
  static const char * capStrings[3] = {"" /* initial value "butt" */, "line cap=round,", "line cap=rect,"};
  static const char * joinStrings[3] = {"" /* initial value "miter" */, "line join=round", "line join=bevel"};

  std::stringstream str;
  str << "fill=" << _fillColor.tikz() << ',';
  str << "draw=" << _penColor.tikz() << ',';
  str << "line width=" << transform.mapWidth(_lineWidth) << "mm,";
  str << xFigDashStylesTikZ[_lineStyle];
  str << capStrings[_lineCap];
  str << joinStrings[_lineJoin];

  return str.str();
}

void Shape::depth(int d)
{
  _depth = d;
}

void Shape::shiftDepth(int shift)
{
  _depth += shift;
}

void Shape::setDefaultLineWidth(double w)
{
  _defaultLineWidth = w;
}

void Shape::setDefaultPenColor(Color c)
{
  _defaultPenColor = c;
}

void Shape::setDefaultFillColor(Color c)
{
  _defaultFillColor = c;
}

void Shape::setDefaultLineStyle(Shape::LineStyle lineStyle)
{
  _defaultLineStyle = lineStyle;
}

void Shape::setDefaultLineCap(Shape::LineCap lineCap)
{
  _defaultLineCap = lineCap;
}

void Shape::setDefaultLineJoin(Shape::LineJoin lineJoin)
{
  _defaultLineJoin = lineJoin;
}

double Shape::defaultLineWidth()
{
  return _defaultLineWidth;
}

Color Shape::defaultPenColor()
{
  return _defaultPenColor;
}

Color Shape::defaultFillColor()
{
  return _defaultFillColor;
}

Shape::LineStyle Shape::defaultLineStyle()
{
  return _defaultLineStyle;
}

Shape::LineCap Shape::defaultLineCap()
{
  return _defaultLineCap;
}

Shape::LineJoin Shape::defaultLineJoin()
{
  return _defaultLineJoin;
}

void Shape::accept(ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Shape::accept(const ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Shape::accept(ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

void Shape::accept(const ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

} // namespace LibBoard
