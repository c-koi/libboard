/* -*- mode: c++ -*- */
/**
 * @file   Shapes.cpp
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
#include "BoardConfig.h"
#include "board/Shapes.h"
#include "board/Rect.h"
#include "board/Tools.h"
#include "board/PathBoundaries.h"
#include "board/PSFonts.h"
#include "board/Transforms.h"
#include "board/ShapeVisitor.h"
#include <cmath>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>
#include <limits>

#ifndef M_PI
#define M_PI		3.14159265358979323846	/* pi */
#endif

namespace {
const char * xFigDashStylesPS[] = {
  " [] 0 sd ", // SolidStyle
  " [1 1] 0 sd ", //DashStyle,
  " [1.5 4.5] 45 sd ", // DotStyle
  " [4.5 2.3 1.5 2.3] 0 sd ", // DashDotStyle:
  " [4.5 2.0 1.5 1.5 1.5 2.0] 0 sd ", // DashDotDotStyle:
  " [4.5 1.8 1.5 1.4 1.5 1.4 1.5 1.8 ] 0 sd " // DashDotDotDotStyle
};

const char * xFigDashStylesSVG[] = {
  "", // SolidStyle
  "stroke-dasharray:1,1;stroke-dashoffset:0", //DashStyle,
  "stroke-dasharray:1.5,4.5;stroke-dashoffset:45", // DotStyle
  "stroke-dasharray:4.5,2.3,1.5,2.3;stroke-dashoffset:0", // DashDotStyle:
  "stroke-dasharray:4.5,2.0,1.5,1.5,1.5,2.0;stroke-dashoffset;0", // DashDotDotStyle:
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

namespace LibBoard {

extern const char * XFigPostscriptFontnames[];

bool Shape::_lineWidthScaling = true;
double Shape::_defaultLineWidth = 1.0;
Color Shape::_defaultPenColor = Color::Black;
Color Shape::_defaultFillColor = Color::Null;
Shape::LineStyle Shape::_defaultLineStyle = Shape::SolidStyle;
Shape::LineCap Shape::_defaultLineCap = Shape::ButtCap;
Shape::LineJoin Shape::_defaultLineJoin = Shape::MiterJoin;

bool
shapeGreaterDepth( const Shape *s1, const Shape *s2 )
{
  return s1->depth() > s2->depth();
}

const std::string Shape::_name("AbstractShape");

const std::string &
Shape::name() const
{
  return _name;
}

Point
Shape::center(LineWidthFlag lineWidthFlag) const
{
  return boundingBox(lineWidthFlag).center();
}


Shape &
Shape::moveCenter(double x, double y, LineWidthFlag lineWidthFlag)
{
  Point c = center(lineWidthFlag);
  translate(x-c.x,y-c.y);
  return *this;
}

Shape &
Shape::moveCenter(Point p, LineWidthFlag lineWidthFlag)
{
  Point c = center(lineWidthFlag);
  translate(p.x-c.x,p.y-c.y);
  return *this;
}

Shape &
Shape::resize(double width, double height, Shape::LineWidthFlag lineWidthFlag)
{
  Rect bbox = boundingBox(lineWidthFlag);
  scale(width/bbox.width,height/bbox.height);
  return *this;
}

Shape &
Shape::scaleToWidth(double w, Shape::LineWidthFlag lineWidthFlag)
{
  double factor = w / boundingBox(lineWidthFlag).width;
  scale(factor);
  return *this;
}

Shape &
Shape::scaleToHeight(double h, Shape::LineWidthFlag lineWidthFlag)
{
  double factor = h / boundingBox(lineWidthFlag).height;
  scale(factor);
  return *this;
}

void
Shape::enableLineWidthScaling()
{
  _lineWidthScaling = true;
}

void
Shape::disableLineWidthScaling()
{
  _lineWidthScaling = false;
}

void Shape::setLineWidthScaling(bool on)
{
  _lineWidthScaling = on;
}

std::string
Shape::svgProperties( const TransformSVG & transform ) const
{
  static const char * capStrings[3] = { "butt", "round", "square" };
  static const char * joinStrings[3] = { "miter", "round", "bevel" };
  std::stringstream str;
  if ( _penColor != Color::Null ) {
    str << " fill=\"" << _fillColor.svg() << '"'
        << " stroke=\"" << _penColor.svg() << '"'
        << " stroke-width=\"" << transform.mapWidth( _lineWidth ) << "mm\""
        << " style=\"stroke-linecap:" << capStrings[ _lineCap ]
           << ";stroke-linejoin:" << joinStrings[ _lineJoin ];
    if ( _lineStyle != SolidStyle )
      str << ";" << xFigDashStylesSVG[ _lineStyle ];
    str << '"'
        << _fillColor.svgAlpha( " fill" )
        << _penColor.svgAlpha( " stroke" );
  } else  {
    str << " fill=\"" << _fillColor.svg() << '"'
           // 	<< " stroke=\"" << _fillColor.svg() << '"'
           // 	<< " stroke-width=\"0.5px\""
        << " stroke=\"none\""
        << " stroke-width=\"0\""
        << " style=\"stroke-linecap:round;stroke-linejoin:round;"
        << '"'
        << _fillColor.svgAlpha( " fill" )
        << _fillColor.svgAlpha( " stroke" );
  }
  return str.str();
}

std::string
Shape::postscriptProperties( const TransformEPS & transform ) const
{
  std::stringstream str;
  str << transform.mapWidth(_lineWidth) << " slw ";
  str << _lineCap << " slc ";
  str << _lineJoin << " slj";
  str << xFigDashStylesPS[ _lineStyle ];

  return str.str();
}

std::string
Shape::tikzProperties( const TransformTikZ & transform ) const
{
  static const char * capStrings[3] = { "" /* initial value "butt" */, "line cap=round,", "line cap=rect," };
  static const char * joinStrings[3] = { "" /* initial value "miter" */, "line join=round", "line join=bevel" };

  std::stringstream str;
  str << "fill=" << _fillColor.tikz() << ',';
  str << "draw=" << _penColor.tikz() << ',';
  str << "line width=" << transform.mapWidth( _lineWidth ) << "mm,";
  str << xFigDashStylesTikZ[ _lineStyle ];
  str << capStrings[ _lineCap ];
  str << joinStrings[ _lineJoin ];

  return str.str();
}

void
Shape::depth( int d )
{
  _depth = d;
}

void
Shape::shiftDepth( int shift )
{
  _depth += shift;
}

void
Shape::setDefaultLineWidth(double w)
{ _defaultLineWidth = w; }

void
Shape::setDefaultPenColor(Color c)
{  _defaultPenColor = c; }

void
Shape::setDefaultFillColor(Color c)
{ _defaultFillColor = c; }

void
Shape::setDefaultLineStyle( Shape::LineStyle lineStyle )
{ _defaultLineStyle = lineStyle; }

void
Shape::setDefaultLineCap( Shape::LineCap lineCap )
{ _defaultLineCap = lineCap; }

void
Shape::setDefaultLineJoin( Shape::LineJoin lineJoin )
{ _defaultLineJoin = lineJoin; }

double
Shape::defaultLineWidth()
{ return _defaultLineWidth; }

Color
Shape::defaultPenColor()
{ return _defaultPenColor; }

Color
Shape::defaultFillColor()
{ return _defaultFillColor; }

Shape::LineStyle
Shape::defaultLineStyle()
{ return _defaultLineStyle; }

Shape::LineCap
Shape::defaultLineCap()
{ return _defaultLineCap; }

Shape::LineJoin
Shape::defaultLineJoin()
{ return _defaultLineJoin; }

void
Shape::accept(ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void
Shape::accept(const ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

/*
 * Dot
 */

const std::string Dot::_name("Dot");

const std::string &
Dot::name() const
{
  return _name;
}

Point
Dot::center(LineWidthFlag) const {
  return Point( _x, _y );
}

Dot &
Dot::rotate( double angle, const Point & center )
{
  Point( _x, _y ).rotate( angle, center ).get( _x, _y );
  return *this;
}

Dot
Dot::rotated( double angle, const Point & center ) const
{
  return Dot(*this).rotate( angle, center );
}

Dot &
Dot::rotate( double )
{
  return *this;
}

Dot
Dot::rotated( double ) const
{
  return *this;
}

Dot &
Dot::translate( double dx, double dy )
{
  _x += dx;
  _y += dy;
  return *this;
}

Dot
Dot::translated( double dx, double dy ) const
{
  return Dot(*this).translate( dx, dy );
}

Dot &
Dot::scale( double , double )
{
  return *this;
}

Dot &
Dot::scale( double )
{
  return *this;
}

Dot
Dot::scaled( double , double ) const
{
  return *this;
}

Dot
Dot::scaled( double ) const
{
  return *this;
}

void
Dot::scaleAll( double s )
{
  _x *= s;
  _y *= s;
}

void
Dot::flushPostscript( std::ostream & stream,
                      const TransformEPS & transform ) const
{
  stream << "\n% Dot\n";
  stream << postscriptProperties(transform) << " "
         << "n "
         << transform.mapX( _x ) << " "
         << transform.mapY( _y ) << " "
         << "m "
         << transform.mapX( _x ) << " "
         << transform.mapY( _y ) << " "
         << "l " << _penColor.postscript() << " srgb stroke" << std::endl;
}

void
Dot::flushFIG( std::ostream & stream,
               const TransformFIG & transform,
               std::map<Color,int> & colormap ) const
{
  stream << "2 1 0 ";
  // Thickness
  stream << ( _penColor.valid()?transform.mapWidth( _lineWidth ):0 ) << " ";
  // Pen color
  stream << colormap[ _penColor ] << " ";
  // Fill color
  stream << "0 ";
  // Depth
  stream << transform.mapDepth( _depth ) << " ";
  // Pen style
  stream <<  "-1 ";
  // Area fill, style val, join style, cap style, radius, f_arrow, b_arrow
  stream << "-1 0.000 " << _lineJoin << " " << _lineCap << " -1 0 0 ";
  // Number of points
  stream << "2\n";
  stream << "         ";
  stream << static_cast<int>( transform.mapX( _x ) ) << " "
         << static_cast<int>( transform.mapY( _y ) ) << " "
         << static_cast<int>( transform.mapX( _x ) ) << " "
         << static_cast<int>( transform.mapY( _y ) ) << std::endl;
}

void
Dot::flushSVG( std::ostream & stream,
               const TransformSVG & transform ) const
{
  stream << "<line x1=\"" << transform.mapX( _x ) << "\""
         << " y1=\"" << transform.mapY( _y ) << "\""
         << " x2=\"" << transform.mapX( _x ) << "\""
         << " y2=\"" << transform.mapY( _y ) << "\""
         << svgProperties( transform )
         << " />" << std::endl;
}

void
Dot::flushTikZ( std::ostream & stream,
                const TransformTikZ & /*transform*/ ) const

{
  // FIXME: unimplemented
  stream << "% FIXME: Dot::flushTikZ unimplemented" << std::endl;
}

Rect
Dot::boundingBox(LineWidthFlag lineWidthFlag) const
{
  switch (lineWidthFlag) {
  case IgnoreLineWidth:
    return Rect( _x, _y, 0.0, 0.0 );
    break;
  case UseLineWidth:
    return Rect( _x-0.5*_lineWidth, _y+0.5*_lineWidth, _lineWidth, _lineWidth );
    break;
  default:
    Tools::error << "LineWidthFlag incorrect value (" << lineWidthFlag << ")\n";
    return Rect();
    break;
  }
}

Dot *
Dot::clone() const {
  return new Dot(*this);
}

/*
   * Line
   */

const std::string Line::_name("Line");

const std::string &
Line::name() const
{
  return _name;
}

Line &
Line::rotate( double angle, const Point & center )
{
  Point( _x1, _y1 ).rotate( angle, center ).get( _x1, _y1 );
  Point( _x2, _y2 ).rotate( angle, center ).get( _x2, _y2 );
  return *this;
}

Line &
Line::rotate( double angle )
{
  return Line::rotate( angle, center() );
}

Line
Line::rotated( double angle, const Point & center ) const
{
  Line res(*this);
  Point( _x1, _y1 ).rotate( angle, center ).get( res._x1, res._y1 );
  Point( _x2, _y2 ).rotate( angle, center ).get( res._x2, res._y2 );
  return res;
}

Line
Line::rotated( double angle ) const
{
  Line res(*this);
  Point c = center();
  Point( _x1, _y1 ).rotate( angle, c ).get( res._x1, res._y1 );
  Point( _x2, _y2 ).rotate( angle, c ).get( res._x2, res._y2 );
  return res;
}

Line &
Line::translate( double dx, double dy )
{
  _x1 += dx; _x2 += dx;
  _y1 += dy; _y2 += dy;
  return *this;
}

Line
Line::translated( double dx, double dy ) const
{
  Line res(*this);
  res._x1 += dx; res._x2 += dx;
  res._y1 += dy; res._y2 += dy;
  return *this;
}

Line &
Line::scale( double sx, double sy )
{
  Point c = center();
  _x1 *= sx;
  _x2 *= sx;
  _y1 *= sy;
  _y2 *= sy;
  Point delta = c - center();
  translate( delta.x, delta.y );
  updateLineWidth(std::max(sx,sy));
  return *this;
}

Line &
Line::scale( double s )
{
  scale( s, s );
  return (*this);
}

Line
Line::scaled( double sx, double sy ) const
{
  return Line(*this).scale(sx,sy);
}

Line
Line::scaled( double s ) const
{
  return Line::scaled( s , s );
}

void
Line::scaleAll( double s )
{
  _x1 *= s;
  _y1 *= s;
  _x2 *= s;
  _y2 *= s;
}

Line
Line::resized(double w, double h, LineWidthFlag lineWidthFlag) const
{
  return static_cast<Line&>( Line(*this).resize(w,h,lineWidthFlag) );
}

Line *
Line::clone() const {
  return new Line(*this);
}

void
Line::flushPostscript( std::ostream & stream,
                       const TransformEPS & transform ) const
{
  stream << "\n% Line\n";
  stream << postscriptProperties(transform) << " "
         << "n "
         << transform.mapX( _x1 ) << " "
         << transform.mapY( _y1 ) << " "
         << "m "
         << transform.mapX( _x2 ) << " "
         << transform.mapY( _y2 ) << " "
         << "l " << _penColor.postscript() << " srgb stroke" << std::endl;
}

void
Line::flushFIG( std::ostream & stream,
                const TransformFIG & transform,
                std::map<Color,int> & colormap ) const
{
  stream << "2 1 ";
  // Line style
  stream << _lineStyle << " ";
  // Thickness
  stream << ( _penColor.valid()?transform.mapWidth( _lineWidth ):0 ) << " ";
  // Pen color
  stream << colormap[ _penColor ] << " ";
  // Fill color
  stream << "0 ";
  // Depth
  stream << transform.mapDepth( _depth ) << " ";
  // Pen style
  stream <<  "-1 ";
  // Area fill, style val, join style, cap style, radius, f_arrow, b_arrow
  stream << "-1 " << (_lineStyle?"4.000 ":"0.000 ") << _lineJoin << " " << _lineCap << " -1 0 0 ";
  // Number of points
  stream << "2\n";
  stream << "         ";
  stream << static_cast<int>( transform.mapX( _x1 ) ) << " "
         << static_cast<int>( transform.mapY( _y1 ) ) << " "
         << static_cast<int>( transform.mapX( _x2 ) ) << " "
         << static_cast<int>( transform.mapY( _y2 ) ) << std::endl;
}

void
Line::flushSVG( std::ostream & stream,
                const TransformSVG & transform ) const
{
  stream << "<line x1=\"" << transform.mapX( _x1 ) << "\""
         << " y1=\"" << transform.mapY( _y1 ) << "\""
         << " x2=\"" << transform.mapX( _x2 ) << "\""
         << " y2=\"" << transform.mapY( _y2 ) << "\""
         << svgProperties( transform )
         << " />" << std::endl;
}

void
Line::flushTikZ( std::ostream & stream,
                 const TransformTikZ & transform ) const
{
  stream << "\\path[" << tikzProperties(transform) << "] ("
         << transform.mapX( _x1 ) << ',' << transform.mapY( _y1 )
         << ") -- ("
         << transform.mapX( _x2 ) << ',' << transform.mapY( _y2 )
         << ");" << std::endl;
}

Rect
Line::boundingBox(LineWidthFlag lineWidthFlag) const
{
  Path p;
  p << Point(_x1,_y1) << Point(_x2,_y2);
  switch (lineWidthFlag) {
  case UseLineWidth:
    return Tools::pathBoundingBox(p,_lineWidth,_lineCap,_lineJoin);
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

/*
 * Arrow
 */

const std::string Arrow::_name("Arrow");

const std::string &
Arrow::name() const
{
  return _name;
}

Arrow
Arrow::rotated( double angle, const Point & center ) const
{
  Arrow res(*this);
  Point( _x1, _y1 ).rotate( angle, center ).get( res._x1, res._y1 );
  Point( _x2, _y2 ).rotate( angle, center ).get( res._x2, res._y2 );
  return res;
}

Arrow
Arrow::rotated( double angle ) const
{
  Arrow res(*this);
  Point c = center();
  Point( _x1, _y1 ).rotate( angle, c ).get( res._x1, res._y1 );
  Point( _x2, _y2 ).rotate( angle, c ).get( res._x2, res._y2 );
  return res;
}

Arrow
Arrow::translated( double dx, double dy ) const
{
  return static_cast<Arrow&>(Arrow(*this).translate(dx,dy));
}

Arrow
Arrow::scaled( double sx, double sy ) const
{
  return static_cast<Arrow&>(Arrow(*this).scale(sx,sy));
}

Arrow
Arrow::scaled( double s ) const
{
  return Arrow::scaled( s, s );
}

Arrow
Arrow::resized(double w, double h, Shape::LineWidthFlag lineWidthFlag) const
{
  return static_cast<Arrow&>(Arrow(*this).resize(w,h,lineWidthFlag));
}

Rect
Arrow::boundingBox(Shape::LineWidthFlag) const
{
  double dx = _x1 - _x2;
  double dy = _y1 - _y2;
  double norm = sqrt( dx*dx + dy*dy );
  dx /= norm;
  dy /= norm;
  dx *= 10*_lineWidth;
  dy *= 10*_lineWidth;

  double ndx1 = dx*cos(0.3)-dy*sin(0.3);
  double ndy1 = dx*sin(0.3)+dy*cos(0.3);
  double ndx2 = dx*cos(-0.3)-dy*sin(-0.3);
  double ndy2 = dx*sin(-0.3)+dy*cos(-0.3);

  Path pLine;
  pLine << Point(_x1,_y1);
  pLine << Point(_x2+(dx*cos(0.3)), _y2+(dy*cos(0.3)));

  Path pArrow;
  pArrow << Point(_x2+ndx1,_y2+ndy1)
         << Point(_x2,_y2 )
         << Point(_x2+ndx2,_y2+ndy2);

  return Tools::pathBoundingBox(pLine,_lineWidth,_lineCap,_lineJoin) || pArrow.boundingBox();
}

Arrow *
Arrow::clone() const {
  return new Arrow(*this);
}

void
Arrow::flushPostscript( std::ostream & stream,
                        const TransformEPS & transform ) const
{
  double dx = _x1 - _x2;
  double dy = _y1 - _y2;
  double norm = sqrt( dx*dx + dy*dy );
  dx /= norm;
  dy /= norm;
  dx *= 10*_lineWidth;
  dy *= 10*_lineWidth;
  double ndx1 = dx*cos(0.3)-dy*sin(0.3);
  double ndy1 = dx*sin(0.3)+dy*cos(0.3);
  double ndx2 = dx*cos(-0.3)-dy*sin(-0.3);
  double ndy2 = dx*sin(-0.3)+dy*cos(-0.3);

  stream << "\n% Arrow\n";
  stream << _penColor.postscript() << " srgb "
         << postscriptProperties(transform) << " "
         << "n "
         << transform.mapX( _x1 ) << " "
         << transform.mapY( _y1 ) << " "
         << "m "
         << transform.mapX( _x2 + ( dx * cos(0.3) ) ) << " "
         << transform.mapY( _y2 + ( dy * cos(0.3) ) ) << " "
         << "l stroke" << std::endl;

  if ( filled() ) {
    stream << "n "
           << transform.mapX( _x2 ) + transform.scale( ndx1 ) << " "
           << transform.mapY( _y2 ) + transform.scale( ndy1 ) << " "
           << "m "
           << transform.mapX( _x2 ) << " "
           << transform.mapY( _y2 ) << " l "
           << transform.mapX( _x2 ) + transform.scale( ndx2 ) << " "
           << transform.mapY( _y2 ) + transform.scale( ndy2 ) << " ";
    stream  << "l cp " << _fillColor.postscript() << " srgb  fill" << std::endl;
  }
}

void
Arrow::flushFIG( std::ostream & stream,
                 const TransformFIG & transform,
                 std::map<Color,int> & colormap ) const
{
  stream << "2 1 ";
  // Line style
  stream << _lineStyle << " ";
  // Thickness
  stream << ( _penColor.valid()?transform.mapWidth( _lineWidth ):0 ) << " ";
  // Pen color
  stream << colormap[ _penColor ] << " ";
  // Fill color
  stream << colormap[ _fillColor ] << " ";
  // Depth
  stream << transform.mapDepth( _depth ) << " ";
  // Pen style
  stream <<  "-1 ";
  // Area fill, style val, join style, cap style, radius, f_arrow, b_arrow
  stream << "-1 " << (_lineStyle?"4.000 ":"0.000 ") << _lineJoin << " " << _lineCap << " -1 1 0 ";
  // Number of points
  stream << "2\n";
  if ( filled() )
    stream << "       1 1 1.00 60.00 120.00\n";
  else
    stream << "       1 0 1.00 60.00 120.00\n";
  stream << "         ";
  stream << static_cast<int>( transform.mapX( _x1 ) ) << " "
         << static_cast<int>( transform.mapY( _y1 ) ) << " "
         << static_cast<int>( transform.mapX( _x2 ) ) << " "
         << static_cast<int>( transform.mapY( _y2 ) ) << std::endl;
}

void
Arrow::flushSVG( std::ostream & stream,
                 const TransformSVG & transform ) const
{
  double dx = _x1 - _x2;
  double dy = _y1 - _y2;
  double norm = sqrt( dx*dx + dy*dy );
  dx /= norm;
  dy /= norm;
  dx *= 10 * _lineWidth;
  dy *= 10 * _lineWidth;
  double ndx1 = dx*cos(0.3)-dy*sin(0.3);
  double ndy1 = dx*sin(0.3)+dy*cos(0.3);
  double ndx2 = dx*cos(-0.3)-dy*sin(-0.3);
  double ndy2 = dx*sin(-0.3)+dy*cos(-0.3);

  stream << "<g>" << std::endl;
  // The line
  stream << " <path "
         << "d=\"M " << transform.mapX( _x1 ) << " " << transform.mapY( _y1 )
         << " L " << transform.mapX( _x2 + ( dx * cos(0.3) ) )
         << " " << transform.mapY( _y2 + ( dy * cos(0.3) ) ) << " z\""
         << " fill=\"none\" stroke=\"" << _penColor.svg() << "\""
         << _penColor.svgAlpha( " stroke" );

  if ( _lineStyle != SolidStyle ) {
    stream << " style=\"" <<   xFigDashStylesSVG[ _lineStyle ] << '"';
  }
  stream << " stroke-width=\"" << transform.mapWidth( _lineWidth ) << "mm\" />";

  // The arrow
  stream << " <polygon";
  stream << " fill=\"" << _fillColor.svg() << "\"";
  stream << " stroke=\"none\""
         << " stroke-width=\"0mm\""
         << " style=\"stroke-linecap:butt;stroke-linejoin:miter\""
         << _fillColor.svgAlpha( " fill" )
         << _penColor.svgAlpha( " stroke" )
         << " points=\""
         << transform.mapX( _x2 ) + transform.scale( ndx1 ) << ","
         << transform.mapY( _y2 ) - transform.scale( ndy1 ) << " "
         << transform.mapX( _x2 ) << ","
         << transform.mapY( _y2 ) << " "
         << transform.mapX( _x2 ) + transform.scale( ndx2 ) << ","
         << transform.mapY( _y2 ) - transform.scale( ndy2 ) << " "
         << transform.mapX( _x2 ) + transform.scale( ndx1 ) << ","
         << transform.mapY( _y2 ) - transform.scale( ndy1 ) << "\" />" << std::endl;
  stream << "</g>" << std::endl;
}

void
Arrow::flushTikZ( std::ostream & stream,
                  const TransformTikZ & transform ) const
{
  stream << "\\path[-latex," << tikzProperties(transform) << "] ("
         << transform.mapX( _x1 ) << ',' << transform.mapY( _y1 )
         << ") -- ("
         << transform.mapX( _x2 ) << ',' << transform.mapY( _y2 )
         << ");" << std::endl;
}


/*
   * Ellipse
   */

const std::string Ellipse::_name("Ellipse");

const std::string &
Ellipse::name() const
{
  return _name;
}

Point
Ellipse::center(LineWidthFlag ) const {
  return _center;
}

Ellipse &
Ellipse::rotate( double angle, const Point & center )
{
  Point c( _center );
  Point e = (c + Point( _xRadius, 0 )).rotate( _angle, c );
  Point rc = c.rotated( angle, center );
  Point re = e.rotated( angle, center );
  Point axis = re - rc;
  _angle = atan( axis.y / axis.x );
  _center = rc;
  return *this;
}

Ellipse
Ellipse::rotated( double angle, const Point & center ) const
{
  return Ellipse(*this).rotate( angle, center );
}

Ellipse &
Ellipse::rotate( double angle )
{
  return Ellipse::rotate( angle, center() );
}

Ellipse
Ellipse::rotated( double angle ) const
{
  return Ellipse(*this).rotate( angle, center() );
}

Ellipse &
Ellipse::translate( double dx, double dy )
{
  _center += Point( dx, dy );
  return *this;
}

Ellipse
Ellipse::translated( double dx, double dy ) const
{
  return Ellipse(*this).translate( dx, dy );
}

Ellipse &
Ellipse::scale( double sx, double sy )
{
  // Thanks to Freddie Exall for pointing an error with the first version
  // of this function, and for pointing to a fix as well!
  if ( _angle != 0 ) {
    double co = cos( _angle );
    double si = sin( _angle );

    // current transformation matrix
    double m00 = ( 1 / _xRadius ) * co;
    double m01 = ( 1 / _xRadius ) * si;
    double m10 = - ( 1 / _yRadius ) * si;
    double m11 = ( 1 / _yRadius ) * co;

    // Implicit function of ellipse at current
    // ax^2 + bxy + cy^2 = 1
    double a = ( m00 * m00 ) + ( m10 * m10 );
    double b = 2 * ( ( m00 * m01 ) + ( m10 * m11 ) );
    double c = ( m01 * m01 ) + ( m11 * m11 );

    // Scale coefficients ( x_new = sx * x, y_new = sy * y )
    a = a / ( sx * sx );
    b = b / ( sx * sy );
    c = c / ( sy * sy );

    if ( b == 0 ) {
      _angle = 0;
    } else if ( a == c ) {
      _angle = M_PI / 4;
      a += ( b / 2 );
      c -= ( b / 2 );
    } else {
      _angle = 0.5 * atan( b / ( a - c ) );
      double k = 1 + ( ( b * b ) / ( ( a - c ) * ( a - c ) ) );
      k = sqrt( k );
      k *= ( a - c );
      c += a;
      a = 0.5 * ( c + k );
      c = 0.5 * ( c - k );
    }
    _xRadius = 1 / sqrt( a );
    _yRadius = 1 / sqrt( c );
  } else {
    _xRadius = _xRadius * sx;
    _yRadius = _yRadius * sy;
  }
  updateLineWidth(std::max(sx,sy));
  return *this;
}

Ellipse &
Ellipse::scale( double s )
{
  return Ellipse::scale( s, s );
}

Ellipse
Ellipse::scaled( double sx, double sy ) const
{
  return Ellipse(*this).scale( sx, sy );
}

Ellipse
Ellipse::scaled( double s ) const
{
  return Ellipse(*this).scale( s, s );
}

void
Ellipse::scaleAll( double s )
{
  _xRadius *= s;
  _yRadius *= s;
  _center *= s;
}

Ellipse
Ellipse::resized(double w, double h, Shape::LineWidthFlag lineWidthFlag) const
{
  return static_cast<Ellipse&>(Ellipse(*this).resize(w,h,lineWidthFlag));
}

Ellipse *
Ellipse::clone() const {
  return new Ellipse(*this);
}

void
Ellipse::flushPostscript( std::ostream & stream,
                          const TransformEPS & transform ) const
{
  double yScale = _yRadius / _xRadius;
  stream << "\n% Ellipse\n";
  if ( filled() ) {
    stream << "gs "
           << transform.mapX( _center.x ) << " " << transform.mapY( _center.y ) << " tr";
    if ( _angle != 0.0 ) stream << " " << (_angle*180/M_PI) << " rot ";
    if ( ! _circle ) stream << " " << 1.0 << " " << yScale << " sc";
    stream << " n " << transform.scale( _xRadius ) << " 0 m "
           << " 0 0 " << transform.scale( _xRadius ) << " 0.0 360.0 arc ";
    stream << " " << _fillColor.postscript() << " srgb";
    stream << " fill gr" << std::endl;
  }

  if ( _penColor != Color::Null && _lineWidth != 0.0 ) {
    stream << postscriptProperties(transform) << "\n";
    stream << "gs " << transform.mapX( _center.x ) << " " << transform.mapY( _center.y ) << " tr";
    if ( _angle != 0.0 ) stream << " " << (_angle*180/M_PI) << " rot ";
    if ( ! _circle ) stream << " " << 1.0 << " " << yScale << " sc";
    stream << " n " << transform.scale( _xRadius ) << " 0 m "
           << " 0 0 " << transform.scale( _xRadius ) << " 0.0 360.0 arc ";
    stream << " " << _penColor.postscript() << " srgb";
    stream << " stroke gr" << std::endl;
  }
}

void
Ellipse::flushFIG( std::ostream & stream,
                   const TransformFIG & transform,
                   std::map<Color,int> & colormap ) const
{
  // Ellipse, Sub type, Line style, Thickness
  if ( _circle )
    stream << "1 3 " << _lineStyle << " ";
  else
    stream << "1 1 " << _lineStyle << " ";
  stream << ( _penColor.valid()?transform.mapWidth( _lineWidth ):0 ) << " ";
  // Pen color, Fill color
  stream << colormap[ _penColor ] << " " << colormap[ _fillColor ] << " ";
  // Depth, Pen style, Area fill, Style val, Direction, angle
  if ( filled() )
    stream << transform.mapDepth( _depth ) << " -1 20 " << (_lineStyle?"4.000 ":"0.000 ") << "  1 " << _angle << " ";
  else
    stream << transform.mapDepth( _depth ) << " -1 -1 " << (_lineStyle?"4.000 ":"0.000 ") << " 1 " << _angle << " ";
  stream << static_cast<int>( transform.mapX( _center.x ) ) << " "
         << static_cast<int>( transform.mapY( _center.y ) ) << " "
         << static_cast<int>( transform.scale( _xRadius ) ) << " "
         << static_cast<int>( transform.scale( _yRadius ) ) << " "
         << static_cast<int>( transform.mapX( _center.x ) ) << " "
         << static_cast<int>( transform.mapY( _center.y ) ) << " "
         << static_cast<int>( transform.mapX( _center.x ) + transform.scale( _xRadius ) ) << " "
         << static_cast<int>( transform.mapY( _center.y ) ) << "\n";
}

void
Ellipse::flushSVG( std::ostream & stream,
                   const TransformSVG & transform ) const
{
  stream << "<ellipse cx=\"" << transform.mapX( _center.x ) << '"'
         << " cy=\"" << transform.mapY( _center.y ) << '"'
         << " rx=\"" << transform.scale( _xRadius ) << '"'
         << " ry=\"" << transform.scale( _yRadius ) << '"'
         << svgProperties( transform ) ;
  if ( _angle != 0.0 ) {
    stream << " transform=\"rotate( "
           << -(_angle*180/M_PI) << ", "
           << transform.mapX( _center.x ) << ", "
           << transform.mapY( _center.y ) << " )\" ";
  }
  stream << " />" << std::endl;
}

void
Ellipse::flushTikZ( std::ostream & stream,
                    const TransformTikZ & transform ) const
{
  // FIXME: unimplemented
  stream << "% FIXME: Ellipse::flushTikZ unimplemented" << std::endl;
  stream << "\\path[" << tikzProperties(transform) << "] ("
         << transform.mapX( _center.x ) << ','
         << transform.mapY( _center.y ) << ')'
         << " circle [x radius=" << transform.scale( _xRadius ) << ','
         <<          "y radius=" << transform.scale( _yRadius ) << ','
                  <<          "rotate=" << -(_angle*180/M_PI)
                           << "];"
                           << std::endl;
}

Rect
Ellipse::boundingBox( LineWidthFlag lineWidthFlag ) const
{
  Rect box;
  if ( _angle == 0.0 ) {
    box = Rect( _center.x - _xRadius, _center.y + _yRadius, 2 * _xRadius, 2 * _yRadius );
  } else {
    double angleXmax = -atan( (_yRadius/_xRadius)*(tan(_angle) ) );
    double angleXmin = -atan( (_yRadius/_xRadius)*(tan(_angle) ) ) + M_PI;
    double angleYmax =  atan( (_yRadius/_xRadius)*(1/tan(_angle) ) );
    double angleYmin =  M_PI + atan( (_yRadius/_xRadius)*(1/tan(_angle) ) );
    if ( _angle < 0.0 ) {
      angleYmax += M_PI;
      angleYmin -= M_PI;
    }
    box = Rect( _center.x + _xRadius*cos(angleXmin)*cos(_angle) - _yRadius*sin(angleXmin)*sin(_angle),
                _center.y + _xRadius*cos(angleYmax)*sin(_angle) + _yRadius*sin(angleYmax)*cos(_angle),
                ( _xRadius*cos(angleXmax)*cos(_angle) - _yRadius*sin(angleXmax)*sin(_angle) ) -
                ( _xRadius*cos(angleXmin)*cos(_angle) - _yRadius*sin(angleXmin)*sin(_angle) ),
                ( _xRadius*cos(angleYmax)*sin(_angle) + _yRadius*sin(angleYmax)*cos(_angle) ) -
                ( _xRadius*cos(angleYmin)*sin(_angle) + _yRadius*sin(angleYmin)*cos(_angle) ) );
  }
  if ( lineWidthFlag == UseLineWidth ) {
    box.grow(0.5*_lineWidth);
  }
  return box;
}

/*
   * Circle
   */

const std::string Circle::_name("Circle");

const std::string &
Circle::name() const
{
  return _name;
}

Point
Circle::center(LineWidthFlag ) const {
  return _center;
}

Circle &
Circle::rotate( double angle, const Point & center )
{
  if ( _circle ) {
    if ( center == _center ) return *this;
    _center.rotate( angle, center );
    return *this;
  }
  Ellipse::rotate( angle, center );
  return *this;
}

Circle
Circle::rotated( double angle, const Point & center ) const
{
  return Circle(*this).rotate( angle, center );
}

Circle &
Circle::rotate( double angle )
{
  if ( !_circle )
    Ellipse::rotate( angle );
  return *this;
}

Circle
Circle::rotated( double angle ) const
{
  return Circle(*this).rotate( angle );
}

Circle &
Circle::translate( double dx, double dy )
{
  _center += Point( dx, dy );
  return *this;
}

Circle
Circle::translated( double dx, double dy ) const
{
  return Circle(*this).translate( dx, dy );
}

Circle &
Circle::scale( double sx, double sy )
{
  _circle = false;
  Ellipse::scale( sx, sy );
  return *this;
}

Circle &
Circle::scale( double s )
{
  Ellipse::scale( s );
  return *this;
}

Circle
Circle::scaled( double sx, double sy ) const
{
  return Circle(*this).scale( sx, sy );
}

Circle
Circle::scaled( double s ) const
{
  return Circle(*this).scale( s, s );
}

Circle
Circle::resized(double w, double h, Shape::LineWidthFlag lineWidthFlag) const
{
  return static_cast<Circle&>(Circle(*this).resize(w,h,lineWidthFlag));
}

void
Circle::scaleAll( double s )
{
  _center *= s;
  _xRadius *= s;
  _yRadius *= s;
}

Circle *
Circle::clone() const {
  return new Circle(*this);
}

void
Circle::flushSVG( std::ostream & stream,
                  const TransformSVG & transform ) const
{
  if ( ! _circle ) {
    Ellipse::flushSVG( stream, transform );
  } else {
    stream << "<circle cx=\"" << transform.mapX( _center.x ) << '"'
           << " cy=\"" << transform.mapY( _center.y ) << '"'
           << " r=\"" << transform.scale( _xRadius ) << '"'
           << svgProperties( transform )
           << " />" << std::endl;
  }
}

void
Circle::flushTikZ( std::ostream & stream,
                   const TransformTikZ & transform ) const
{
  if ( ! _circle ) {
    Ellipse::flushTikZ( stream, transform );
  } else {
    stream << "\\path[" << tikzProperties(transform) << "] ("
           << transform.mapX( _center.x ) << ','
           << transform.mapY( _center.y ) << ')'
           << " circle (" << transform.scale( _xRadius ) << ");"
           << std::endl;
  }
}

/*
 * Polyline
 */

const std::string Polyline::_name("Polyline");

const std::string &
Polyline::name() const
{
  return _name;
}

Polyline &
Polyline::operator<<( const Point & p )
{
  _path << p;
  return *this;
}

Polyline &
Polyline::rotate( double angle, const Point & center )
{
  _path.rotate( angle, center );
  return *this;
}

Polyline
Polyline::rotated( double angle, const Point & center ) const
{
  return Polyline(*this).rotate( angle, center );
}

Polyline &
Polyline::rotate( double angle )
{
  _path.rotate( angle, center() );
  return *this;
}

Polyline
Polyline::rotated( double angle ) const
{
  return Polyline(*this).rotate( angle, center() );
}

Polyline &
Polyline::translate( double dx, double dy )
{
  _path.translate( dx, dy );
  return *this;
}

Polyline
Polyline::translated( double dx, double dy ) const
{
  return Polyline(*this).translate( dx, dy );
}

Polyline &
Polyline::scale( double sx, double sy )
{
  _path.scale( sx, sy );
  updateLineWidth(std::max(sx,sy));
  return *this;
}

Polyline &
Polyline::scale( double s )
{
  Polyline::scale( s, s );
  return *this;
}

Polyline
Polyline::scaled( double sx, double sy ) const
{
  return Polyline(*this).scale( sx, sy );
}

Polyline
Polyline::scaled( double s) const
{
  return Polyline(*this).scale( s, s );
}

void
Polyline::scaleAll( double s )
{
  _path.scaleAll( s );
}

Polyline
Polyline::resized(double w, double h, Shape::LineWidthFlag lineWidthFlag) const
{
  return static_cast<Polyline&>( Polyline(*this).resize(w,h,lineWidthFlag));
}

Polyline *
Polyline::clone() const {
  return new Polyline(*this);
}

void
Polyline::flushPostscript( std::ostream & stream,
                           const TransformEPS & transform ) const
{
  if ( _path.empty() ) return;
  stream << "\n% Polyline\n";
  if ( filled() ) {
    stream << "n ";
    _path.flushPostscript( stream, transform );
    stream << " ";
    _fillColor.flushPostscript( stream );
    stream << " " << postscriptProperties(transform);
    stream << " fill" << std::endl;
  }
  if ( _penColor != Color::Null && _lineWidth != 0.0 ) {
    stream << " " << postscriptProperties(transform) << "\n";
    stream << "n ";
    _path.flushPostscript( stream, transform );
    stream << " ";
    _penColor.flushPostscript( stream );
    stream << " stroke" << std::endl;
  }
}

void
Polyline::flushFIG( std::ostream & stream,
                    const TransformFIG & transform,
                    std::map<Color,int> & colormap ) const
{
  if ( _path.empty() )
    return;
  if ( _path.closed() )
    stream << "2 3 " << _lineStyle << " ";
  else
    stream << "2 1 " << _lineStyle << " ";
  // Thickness
  stream << ( _penColor.valid()?transform.mapWidth( _lineWidth ):0 ) << " ";
  // Pen color
  stream << colormap[ _penColor ] << " ";
  // Fill color
  stream << colormap[ _fillColor ] << " ";
  // Depth
  stream << transform.mapDepth( _depth ) << " ";
  // Pen style
  stream <<  "-1 ";
  // Area fill, style val, join style, cap style, radius, f_arrow, b_arrow
  if ( filled() )
    stream << "20 " << (_lineStyle?"4.000 ":"0.000 ") << _lineJoin << " " << _lineCap << " -1 0 0 ";
  else
    stream << "-1 " << (_lineStyle?"4.000 ":"0.000 ")  << _lineJoin << " " << _lineCap << " -1 0 0 ";
  // Number of points
  stream << _path.size() + _path.closed() << std::endl;
  _path.flushFIG( stream << "         ", transform );
  stream << std::endl;
}

void
Polyline::flushSVG( std::ostream & stream,
                    const TransformSVG & transform ) const
{
  if ( _path.empty() )
    return;
  if ( _path.closed() )
    stream << "<polygon";
  else
    stream << "<polyline";
  stream << svgProperties( transform ) << std::endl;
  stream << "          points=\"";
  _path.flushSVGPoints( stream, transform );
  stream << "\" />" << std::endl;
}

void
Polyline::flushTikZ( std::ostream & stream,
                     const TransformTikZ & transform ) const
{
  if ( _path.empty() )
    return;

  stream << "\\path[" << tikzProperties(transform) << "] ";
  _path.flushTikZPoints( stream, transform );
  if ( _path.closed() )
    stream << " -- cycle";
  stream << ";" << std::endl;
}

Rect
Polyline::boundingBox(LineWidthFlag lineWidthFlag) const
{
  switch (lineWidthFlag) {
  case UseLineWidth:
    return Tools::pathBoundingBox(_path,_lineWidth,_lineCap,_lineJoin);
    break;
  case IgnoreLineWidth:
    return _path.boundingBox();
    break;
  default:
    Tools::error << "LineWidthFlag incorrect value (" << lineWidthFlag << ")\n";
    return Rect();
    break;
  }
}

/*
 * Rectangle
 */

const std::string Rectangle::_name("Rectangle");

const std::string &
Rectangle::name() const
{
  return _name;
}

Rectangle
Rectangle::rotated( double angle, const Point & center ) const
{
  return static_cast<Rectangle &>( Rectangle(*this).rotate( angle, center ) );
}

Rectangle
Rectangle::rotated( double angle ) const
{
  return static_cast<Rectangle &>( Rectangle(*this).rotate( angle, center() ) );
}

Rectangle
Rectangle::translated( double dx, double dy ) const
{
  return static_cast<Rectangle &>( Rectangle(*this).translate( dx, dy ) );
}

Rectangle
Rectangle::scaled( double sx, double sy ) const
{
  return static_cast<Rectangle &>( Rectangle(*this).scale( sx, sy ) );
}

Rectangle
Rectangle::scaled( double s ) const
{
  return static_cast<Rectangle &>( Rectangle(*this).scale( s, s ) );
}

void
Rectangle::scaleAll( double s )
{
  _path.scaleAll( s );
}

Rectangle
Rectangle::resized(double w, double h, Shape::LineWidthFlag lineWidthFlag) const
{
  return static_cast<Rectangle&>(Rectangle(*this).resize(w,h,lineWidthFlag));
}

Rectangle *
Rectangle::clone() const {
  return new Rectangle(*this);
}

void
Rectangle::flushFIG( std::ostream & stream,
                     const TransformFIG & transform,
                     std::map<Color,int> & colormap ) const
{
  if ( _path[0].y != _path[1].y ) {
    Polyline::flushFIG( stream, transform, colormap );
    return;
  }
  if ( _path[0].x != _path[3].x ) {
    Polyline::flushFIG( stream, transform, colormap );
    return;
  }
  {
    double x1 = _path[1].x - _path[0].x;
    double y1 = _path[1].y - _path[0].y;
    double x2 = _path[3].x - _path[0].x;
    double y2 = _path[3].y - _path[0].y;
    if ( fabs(x1*x2 + y1*y2) > 0.01 ) {
      Polyline::flushFIG( stream, transform, colormap );
      return;
    }
  }
  stream << "2 2 " << _lineStyle << " ";
  // Thickness
  stream << ( _penColor.valid()?transform.mapWidth( _lineWidth ):0 ) << " ";
  // Pen color
  stream << colormap[ _penColor ] << " ";
  // Fill color
  stream << colormap[ _fillColor ] << " ";
  // Depth
  stream << transform.mapDepth( _depth ) << " ";
  // Pen style
  stream <<  "-1 ";
  // Area fill, style val, join style, cap style, radius, f_arrow, b_arrow, number of points
  if ( filled() )
    stream << "20 " << (_lineStyle?"4.000 ":"0.000 ") << _lineJoin << " " << _lineCap << " -1 0 0 5\n";
  else
    stream << "-1 " << (_lineStyle?"4.000 ":"0.000 ") << _lineJoin << " " << _lineCap << " -1 0 0 5\n";
  stream << "         ";
  _path.flushFIG( stream, transform );
  stream << std::endl;
}

void
Rectangle::flushSVG( std::ostream & stream,
                     const TransformSVG & transform ) const
{
  {
    double x1 = _path[1].x - _path[0].x;
    double y1 = _path[1].y - _path[0].y;
    double x2 = _path[3].x - _path[0].x;
    double y2 = _path[3].y - _path[0].y;
    if ( fabs(x1*x2 + y1*y2) > 0.001  ) {
      Polyline::flushSVG( stream, transform );
      return;
    }
  }

  if ( _path[0].y == _path[1].y ) {
    stream << "<rect x=\""
           << transform.mapX( _path[0].x )
        << '"'
        << " y=\"" << transform.mapY( _path[0].y )
        << '"'
        << " width=\"" << transform.scale( _path[1].x - _path[0].x )
        << '"'
        << " height=\"" << transform.scale( _path[0].y - _path[3].y )
        << '"'
        << svgProperties( transform )
        << " />" << std::endl;
  } else {
    Point v = _path[1] - _path[0];
    v /= v.norm();
    double angle = ( _path[1].y > _path[0].y ) ? acos( v * Point(1,0) ) : -acos( v * Point( 1, 0 ) );
    angle = ( angle * 180 ) / M_PI;
    stream << "<rect x=\""
           << transform.mapX( _path[0].x )
        << '"'
        << " y=\""
        << transform.mapY( _path[0].y )
        << '"'
        << " width=\""
        << transform.scale( (_path[1] - _path[0]).norm() )
        << '"'
        << " height=\""
        << transform.scale( (_path[0] - _path[3]).norm() )
        << '"'
        << svgProperties( transform )
        << ' '
        << " transform=\"rotate(" << -angle << ", "
        << transform.mapX( _path[0].x )
        << ", "
        << transform.mapY( _path[0].y )
        << ") \" "
        << " />"
        << std::endl;
  }
}

void
Rectangle::flushTikZ( std::ostream & stream,
                      const TransformTikZ & transform ) const
{
  Polyline::flushTikZ( stream, transform );
  return;
}

/*
 * GouraudTriangle
 */

const std::string GouraudTriangle::_name("GouraudTriangle");

const std::string &
GouraudTriangle::name() const
{
  return _name;
}

GouraudTriangle::GouraudTriangle( const Point & p0, const Color & color0,
                                  const Point & p1, const Color & color1,
                                  const Point & p2, const Color & color2,
                                  int subdivisions,
                                  int depth )
  : Polyline( std::vector<Point>(), true, Color::Null, Color::Null,
              0.0f, SolidStyle, ButtCap, MiterJoin, depth ),
    _color0( color0 ), _color1( color1 ), _color2( color2 ), _subdivisions( subdivisions ) {
  _path << p0;
  _path << p1;
  _path << p2;

  Shape::_fillColor.red( ( color0.red() + color1.red() + color2.red() ) / 3 );
  Shape::_fillColor.green( ( color0.green() + color1.green() + color2.green() ) / 3 );
  Shape::_fillColor.blue( ( color0.blue() + color1.blue() + color2.blue() ) / 3 );
}

GouraudTriangle::GouraudTriangle( const Point & p0, float brightness0,
                                  const Point & p1, float brightness1,
                                  const Point & p2, float brightness2,
                                  const Color & _fillColor,
                                  int subdivisions,
                                  int depth )
  : Polyline( std::vector<Point>(), true, Color::Null, Color::Null,
              0.0f, SolidStyle, ButtCap, MiterJoin, depth ),
    _color0( _fillColor ), _color1( _fillColor ), _color2( _fillColor ), _subdivisions( subdivisions )
{
  _path << p0;
  _path << p1;
  _path << p2;
  _color0.red( static_cast<unsigned char>( std::min( 255.0f, _color0.red() * brightness0 ) ) );
  _color0.green( static_cast<unsigned char>( std::min( 255.0f, _color0.green() * brightness0 ) ) );
  _color0.blue( static_cast<unsigned char>( std::min( 255.0f, _color0.blue() * brightness0 ) ) );
  _color1.red( static_cast<unsigned char>( std::min( 255.0f, _color1.red() * brightness1 ) ) );
  _color1.green( static_cast<unsigned char>( std::min( 255.0f, _color1.green() * brightness1 ) ) );
  _color1.blue( static_cast<unsigned char>( std::min( 255.0f, _color1.blue() * brightness1 ) ) );
  _color2.red( static_cast<unsigned char>( std::min( 255.0f, _color2.red() * brightness2 ) ) );
  _color2.green( static_cast<unsigned char>( std::min( 255.0f, _color2.green() * brightness2 ) ) );
  _color2.blue( static_cast<unsigned char>( std::min( 255.0f, _color2.blue() * brightness2 ) ) );

  Shape::_fillColor.red( ( _color0.red() + _color1.red() + _color2.red() ) / 3 );
  Shape::_fillColor.green( ( _color0.green() + _color1.green() + _color2.green() ) / 3 );
  Shape::_fillColor.blue( ( _color0.blue() + _color1.blue() + _color2.blue() ) / 3 );
}

GouraudTriangle &
GouraudTriangle::rotate( double angle, const Point & center )
{
  _path.rotate( angle, center );
  return *this;
}

GouraudTriangle
GouraudTriangle::rotated( double angle, const Point & center ) const
{
  return GouraudTriangle(*this).rotate( angle, center );
}

GouraudTriangle &
GouraudTriangle::rotate( double angle )
{
  return GouraudTriangle::rotate( angle, center() );
}

GouraudTriangle
GouraudTriangle::rotated( double angle ) const
{
  return GouraudTriangle(*this).rotate( angle, center() );
}

GouraudTriangle
GouraudTriangle::translated( double dx, double dy ) const
{
  return static_cast<GouraudTriangle &>( GouraudTriangle(*this).translate( dx, dy ) );
}

GouraudTriangle
GouraudTriangle::scaled( double sx, double sy ) const
{
  return static_cast<GouraudTriangle &>( GouraudTriangle(*this).scale( sx, sy ) );
}

GouraudTriangle
GouraudTriangle::scaled( double s ) const
{
  return static_cast<GouraudTriangle &>( GouraudTriangle(*this).scale( s, s ) );
}

void
GouraudTriangle::scaleAll( double s )
{
  _path.scaleAll( s );
}

GouraudTriangle
GouraudTriangle::resized(double w, double h, Shape::LineWidthFlag lineWidthFlag) const
{
  return static_cast<GouraudTriangle&>(GouraudTriangle(*this).resize(w,h,lineWidthFlag));
}

GouraudTriangle *
GouraudTriangle::clone() const {
  return new GouraudTriangle(*this);
}

void
GouraudTriangle::flushPostscript( std::ostream & stream,
                                  const TransformEPS & transform ) const
{
  if ( ! _subdivisions ) {
    Polyline::flushPostscript( stream, transform );
    return;
  }
  const Point & p0 = _path[0];
  const Point & p1 = _path[1];
  const Point & p2 = _path[2];
  Point p01( 0.5*(p0.x+p1.x), 0.5*(p0.y+p1.y) );
  Color c01( (_color0.red() + _color1.red())/2,
             (_color0.green() + _color1.green())/2,
             (_color0.blue() + _color1.blue())/2 );
  Point p12( 0.5*(p1.x+p2.x), 0.5*(p1.y+p2.y) );
  Color c12( (_color1.red() + _color2.red())/2,
             (_color1.green() + _color2.green())/2,
             (_color1.blue() + _color2.blue())/2 );
  Point p20( 0.5*(p2.x+p0.x), 0.5*(p2.y+p0.y) );
  Color c20( (_color2.red() + _color0.red())/2,
             (_color2.green() + _color0.green())/2,
             (_color2.blue() + _color0.blue())/2 );
  GouraudTriangle( p0, _color0, p20, c20, p01, c01, _subdivisions - 1, _depth ).flushPostscript( stream, transform );
  GouraudTriangle( p1, _color1, p01, c01, p12, c12, _subdivisions - 1, _depth ).flushPostscript( stream, transform );
  GouraudTriangle( p2, _color2, p20, c20, p12, c12, _subdivisions - 1, _depth ).flushPostscript( stream, transform );
  GouraudTriangle( p01, c01, p12, c12, p20, c20,  _subdivisions - 1, _depth ).flushPostscript( stream, transform );
}

void
GouraudTriangle::flushFIG( std::ostream & stream,
                           const TransformFIG & transform,
                           std::map<Color,int> & colormap ) const
{

  Color c( static_cast<unsigned char>((_color0.red() + _color1.red() + _color2.red() )/3.0),
           static_cast<unsigned char>((_color0.green() + _color1.green() + _color2.green())/3.0),
           static_cast<unsigned char>((_color0.blue() + _color1.blue() + _color2.blue())/3.0 ));
  Polyline( _path, Color::Null, c, 0.0f ).flushFIG( stream, transform, colormap );

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

void
GouraudTriangle::flushSVG( std::ostream & stream,
                           const TransformSVG & transform ) const
{
  if ( ! _subdivisions ) {
    Polyline::flushSVG( stream, transform );
    return;
  }
  const Point & p0 = _path[0];
  const Point & p1 = _path[1];
  const Point & p2 = _path[2];
  Point p01( 0.5*(p0.x+p1.x), 0.5*(p0.y+p1.y) );
  Color c01( (_color0.red() + _color1.red())/2,
             (_color0.green() + _color1.green())/2,
             (_color0.blue() + _color1.blue())/2 );
  Point p12( 0.5*(p1.x+p2.x), 0.5*(p1.y+p2.y) );
  Color c12( (_color1.red() + _color2.red())/2,
             (_color1.green() + _color2.green())/2,
             (_color1.blue() + _color2.blue())/2 );
  Point p20( 0.5*(p2.x+p0.x), 0.5*(p2.y+p0.y) );
  Color c20( (_color2.red() + _color0.red())/2,
             (_color2.green() + _color0.green())/2,
             (_color2.blue() + _color0.blue())/2 );
  GouraudTriangle( p0, _color0, p20, c20, p01, c01,
                   _subdivisions - 1, _depth ).flushSVG( stream, transform );
  GouraudTriangle( p1, _color1, p01, c01, p12, c12,
                   _subdivisions - 1, _depth ).flushSVG( stream, transform );
  GouraudTriangle( p2, _color2, p20, c20, p12, c12,
                   _subdivisions - 1, _depth ).flushSVG( stream, transform );
  GouraudTriangle( p01, c01, p12, c12, p20, c20,
                   _subdivisions - 1, _depth ).flushSVG( stream, transform );
}

void
GouraudTriangle::flushTikZ( std::ostream & stream,
                            const TransformTikZ & /*transform*/ ) const
{
  // FIXME: unimplemented
  stream << "% FIXME: GouraudTriangle::flushTikZ unimplemented" << std::endl;
}

/*
 * Triangle
 */

const std::string Triangle::_name("Triangle");

const std::string &
Triangle::name() const
{
  return _name;
}

Triangle
Triangle::rotated( double angle ) const
{
  return static_cast<Triangle &>( Triangle( *this ).rotate( angle ) );
}

Triangle
Triangle::translated( double dx, double dy ) const
{
  return static_cast<Triangle &>( Triangle( *this ).translate( dx, dy ) );
}

Triangle
Triangle::scaled( double sx, double sy ) const
{
  return static_cast<Triangle &>( Triangle( *this ).scale( sx, sy ) );
}

Triangle
Triangle::scaled( double s ) const
{
  return static_cast<Triangle &>( Triangle( *this ).scale( s ) );
}

Triangle
Triangle::resized(double w, double h, Shape::LineWidthFlag lineWidthFlag) const
{
  return static_cast<Triangle&>(Triangle(*this).resize(w,h,lineWidthFlag));
}

Triangle *
Triangle::clone() const {
  return new Triangle(*this);
}

/*
 * Text
 */

const std::string Text::_name("Text");


Text::Text( double x, double y,
            const std::string & text,
            const Fonts::Font font,
            double size,
            Color color,
            int depth )
  : Shape( color, Color::Null, 1.0, SolidStyle, ButtCap, MiterJoin, depth ),
    _text( text ), _font( font ),
    _size( size ),
    _xScale( 1.0 ), _yScale( 1.0 )
{
  const double width = text.length() * size * 0.71;   // Why 0.71? Well, give it a try!
  _box << Point(x,y);
  _box << _box[0] + Point(width,0);
  _box << _box[0] + Point(width,size);
  _box << _box[0] + Point(0,size);
  _box.setClosed(true);
}

Text::Text(Point p,
           const std::string & text,
           const Fonts::Font font,
           double size,
           Color color,
           int depth)
  : Shape( color, Color::Null, 1.0, SolidStyle, ButtCap, MiterJoin, depth ),
    _text( text ), _font( font ),
    _size( size ),
    _xScale( 1.0 ), _yScale( 1.0 )
{
  const double width = text.length() * size * 0.71;   // Why 0.71? Well, give it a try!
  _box << p;
  _box << _box[0] + Point(width,0);
  _box << _box[0] + Point(width,size);
  _box << _box[0] + Point(0,size);
  _box.setClosed(true);
}

Text::Text( double x, double y,
            const std::string & text,
            const Fonts::Font font,
            const std::string & svgFont,
            double size,
            Color color,
            int depth )
  : Shape( color, Color::Null, 1.0, SolidStyle, ButtCap, MiterJoin, depth ),
    _text( text ), _font( font ), _svgFont( svgFont ),
    _size( size ),
    _xScale( 1.0 ), _yScale( 1.0 )
{
  const double width = text.length() * size * 0.71;   // Why 0.71? Well, give it a try!
  _box << Point(x,y);
  _box << _box[0] + Point(width,0);
  _box << _box[0] + Point(width,size);
  _box << _box[0] + Point(0,size);
  _box.setClosed(true);
}

Text::Text(Point p,
           const std::string & text,
           const Fonts::Font font,
           const std::string & svgFont,
           double size,
           Color color,
           int depth)
  : Shape( color, Color::Null, 1.0, SolidStyle, ButtCap, MiterJoin, depth ),
    _text( text ), _font( font ), _svgFont( svgFont ),
    _size( size ),
    _xScale( 1.0 ), _yScale( 1.0 )
{
  const double width = text.length() * size * 0.71;   // Why 0.71? Well, give it a try!
  _box << p;
  _box << _box[0] + Point(width,0);
  _box << _box[0] + Point(width,size);
  _box << _box[0] + Point(0,size);
  _box.setClosed(true);
}

const std::string &
Text::name() const
{
  return _name;
}

Point
Text::center(LineWidthFlag ) const {
  return _box.center();
}

Text &
Text::rotate( double angle, const Point & center )
{
  _box.rotate(angle,center);
  //  Point endPos = _position + Point( 10000 * cos( _angle ), 10000 * sin( _angle ) );
  //  _position.rotate( angle, center );
  //  endPos.rotate( angle, center );
  //  Point v = endPos - _position;
  //  v /= v.norm();
  //  if ( v.x >= 0 ) _angle = asin( v.y );
  //  else if ( v.y > 0 ) _angle = (M_PI/2.0) + acos( v.y );
  //  else _angle = (-M_PI/2.0) - acos( -v.y );
  return *this;
}

Text
Text::rotated( double angle, const Point & center ) const
{
  return Text(*this).rotate( angle, center );
}

Text &
Text::rotate( double angle )
{
  _box.rotate(angle);
  //  _angle += angle;
  //  while ( _angle < -M_PI ) {
  //    _angle += 2 * M_PI;
  //  }
  //  while ( _angle > M_PI ) {
  //    _angle -= 2 * M_PI;
  //  }
  return *this;
}

Text
Text::rotated( double angle ) const
{
  return Text(*this).rotate( angle );
}

Text &
Text::translate( double dx, double dy )
{
  _box.translate(dx,dy);
  return *this;
}

Text
Text::translated( double dx, double dy ) const
{
  return static_cast<Text&>( Text(*this).translate( dx, dy ) );
}

Text &
Text::scale( double sx, double sy )
{
  // TODO Actually handle scaling.
  _xScale *= sx;
  _yScale *= sy;
  _box.scale(sx,sy);
  return *this;
}

Text &
Text::scale( double s )
{
  _xScale *= s;
  _yScale *= s;
  _box.scale(s);
  return *this;
}

Text
Text::scaled( double sx, double sy ) const
{
  return Text(*this).scale( sx, sy );
}

Text
Text::scaled( double s ) const
{
  return Text(*this).scale( s, s );
}

void
Text::scaleAll( double s )
{
  _box.scaleAll(s);
}

Text *
Text::clone() const {
  return new Text(*this);
}

double Text::boxHeight(const Transform & transform) const
{
  Point baseline = (_box[1] - _box[0]);
  Point leftSide = (_box[3] - _box[0]);
  Point transformedLeftSide( transform.map(_box[3]) - transform.map(_box[0]));
  double angle = std::acos(baseline.normalised() * leftSide.normalised());
  if ( std::fabs(2*angle - M_PI) < std::numeric_limits<float>::epsilon() ) {
    return transformedLeftSide.norm();
  }
  return std::sin(angle) * transformedLeftSide.norm();
}

double Text::boxLength(const Transform & transform) const
{
  Point transformedBaseline( transform.map(_box[1]) - transform.map(_box[0]));
  return transformedBaseline.norm();
}

double
Text::angle() const
{
  Point v = (_box[1] - _box[0]).normalise();
  return std::atan2(v.y,v.x);
}

Point
Text::position() const
{
  return _box[0];
}

void
Text::flushPostscript( std::ostream & stream,
                       const TransformEPS & transform ) const
{
  stream << "\n% Text\n";
  stream << "gs /" << PSFontNames[ _font ] << " ff " << boxHeight(transform) << " scf sf";
  stream << " " << transform.mapX( position().x ) << " " << transform.mapY( position().y ) << " m";
  if ( angle() != 0.0 ) {
    stream << " " << (angle()*180.0/M_PI) << " rot ";
  }
  stream << " (" << _text << ")"
         << " " << _penColor.postscript() << " srgb"
         << " sh gr" << std::endl;
}

void
Text::flushFIG( std::ostream & stream,
                const TransformFIG & transform,
                std::map<Color,int> & colormap ) const
{
  const float ppmm = 720.0f / 254.0f;
  const float fig_ppmm = 45.0f;


  stream << "4 0 " ;
  // Color, depth, unused, Font
  stream << colormap[ _penColor ] <<  " " << transform.mapDepth( _depth ) << " -1 " << _font << " ";
  // Font size, Angle, Font flags
  stream << (ppmm * boxHeight(transform) / fig_ppmm) << " " << angle() << " 4 ";
  // Height
  stream << static_cast<int>( boxHeight(transform) /* * 135 / 12.0 */ ) << " ";
  // Width
  stream << static_cast<int>( boxLength(transform) /* * 135 / 12.0 */ ) << " ";
  // x y
  stream << static_cast<int>( transform.mapX( position().x ) ) << " "
         << static_cast<int>( transform.mapY( position().y ) ) << " ";
  stream << _text << "\\001\n";
}

void
Text::flushSVG( std::ostream & stream,
                const TransformSVG & transform ) const
{
  if ( angle() != 0.0f ) {
    stream << "<g transform=\"translate("
           << transform.mapX( position().x ) << ","
           << transform.mapY( position().y ) << ")\" >"
           << "<g transform=\"rotate(" << (-angle()*180.0/M_PI) << ")\" >"
           << "<text x=\"0\" y=\"0\""
           << " font-family=\"" << ( _svgFont.length() ? _svgFont : PSFontNames[ _font ] ) << "\""
           << " font-size=\"" << boxHeight(transform) << "\""
           << " fill=\"" << _penColor.svg() << "\""
           << _fillColor.svgAlpha( " fill" )
           << _penColor.svgAlpha( " stroke" )
           << ">"
           << _text
           << "</text></g></g>" << std::endl;
  } else {
    stream << "<text x=\"" << transform.mapX( position().x )
           << "\" y=\"" << transform.mapY( position().y ) << "\" "
           << " font-family=\"" << ( _svgFont.length() ? _svgFont : PSFontNames[ _font ] ) << "\""
           << " font-size=\"" << boxHeight(transform) << "\""
           << " fill=\"" << _penColor.svg() << "\""
           << _fillColor.svgAlpha( " fill" )
           << _penColor.svgAlpha( " stroke" )
           << ">"
           << _text
           << "</text>" << std::endl;
  }
  // DEBUG
  // Polyline(_box,Color::Black,Color::None,0.5).flushSVG(stream,transform);
}

void
Text::flushTikZ( std::ostream & stream,
                 const TransformTikZ & transform ) const
{
  // FIXME: honor font-size
  static const char BOLD_FONT =	0x01;
  static const char ITALIC_FONT = 0x02;
  static const char MONOSPACE_FONT = 0x04;
  static const char SANSSERIF_FONT = 0x08;
  char fontTraits[] = {
    0,						// TimesRoman,
    ITALIC_FONT,					// TimesItalic,
    BOLD_FONT,					// TimesBold,
    BOLD_FONT | ITALIC_FONT,			// TimesBoldItalic,
    SANSSERIF_FONT,					// AvantGardeBook,
    SANSSERIF_FONT | ITALIC_FONT,			// AvantGardeBookOblique,
    SANSSERIF_FONT,					// AvantGardeDemi,
    SANSSERIF_FONT | ITALIC_FONT,			// AvantGardeDemiOblique,
    0,						// BookmanLight,
    ITALIC_FONT,					// BookmanLightItalic,
    0,						// BookmanDemi,
    ITALIC_FONT,					// BookmanDemiItalic,
    MONOSPACE_FONT,					// Courier,
    MONOSPACE_FONT | ITALIC_FONT,			// CourierOblique,
    MONOSPACE_FONT | BOLD_FONT,			// CourierBold,
    MONOSPACE_FONT | BOLD_FONT | ITALIC_FONT,	// CourierBoldOblique,
    SANSSERIF_FONT,					// Helvetica,
    SANSSERIF_FONT | ITALIC_FONT,			// HelveticaOblique,
    SANSSERIF_FONT | BOLD_FONT,			// HelveticaBold,
    SANSSERIF_FONT | BOLD_FONT | ITALIC_FONT,	// HelveticaBoldOblique,
    SANSSERIF_FONT,					// HelveticaNarrow,
    SANSSERIF_FONT | ITALIC_FONT,			// HelveticaNarrowOblique,
    SANSSERIF_FONT | BOLD_FONT,			// HelveticaNarrowBold,
    SANSSERIF_FONT | BOLD_FONT | ITALIC_FONT,	// HelveticaNarrowBoldOblique,
    0,						// NewCenturySchoolbookRoman,
    ITALIC_FONT,					// NewCenturySchoolbookItalic,
    BOLD_FONT,					// NewCenturySchoolbookBold,
    BOLD_FONT | ITALIC_FONT,			// NewCenturySchoolbookBoldItalic,
    0,						// PalatinoRoman,
    ITALIC_FONT,					// PalatinoItalic,
    BOLD_FONT,					// PalatinoBold,
    BOLD_FONT | ITALIC_FONT,			// PalatinoBoldItalic,
    0,						// Symbol,
    ITALIC_FONT,					// ZapfChanceryMediumItalic,
    0						// ZapfDingbats
  };

  stream << "\\path[" << tikzProperties(transform) << "] ("
         << transform.mapX( position().x ) << ',' << transform.mapY( position().y )
         << ") node {"
         << (fontTraits[ _font ] & ITALIC_FONT ? "\\itshape " : "")
         << (fontTraits[ _font ] & BOLD_FONT ? "\\bfseries " : "")
         << (fontTraits[ _font ] & MONOSPACE_FONT ? "\\ttfamily " : "")
         << (fontTraits[ _font ] & SANSSERIF_FONT ? "\\sffamily " : "")
         << _text
         << "};" << std::endl;
}

Rect
Text::boundingBox( LineWidthFlag ) const
{
  // TODO Get correct values!
  return _box.boundingBox();
}

} // namespace LibBoard
