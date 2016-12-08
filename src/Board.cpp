/* -*- mode: c++ -*- */
/**
 * @file   Board.cpp
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

const float ppmm = 72.0f / 25.4f;
}

namespace LibBoard {

const double Board::Degree =  3.14159265358979323846 / 180.0;

Board::State::State()
{
  penColor = Color::Black;
  fillColor =  Color::Null;
  lineWidth = 0.5;
  lineStyle = Shape::SolidStyle;
  lineCap = Shape::ButtCap;
  lineJoin = Shape::MiterJoin;
  font = Fonts::TimesRoman;
  fontSize = 11.0;
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
  ShapeList::addShape( shape, 1.0 );
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
  if ( _clippingPath.size() ) {
    Point delta = _clippingPath.center() - center();
    delta.x *= sx;
    delta.y *= sy;
    _clippingPath.scale( sx, sy );
    ShapeList::scale( sx, sy );
    delta = ( center() + delta ) - _clippingPath.center();
    _clippingPath.translate( delta.x, delta.y );
  } else {
    ShapeList::scale( sx, sy );
  }
  return (*this);
}

Board &
Board::scale( double s )
{
  if ( _clippingPath.size() ) {
    Point delta = _clippingPath.center() - center();
    delta *= s;
    _clippingPath.scale( s );
    ShapeList::scale( s );
    delta = ( center() + delta ) - _clippingPath.center();
    _clippingPath.translate( delta.x, delta.y );
  } else {
    ShapeList::scale( s );
  }
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
    _shapes.push_back( new Dot( x, y, _state.penColor, _state.lineWidth, depth ) );
  else
    _shapes.push_back( new Dot( x, y, _state.penColor, _state.lineWidth, _nextDepth-- ) );
}

void
Board::drawLine( double x1, double y1, double x2, double y2,
                 int depth /* = -1 */  )
{
  if ( depth != -1 )
    _shapes.push_back( new Line( x1, y1,
                                 x2, y2,
                                 _state.penColor, _state.lineWidth,
                                 _state.lineStyle, _state.lineCap,
                                 _state.lineJoin, depth ) );
  else
    _shapes.push_back( new Line( x1, y1,
                                 x2, y2,
                                 _state.penColor, _state.lineWidth,
                                 _state.lineStyle, _state.lineCap,
                                 _state.lineJoin, _nextDepth-- ) );
}

void
Board::drawLine( Point p, Point q, int depth /* = -1 */  )
{
  if ( depth != -1 )
    _shapes.push_back( new Line( p.x, p.y,
                                 q.x, q.y,
                                 _state.penColor, _state.lineWidth,
                                 _state.lineStyle, _state.lineCap,
                                 _state.lineJoin, depth ) );
  else
    _shapes.push_back( new Line( p.x, p.y,
                                 q.x, q.y,
                                 _state.penColor, _state.lineWidth,
                                 _state.lineStyle, _state.lineCap,
                                 _state.lineJoin, _nextDepth-- ) );
}

void
Board::drawArrow( double x1, double y1, double x2, double y2, int depth /* = -1 */  )
{
  if ( depth != -1 )
    _shapes.push_back( new Arrow( x1, y1,
                                  x2, y2,
                                  _state.penColor,
                                  _state.fillColor,
                                  _state.lineWidth, _state.lineStyle,
                                  _state.lineCap, _state.lineJoin, depth ) );
  else
    _shapes.push_back( new Arrow( x1, y1,
                                  x2, y2,
                                  _state.penColor,
                                  _state.fillColor,
                                  _state.lineWidth, _state.lineStyle,
                                  _state.lineCap, _state.lineJoin,
                                  _nextDepth-- ) );
}

