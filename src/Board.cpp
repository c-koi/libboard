/* -*- mode: c++ -*- */
/**
 * @file   Board.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   Sat Aug 18 2007
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

#include "Board.h"
#include "board/Point.h"
#include "board/Rect.h"
#include "board/Shapes.h"
#include "board/Tools.h"
#include "board/PSFonts.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <ctime>
#include <cstring>
#include <map>
#include <algorithm>
#include <cstdio>
#include <algorithm>

#if defined( max )
#undef max
#endif

namespace {

const float pageSizes[][2] = { { 0.0f, 0.0f },               // BoundingBox
                               { 841.0f, 1189.0f },          // A0
                               { 594.0f, 841.0f },           // A1
                               { 420.0f, 594.0f },           // A2
                               { 297.0f, 420.0f },           // A3
                               { 210.0f, 297.0f },           // A4
                               { 148.0f, 210.0f },           // A5
                               { 105.0f, 148.0f },           // A6
                               { 74.0f, 105.0f },            // A7
                               { 52.0f, 74.0f },             // A8
                               { 37.0f, 52.0f },             // A9
                               { 26.0f, 37.0f },             // A10
                               { 8.5f*25.4f, 11.0f*25.4f },  // Letter
                               { 8.5f*25.4f, 14.0f*25.4f },  // Legal
                               { 7.2f*25.4f, 10.5f*25.4f }   // Executive
                             };

const float ppmm = 720.0f / 254.0f;
}

namespace LibBoard {

const double Board::Degree =  3.14159265358979323846 / 180.0;

Board::State::State()
{
  penColor = Color::Black;
  fillColor =  Color::None;
  lineWidth = 0.5;
  lineStyle = Shape::SolidStyle;
  lineCap = Shape::ButtCap;
  lineJoin = Shape::MiterJoin;
  font = Fonts::TimesRoman;
  fontSize = 11.0;
  unitFactor = 1.0;
}

Board::Board( const Color & backgroundColor )
  : _backgroundColor( backgroundColor )
{
}

Board::Board( const Board & other )
  : ShapeList( other ),
    _state( other._state ),
    _backgroundColor( other._backgroundColor )
{
}

Board &
Board::operator=( const Board & other )
{
  free();
  if ( ! other._shapes.size() ) return (*this);
  _shapes.resize( other._shapes.size(), 0 );
  std::vector<Shape*>::iterator t = _shapes.begin();
  std::vector<Shape*>::const_iterator i = other._shapes.begin();
  std::vector<Shape*>::const_iterator end = other._shapes.end();
  while ( i != end ) {
    *t = (*i)->clone();
    ++i; ++t;
  }
  return *this;
}

Board &
Board::operator<<( const Shape & shape )
{
  ShapeList::addShape( shape, _state.unitFactor );
  return *this;
}

Board &
Board::operator<<( Unit unit )
{
  setUnit( unit );
  return *this;
}

Board::~Board()
{

}

void
Board::clear( const Color & color )
{
  ShapeList::clear();
  _backgroundColor = color;
}

Board &
Board::rotate( double angle, const Point & center )
{
  ShapeList::rotate( angle, center );
  _clippingPath.rotate( angle, center );
  return (*this);
}

Board &
Board::rotate( double angle )
{
  ShapeList::rotate( angle );
  _clippingPath.rotate( angle, center() );
  return (*this);
}

Board &
Board::translate( double dx, double dy )
{
  ShapeList::translate( dx, dy );
  _clippingPath.translate( dx, dy );
  return (*this);
}

Board &
Board::scale( double sx, double sy )
{
  Point delta = _clippingPath.center() - center();
  delta.x *= sx;
  delta.y *= sy;
  _clippingPath.scale( sx, sy );
  ShapeList::scale( sx, sy );
  delta = ( center() + delta ) - _clippingPath.center();
  _clippingPath.translate( delta.x, delta.y );
  return (*this);
}

Board &
Board::scale( double s )
{
  Point delta = _clippingPath.center() - center();
  delta *= s;
  _clippingPath.scale( s );
  ShapeList::scale( s );
  delta = ( center() + delta ) - _clippingPath.center();
  _clippingPath.translate( delta.x, delta.y );
  return (*this);
}

Board
Board::rotated( double angle, const Point & center )
{
  return static_cast<const Board &>( Board( *this ).rotate( angle, center ) );
}

Board
Board::rotated( double angle )
{
  return static_cast<const Board &>( Board( *this ).rotate( angle ) );
}

Board
Board::translated( double dx, double dy )
{
  return static_cast<const Board &>( Board( *this ).translate( dx, dy ) );
}

Board
Board::scaled( double sx, double sy )
{
  return static_cast<const Board &>( Board( *this ).scale( sx, sy ) );
}

Board
Board::scaled( double s )
{
  return static_cast<const Board &>( Board( *this ).scale( s ) );
}


void
Board::setUnit( Unit unit )
{
  switch ( unit ) {
  case UPoint:
    _state.unitFactor = 1.0;
    break;
  case UInche:
    _state.unitFactor = 25.4f * ppmm;
    break;
  case UCentimeter:
    _state.unitFactor = 10.0f * ppmm;
    break;
  case UMillimeter:
    _state.unitFactor = ppmm;
    break;
  }
}

void
Board::setUnit( double factor, Unit unit )
{
  switch ( unit ) {
  case UPoint:
    _state.unitFactor = factor;
    break;
  case UInche:
    _state.unitFactor = 720.0f * factor;
    break;
  case UCentimeter:
    _state.unitFactor = 10.0f * ppmm * factor;
    break;
  case UMillimeter:
    _state.unitFactor = ppmm * factor;
    break;
  }
}

Board &
Board::setPenColorRGBi( unsigned char red,
                        unsigned char green,
                        unsigned char blue,
                        unsigned char alpha )
{
  _state.penColor.setRGBi( red, green, blue, alpha );
  return *this;
}

Board &
Board::setPenColorRGBf(  float red,
                         float green,
                         float blue,
                         float alpha )
{
  _state.penColor.setRGBf( red, green, blue, alpha );
  return *this;
}

Board &
Board::setPenColor( const Color & color )
{
  _state.penColor = color;
  return *this;
}

Board &
Board::setFillColorRGBi( unsigned char red,
                         unsigned char green,
                         unsigned char blue,
                         unsigned char alpha )
{
  _state.fillColor.setRGBi( red, green, blue, alpha );
  return *this;
}

Board &
Board::setFillColorRGBf( float red, float green, float blue, float alpha )
{
  _state.fillColor.setRGBf( red, green, blue, alpha );
  return *this;
}

Board &
Board::setFillColor( const Color & color )
{
  _state.fillColor = color;
  return *this;
}

Board &
Board::setLineWidth( double width )
{
  _state.lineWidth = width;
  return *this;
}

Board &
Board::setFont( const Fonts::Font font, double fontSize )
{
  _state.font = font;
  _state.fontSize = fontSize;
  return *this;
}

Board &
Board::setFontSize( double fontSize )
{
  _state.fontSize = fontSize;
  return *this;
}

void
Board::backgroundColor( const Color & color )
{
  _backgroundColor = color;
}

void
Board::drawDot( double x, double y, int depth )
{
  if ( depth != -1 )
    _shapes.push_back( new Dot( _state.unit(x), _state.unit(y),
                                _state.penColor, _state.lineWidth, depth ) );
  else
    _shapes.push_back( new Dot( _state.unit(x), _state.unit(y),
                                _state.penColor, _state.lineWidth, _nextDepth-- ) );
}

void
Board::drawLine( double x1, double y1, double x2, double y2,
                 int depth /* = -1 */  )
{
  if ( depth != -1 )
    _shapes.push_back( new Line( _state.unit(x1), _state.unit(y1),
                                 _state.unit(x2), _state.unit(y2),
                                 _state.penColor, _state.lineWidth,
                                 _state.lineStyle, _state.lineCap,
                                 _state.lineJoin, depth ) );
  else
    _shapes.push_back( new Line( _state.unit(x1), _state.unit(y1),
                                 _state.unit(x2), _state.unit(y2),
                                 _state.penColor, _state.lineWidth,
                                 _state.lineStyle, _state.lineCap,
                                 _state.lineJoin, _nextDepth-- ) );
}

