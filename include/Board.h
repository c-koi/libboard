/* -*- mode: c++ -*- */
/**
 * @file   Board.h
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
 *
 * @brief  Declaration of the Board class.
 *
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
#ifndef _BOARD_BOARD_H_
#define _BOARD_BOARD_H_

#include <ostream>
#include <string>
#include <vector>

#include "BoardConfig.h"
#include "board/Point.h"
#include "board/Path.h"
#include "board/Shapes.h"
#include "board/Image.h"
#include "board/ShapeList.h"

namespace LibBoard {

/**
 * The Board class.
 * @brief Class for EPS, FIG or SVG drawings.
 * @version 0.5
 */
class Board : public ShapeList {

public:

  enum PageSize { BoundingBox = 0,
                  A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10,
                  Letter, Legal, Executive };

  enum Unit { UPoint, UInche, UCentimeter, UMillimeter };
  static const double Degree;

  enum AspectRatioFlag { IgnoreAspectRatio, KeepAspectRatio };

  /**
   * Constructs a new board and sets the background color, if any.
   *
   * @param backgroundColor A color for the drawing's background.
   */
  Board( const Color & backgroundColor = Color::Null );

  /**
   * Copy constructor.
   *
   * @param other The object to be copied.
   */
  Board( const Board & other );

  ~Board();

  /**
   * The operator =
   *
   * @param other The object to be copied.
   *
   * @return A reference to the left operand.
   */
  Board & operator=( const Board & other );


  /**
   * Add a shape to the board, using the current unit factor.
   *
   * @param shape A shape. (Might be a list of shapes, actually!)
   *
   * @return The board itself, as a ShapeList.
   */
  Board & operator<<( const Shape & shape );

  /**
   * Clears the board with a given background color.
   *
   * @param color The board background color (may be Color::None).
   */
  void clear( const Color & color = Color::Null );

  /**
   * Clears the board and set the background color from an RGB triple.
   *
   * @param red
   * @param green
   * @param blue
   */
  inline void clear( unsigned char red, unsigned char green, unsigned char blue );

  Board & rotate( double angle, const Point & center );

  Board & rotate( double angle );

  Board & translate( double dx, double dy );

  Board & scale( double sx, double sy );

  Board & scale( double s );

  Board rotated( double angle, const Point & center );

  Board rotated( double angle );

  Board translated( double dx, double dy );

  Board scaled( double sx, double sy );

  Board scaled( double s );

  /**
   * Draws a dot at coordinates (x,y).
   *
   * @param x First coordinate of the dot.
   * @param y Second coordinate of the dot.
   * @param depth Depth of the line.
   */
  void drawDot( double x, double y, int depth = -1 );

  /**
   * Draws a line from (x1,y1) to (x2,y2).
   *
   * @param x1 First coordinate of the first extremity.
   * @param y1 Second coordinate of the first extremity.
   * @param x2 First coordinate of the second extremity.
   * @param y2 Second coordinate of the second extremity.
   * @param depth Depth of the line.
   */
  void drawLine( double x1, double y1, double x2, double y2,
                 int depth = -1 );

  /**
   * Draws a line from p to q.
   *
   * @param p First extremity.
   * @param q Second extremity.
   * @param filled Whether or not the arrow is filled.
   * @param depth Depth of the line.
   */
  void drawLine( Point p, Point q, int depth = -1 );

  /**
   * Draws a line from (x1,y1) to (x2,y2) with an arrow at (x2,y2).
   *
   * @param x1 First coordinate of the first extremity.
   * @param y1 Second coordinate of the first extremity.
   * @param x2 First coordinate of the second extremity.
   * @param y2 Second coordinate of the second extremity.
   * @param depth Depth of the line.
   */
  void drawArrow( double x1, double y1, double x2, double y2, int depth = -1 );

  /**
   * Draws a line from p to q with an arrow at (x2,y2).
   *
   * @param p First extremity.
   * @param q Second extremity.
   * @param depth Depth of the line.
   */
  void drawArrow( Point p, Point q, int depth = -1 );

