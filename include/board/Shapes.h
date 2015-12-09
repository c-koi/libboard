/* -*- mode: c++ -*- */
/**
 * @file   Shapes.h
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
#ifndef _BOARD_SHAPES_H_
#define _BOARD_SHAPES_H_

#include "board/Point.h"
#include "board/Rect.h"
#include "board/Path.h"
#include "board/Color.h"
#include "board/Transforms.h"
#include "board/PSFonts.h"
#include "board/Tools.h"
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <cmath>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2         1.57079632679489661923
#endif

namespace LibBoard {

/**
 * Shape structure.
 * @brief Abstract structure for a 2D shape.
 */
struct Shape {

  enum LineCap { ButtCap = 0, RoundCap, SquareCap };
  enum LineJoin { MiterJoin = 0, RoundJoin, BevelJoin };
  enum LineStyle { SolidStyle = 0,
                   DashStyle,
                   DotStyle,
                   DashDotStyle,
                   DashDotDotStyle,
                   DashDotDotDotStyle };

  /**
   * Shape constructor.
   *
   * @param penColor The pen color of the shape.
   * @param fillColor The fill color of the shape.
   * @param lineWidth The line thickness.
   * @param depth The depth of the shape.
   */
  inline Shape( Color penColor, Color fillColor,
                double lineWidth,
                LineStyle style,
                const LineCap cap,
                const LineJoin join,
                int depth );
  
  /**
   * Shape destructor.
   */
  virtual ~Shape() { }

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  virtual const std::string & name() const;

  /**
   * Return a copy of the shape.
   *
   * @return A copy of the shape.
   */
  virtual Shape * clone() const = 0;
  
  /**
   * Checks whether a shape is filled with a color or not.
   *
   * @return true if the shape is filled.
   */
  inline bool filled() const { return _fillColor != Color::None; }
  
  /**
   * Returns the gravity center of the shape.
   *
   * @return The center of the shape, i.e. the center of its bounding box.
   */
  virtual Point center() const = 0;
  
  /**
   * Rotate the shape around a given center of rotation.
   *
   * @param angle The rotation angle in radian.
   * @param center The center of rotation.
   *
   * @return A reference to the shape itself.
   */
  virtual Shape & rotate( double angle, const Point & center ) = 0;

  /**
   * Rotate the shape around its center.
   *
   * @param angle The rotation angle in radian.
   *
   * @return A reference to the shape itself.
   */
  virtual Shape & rotate( double angle ) = 0;

  /**
   * Rotate the shape around a given center of rotation.
   *
   * @param angle The rotation angle in degree.
   * @param center The center of rotation.
   *
   * @return A reference to the shape itself.
   */
  inline Shape & rotateDeg( double angle, const Point & center );
  
  /**
   * Rotate the shape around its center.
   *
   * @param angle The rotation angle in degree.
   * @param center The center of rotation.
   *
   * @return A reference to the shape itself.
   */
  inline Shape & rotateDeg( double angle );

  /**
   * Translate the shape by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the shape itself.
   */
  virtual Shape & translate( double dx, double dy ) = 0;

  /**
   * Scale the shape along the x an y axis.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return The shape itself.
   */
  virtual Shape & scale( double sx, double sy ) = 0;

  /**
   * Scale the shape along both axis.
   *
   * @param s The scaling factor along both axis.
   *
   * @return The shape itself.
   */
  virtual Shape & scale( double s ) = 0;

  /**
   * Compute the bounding box of the figure.
   *
   * @return The rectangle of the bounding box.
   */
  virtual Rect boundingBox() const = 0;

  /**
   * Compute the bounding box of the figure. (Convenience method to call
   * "boundingBox" with a short name.)
   *
   */
  inline Rect bbox();
  
  /**
   * Decrement the depth of the shape. (Pull the shape toward the foreground.)
   *
   * @return A reference to the shape itself.
   */
  inline Shape & operator--();

  /**
   * Increment the depth of the shape. (Push the shape toward the background.)
   *
   * @return A reference to the shape itself.
   */
  inline Shape & operator++();