void
Board::drawArrow( double x1, double y1, double x2, double y2,
                  bool filled /* = false */,
                  int depth /* = -1 */  )
{
  if ( depth != -1 )
    _shapes.push_back( new Arrow( _state.unit(x1), _state.unit(y1),
                                  _state.unit(x2), _state.unit(y2),
                                  _state.penColor,
                                  filled ? _state.penColor : Color::None,
                                  _state.lineWidth, _state.lineStyle,
                                  _state.lineCap, _state.lineJoin, depth ) );
  else
    _shapes.push_back( new Arrow( _state.unit(x1), _state.unit(y1),
                                  _state.unit(x2), _state.unit(y2),
                                  _state.penColor,
                                  filled ? _state.penColor : Color::None,
                                  _state.lineWidth, _state.lineStyle,
                                  _state.lineCap, _state.lineJoin,
                                  _nextDepth-- ) );
}

void
Board::drawRectangle( double left, double top,
                      double width, double height,
                      int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Rectangle( _state.unit(left),
                                    _state.unit(top),
                                    _state.unit(width),
                                    _state.unit(height),
                                    _state.penColor, _state.fillColor,
                                    _state.lineWidth, _state.lineStyle,
                                    _state.lineCap, _state.lineJoin, d ) );
}

void
Board::fillRectangle( double left, double top,
                      double width, double height,
                      int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Rectangle( _state.unit(left),
                                    _state.unit(top),
                                    _state.unit(width),
                                    _state.unit(height),
                                    Color::None, _state.penColor,
                                    0.0f, _state.lineStyle,
                                    _state.lineCap, _state.lineJoin,
                                    d ) );
}