void
Board::drawArrow( Point p, Point q, int depth /* = -1 */  )
{
  if ( depth != -1 )
    _shapes.push_back( new Arrow( p.x, p.y,
                                  q.x, q.y,
                                  _state.penColor,
                                  _state.fillColor,
                                  _state.lineWidth, _state.lineStyle,
                                  _state.lineCap, _state.lineJoin, depth ) );
  else
    _shapes.push_back( new Arrow( p.x, p.y,
                                  q.x, q.y,
                                  _state.penColor,
                                  _state.fillColor,
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
  _shapes.push_back( new Rectangle( left,
                                    top,
                                    width,
                                    height,
                                    _state.penColor, _state.fillColor,
                                    _state.lineWidth, _state.lineStyle,
                                    _state.lineCap, _state.lineJoin, d ) );
}

void
Board::drawRectangle(const Rect & r, int depth)
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Rectangle( r.left,
                                    r.top,
                                    r.width,
                                    r.height,
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
  _shapes.push_back( new Rectangle( left,
                                    top,
                                    width,
                                    height,
                                    Color::Null, _state.penColor,
                                    0.0f, _state.lineStyle,
                                    _state.lineCap, _state.lineJoin,
                                    d ) );
}

void
Board::fillRectangle(const Rect & r, int depth)
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Rectangle( r.left,
                                    r.top,
                                    r.width,
                                    r.height,
                                    Color::Null, _state.penColor,
                                    0.0f, _state.lineStyle,
                                    _state.lineCap, _state.lineJoin,
                                    d ) );
}

void
Board::drawCircle( double x, double y, double radius,
                   int depth /* = -1 */  )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Circle( x, y,
                                 radius,
                                 _state.penColor, _state.fillColor,
                                 _state.lineWidth, _state.lineStyle, d ) );
}

void
Board::fillCircle( double x, double y, double radius,
                   int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Circle( x, y, radius,
                                 Color::Null, _state.penColor,
                                 0.0f, _state.lineStyle, d ) );
}