  /**
   * Draws a triangle.
   *
   * @param x1 First coordinate of the first vertex.
   * @param y1 Second coordinate of the first vertex.
   * @param x2 First coordinate of the second vertex.
   * @param y3 Second coordinate of the second vertex.
   * @param x3 First coordinate of the third vertex.
   * @param y3 Second coordinate of the third vertex.
   * @param depth Depth of the triangle.
   */
  void drawTriangle( double x1, double y1,
                     double x2, double y2,
                     double x3, double y3,
                     int depth = -1 );

  /**
   * Draws a triangle.
   *
   * @param p1 First vertex.
   * @param p2 Second vertex.
   * @param p3 Third vertex.
   * @param depth Depth of the triangle.
   */
  void drawTriangle( const Point & p1,
                     const Point & p2,
                     const Point & p3,
                     int depth = -1 );

  /**
   * Draws a filled triangle.
   *
   * @param x1 First coordinate of the first vertex.
   * @param y1 Second coordinate of the first vertex.
   * @param x2 First coordinate of the second vertex.
   * @param y3 Second coordinate of the second vertex.
   * @param x3 First coordinate of the third vertex.
   * @param y3 Second coordinate of the third vertex.
   * @param depth Depth of the triangle.
   */
  void fillTriangle( double x1, double y1,
                     double x2, double y2,
                     double x3, double y3,
                     int depth = -1 );

  /**
   * Draws a triangle with Gouraud-like shaded colors.
   *
   * @param p1
   * @param color1
   * @param p2
   * @param color2
   * @param p3
   * @param color3
   * @param divisions number of triangle subdivisions.
   * @param depth The depth of the triangle.
   */
  void fillGouraudTriangle( const Point & p1,
                            const Color & color1,
                            const Point & p2,
                            const Color & color2,
                            const Point & p3,
                            const Color & color3,
                            unsigned char divisions = 3,
                            int depth = -1 );

  /**
   * Draws a triangle with Gouraud-like shaded colors.
   *
   * @param x1
   * @param y1
   * @param color1
   * @param x2
   * @param y2
   * @param color2
   * @param x3
   * @param y3
   * @param color3
   * @param divisions
   * @param depth
   */
  inline void fillGouraudTriangle( const double x1, const double y1,
                                   const Color & color1,
                                   const double x2, const double y2,
                                   const Color & color2,
                                   const double x3, const double y3,
                                   const Color & color3,
                                   unsigned char divisions = 3,
                                   int depth = -1 );

  /**
   * Draws a triangle with a Gouraud-like shaded color according to
   * the current fill color and a brightness value given for each vertex.
   * @param p1
   * @param brightness1
   * @param p2
   * @param brightness2
   * @param p3
   * @param brightness3
   * @param divisions number of triangle subdivisions.
   * @param depth The depth of the triangle.
   */
  void fillGouraudTriangle( const Point & p1,
                            const float brightness1,
                            const Point & p2,
                            const float brightness2,
                            const Point & p3,
                            const float brightness3,
                            unsigned char divisions = 3,
                            int depth = -1 );

  /**
   * Draws a triangle with a Gouraud-like shaded color according to
   * the current fill color and a brightness value given for each vertex.
   *
   * @param x1
   * @param y1
   * @param brightness1
   * @param x2
   * @param y2
   * @param brightness2
   * @param x3
   * @param y3
   * @param brightness3
   * @param divisions
   * @param depth
   */
  inline void fillGouraudTriangle( const double x1, const double y1,
                                   const float brightness1,
                                   const double x2, const double y2,
                                   const float brightness2,
                                   const double x3, const double y3,
                                   const float brightness3,
                                   unsigned char divisions = 3,
                                   int depth = -1 );