void
Board::drawCircle( double x, double y, double radius,
                   int depth /* = -1 */  )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Circle( _state.unit(x), _state.unit(y),
                                 _state.unit(radius),
                                 _state.penColor, _state.fillColor,
                                 _state.lineWidth, _state.lineStyle, d ) );
}

void
Board::fillCircle( double x, double y, double radius,
                   int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Circle( _state.unit(x), _state.unit(y), _state.unit(radius),
                                 Color::None, _state.penColor,
                                 0.0f, _state.lineStyle, d ) );
}

void
Board::drawEllipse( double x, double y,
                    double xRadius, double yRadius,
                    int depth /* = -1 */  )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Ellipse( _state.unit(x), _state.unit(y),
                                  _state.unit(xRadius), _state.unit(yRadius),
                                  _state.penColor,
                                  _state.fillColor,
                                  _state.lineWidth,
                                  _state.lineStyle,
                                  d ) );
}

void
Board::fillEllipse( double x, double y,
                    double xRadius, double yRadius,
                    int depth /* = -1 */ )
{
  int d = depth ? depth : _nextDepth--;
  _shapes.push_back( new Ellipse( _state.unit(x), _state.unit(y),
                                  _state.unit(xRadius), _state.unit(yRadius),
                                  Color::None,
                                  _state.penColor,
                                  0.0f,
                                  _state.lineStyle,
                                  d ) );
}

void
Board::drawPolyline( const std::vector<Point> & points,
                     int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  std::vector<Point> v = points;
  std::vector<Point>::iterator it = v.begin();
  std::vector<Point>::iterator end = v.end();
  while ( it != end ) {
    (*it) = _state.unit( *it );
    ++it;
  }
  _shapes.push_back( new Polyline( v, false, _state.penColor, _state.fillColor,
                                   _state.lineWidth,
                                   _state.lineStyle,
                                   _state.lineCap,
                                   _state.lineJoin,
                                   d ) );
}

void
Board::drawClosedPolyline( const std::vector<Point> & points,
                           int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  std::vector<Point> v = points;
  std::vector<Point>::iterator it = v.begin();
  std::vector<Point>::iterator end = v.end();
  while ( it != end ) {
    (*it) = _state.unit( *it );
    ++it;
  }
  _shapes.push_back( new Polyline( v, true, _state.penColor, _state.fillColor,
                                   _state.lineWidth,
                                   _state.lineStyle,
                                   _state.lineCap,
                                   _state.lineJoin,
                                   d ) );
}

void
Board::fillPolyline( const std::vector<Point> & points,
                     int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Polyline( points, true, Color::None, _state.penColor,
                                   0.0f,
                                   _state.lineStyle,
                                   _state.lineCap,
                                   _state.lineJoin,
                                   d ) );
}

void
Board::drawTriangle( double x1, double y1,
                     double x2, double y2,
                     double x3, double y3,
                     int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  std::vector<Point> points;
  points.push_back( Point( _state.unit(x1), _state.unit(y1) ) );
  points.push_back( Point( _state.unit(x2), _state.unit(y2) ) );
  points.push_back( Point( _state.unit(x3), _state.unit(y3) ) );
  _shapes.push_back( new Polyline( points, true, _state.penColor, _state.fillColor,
                                   _state.lineWidth,
                                   _state.lineStyle,
                                   _state.lineCap,
                                   _state.lineJoin,
                                   d ) );
}

void
Board::drawTriangle( const Point & p1,
                     const Point & p2,
                     const Point & p3,
                     int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  std::vector<Point> points;
  points.push_back( Point( _state.unit(p1.x), _state.unit(p1.y) ) );
  points.push_back( Point( _state.unit(p2.x), _state.unit(p2.y) ) );
  points.push_back( Point( _state.unit(p3.x), _state.unit(p3.y) ) );
  _shapes.push_back( new Polyline( points, true,
                                   _state.penColor, _state.fillColor,
                                   _state.lineWidth,
                                   _state.lineStyle,
                                   _state.lineCap,
                                   _state.lineJoin,
                                   d ) );
}