  /**
   * Scale all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  virtual void scaleAll( double s ) = 0;

  /**
   * Write the EPS code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  virtual void flushPostscript( std::ostream & stream,
                                const TransformEPS & transform ) const = 0;

  /**
   * Write the FIG code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  virtual void flushFIG( std::ostream & stream,
                         const TransformFIG & transform,
                         std::map<Color,int> & colormap ) const = 0;

  /**
   * Write the SVG code of the shape in a stream according to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  virtual void flushSVG( std::ostream & stream,
                         const TransformSVG & transform ) const = 0;

  /**
   * Write the TikZ code of the shape in a stream according to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  virtual void flushTikZ( std::ostream & stream,
                          const TransformTikZ & transform ) const = 0;

  inline int depth() const;

  virtual void depth( int );

  virtual void shiftDepth( int shift );
  
  inline const Color & penColor() const;

  inline const Color & fillColor() const;

private:

  static const std::string _name;       /**< The generic name of the shape. */
  
protected:

  int _depth;          /**< The depth of the shape. */
  Color _penColor;     /**< The color of the shape. */
  Color _fillColor;    /**< The color of the shape. */
  double _lineWidth;   /**< The line thickness. */
  LineStyle _lineStyle;/**< The line style (solid, dashed, etc.). */
  LineCap _lineCap;    /**< The linecap attribute. (The way line terminates.) */
  LineJoin _lineJoin;  /**< The linejoin attribute. (The shape of line junctions.) */

  /**
   * Return a string of the svg properties lineWidth, opacity, penColor, fillColor,
   * lineCap, and lineJoin.
   *
   * @return A string of the properties suitable for inclusion in an svg tag.
   */
  std::string svgProperties( const TransformSVG & transform ) const;

  
  /**
   * Return a string of the properties lineWidth, penColor, lineCap, and lineJoin
   * as Postscript commands.
   * @return A string of the Postscript commands.
   */
  std::string postscriptProperties() const;

  /**
   * Return a string of the properties lineWidth, penColor, lineCap, and lineJoin
   * as TikZ commands.
   * @return A string of the TikZ commands.
   */
  std::string tikzProperties( const TransformTikZ & transform ) const;

};

/**
 * The dot structure. This primitive ha been reported as "missing" by
 * Manuel Peinado from Spain. Manuel also suggested
 * that rendering a dot consists in drawing a line
 * from a point to the same point with a roundcap style.
 *
 * @brief A line between two points.
 */
struct Dot : public Shape { 
  
  inline Dot( double x, double y,
              Color color,
              double lineWidth,
              int depth = -1 );

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const;
  
  Point center() const;

  /**
   * Rotates the dot around a given center of rotation.
   *
   * @param angle The rotation angle.
   * @param center The center of rotation.
   *
   * @return A reference to the Dot itself.
   */
  Dot & rotate( double angle, const Point & center );

  /**
   * Returns a rotated copy of the dot around a given rotation center.
   *
   * @param angle Th rotation angle.
   * @param center The center of rotation.
   *
   * @return A rotated copy of the Dot.
   */
  Dot rotated( double angle, const Point & center ) const;

  /**
   * Rotates the dot around its center (actually does nothing!).
   *
   * @param angle The rotation angle.
   *
   * @return A reference to the Dot itself.
   */
  Dot & rotate( double angle );

  /**
   * Returns a copy of the dot rotated around its center (i.e. left unchanged!)
   *
   * @param angle The rotation angle.
   *
   * @return A copy of the dot.
   */
  Dot rotated( double angle ) const;

  /**
   * Translate the dot by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the dot itself.
   */
  Dot & translate( double dx, double dy );

  /**
   * Returns a translated copy of the dot.
   *
   * @param dx Shift of the first coordinate.
   * @param dy Shift of the second coordinate.
   *
   * @return A copy of the dot, translated.
   */
  Dot translated( double dx, double dy ) const;

  /**
   * Scale the dot, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   *
   * @return A reference to the dot itself, once scaled.
   */
  Dot & scale( double sx, double sy );

  /**
   * Scale the dot, given a scaling factor.
   *
   * @param s Scaling factor.
   *
   * @return A reference to the dot itself, once scaled.
   */
  Dot & scale( double s );