  /**
   * Draws a filled triangle.
   *
   * @param p1 First vertex.
   * @param p2 Second vertex.
   * @param p3 Third vertex.
   * @param depth Depth of the triangle.
   */
  void fillTriangle( const Point & p1,
                     const Point & p2,
                     const Point & p3,
                     int depth = -1 );

  /**
   * Draws a rectangle.
   *
   * @param left First coordinate of the upper left corner.
   * @param top Second coordinate of the upper left corner.
   * @param width Width of the rectangle.
   * @param height Height of the rectangle.
   * @param depth Depth of the rectangle.
   */
  void drawRectangle( double left, double top,
                      double width, double height,
                      int depth = -1 );

  /**
   * Draws a rectangle.
   *
   * @param rect Position and size of the rectangle.
   * @param depth Depth of the rectangle.
   */
  void drawRectangle( const Rect & r, int depth = -1 );

  /**
   * Draws a rectangle filled with the current pen color.
   *
   * @param left First coordinate of the upper left corner.
   * @param top Second coordinate of the upper left corner.
   * @param width Width of the rectangle.
   * @param height Height of the rectangle.
   * @param depth Depth of the rectangle.
   */
  void fillRectangle( double left, double top,
                      double width, double height,
                      int depth = -1 );

  /**
   * Draws a rectangle filled with the current pen color.
   *
   * @param rect Position and size of the rectangle.
   * @param depth Depth of the rectangle.
   */
  void fillRectangle( const Rect & r, int depth = -1 );

  /**
   * Draws a circle.
   *
   * @param x First coordinate of the circle's center.
   * @param y Second coordinate of the circle's center.
   * @param radius Radius of the circle.
   * @param depth Depth of the circle.
   */
  void drawCircle( double x, double y, double radius,
                   int depth = -1 );

  /**
   * Draws a circle filled with the current pen color.
   *
   * @param x First coordinate of the circle's center.
   * @param y Second coordinate of the circle's center.
   * @param radius Radius of the circle.
   * @param depth Depth of the circle.
   */
  void fillCircle( double x, double y, double radius,
                   int depth = -1);

  /**
   * Draws an ellipse.
   *
   * @param x First coordinate of the circle's center.
   * @param y Second coordinate of the circle's center.
   * @param radius Radius of the circle.
   * @param depth Depth of the circle.
   */
  void drawEllipse( double x, double y,
                    double xRadius, double yRadius,
                    int depth = -1);

  /**
   * Draws an ellipse filled with the current pen color.
   *
   * @param x First coordinate of the circle's center.
   * @param y Second coordinate of the circle's center.
   * @param xRadius X axis radius of the ellipse.
   * @param yRadius Y axis radius of the ellipse.
   * @param depth Depth of the circle.
   */
  void fillEllipse( double x, double y,
                    double xRadius, double yRadius,
                    int depth = -1);

  /**
   * Draws a polygonal line.
   *
   * @param points A vector of points.
   * @param depth The depth of the polyline.
   */
  void drawPolyline( const std::vector<Point> & points,
                     int depth = -1 );

  /**
   * Draws a closed polygonal line.
   *
   * @param points A vector of points.
   * @param depth The depth of the polyline.
   */
  void drawClosedPolyline( const std::vector<Point> & points,
                           int depth = -1 );

  /**
   * Draws a filled polygon.
   *
   * @param points A vector of points.
   * @param depth The depth of the polygon.
   */
  void fillPolyline( const std::vector<Point> & points,
                     int depth = -1 );

  /**
   * Draws a string of text.
   *
   * @param x The first coordinates of the lower left corner.
   * @param y The second coordinates of the lower left corner.
   * @param text The text.
   * @param depth The depth of the text.
   */
  void drawText( double x, double y, const char * text, int depth = -1 );

  /**
   * Draws a string of text.
   *
   * @param p Position of the bottom-left corner.
   * @param text The text.
   * @param depth The depth of the text.
   */
  void drawText( Point p, const char * text, int depth = -1 );