void
Board::fillTriangle( double x1, double y1,
                     double x2, double y2,
                     double x3, double y3,
                     int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  std::vector<Point> points;
  points.push_back( Point( _state.unit(x1), _state.unit(y1) ) );
  points.push_back( Point( _state.unit(x2), _state.unit(y2) ) );
  points.push_back( Point( _state.unit(x3), _state.unit(y3) ) );
  _shapes.push_back( new Polyline( points, true, Color::None, _state.penColor,
                                   0.0f,
                                   _state.lineStyle,
                                   _state.lineCap,
                                   _state.lineJoin,
                                   d ) );
}

void
Board::fillTriangle( const Point & p1,
                     const Point & p2,
                     const Point & p3,
                     int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  std::vector<Point> points;
  points.push_back( Point( _state.unit(p1.x), _state.unit(p1.y) ) );
  points.push_back( Point( _state.unit(p2.x), _state.unit(p2.y) ) );
  points.push_back( Point( _state.unit(p3.x), _state.unit(p3.y) ) );
  _shapes.push_back( new Polyline( points, true, Color::None, _state.penColor,
                                   0.0f,
                                   _state.lineStyle,
                                   _state.lineCap,
                                   _state.lineJoin,
                                   d ) );
}

void
Board::fillGouraudTriangle( const Point & p1,
                            const Color & color1,
                            const Point & p2,
                            const Color & color2,
                            const Point & p3,
                            const Color & color3,
                            unsigned char divisions,
                            int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new GouraudTriangle( Point( _state.unit(p1.x),
                                                 _state.unit(p1.y) ), color1,
                                          Point( _state.unit(p2.x),
                                                 _state.unit(p2.y) ), color2,
                                          Point( _state.unit(p3.x),
                                                 _state.unit(p3.y) ), color3,
                                          divisions, d ) );
}

void
Board::fillGouraudTriangle( const Point & p1,
                            const float brightness1,
                            const Point & p2,
                            const float brightness2,
                            const Point & p3,
                            const float brightness3,
                            unsigned char divisions,
                            int depth /* = -1 */ )
{
  Color color1( _state.penColor );
  Color color2( _state.penColor );
  Color color3( _state.penColor );
  color1.red( static_cast<unsigned char>( std::min( 255.0f, color1.red() * brightness1 ) ) );
  color1.green( static_cast<unsigned char>( std::min( 255.0f, color1.green() * brightness1 ) ) );
  color1.blue( static_cast<unsigned char>( std::min( 255.0f, color1.blue() * brightness1 ) ) );
  color2.red( static_cast<unsigned char>( std::min( 255.0f, color2.red() * brightness2 ) ) );
  color2.green( static_cast<unsigned char>( std::min( 255.0f, color2.green() * brightness2 ) ) );
  color2.blue( static_cast<unsigned char>( std::min( 255.0f, color2.blue() * brightness2 ) ) );
  color3.red( static_cast<unsigned char>( std::min( 255.0f, color3.red() * brightness3 ) ) );
  color3.green( static_cast<unsigned char>( std::min( 255.0f, color3.green() * brightness3 ) ) );
  color3.blue( static_cast<unsigned char>( std::min( 255.0f, color3.blue() * brightness3 ) ) );
  fillGouraudTriangle( Point( _state.unit(p1.x), _state.unit(p1.y) ), color1,
                       Point( _state.unit(p2.x), _state.unit(p2.y) ), color2,
                       Point( _state.unit(p3.x), _state.unit(p3.y) ), color3,
                       divisions,
                       depth );
}

void
Board::drawText( double x, double y, const char * text,
                 int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Text( _state.unit(x), _state.unit(y), text,
                               _state.font, _state.fontSize,
                               _state.penColor, d ) );
}

void
Board::drawText( double x, double y, const std::string & str, int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Text( _state.unit(x), _state.unit(y), str,
                               _state.font, _state.fontSize,
                               _state.penColor, d ) );
}

void
Board::drawBoundingBox( int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  Rect bbox = boundingBox();
  _shapes.push_back( new Rectangle( _state.unit(bbox.left),
                                    _state.unit(bbox.top),
                                    _state.unit(bbox.width),
                                    _state.unit(bbox.height),
                                    _state.penColor,
                                    _state.fillColor,
                                    _state.lineWidth,
                                    _state.lineStyle,
                                    _state.lineCap,
                                    _state.lineJoin,
                                    d ) );
}

