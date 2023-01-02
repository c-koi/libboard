/* -*- mode: c++ -*- */
/**
 * @file   Style.cpp
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
#include <board/Style.h>
#include <sstream>
#include <board/Transforms.h>

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

Style Style::_defaultStyle{Color(0, 0, 0, 255), Color(nullptr), 1.0, SolidStyle, ButtCap, MiterJoin};

std::stack<Style> Style::_styleStack;

Style::Style()
{
  *this = _defaultStyle;
}

Style::Style(Color penColor, Color fillColor, double lineWidth, LineStyle lineStyle, LineCap lineCap, LineJoin lineJoin) //
    : penColor(penColor), fillColor(fillColor), lineWidth(lineWidth), lineStyle(lineStyle), lineCap(lineCap), lineJoin(lineJoin)
{
}

std::string Style::svgProperties(const TransformSVG & transform) const
{
  static const char * capStrings[3] = {"butt", "round", "square"};
  static const char * joinStrings[3] = {"miter", "round", "bevel"};
  std::stringstream str;
  if (penColor != Color::Null) {
    str << " fill=\"" << fillColor.svg() << '"'  //
        << " stroke=\"" << penColor.svg() << '"' //
        << " stroke-width=\"" << transform.mapWidth(lineWidth) << "mm\""
        << " style=\"stroke-linecap:" << capStrings[lineCap] << ";stroke-linejoin:" << joinStrings[lineJoin];
    if (lineStyle != SolidStyle) {
      str << ";" << xFigDashStylesSVG[lineStyle];
    }
    str << '"' << fillColor.svgAlpha(" fill") << penColor.svgAlpha(" stroke");
  } else {
    str << " fill=\"" << fillColor.svg()
        << '"'
        // 	<< " stroke=\"" << fillColor().svg() << '"'
        // 	<< " stroke-width=\"0.5px\""
        << " stroke=\"none\""
        << " stroke-width=\"0\""
        << " style=\"stroke-linecap:round;stroke-linejoin:round;\"" //
        << fillColor.svgAlpha(" fill") << fillColor.svgAlpha(" stroke");
  }
  return str.str();
}

std::string Style::postscriptProperties(const TransformEPS & transform) const
{
  std::stringstream str;
  str << transform.mapWidth(lineWidth) << " slw ";
  str << lineCap << " slc ";
  str << lineJoin << " slj";
  str << xFigDashStylesPS[lineStyle];
  return str.str();
}

std::string Style::tikzProperties(const TransformTikZ & transform) const
{
  static const char * capStrings[3] = {"" /* initial value "butt" */, "line cap=round,", "line cap=rect,"};
  static const char * joinStrings[3] = {"" /* initial value "miter" */, "line join=round", "line join=bevel"};
  std::stringstream str;
  str << "fill=" << fillColor.tikz() << ',';
  str << "draw=" << penColor.tikz() << ',';
  str << "line width=" << transform.mapWidth(lineWidth) << "mm,";
  str << xFigDashStylesTikZ[lineStyle];
  str << capStrings[lineCap];
  str << joinStrings[lineJoin];
  return str.str();
}

void Style::setDefaultStyle(const Style & style)
{
  _defaultStyle = style;
}

void Style::setDefaultLineWidth(double w)
{
  _defaultStyle.lineWidth = w;
}

void Style::setDefaultPenColor(Color c)
{
  _defaultStyle.penColor = c;
}

void Style::setDefaultFillColor(Color c)
{
  _defaultStyle.fillColor = c;
}

void Style::setDefaultLineStyle(LineStyle lineStyle)
{
  _defaultStyle.lineStyle = lineStyle;
}

void Style::setDefaultLineCap(LineCap lineCap)
{
  _defaultStyle.lineCap = lineCap;
}

void Style::setDefaultLineJoin(LineJoin lineJoin)
{
  _defaultStyle.lineJoin = lineJoin;
}

void Style::push()
{
  _styleStack.push(_defaultStyle);
}

void Style::pop()
{
  if (!_styleStack.empty()) {
    _defaultStyle = _styleStack.top();
    _styleStack.pop();
  }
}

std::ostream & operator<<(std::ostream & out, const Style & style)
{
  static const char * lineCap[] = {"ButtCap", "RoundCap", "SquareCap"};
  static const char * lineJoin[] = {"MiterJoin", "RoundJoin", "BevelJoin"};
  static const char * lineStyle[] = {"SolidStyle", "DashStyle", "DotStyle", "DashDotStyle", "DashDotDotStyle", "DashDotDotDotStyle"};

  out << "Style(pen:" << style.penColor << ", fill:" << style.fillColor << ", linewidth:" << style.lineWidth //
      << ", " << lineCap[style.lineCap]                                                                      //
      << ", " << lineJoin[style.lineJoin]                                                                    //
      << ", " << lineStyle[style.lineStyle] << ")";

  return out;
}

} // namespace LibBoard