  /**
   * Draws a string of text.
   *
   * @param x The first coordinates of the lower left corner.
   * @param y The second coordinates of the lower left corner.
   * @param text The text.
   * @param depth The depth of the text.
   */
  void drawText( double x, double y, const std::string & str, int depth = -1 );

  /**
   * Draws a string of text.
   *
   * @param p Position of the bottom-left corner.
   * @param str The text.
   * @param depth The depth of the text.
   */
  void drawText( Point p, const std::string & str, int depth = -1 );

  /**
   * Changes the current font and font size.
   *
   * @param font The name of the font.
   * @param fontSize The new font size.
   * @return The board itself.
   */
  Board & setFont( const Fonts::Font font, double fontSize );

  /**
   * Changes the font size.
   *
   * @param fontSize The new font size.
   * @return The board itself.
   */
  Board & setFontSize( double fontSize );

  /**
   * Changes the current pen color.
   *
   * @param red Red component.
   * @param green Green component.
   * @param blue Blue component.
   * @return The board itself.
   */
  Board & setPenColorRGBi( unsigned char red,
                           unsigned char green,
                           unsigned char blue,
                           unsigned char alpha = 255 );

  /**
   * Changes the current pen color.
   *
   * @param red Red
   * @param green
   * @param blue
   * @param alpha
   * @return The board itself.
   */
  Board & setPenColorRGBf(  float red,
                            float green,
                            float blue,
                            float alpha = 1.0f );

  /**
   * Changes the current pen color.
   *
   * In order to use no pen, one may set the pen color to Color::None.
   * @param color The pen color.
   * @return The board itself.
   */
  Board & setPenColor( const Color & color );


  /**
   * Changes the current fill color.
   *
   * @param red Red component.
   * @param green Green component.
   * @param blue Blue component.
   * @param alpha The opacity.
   * @return The board itself.
   */
  Board & setFillColorRGBi( unsigned char red,
                            unsigned char green,
                            unsigned char blue,
                            unsigned char alpha = 255 );

  /**
   * Changes the current fill color.
   *
   * @param red Red component.
   * @param green Green component.
   * @param blue Blue component.
   * @param alpha The opacity.
   * @return The board itself.
   */
  Board & setFillColorRGBf( float red, float green, float blue, float alpha = 1.0f );

  /**
   * Changes the current fill color.
   *
   * In order to use no fill color, one may set this color to Color::None.
   * @param color The fill color.
   * @return The board itself.
   */
  Board & setFillColor( const Color & color );

  /**
   * Changes the current line thickness.
   *
   * @param width The new line thickness.
   * @return The board itself.
   */
  Board & setLineWidth( double width );

  /**
   * Changes the current line style.
   *
   * @param style The new line style.
   * @return The board itself.
   */
  Board & setLineStyle( Shape::LineStyle style );

  /**
   * Set the line cap style.
   *
   * @param cap The cap-style which can be Shape::ButtCap,
   * Shape::RoundCap or Shape::SquareCap.
   *
   * @return The board itself.
   */
  inline Board & setLineCap( Shape::LineCap cap );

  /**
   * Set the line joine style.
   *
   * @param cap The join-style which can be Shape::MiterJoin,
   * Shape::RoundJoin or Shape::BevelJoin.
   *
   * @return The board itself.
   */
  inline Board & setLineJoin( Shape::LineJoin join );

  /**
   * Changes the background color of the whole drawing.
   *
   * @param color A color (may be Color::None).
   */
  void backgroundColor( const Color & color );

  /**
   * Draws the current drawing's bounding box as a rectangle.
   *
   * @param depth The depth of the rectangle.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   */
  void drawBoundingBox( LineWidthFlag lineWidthFlag, int depth = -1 );

  /**
   * Define a clipping rectangle for the whole drawing.
   *
   * @param x
   * @param y
   * @param width
   * @param height
   */
  void setClippingRectangle(  double x, double y, double width, double height );