void
Board::setClippingRectangle(  double x, double y,
                              double width, double height )
{
  _clippingPath.clear();
  _clippingPath << _state.unit( Point( x, y ) );
  _clippingPath << _state.unit( Point( x + width, y ) );
  _clippingPath << _state.unit( Point( x + width, y - height) );
  _clippingPath << _state.unit( Point( x , y - height ) );
}

void
Board::setClippingPath(  const std::vector<Point> & points  )
{
  _clippingPath.clear();
  std::vector<Point>::const_iterator it = points.begin();
  std::vector<Point>::const_iterator end = points.end();
  while ( it != end ) {
    _clippingPath << _state.unit( *it );
    ++it;
  }
}

void
Board::setClippingPath(  const Path & path  )
{
  _clippingPath = path;
  _clippingPath.setClosed( true );
  if ( _clippingPath.size() > 1 ) {
    if ( _clippingPath[0] == _clippingPath[ _clippingPath.size() - 1 ] )
      _clippingPath.pop_back();
  }
  std::size_t n = _clippingPath.size();
  for ( std::size_t i = 0; i < n; ++i ) {
    _clippingPath[i] = _state.unit( _clippingPath[i] );
  }
}


void
Board::addDuplicates( const Shape & shape,
                      std::size_t times,
                      double dx, double dy, double scale )
{
  Shape * s = shape.clone();
  while ( times-- ) {
    (*this) << (*s);
    if ( scale != 1.0 )
      s->scale( scale );
    s->translate( dx, dy );
  }
  delete s;
}

void
Board::addDuplicates( const Shape & shape,
                      std::size_t times,
                      double dx, double dy,
                      double scaleX, double scaleY,
                      double angle )
{
  Shape * s = shape.clone();
  while ( times-- ) {
    (*this) << (*s);
    if ( scaleX != 1.0 || scaleY != 1.0 ) s->scale( scaleX, scaleY );
    if ( dx != 0.0 || dy != 0.0 ) s->translate( dx, dy );
    if ( angle != 0.0 ) s->rotate( angle );
  }
  delete s;
}

void
Board::saveEPS( const char * filename, PageSize size, double margin, const std::string & title  ) const
{
  if ( title == std::string() )
    saveEPS( filename, pageSizes[size][0], pageSizes[size][1], margin, filename );
  else
    saveEPS( filename, pageSizes[size][0], pageSizes[size][1], margin, title );
}

void
Board::saveEPS( std::ostream & out, PageSize size, double margin, const std::string & title  ) const
{
  saveEPS( out, pageSizes[size][0], pageSizes[size][1], margin, title );
}

void
Board::saveEPS( std::ostream & out, double pageWidth, double pageHeight, double margin, const std::string & title ) const
{
  Rect bbox = boundingBox();
  bool clipping = _clippingPath.size() > 2;
  if ( clipping )
    bbox = bbox && _clippingPath.boundingBox();

  TransformEPS transform;
  transform.setBoundingBox( bbox, pageWidth, pageHeight, margin );

  out << "%!PS-Adobe-2.0 EPSF-2.0" << std::endl;
  out << "%%Title: " << title << std::endl;
  out << "%%Creator: Board library (Copyleft)2007 Sebastien Fourey" << std::endl;
  {
    time_t t = time(0);
    char str_time[255];
    secured_ctime( str_time, &t, 255 );
    out << "%%CreationDate: " << str_time;
  }
  out << "%%BoundingBox: " << std::setprecision( 8 )
      << transform.mapX( bbox.left ) << " "
      << transform.mapY( bbox.top - bbox.height ) << " "
      << transform.mapX( bbox.left + bbox.width ) << " "
      << transform.mapY( bbox.top ) << std::endl;

  out << "%Magnification: 1.0000" << std::endl;
  out << "%%EndComments" << std::endl;

  out << std::endl;
  out << "/cp {closepath} bind def" << std::endl;
  out << "/ef {eofill} bind def" << std::endl;
  out << "/gr {grestore} bind def" << std::endl;
  out << "/gs {gsave} bind def" << std::endl;
  out << "/sa {save} bind def" << std::endl;
  out << "/rs {restore} bind def" << std::endl;
  out << "/l {lineto} bind def" << std::endl;
  out << "/m {moveto} bind def" << std::endl;
  out << "/rm {rmoveto} bind def" << std::endl;
  out << "/n {newpath} bind def" << std::endl;
  out << "/s {stroke} bind def" << std::endl;
  out << "/sh {show} bind def" << std::endl;
  out << "/slc {setlinecap} bind def" << std::endl;
  out << "/slj {setlinejoin} bind def" << std::endl;
  out << "/slw {setlinewidth} bind def" << std::endl;
  out << "/srgb {setrgbcolor} bind def" << std::endl;
  out << "/rot {rotate} bind def" << std::endl;
  out << "/sc {scale} bind def" << std::endl;
  out << "/sd {setdash} bind def" << std::endl;
  out << "/ff {findfont} bind def" << std::endl;
  out << "/sf {setfont} bind def" << std::endl;
  out << "/scf {scalefont} bind def" << std::endl;
  out << "/sw {stringwidth} bind def" << std::endl;
  out << "/sd {setdash} bind def" << std::endl;
  out << "/tr {translate} bind def" << std::endl;
  out << " 0.5 setlinewidth" << std::endl;

  if ( clipping ) {
    out << " newpath ";
    _clippingPath.flushPostscript( out, transform );
    out << " 0 slw clip " << std::endl;
  }

  // Draw the background color if needed.
  if ( _backgroundColor != Color::None ) {
    Rectangle r( bbox, Color::None, _backgroundColor, 0.0f );
    r.flushPostscript( out, transform );
  }

  // Draw the shapes
  std::vector< Shape* > shapes = _shapes;

  stable_sort( shapes.begin(), shapes.end(), shapeGreaterDepth );
  std::vector< Shape* >::const_iterator i = shapes.begin();
  std::vector< Shape* >::const_iterator end = shapes.end();

  while ( i != end ) {
    (*i)->flushPostscript( out, transform );
    ++i;
  }
  out << "showpage" << std::endl;
  out << "%%Trailer" << std::endl;
  out << "%EOF" << std::endl;
}