void
Board::drawEllipse( double x, double y,
                    double xRadius, double yRadius,
                    int depth /* = -1 */  )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Ellipse( x, y,
                                  xRadius, yRadius,
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
  _shapes.push_back( new Ellipse( x, y,
                                  xRadius, yRadius,
                                  Color::Null,
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
  _shapes.push_back( new Polyline( points,
                                   false,
                                   _state.penColor,
                                   _state.fillColor,
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
  _shapes.push_back( new Polyline( points, true, _state.penColor, _state.fillColor,
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
  _shapes.push_back( new Polyline( points, true, Color::Null, _state.penColor,
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
  points.push_back( Point( x1, y1 ) );
  points.push_back( Point( x2, y2 ) );
  points.push_back( Point( x3, y3 ) );
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
  points.push_back( Point( p1.x, p1.y ) );
  points.push_back( Point( p2.x, p2.y ) );
  points.push_back( Point( p3.x, p3.y ) );
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
  points.push_back( Point( x1, y1 ) );
  points.push_back( Point( x2, y2 ) );
  points.push_back( Point( x3, y3 ) );
  _shapes.push_back( new Polyline( points, true, Color::Null, _state.penColor,
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
  points.push_back( Point( p1.x, p1.y ) );
  points.push_back( Point( p2.x, p2.y ) );
  points.push_back( Point( p3.x, p3.y ) );
  _shapes.push_back( new Polyline( points, true, Color::Null, _state.penColor,
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
  _shapes.push_back( new GouraudTriangle( p1, color1,
                                          p2, color2,
                                          p3, color3,
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
  fillGouraudTriangle( p1, color1,
                       p2, color2,
                       p3, color3,
                       divisions,
                       depth );
}

void
Board::drawText( double x, double y, const char * text, int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Text( x, y, text,
                               _state.font, _state.fontSize,
                               _state.penColor, d ) );
}

void Board::drawText( Point p, const char *text, int depth )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Text( p, text, _state.font, _state.fontSize, _state.penColor, d ) );
}

void
Board::drawText( double x, double y, const std::string & str, int depth /* = -1 */ )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Text( x,
                               y,
                               str,
                               _state.font,
                               _state.fontSize,
                               _state.penColor, d ) );
}

void Board::drawText( Point p, const std::string & str, int depth )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  _shapes.push_back( new Text( p,
                               str,
                               _state.font,
                               _state.fontSize,
                               _state.penColor, d ) );
}

void
Board::drawBoundingBox( LineWidthFlag lineWidthFlag, int depth )
{
  int d = (depth!=-1) ? depth : _nextDepth--;
  Rect bbox = boundingBox(lineWidthFlag);
  _shapes.push_back( new Rectangle( bbox.left,
                                    bbox.top,
                                    bbox.width,
                                    bbox.height,
                                    _state.penColor,
                                    _state.fillColor,
                                    _state.lineWidth,
                                    _state.lineStyle,
                                    _state.lineCap,
                                    _state.lineJoin,
                                    d ) );
}

void
Board::setClippingRectangle( double x, double y, double width, double height )
{
  _clippingPath.clear();
  _clippingPath <<  Point( x, y  );
  _clippingPath <<  Point( x + width, y  );
  _clippingPath <<  Point( x + width, y - height );
  _clippingPath <<  Point( x , y - height  );
}

void
Board::setClippingRectangle( const Rect & rect)
{
  setClippingRectangle(rect.left,rect.top,rect.width,rect.height);
}

void
Board::setClippingPath(  const std::vector<Point> & points  )
{
  _clippingPath.clear();
  std::vector<Point>::const_iterator it = points.begin();
  std::vector<Point>::const_iterator end = points.end();
  while ( it != end ) {
    _clippingPath <<  *it ;
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
}

void
Board::addDuplicates( const Shape & shape, std::size_t times, double dx, double dy, double scale )
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
Board::saveEPS( const char * filename, PageSize size, double margin, Unit unit, const std::string & title  ) const
{
  if ( size == BoundingBox ) {
    if ( title == std::string() )
      saveEPS( filename, 0.0, 0.0, margin, unit, filename );
    else
      saveEPS( filename, 0.0, 0.0, margin, unit, title );
  } else {
    if ( title == std::string() )
      saveEPS( filename, pageSizes[size][0], pageSizes[size][1], toMillimeter(margin,unit), UMillimeter, filename );
    else
      saveEPS( filename, pageSizes[size][0], pageSizes[size][1], toMillimeter(margin,unit), UMillimeter, title );
  }
}

void
Board::saveEPS(std::ostream & out, PageSize size, double margin, Unit unit, const std::string & title  ) const
{
  if ( size == BoundingBox ) {
    saveEPS( out, 0.0, 0.0, margin, unit, title );
  } else {
    saveEPS( out, pageSizes[size][0], pageSizes[size][1], toMillimeter(margin,unit), UMillimeter, title );
  }
}

void
Board::saveEPS( std::ostream & out, double pageWidth, double pageHeight, double margin, Unit unit, const std::string & title ) const
{
  out << "%!PS-Adobe-2.0 EPSF-2.0" << std::endl;
  out << "%%Title: " << title << std::endl;
  out << "%%Creator: Board library (v" << _BOARD_VERSION_STRING_ << ") Copyleft 2007 Sebastien Fourey" << std::endl;
  {
    time_t t = time(0);
    char str_time[255];
    Tools::secured_ctime( str_time, &t, 255 );
    out << "%%CreationDate: " << str_time;
  }

  Rect bbox = boundingBox(UseLineWidth);
  bool clipping = _clippingPath.size() > 2;
  if ( clipping ) {
    bbox = bbox && _clippingPath.boundingBox();
  }
  TransformEPS transform;
  if ( pageWidth == 0.0 && pageHeight == 0.0 ) { // Fit to bounding box using given unit.
    transform.setBoundingBox( bbox,
                              toMillimeter(bbox.width,unit),
                              toMillimeter(bbox.height,unit),
                              -toMillimeter(margin,unit) );

    Rect page = transform.pageBoundingBox();
    out << "%%BoundingBox: "
        << std::setprecision( 8 )
        << page.left << " "
        << page.bottom() << " "
        << page.right() << " "
        << page.top << std::endl;


  } else {
    transform.setBoundingBox( bbox,
                              toMillimeter(pageWidth,unit),
                              toMillimeter(pageHeight,unit),
                              toMillimeter(margin,unit) );
    Rect page = transform.pageBoundingBox();
    out << "%%BoundingBox: "
        << std::setprecision( 8 )
        << page.left << " "
        << page.bottom() << " "
        << page.right() << " "
        << page.top << std::endl;
  }


  out << "%Magnification: 1.0000" << std::endl;
  out << "%%EndComments" << std::endl;

  out << "\n"
         "/cp {closepath} bind def\n"
         "/ef {eofill} bind def\n"
         "/gr {grestore} bind def\n"
         "/gs {gsave} bind def\n"
         "/sa {save} bind def\n"
         "/rs {restore} bind def\n"
         "/l {lineto} bind def\n"
         "/ct {curveto} bind def\n"
         "/m {moveto} bind def\n"
         "/rm {rmoveto} bind def\n"
         "/n {newpath} bind def\n"
         "/s {stroke} bind def\n"
         "/sh {show} bind def\n"
         "/slc {setlinecap} bind def\n"
         "/slj {setlinejoin} bind def\n"
         "/slw {setlinewidth} bind def\n"
         "/srgb {setrgbcolor} bind def\n"
         "/rot {rotate} bind def\n"
         "/sc {scale} bind def\n"
         "/sd {setdash} bind def\n"
         "/ff {findfont} bind def\n"
         "/sf {setfont} bind def\n"
         "/scf {scalefont} bind def\n"
         "/sw {stringwidth} bind def\n"
         "/sd {setdash} bind def\n"
         "/tr {translate} bind def\n"
         " 0.5 setlinewidth\n";

  if ( clipping ) {
    out << " newpath ";
    _clippingPath.flushPostscript( out, transform );
    out << " 0 slw clip " << std::endl;
  }

  // Draw the background color if needed.
  if ( _backgroundColor != Color::Null ) {
    Rectangle r( bbox, Color::Null, _backgroundColor, 0.0f );
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
Board::saveEPS( const char * filename, double pageWidth, double pageHeight, double margin, Unit unit, const std::string & title ) const
{
  std::ofstream out(filename);
  saveEPS(out,pageWidth,pageHeight,margin,unit,title);
  out.close();
}

void
Board::saveFIG( const char * filename, PageSize size, double margin, Unit unit ) const
{
  if ( size == BoundingBox ) {
    saveFIG( filename, 0.0, 0.0, margin, unit );
  } else {
    saveFIG( filename, pageSizes[size][0], pageSizes[size][1], toMillimeter(margin,unit), UMillimeter );
  }
}

void
Board::saveFIG( std::ostream & out, PageSize size, double margin, Unit unit ) const
{
  if ( size == BoundingBox ) {
    saveFIG( out, 0.0, 0.0, margin, unit );
  } else {
    saveFIG( out, pageSizes[size][0], pageSizes[size][1], toMillimeter(margin,unit), UMillimeter );
  }
}

void
Board::saveFIG( std::ostream & out, double pageWidth, double pageHeight, double margin, Unit unit ) const
{
  Rect bbox = boundingBox(UseLineWidth);
  TransformFIG transform;

  if ( pageWidth == 0.0 && pageHeight == 0.0 ) {
    transform.setBoundingBox( bbox,
                              toMillimeter(bbox.width,unit),
                              toMillimeter(bbox.height,unit),
                              -toMillimeter(margin,unit) );
  } else {
    transform.setBoundingBox( bbox,
                              toMillimeter(pageWidth,unit),
                              toMillimeter(pageHeight,unit),
                              toMillimeter(margin,unit) );
  }

  transform.setDepthRange( *this );

  out << "#FIG 3.2  Produced by the Board library (v"
         _BOARD_VERSION_STRING_
         ") Copyleft 2007 Sebastien Fourey\n";
  out << "Portrait\n"
         "Center\n"
         "Metric\n"
         "A4\n"
         "100.00\n"
         "Single\n"
         "-2\n"
         "1200 2\n";

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
  if ( _backgroundColor != Color::Null ) {
    Rectangle r( bbox, Color::Null, _backgroundColor, 0.0f );
    r.depth( std::numeric_limits<int>::max() );
    r.flushFIG( out, transform, colormap );
  }

  // Draw the shapes.
  i = shapes.begin();
  while ( i != end ) {
    (*i)->flushFIG( out, transform, colormap );
    ++i;
  }
}

void
Board::saveFIG( const char * filename, double pageWidth, double pageHeight, double margin, Unit unit ) const
{
  std::ofstream out( filename );
  saveFIG(out,pageWidth,pageHeight,margin,unit);
  out.close();
}

void
Board::saveSVG( const char * filename, PageSize size, double margin, Unit unit ) const
{
  if ( size == BoundingBox ) {
    saveSVG( filename, 0.0, 0.0, margin, unit );
  } else {
    saveSVG( filename, pageSizes[size][0], pageSizes[size][1], toMillimeter(margin,unit), UMillimeter );
  }
}

void
Board::saveSVG( std::ostream & out, PageSize size, double margin, Unit unit ) const
{
  if ( size == BoundingBox ) {
    saveSVG( out, 0.0, 0.0, margin, unit );
  } else {
    saveSVG( out, pageSizes[size][0], pageSizes[size][1], toMillimeter(margin,unit), UMillimeter );
  }
}

void
Board::saveSVG( const char * filename, double pageWidth, double pageHeight, double margin, Unit unit ) const
{
  std::ofstream out( filename );
  saveSVG(out,pageWidth,pageHeight,margin,unit);
  out.close();
}

void
Board::saveSVG( std::ostream & out, double pageWidth, double pageHeight, double margin, Unit unit ) const
{
  Rect bbox = boundingBox(UseLineWidth);
  TransformSVG transform;
  bool clipping = _clippingPath.size() > 2;
  if ( clipping ) {
    bbox = bbox && _clippingPath.boundingBox();
  }

  out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
  out << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"" << std::endl;
  out << " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;

  if ( pageWidth == 0.0 && pageHeight == 0.0 ) {
    transform.setBoundingBox( bbox,
                              toMillimeter(bbox.width,unit),
                              toMillimeter(bbox.height,unit),
                              -toMillimeter(margin,unit) );
    out << "<svg width=\""
        << toMillimeter(bbox.width+2*margin,unit) << "mm"
        << "\" height=\""
        << toMillimeter(bbox.height+2*margin,unit)  << "mm"
        << "\" " << std::endl;
    out << "     viewBox=\"0 0 "
        << toMillimeter(bbox.width+2*margin,unit) * ppmm   << " "
        << toMillimeter(bbox.height+2*margin,unit) * ppmm << "\" " << std::endl;
    out << "     xmlns=\"http://www.w3.org/2000/svg\""
        << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
        << " version=\"1.1\" >"
        << std::endl;
  } else {
    transform.setBoundingBox( bbox,
                              toMillimeter(pageWidth,unit),
                              toMillimeter(pageHeight,unit),
                              toMillimeter(margin,unit) );

    out << "<svg width=\""
        << toMillimeter(pageWidth,unit) << "mm\" height=\""
        << toMillimeter(pageHeight,unit) << "mm\" " << std::endl;
    out << "     viewBox=\"0 0 "
        << toMillimeter(pageWidth,unit) * ppmm  << " "
        << toMillimeter(pageHeight,unit) * ppmm  << "\" " << std::endl;
    out << "     xmlns=\"http://www.w3.org/2000/svg\""
        << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
        << " version=\"1.1\" >"
        << std::endl;
  }

  out << "<desc>"
         "Drawing created with the Board library (v" << _BOARD_VERSION_STRING_ << ") Copyleft 2007 Sebastien Fourey"
         "</desc>" << std::endl;

  if ( clipping  ) {
    out << "<g clip-rule=\"nonzero\">\n"
           " <clipPath id=\"GlobalClipPath\">\n"
           "  <path clip-rule=\"evenodd\"  d=\"";
    _clippingPath.flushSVGCommands( out, transform );
    out << "\" />\n"
           " </clipPath>\n"
           "<g clip-path=\"url(#GlobalClipPath)\">\n";
  }

  // Draw the background color if needed.
  if ( _backgroundColor != Color::Null ) {
    Rectangle r( bbox, Color::Null, _backgroundColor, 0.0 );
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
  Rect box = boundingBox(UseLineWidth);
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
  if ( _backgroundColor != Color::Null ) {
    Rectangle r( box, Color::Null, _backgroundColor, 0.0 );
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

Group
Board::makeGrid( Point topLeft,
                 size_t columns, size_t rows,
                 double width, double height,
                 Color penColor, Color fillColor, double lineWidth,
                 const LineStyle style,
                 const LineCap cap,
                 const LineJoin join,
                 int depth )
{
  Group group;
  double cellSide = width / columns;
  group << Rectangle(topLeft.x,topLeft.y,width,height,penColor,fillColor,lineWidth,style,cap,join,depth);
  Line vLine(topLeft.x+cellSide,topLeft.y,topLeft.x+cellSide,topLeft.y-height,penColor,lineWidth,style,Shape::RoundCap,join,depth);
  while ( --columns ) {
    group << vLine;
    vLine.translate(cellSide,0);
  }
  cellSide = height / rows;
  Line hLine(topLeft.x,topLeft.y-cellSide,topLeft.x+width,topLeft.y-cellSide,penColor,lineWidth,style,Shape::RoundCap,join,depth);
  while ( --rows ) {
    group << hLine;
    hLine.translate(0,-cellSide);
  }

  return group;
}

Color Board::penColor() const
{
  return _state.penColor;
}

Color Board::fillColor() const
{
  return _state.fillColor;
}

double
Board::toMillimeter(double x, Board::Unit unit)
{
  // enum Unit { UPoint, UInche, UCentimeter, UMillimeter };
  switch (unit) {
  case UPoint:
    return x * 25.4 / 72.0;
    break;
  case UInche:
    return x * 25.4;
    break;
  case UCentimeter:
    return x * 10.0;
    break;
  case UMillimeter:
    return x;
    break;
  default:
    Tools::error << "toMillimeter(): bad unit (" << unit << ")\n";
    return 0;
    break;
  }
}

void
Board::saveTikZ( const char * filename, double pageWidth, double pageHeight, double margin ) const
{
  std::ofstream out( filename );
  saveTikZ(out,pageWidth,pageHeight,margin);
  out.close();
}

void
Board::save(const char * filename, double pageWidth, double pageHeight, double margin , Unit unit) const
{
  if ( Tools::stringEndsWith(filename,".eps", Tools::CaseInsensitive) ) {
    saveEPS( filename, pageWidth, pageHeight, margin, unit );
    return;
  }
  if ( Tools::stringEndsWith(filename,".fig", Tools::CaseInsensitive) ) {
    saveFIG( filename, pageWidth, pageHeight, margin, unit );
    return;
  }
  if ( Tools::stringEndsWith(filename,".svg", Tools::CaseInsensitive) ) {
    saveSVG( filename, pageWidth, pageHeight, margin, unit );
    return;
  }
  if ( Tools::stringEndsWith(filename,".tikz", Tools::CaseInsensitive) ) {
    saveTikZ( filename, pageWidth, pageHeight, margin );
    return;
  }
}

void
Board::save(const char * filename, PageSize size, double margin , Unit unit) const
{
  if ( size == BoundingBox ) {
    save( filename, 0.0, 0.0, margin, unit );
  } else {
    save( filename, pageSizes[size][0], pageSizes[size][1], toMillimeter(margin,unit), UMillimeter );
  }
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
 * @example examples/stroke_path.cpp
 * @example examples/tilings.cpp
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
 * <li>See the "Examples" tab above or visit
 *     <a href="https://github.com/c-koi/libboard/blob/v0.9.5/EXAMPLES.md">dedicated page on GitHub</a>.
 * </ul>
 *
 * @section links_sec Links
 *
 * <ul>
 * <li>Visit the <a href="https://github.com/c-koi/libboard">official source code repository</a>.</li>
 * </ul>
 */