  /**
   * Define a clipping rectangle for the whole drawing.
   *
   * @param rect The clipping rectangle.
   */
  void setClippingRectangle(const Rect & rect );

  /**
   * Define a clipping path for the whole drawing.
   *
   * @param points A path.
   */
  void setClippingPath(  const std::vector<Point> & points  );

  /**
   * Define a clipping path for the whole drawing.
   *
   * @param points A path.
   */
  void setClippingPath( const Path & path );

  /**
   * Insert duplicates of a shape, n times, starting at its current position
   * and iterating given translations and scalings.
   *
   * @param shape The shape to be duplicated.
   * @param times The number of duplicates.
   * @param dx The x shift.
   * @param dy The y shift.
   * @param scale A scale factor between each copy.
   */
  void addDuplicates( const Shape & shape,
                      std::size_t times,
                      double dx,
                      double dy,
                      double scale = 1.0 );

  /**
   * Insert duplicates of a shape, n times, starting at its current position
   * and iterating a given translation.
   *
   * @param shape The shape to be duplicated.
   * @param times The number of duplicates.
   * @param dx The x shift.
   * @param dy The y shift.
   * @param scaleX An x scale factor between each copy.
   * @param scaleY A y scale factor between each copy.
   * @param angle An angular increment.
   */
  void addDuplicates( const Shape & shape,
                      std::size_t times,
                      double dx, double dy,
                      double scaleX, double scaleY,
                      double angle = 0.0 );

  /**
   * Save the drawing in an EPS, XFIG of SVG file depending
   * on the filename extension. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename Path of the file to be created.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   */
  void save( const char * filename, PageSize size = Board::BoundingBox, double margin = 0.0, Unit unit = UMillimeter ) const;

  /**
   * Save the drawing in an EPS, XFIG of SVG file depending
   * on the filename extension. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename Path of the file to be created.
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   */
  void save(const char * filename, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = UMillimeter ) const;

  /**
   * Writes the drawing in a stream as an EPS file. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   * @param title Document title (Postscript comment).
   */
  void saveEPS( std::ostream & out, PageSize size = Board::BoundingBox, double margin = 0.0, Unit unit = UMillimeter, const std::string & title = std::string() ) const ;

  /**
   * Saves the drawing in an EPS file. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The EPS file name.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   * @param title Document title (Postscript comment).
   */
  void saveEPS( const char * filename, PageSize size = Board::BoundingBox, double margin = 0.0, Unit unit = UMillimeter, const std::string & title = std::string() ) const ;

  /**
   * Writes the drawing in a stream as an EPS file. The drawing is scaled (up or
   * down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   * @param title Document title (Postscript comment).
   */
  void saveEPS( std::ostream & out, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = UMillimeter, const std::string & title = std::string() ) const ;

  /**
   * Saves the drawing in an EPS file. The drawing is scaled (up or down) so
   * that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The EPS file name.
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   * @param title Document title (Postscript comment).
   */
  void saveEPS( const char * filename, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = UMillimeter, const std::string & title = std::string()  ) const ;

  /**
   * Saves the drawing in an XFig file. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The name of the FIG file.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   */
  void saveFIG( const char * filename, PageSize size = Board::BoundingBox, double margin = 0.0, Unit unit = UMillimeter ) const;

  /**
   * Saves the drawing in a stream as an XFig file. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   */
  void saveFIG( std::ostream & out, PageSize size = Board::BoundingBox, double margin = 0.0, Unit unit = UMillimeter ) const;

  /**
   * Saves the drawing in an XFig file. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The XFig file name.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   */
  void saveFIG( const char * filename, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = UMillimeter ) const ;

  /**
   * Saves the drawing in a stream as an XFig file. The drawing is scaled (up or
   * down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   */
  void saveFIG( std::ostream & out, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = UMillimeter ) const ;

  /**
   * Save the drawing in an SVG file. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The name of the file.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   */
  void saveSVG( const char * filename, PageSize size = Board::BoundingBox, double margin = 0.0, Unit unit = UMillimeter ) const;