void
Board::saveEPS( const char * filename, double pageWidth, double pageHeight, double margin, const std::string & title ) const
{
  std::ofstream out( filename );
  saveEPS(out,pageWidth,pageHeight,margin, title);
  out.close();
}

void
Board::saveFIG( const char * filename, PageSize size, double margin ) const
{
  saveFIG( filename, pageSizes[size][0], pageSizes[size][1], margin );
}

void
Board::saveFIG( std::ostream & out, PageSize size, double margin ) const
{
  saveFIG( out, pageSizes[size][0], pageSizes[size][1], margin );
}

void
Board::saveFIG( std::ostream & out, double pageWidth, double pageHeight, double margin ) const
{
  TransformFIG transform;
  Rect bbox = boundingBox();
  transform.setBoundingBox( bbox, pageWidth, pageHeight, margin  );
  transform.setDepthRange( *this );

  out << "#FIG 3.2  Produced by the Board library (Copyleft)2007 Sebastien Fourey\n";
  out << "Portrait\n";
  out << "Center\n";
  out << "Metric\n";
  out << "A4\n";
  out << "100.00\n";
  out << "Single\n";
  out << "-2\n";
  out << "1200 2\n";

  std::map<Color,int> colormap;
  int maxColor = 32;


  colormap[Color(0,0,0)] = 0;
  colormap[Color(0,0,255)] = 1;
  colormap[Color(0,255,0)] = 2;
  colormap[Color(0,255,255)] = 0;
  colormap[Color(255,0,0)] = 4;
  colormap[Color(255,0,255)] = 0;
  colormap[Color(255,255,0)] = 6;
  colormap[Color(255,255,255)] = 7;


  std::vector< Shape* > shapes = _shapes;
  stable_sort( shapes.begin(), shapes.end(), shapeGreaterDepth );
  std::vector< Shape* >::const_iterator i = shapes.begin();
  std::vector< Shape* >::const_iterator end = shapes.end();
  while ( i != end ) {
    if ( colormap.find( (*i)->penColor() ) == colormap.end()
         && (*i)->penColor().valid() )
      colormap[ (*i)->penColor() ] = maxColor++;
    if ( colormap.find( (*i)->fillColor() ) == colormap.end()
         && (*i)->fillColor().valid() )
      colormap[ (*i)->fillColor() ] = maxColor++;
    ++i;
  }

  if ( colormap.find( _backgroundColor ) == colormap.end()
       && _backgroundColor.valid() )
    colormap[ _backgroundColor ] = maxColor++;

  // Write the colormap
  std::map<Color,int>::const_iterator iColormap = colormap.begin();
  std::map<Color,int>::const_iterator endColormap = colormap.end();
  char colorString[255];
  while ( iColormap != endColormap ) {
    secured_sprintf( colorString, 255,
                     "0 %d #%02x%02x%02x\n",
                     iColormap->second,
                     iColormap->first.red(),
                     iColormap->first.green(),
                     iColormap->first.blue() );
    if ( iColormap->second >= 32 ) out << colorString;
    ++iColormap;
  }

  // Draw the background color if needed.
  if ( _backgroundColor != Color::None ) {
    Rectangle r( bbox, Color::None, _backgroundColor, 0.0f );
    r.depth( std::numeric_limits<int>::max() );
    r.flushFIG( out, transform, colormap );
  }

  // Draw the shapes.
  i = shapes.begin();
  while ( i != end ) {
    // notice << (*i)->name() << " " << (*i)->depth() <<  '\n';
    (*i)->flushFIG( out, transform, colormap );
    ++i;
  }
}