  /**
   * Returns a scaled copy of the dot, i.e. the dot itself.
   * (Dots are not actually scaled since their diameters is given by
   * the line width.)
   *
   * @param sx The scaling factor around the x axis.
   * @param sy The scaling factor around the y axis.
   *
   * @return A scaled copy of the Dot, i.e. the Dot itself.
   */
  Dot scaled( double sx, double sy ) const;

  /**
   * Returns a scaled copy of the dot, i.e. the dot itself.
   * (Dots are not actually scaled since their diameters is given by
   * the line width.)
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the Dot, i.e. the Dot itself.
   */
  Dot scaled( double s ) const;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll( double s );

  void flushPostscript( std::ostream & stream,
                        const TransformEPS & transform ) const;
  
  void flushFIG( std::ostream & stream,
                 const TransformFIG & transform,
                 std::map<Color,int> & colormap ) const;

  void flushSVG( std::ostream & stream,
                 const TransformSVG & transform ) const;

  void flushTikZ( std::ostream & stream,
                  const TransformTikZ & transform ) const;
  
  Rect boundingBox() const;

  Dot * clone() const;

private:

  static const std::string _name; /**< The generic name of the shape. */

protected:
  double _x;  /**< First coordinate of the dot. */
  double _y;  /**< Second coordinate of the dot. */
};

/**
 * The line structure.
 * @brief A line between two points.
 */
struct Line : public Shape { 
  
  /**
   * Constructs a line.
   *
   * @param x1 First coordinate of the start point.
   * @param y1 Second coordinate of the start point.
   * @param x2 First coordinate of the end point.
   * @param y2 Second coordinate of the end point.
   * @param color The color of the line.
   * @param lineWidth The line thickness.
   * @param depth The depth of the line.
   */
  inline Line( double x1, double y1, double x2, double y2,
               Color color,
               double lineWidth,
               const LineStyle style = SolidStyle,
               const LineCap cap = ButtCap,
               const LineJoin join = MiterJoin,
               int depth = -1 );

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const;

  Point center() const;

  Line & rotate( double angle, const Point & center );

  /**
   * Returns a copy of the line, rotated around a given rotation center.
   *
   * @param angle The rotation angle.
   * @param center The center of rotation.
   *
   * @return The rotated copy of the line.
   */
  Line rotated( double angle, const Point & center ) const;

  Line & rotate( double angle );

  /**
   * Returns a copy of the line, rotated around its center.
   *
   * @param angle The angle of rotation.
   *
   * @return A copy of the lines, rotated around its center by the given angle.
   */
  Line rotated( double angle ) const;


  /**
   * Translate the line by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the line itself.
   */
  Line & translate( double dx, double dy );

  /**
   * Returns a translated copy of the line.
   *
   * @param dx The shift along the x axis.
   * @param dy The shift along the y axis.
   *
   * @return A translated copy of the line.
   */
  Line translated( double dx, double dy ) const;

  /**
   * Scale the line, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   *
   * @return A reference to the line itself, once scaled.
   */
  Line & scale( double sx, double sy );

  /**
   * Scale the line, given a scaling factor.
   *
   * @param s Scaling factor.
   *
   * @return A reference to the line itself, once scaled.
   */
  Line & scale( double s );

  /**
   * Returns a scaled copy of the line.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy scaling factor along the y axis.
   *
   * @return A scaled copy of the line.
   */
  Line scaled( double sx, double sy ) const;

  /**
   * Returns a scaled copy of the line.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the line.
   */
  Line scaled( double s ) const;
  
  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll( double s );

  void flushPostscript( std::ostream & stream,
                        const TransformEPS & transform ) const;
  
  void flushFIG( std::ostream & stream,
                 const TransformFIG & transform,
                 std::map<Color,int> & colormap ) const;

  void flushSVG( std::ostream & stream,
                 const TransformSVG & transform ) const;

  void flushTikZ( std::ostream & stream,
                  const TransformTikZ & transform ) const;

  Rect boundingBox() const;

  Line * clone() const;

private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
  double _x1;   /**< First coordinate of the start point. */
  double _y1;   /**< Second coordinate of the start point. */
  double _x2;   /**< First coordinate of the end point. */
  double _y2;   /**< Second coordinate of the end point. */
};

/**
 * The arrow structure.
 * @brief A line between two points with an arrow at one extremity.
 */
struct Arrow : public Line { 