  /**
   * Saves the drawing in a stream as an SVG file. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   */
  void saveSVG( std::ostream & out, PageSize size = Board::BoundingBox, double margin = 0.0, Unit unit = UMillimeter ) const;

  /**
   * Saves the drawing in an SVG file. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The SVG file name.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   */
  void saveSVG( const char * filename, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = UMillimeter ) const ;

  /**
   * Saves the drawing in a stream as an SVG file. The drawing is scaled (up or down) so
   * that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   */
  void saveSVG( std::ostream & out, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = UMillimeter) const ;

  /**
   * Save the drawing in an TikZ file. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The name of the file.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page, in millimeters.
   */
  void saveTikZ( const char * filename, PageSize size = Board::BoundingBox, double margin = 0.0 ) const;

  /**
   * Save the drawing in a stream as TikZ file. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page, in millimeters.
   */
  void saveTikZ( std::ostream & out, PageSize size = Board::BoundingBox, double margin = 0.0 ) const;

  /**
   * Save the drawing in an TikZ file. When a size is given (not BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The name of the file.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param pageWidth Width of the page in millimeters.
   * @param pageHeight Height of the page in millimeters.
   * @param margin Minimal margin around the figure in the page, in millimeters.
   */
  void saveTikZ( const char * filename, double pageWidth, double pageHeight, double margin = 0.0 ) const ;

  /**
   * Save the drawing in a stream as a TikZ file. The drawing is scaled (up or
   * down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param size Page size (Either BoundingBox (default), A4 or Letter).
   * @param pageWidth Width of the page in millimeters.
   * @param pageHeight Height of the page in millimeters.
   * @param margin Minimal margin around the figure in the page, in millimeters.
   */
  void saveTikZ( std::ostream & out, double pageWidth, double pageHeight, double margin = 0.0 ) const ;


  /**
   * Build a grid with specified number of rows and columns and a given size.
   *
   * @param topLeft Coordinates of the top left point of the grid.
   * @param columns Number of columns of the grid.
   * @param rows Number of rows of the grid.
   * @param width Width of the grid.
   * @param height Height of the grid.
   * @param penColor Color of the lines.
   * @param fillColor Background color.
   * @param lineWidth Line width.
   * @param style Line style (default is Board::SolidStyle).
   * @param cap Line cap (default is Board::ButtCap).
   * @param join Line join (default is Board::MiterJoin).
   * @param depth Depth of the grid (default is -1 (auto)).
   *
   * @return The grid as a group.
   */
  static Group makeGrid(Point topLeft,
                        size_t columns, size_t rows,
                        double width, double height,
                        Color penColor,
                        Color fillColor,
                        double lineWidth,
                        const LineStyle style = SolidStyle,
                        const LineCap cap = ButtCap,
                        const LineJoin join = MiterJoin,
                        int depth = -1 );
  /**
   * @brief penColor
   *
   * @return The current pen color.
   */
  Color penColor() const;

  /**
   * @brief fillColor
   *
   * @return The current fill color.
   */
  Color fillColor() const;

protected:

  static double toMillimeter( double x, Unit unit);


  /**
   * Current graphical state for drawings made by the drawSomething() methods.
   *
   */
  struct State {
    Color penColor;             /**< The pen color. */
    Color fillColor;            /**< The fill color. */
    double lineWidth;           /**< The line thickness. */
    Shape::LineStyle lineStyle; /**< The line style. */
    Shape::LineCap lineCap;     /**< The type of line extremities. */
    Shape::LineJoin lineJoin;   /**< The type of line junction. */
    Fonts::Font font;           /**< The font. */
    double fontSize;            /**< The font size. */
    State();
  };
  State _state;                 /**< The current state. */
  Color _backgroundColor;       /**< The color of the background. */
  Path _clippingPath;
};
} // namespace LibBoard

#include "board/Board.ih"

#endif