void
Board::saveFIG( const char * filename, double pageWidth, double pageHeight, double margin ) const
{
  std::ofstream out( filename );
  saveFIG(out,pageWidth,pageHeight,margin);
  out.close();
}

void
Board::saveSVG( const char * filename, PageSize size, double margin ) const
{
  saveSVG( filename, pageSizes[size][0], pageSizes[size][1], margin );
}

void
Board::saveSVG( std::ostream & out, PageSize size, double margin ) const
{
  saveSVG( out, pageSizes[size][0], pageSizes[size][1], margin );
}

void
Board::saveSVG( std::ostream & out, double pageWidth, double pageHeight, double margin ) const
{
  TransformSVG transform;
  Rect bbox = boundingBox();
  bool clipping = _clippingPath.size() > 2;
  if ( clipping )
    bbox = bbox && _clippingPath.boundingBox();
  transform.setBoundingBox( bbox, pageWidth, pageHeight, margin );

  out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
  out << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"" << std::endl;
  out << " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;

  if ( pageWidth > 0 && pageHeight > 0 ) {
    out << "<svg width=\""
        << pageWidth << "mm\" height=\""
        << pageHeight << "mm\" " << std::endl;
    out << "     viewBox=\"0 0 "
        << pageWidth * ppmm  << " "
        << pageHeight * ppmm  << "\" " << std::endl;
    out << "     xmlns=\"http://www.w3.org/2000/svg\""
	<< " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
	<< " version=\"1.1\" >"
	<< std::endl;
  } else {
    out << "<svg width=\""
        << ( bbox.width / ppmm )  << "mm"
        << "\" height=\""
        << ( bbox.height / ppmm ) << "mm"
        << "\" " << std::endl;
    out << "     viewBox=\"0 0 "
        << bbox.width  << " "
        << bbox.height << "\" " << std::endl;
    out << "     xmlns=\"http://www.w3.org/2000/svg\""
	<< " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
	<< " version=\"1.1\" >"
	<< std::endl;
  }

  out << "<desc>"
         "Drawing created with the Board library (Copyleft) 2007 Sebastien Fourey"
         "</desc>" << std::endl;

  if ( clipping  ) {
    out << "<g clip-rule=\"nonzero\">\n"
        << " <clipPath id=\"GlobalClipPath\">\n"
        << "  <path clip-rule=\"evenodd\"  d=\"";
    _clippingPath.flushSVGCommands( out, transform );
    out << "\" />\n";
    out << " </clipPath>\n";
    out << "<g clip-path=\"url(#GlobalClipPath)\">\n";
  }

  // Draw the background color if needed.
  if ( _backgroundColor != Color::None ) {
    Rectangle r( bbox, Color::None, _backgroundColor, 0.0 );
    r.flushSVG( out, transform );
  }

  // Draw the shapes.
  std::vector< Shape* > shapes = _shapes;
  stable_sort( shapes.begin(), shapes.end(), shapeGreaterDepth );
  std::vector< Shape* >::const_iterator i = shapes.begin();
  std::vector< Shape* >::const_iterator end = shapes.end();
  while ( i != end ) {
    (*i)->flushSVG( out, transform );
    ++i;
  }

  if ( clipping )
    out << "</g>\n</g>";
  out << "</svg>" << std::endl;
}

void
Board::saveSVG( const char * filename, double pageWidth, double pageHeight, double margin ) const
{
  std::ofstream out( filename );
  saveSVG(out,pageWidth,pageHeight,margin);
  out.close();
}

void
Board::saveTikZ( const char * filename, PageSize size, double margin ) const
{
  saveTikZ( filename, pageSizes[size][0], pageSizes[size][1], margin );
}

void
Board::saveTikZ( std::ostream & out, PageSize size, double margin ) const
{
  saveTikZ( out, pageSizes[size][0], pageSizes[size][1], margin );
}