  /**
   * Constructs an arrow.
   *
   * @param x1 First coordinate of the start point.
   * @param y1 Second coordinate of the start point.
   * @param x2 First coordinate of the end point.
   * @param y2 Second coordinate of the end point.
   * @param penColor The color of the line.
   * @param fillColor The fill color of the sharp end.
   * @param lineWidth The line thickness.
   * @param depth The depth of the line.
   */
  inline Arrow( double x1, double y1, double x2, double y2,
                Color penColor, Color fillColor,
                double lineWidth,
                const LineStyle style = SolidStyle,
                const LineCap cap = ButtCap,
                const LineJoin join = MiterJoin,
                int depth = -1 );

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const;
  
  /**
   * Returns a copy of the arrow, rotated around a given rotation center.
   *
   * @param angle The rotation angle.
   * @param center The center of rotation.
   *
   * @return The rotated copy of the line.
   */
  Arrow rotated( double angle, const Point & center ) const;
  
  /**
   * Returns a copy of the arrow, rotated around its center.
   *
   * @param angle The angle of rotation.
   *
   * @return A copy of the lines, rotated around its center by the given angle.
   */
  Arrow rotated( double angle ) const;

  /**
   * Returns a translated copy of the arrow.
   *
   * @param dx The shift along the x axis.
   * @param dy The shift along the y axis.
   *
   * @return A translated copy of the line.
   */
  Arrow translated( double dx, double dy ) const;

  /**
   * Returns a scaled copy of the arrow.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy scaling factor along the y axis.
   *
   * @return A scaled copy of the arrow.
   */
  Arrow scaled( double sx, double sy ) const;

  /**
   * Returns a scaled copy of the arrow.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the arrow.
   */
  Arrow scaled( double s ) const;

  void flushPostscript( std::ostream & stream,
                        const TransformEPS & transform ) const;
  
  void flushFIG( std::ostream & stream,
                 const TransformFIG & transform,
                 std::map<Color,int> & colormap ) const;

  void flushSVG( std::ostream & stream,
                 const TransformSVG & transform ) const;

  void flushTikZ( std::ostream & stream,
                  const TransformTikZ & transform ) const;

  Arrow * clone() const;

private:
  static const std::string _name; /**< The generic name of the shape. */
};

/**
 * The polyline structure.
 * @brief A polygonal line described by a series of 2D points.
 */
struct Polyline : public Shape {

  inline Polyline( const std::vector<Point> & points,
                   bool closed,
                   Color penColor, Color fillColor,
                   double lineWidth,
                   const LineStyle lineStyle = SolidStyle,
                   const LineCap cap = ButtCap,
                   const LineJoin join = MiterJoin,
                   int depth = -1 );

  inline Polyline( const Path & path,
                   Color penColor, Color fillColor,
                   double lineWidth,
                   const LineStyle lineStyle = SolidStyle,
                   const LineCap cap = ButtCap,
                   const LineJoin join = MiterJoin,
                   int depth = -1 );

  inline Polyline( bool closed, Color penColor, Color fillColor,
                   double lineWidth,
                   const LineStyle lineStyle = SolidStyle,
                   const LineCap cap = ButtCap,
                   const LineJoin join = MiterJoin,
                   int depth = -1 );

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const;

  Point center() const;

  /**
   * Add a point to the polyline.
   *
   * @param p
   *
   * @return
   */
  Polyline & operator<<( const Point & p );

  /**
   * Returns the n-th point of the polyline.
   *
   * @param i
   *
   * @return
   */
  Point & operator[]( const std::size_t n ) {
    return _path[ n ];
  }

  /**
   * Returns the n-th point of the polyline.
   *
   * @param i
   *
   * @return
   */
  const Point & operator[]( const std::size_t n ) const {
    return _path[ n ];
  }

  Polyline & rotate( double angle, const Point & center );

  /**
   *
   *
   * @param angle
   * @param center
   *
   * @return
   */
  Polyline rotated( double angle, const Point & center ) const;

  Polyline & rotate( double angle );
  
  /**
   *
   *
   * @param angle
   *
   * @return
   */
  Polyline rotated( double angle ) const;

  /**
   * Translate the polyline by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the polyline itself.
   */
  Polyline & translate( double dx, double dy );
  