void
Board::saveTikZ( std::ostream & out, double pageWidth, double pageHeight, double margin ) const
{
  TransformTikZ transform;
  Rect box = boundingBox();
  bool clipping = _clippingPath.size() > 2;
  if ( clipping )
    box = box && _clippingPath.boundingBox();
  transform.setBoundingBox( box, pageWidth, pageHeight, margin );

  out << "\\begin{tikzpicture}[anchor=south west,text depth=0,x={(1pt,0pt)},y={(0pt,-1pt)}]" << std::endl;

  if ( clipping  ) {
    out << "\\clip ";
    _clippingPath.flushTikZPoints( out, transform );
    out << "\n";
  }

  // Draw the background color if needed.
  if ( _backgroundColor != Color::None ) {
    Rectangle r( box, Color::None, _backgroundColor, 0.0 );
    r.flushTikZ( out, transform );
  }

  // Draw the shapes.
  std::vector< Shape* > shapes = _shapes;
  stable_sort( shapes.begin(), shapes.end(), shapeGreaterDepth );
  std::vector< Shape* >::const_iterator i = shapes.begin();
  std::vector< Shape* >::const_iterator end = shapes.end();
  while ( i != end ) {
    (*i)->flushTikZ( out, transform );
    ++i;
  }
  out << "\\end{tikzpicture}" << std::endl;
}

void
Board::saveTikZ( const char * filename, double pageWidth, double pageHeight, double margin ) const
{
  std::ofstream out( filename );
  saveTikZ(out,pageWidth,pageHeight,margin);
  out.close();
}

void
Board::save( const char * filename, double pageWidth, double pageHeight, double margin ) const
{
  const char * extension = filename + strlen( filename );
  while ( extension > filename && *extension != '.' )
    --extension;
  if ( !(strcmp( extension, ".eps" )) || !(strcmp( extension, ".EPS" )) ) {
    saveEPS( filename, pageWidth, pageHeight, margin );
    return;
  }
  if ( !(strcmp( extension, ".fig" )) || !(strcmp( extension, ".FIG" )) ) {
    saveFIG( filename, pageWidth, pageHeight, margin );
    return;
  }
  if ( !(strcmp( extension, ".svg" )) || !(strcmp( extension, ".SVG" )) ) {
    saveSVG( filename, pageWidth, pageHeight, margin );
    return;
  }
  if ( !(strcmp( extension, ".tikz" )) || !(strcmp( extension, ".TIKZ" )) ) {
    saveTikZ( filename, pageWidth, pageHeight, margin );
    return;
  }
}

void
Board::save( const char * filename, PageSize size, double margin ) const
{
  save( filename, pageSizes[size][0], pageSizes[size][1], margin );
}

} // namespace LibBoard;

/**
 * @example examples/arithmetic.cpp
 * @example examples/arrows.cpp
 * @example examples/clipping.cpp
 * @example examples/ellipse.cpp
 * @example examples/example1.cpp
 * @example examples/example2.cpp
 * @example examples/example3.cpp
 * @example examples/example4.cpp
 * @example examples/flag.cpp
 * @example examples/graph.cpp
 * @example examples/koch.cpp
 * @example examples/line_style.cpp
 * @example examples/line_segment.cpp
 * @example examples/logo.cpp
 * @example examples/ruler.cpp
 * @example examples/scale_ellipse.cpp
 * @example examples/Makefile
 */

/**
 * @mainpage LibBoard - A C++ library for simple Postscript, SVG, and XFig drawings
 *
 * <table border="0"><tr><td><img align=left src="http://foureys.users.greyc.fr/board/doc/LibBoardLogoII_Small.png"/></td>
 * <td>(Copyleft) 2007 S&eacute;bastien Fourey - GREYC ENSICAEN</td></tr></table>
 *
 * @section intro_sec Introduction
 *
 * The board library allows simple drawings in:
 * <ul>
 *  <li>Encapsulated Postcript files (EPS) ;
 *  <li>XFig files (FIG) ;
 *  <li>Scalable Vector Graphics files (SVG).
 * </ul>
 *
 * The main class of the library is the #LibBoard#Board class. It is intented to
 * be as simple as possible so that it can be used quickly in programs to
 * generate the kind of figure one would rather not draw by hand, but which can
 * be easily drawn by a computer (C++) program.
 *
 * @section examples_sec Code examples
 *
 * <ul>
 * <li>Se the "Examples" tab above.
 * </ul>
 *
 * @section links_sec Links
 *
 * <ul>
 * <li>Visit the <a href="https://github.com/c-koi/libboard">official source code repository</a>.</li>
 * </ul>
 */