  /**
   * Returns a translated copy of the polyline.
   *
   * @param dx Shift of the first coordinate.
   * @param dy Shift of the second coordinate.
   *
   * @return A copy of the polyline, translated.
   */
  Polyline translated( double dx, double dy ) const;

  /**
   * Scale the polyline, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   *
   * @return A reference to the polyline itself, once scaled.
   */
  Polyline & scale( double sx, double sy );

  /**
   * Scale the polyline, given a scaling factor.
   *
   * @param s Scaling factor.
   *
   * @return A reference to the polyline itself, once scaled.
   */
  Polyline & scale( double s );
  
  /**
   * Returns a scaled copy of the line.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return A scaled copy of the line.
   */
  Polyline scaled( double sx, double sy )  const;

  /**
   * Returns a scaled copy of the line.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the line.
   */
  Polyline scaled( double s )  const;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll( double s );

  void flushPostscript( std::ostream & stream,
                        const TransformEPS & transform ) const;

  void flushFIG( std::ostream & stream,
                 const TransformFIG & transform,
                 std::map<Color,int> & colormap ) const;

  void flushSVG( std::ostream & stream,
                 const TransformSVG & transform ) const;

  void flushTikZ( std::ostream & stream,
                  const TransformTikZ & transform ) const;

  Rect boundingBox() const;

  Polyline * clone() const;

  inline std::size_t vertexCount() const;

private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
  Path _path;
};

/**
 * The rectangle structure.
 * @brief A rectangle.
 */
struct Rectangle : public Polyline {

  inline Rectangle( double left, double top, double width, double height,
                    Color penColor, Color fillColor,
                    double lineWidth,
                    const LineStyle style = SolidStyle,
                    const LineCap cap = ButtCap,
                    const LineJoin join = MiterJoin,
                    int depth = -1 );

  inline Rectangle( const Rect & rect,
                    Color penColor, Color fillColor,
                    double lineWidth,
                    const LineStyle style = SolidStyle,
                    const LineCap cap = ButtCap,
                    const LineJoin join = MiterJoin,
                    int depth = -1 );

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const;
  double x() const { return _path[0].x; }
  double y() const { return _path[0].y; }
  double width() const { return (_path[1] - _path[0]).norm(); }
  double height() const { return (_path[0] - _path[3]).norm(); }
  Point topLeft() const { return Point( _path[0].x, _path[0].y ); }
  Point topRight() const { return Point( _path[1].x, _path[1].y ); }
  Point bottomLeft() const { return Point( _path[3].x, _path[3].y ); }
  Point bottomRight() const { return Point( _path[2].x, _path[2].y ); }

  /**
   * Returns a copy of the arrow, rotated around a given rotation center.
   *
   * @param angle The rotation angle.
   * @param center The center of rotation.
   *
   * @return The rotated copy of the line.
   */
  Rectangle rotated( double angle, const Point & center )  const;
  
  /**
   * Returns a copy of the arrow, rotated around its center.
   *
   * @param angle The angle of rotation.
   *
   * @return A copy of the lines, rotated around its center by the given angle.
   */
  Rectangle rotated( double angle ) const;

  /**
   * Returns a translated copy of the rectangle.
   *
   * @param dx The shift along the x axis.
   * @param dy The shift along the y axis.
   *
   * @return A translated copy of the rectangle.
   */
  Rectangle translated( double dx, double dy ) const;

  /**
   * Returns a scaled copy of the rectangle.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy scaling factor along the y axis.
   *
   * @return A scaled copy of the rectangle.
   */
  Rectangle scaled( double sx, double sy ) const;

  /**
   * Returns a scaled copy of the rectangle.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the rectangle.
   */
  Rectangle scaled( double s ) const;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll( double s );

  void flushFIG( std::ostream & stream,
                 const TransformFIG & transform,
                 std::map<Color,int> & colormap ) const;

  void flushSVG( std::ostream & stream,
                 const TransformSVG & transform ) const;

  void flushTikZ( std::ostream & stream,
                  const TransformTikZ & transform ) const;

  Rectangle * clone() const;

private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
  bool _isRectangle;
};


/**
 * The Triangle structure.
 * @brief A triangle. Basically a Polyline with a convenient constructor.
 */
struct Triangle : public Polyline {

  Triangle( const Point & p1, const Point & p2, const Point & p3,
            Color penColor, Color fillColor,
            double lineWidth,
            const LineStyle style = SolidStyle,
            const LineCap cap = ButtCap,
            const LineJoin join = MiterJoin,
            int depth = -1 )
    : Polyline( std::vector<Point>(), true, penColor, fillColor, lineWidth, style, cap, join, depth ) {
    _path << p1;
    _path << p2;
    _path << p3;
  }

  Triangle( const double x1, const double y1,
            const double x2, const double y2,
            const double x3, const double y3,
            Color penColor, Color fillColor,
            double lineWidth,
            const LineStyle style = SolidStyle,
            const LineCap cap = ButtCap,
            const LineJoin join = MiterJoin,
            int depth = -1 )
    : Polyline( std::vector<Point>(), true, penColor, fillColor, lineWidth, style, cap, join, depth ) {
    _path << Point( x1, y1 );
    _path << Point( x2, y2 );
    _path << Point( x3, y3 );
  }

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const;

  Triangle rotated( double angle ) const;

  /**
   * Returns a translated copy of the triangle.
   *
   * @param dx The shift along the x axis.
   * @param dy The shift along the y axis.
   *
   * @return A translated copy of the triangle.
   */
  Triangle translated( double dx, double dy ) const;

  /**
   * Returns a scaled copy of the triangle.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy scaling factor along the y axis.
   *
   * @return A scaled copy of the arrow.
   */
  Triangle scaled( double sx, double sy ) const;

  /**
   * Returns a scaled copy of the triangle.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the triangle.
   */
  Triangle scaled( double s ) const;

  Triangle * clone() const;
  
private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
};


/**
 * The GouraudTriangle structure.
 * @brief A triangle with shaded filling according to colors given for each vertex.
 */
struct GouraudTriangle : public Polyline {


  GouraudTriangle( const Point & p0, const Color & color0,
                   const Point & p1, const Color & color1,
                   const Point & p2, const Color & color2,
                   int subdivisions,
                   int depth = -1 );

  GouraudTriangle( const Point & p0, float brightness0,
                   const Point & p1, float brightness1,
                   const Point & p2, float brightness2,
                   const Color & fillColor,
                   int subdivisions,
                   int depth = -1 );

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const;

  Point center() const;

  GouraudTriangle & rotate( double angle, const Point & center );

  GouraudTriangle rotated( double angle, const Point & center ) const;

  GouraudTriangle & rotate( double angle );

  GouraudTriangle rotated( double angle ) const;

  /**
   * Returns a translated copy of the triangle.
   *
   * @param dx The shift along the x axis.
   * @param dy The shift along the y axis.
   *
   * @return A translated copy of the triangle.
   */
  GouraudTriangle translated( double dx, double dy ) const;

  /**
   * Returns a scaled copy of the triangle.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy scaling factor along the y axis.
   *
   * @return A scaled copy of the arrow.
   */
  GouraudTriangle scaled( double sx, double sy ) const;

  /**
   * Returns a scaled copy of the triangle.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the tiangle.
   */
  GouraudTriangle scaled( double s ) const;


  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll( double s );

  /**
   * Sends the triangle to a Postscript document.
   *
   * @param stream
   * @param transform
   */
  void flushPostscript( std::ostream & stream,
                        const TransformEPS & transform ) const;

  /**
   * Sends the Triangle to a FIG file format stream.
   * <p><b>Warning!</b> Because shading would generally require
   * more colors in the colormap than allowed by the FIG file format,
   * rendering a Gouraud triangle in an XFig file is the same as rendering
   * a simple triangle whose filling color is the average of the vertex colors.
   *
   * @param stream
   * @param transform
   * @param Color
   * @param colormap
   */
  void flushFIG( std::ostream & stream,
                 const TransformFIG & transform,
                 std::map<Color,int> & colormap ) const;

  void flushSVG( std::ostream & stream,
                 const TransformSVG & transform ) const;

  void flushTikZ( std::ostream & stream,
                  const TransformTikZ & transform ) const;

  GouraudTriangle * clone() const;

private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
  Color _color0;
  Color _color1;
  Color _color2;
  int _subdivisions;
};

/**
 * The ellipse structure.
 * @brief An ellipse.
 */
struct Ellipse : public Shape {
  
  Ellipse( double x, double y,
           double xRadius, double yRadius,
           Color penColor, Color fillColor,
           double lineWidth,
           const LineStyle lineStyle = SolidStyle,
           int depth = -1 )
    : Shape( penColor, fillColor,
             lineWidth, lineStyle, ButtCap, MiterJoin, depth ),
      _center( x, y ), _xRadius( xRadius ), _yRadius( yRadius ),
      _angle( 0.0 ),
      _circle( false ) {
    while ( _angle > M_PI_2 ) _angle -= M_PI;
    while ( _angle < -M_PI_2 ) _angle += M_PI;
  }

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const;

  Point center() const;

  Ellipse & rotate( double angle, const Point & center );

  /**
   *
   *
   * @param angle
   * @param center
   *
   * @return
   */
  Ellipse rotated( double angle, const Point & center ) const;

  Ellipse & rotate( double angle );

  /**
   *
   *
   * @param angle
   *
   * @return
   */
  Ellipse rotated( double angle ) const;
  
  /**
   * Translate the ellipse by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the ellipse itself.
   */
  Ellipse & translate( double dx, double dy );

  /**
   * Returns a translated copy of the ellipse.
   *
   * @param dx Shift of the first coordinate.
   * @param dy Shift of the second coordinate.
   *
   * @return A copy of the ellipse, translated.
   */
  Ellipse translated( double dx, double dy ) const;

  /**
   * Scale the polyline, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   *
   * @return A reference to the polyline itself, once scaled.
   */
  Ellipse & scale( double sx, double sy );

  /**
   * Scale the ellipse, given a scaling factor.
   *
   * @param s Scaling factor.
   *
   * @return A reference to the ellipse itself, once scaled.
   */
  Ellipse & scale( double s );

  /**
   * Returns a scaled copy of the ellipse.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return A scaled copy of the ellipse.
   */
  Ellipse scaled( double sx, double sy ) const;

  /**
   * Returns a scaled copy of the ellipse.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the ellipse.
   */
  Ellipse scaled( double s ) const;
  
  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll( double s );

  void flushPostscript( std::ostream & stream,
                        const TransformEPS & transform ) const;

  void flushFIG( std::ostream & stream,
                 const TransformFIG & transform,
                 std::map<Color,int> & colormap ) const;

  void flushSVG( std::ostream & stream,
                 const TransformSVG & transform ) const;

  void flushTikZ( std::ostream & stream,
                  const TransformTikZ & transform ) const;

  Rect boundingBox() const;

  Ellipse * clone() const;

private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
  Point _center;
  double _xRadius;
  double _yRadius;
  double _angle;
  bool _circle;
};

/**
 * The circle structure.
 * @brief A circle.
 */
struct Circle : public Ellipse {

  Circle( double x, double y, double radius,
          Color penColor, Color fillColor,
          double lineWidth,
          const LineStyle style = SolidStyle,
          int depth = -1 )
    : Ellipse( x, y, radius, radius, penColor, fillColor, lineWidth, style, depth )
  { _circle = true; }

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const;

  Point center() const;

  Circle & rotate( double angle, const Point & center );

  Circle rotated( double angle, const Point & center ) const;

  Circle & rotate( double angle );

  Circle rotated( double angle ) const;
  
  /**
   * Translate the circle by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the circle itself.
   */
  Circle & translate( double dx, double dy );

  /**
   * Returns a translated copy of the circle.
   *
   * @param dx Shift of the first coordinate.
   * @param dy Shift of the second coordinate.
   *
   * @return A copy of the circle, translated.
   */
  Circle translated( double dx, double dy ) const;

  /**
   * Scale the circle, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   *
   * @return A reference to the circle itself, once scaled.
   */
  Circle & scale( double sx, double sy );

  /**
   * Scale the circle, given a scaling factor.
   *
   * @param s Scaling factor.
   *
   * @return A reference to the circle itself, once scaled.
   */
  Circle & scale( double s );

  /**
   * Returns a scaled copy of the circle.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return A scaled copy of the circle.
   */
  Circle scaled( double sx, double sy ) const;

  /**
   * Returns a scaled copy of the circle.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the circle.
   */
  Circle scaled( double s ) const;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll( double s );

  void flushSVG( std::ostream & stream,
                 const TransformSVG & transform ) const;

  void flushTikZ( std::ostream & stream,
                  const TransformTikZ & transform ) const;

  Circle * clone() const;

private:
  static const std::string _name; /**< The generic name of the shape. */
};

/**
 * The text structure.
 * @brief A piece of text.
 */
struct Text : public Shape {
  
  /**
   * Create a Text sctucture.
   *
   * @param x
   * @param y
   * @param text
   * @param font
   * @param size The font size expressed in 1/72 inches.
   * @param color
   * @param depth
   *
   * @return
   */
  Text( double x, double y,
        const std::string & text,
        const Fonts::Font font,
        double size,
        Color color = Color::Black,
        int depth = -1 )
    : Shape( color, Color::None, 1.0, SolidStyle, ButtCap, MiterJoin, depth ),
      _position( x, y ), _text( text ), _font( font ),
      _angle( 0.0 ), _size( size ),
      _xScale( 1.0 ), _yScale( 1.0 ) { }

  
  /**
   * Create a Text sctucture.
   *
   * @param x
   * @param y
   * @param text
   * @param font
   * @param svgFont The font family for an SVG file. (E.g. "Verdana, Arial" or "'Time New Roman', Serif" )
   * @param size The font size expressed in 1/72 inches.
   * @param color
   * @param depth
   *
   * @return
   */
  Text( double x, double y,
        const std::string & text,
        const Fonts::Font font,
        const std::string & svgFont,
        double size,
        Color color = Color::Black,
        int depth = -1 )
    : Shape( color, Color::None, 1.0, SolidStyle, ButtCap, MiterJoin, depth ),
      _position( x, y ),
      _text( text ), _font( font ), _svgFont( svgFont ),
      _angle( 0.0 ),
      _size( size ),
      _xScale( 1.0 ), _yScale( 1.0 ) { }
  
  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const;

  Point center() const;

  Text & rotate( double angle, const Point & center );

  Text rotated( double angle, const Point & center ) const;

  Text & rotate( double angle );

  Text rotated( double angle ) const;
  
  /**
   * Translate the text by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the text itself.
   */
  Text & translate( double dx, double dy );

  /**
   * Returns a translated copy of the text.
   *
   * @param dx Shift of the first coordinate.
   * @param dy Shift of the second coordinate.
   *
   * @return A copy of the text, translated.
   */
  Text translated( double dx, double dy ) const;

  /**
   * Scale the text, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   *
   * @return A reference to the text itself, once scaled.
   */
  Text & scale( double sx, double sy );

  /**
   * Scale the text, given a scaling factor.
   *
   * @param s Scaling factor.
   *
   * @return A reference to the text itself, once scaled.
   */
  Text & scale( double s );

  /**
   * Returns a scaled copy of the text.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return A scaled copy of the text.
   */
  Text scaled( double sx, double sy ) const;

  /**
   * Returns a scaled copy of the text.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the text.
   */
  Text scaled( double s ) const;
  
  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll( double s );

  void flushPostscript( std::ostream & stream,
                        const TransformEPS & transform ) const;

  void flushFIG( std::ostream & stream,
                 const TransformFIG & transform,
                 std::map<Color,int> & colormap ) const;

  void flushSVG( std::ostream & stream,
                 const TransformSVG & transform ) const;

  void flushTikZ( std::ostream & stream,
                  const TransformTikZ & transform ) const;

  Rect boundingBox() const;

  Text * clone() const;

private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
  Point _position;
  std::string _text;
  Fonts::Font _font;
  std::string _svgFont;
  double _angle;
  double _size;
  double _xScale;
  double _yScale;
};

/** 
 * Compares two shapes according to their depths.
 *
 * @param s1 A pointer to a first shape.
 * @param s2 A pointer to a second shape.
 *
 * @return
 */
bool shapeGreaterDepth( const Shape *s1, const Shape *s2 );


} // namespace LibBoard

/*
 * Inline methods
 */
#include "Shapes.ih"


#endif /* _SHAPE_H_ */

