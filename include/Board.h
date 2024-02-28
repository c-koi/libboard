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
#ifndef BOARD_BOARD_H
#define BOARD_BOARD_H

#include <ostream>
#include <string>
#include <vector>

#include <BoardConfig.h>
#include <board/Arrow.h>
#include <board/Bezier.h>
#include <board/BoardFontText.h>
#include <board/Dot.h>
#include <board/Ellipse.h>
#include <board/Exception.h>
#include <board/Group.h>
#include <board/Image.h>
#include <board/Line.h>
#include <board/Path.h>
#include <board/Point.h>
#include <board/Polyline.h>
#include <board/RoughVisitor.h>
#include <board/Shape.h>
#include <board/ShapeList.h>
#include <board/SketchFilter.h>
#include <board/Style.h>
#include <board/Text.h>

namespace LibBoard
{

/**
 * The Board class.
 * @brief Class for EPS, FIG or SVG drawings.
 */
struct Board : public ShapeList {

public:
  static const double Degree;

  /**
   * @brief The AspectRatio enum
   */
  enum class AspectRatio
  {
    Ignore,  //!< Ignore aspect ratio, e.g. for resize operations
    Preserve //!< Preserve aspect ratio, e.g. for resize operations
  };

  /**
   * Constructs a new board and sets the background color, if any.
   *
   * @param backgroundColor A color for the drawing's background.
   */
  Board(const Color & backgroundColor = Color::Null);

  /**
   * Copy constructor.
   *
   * @param other The object to be copied.
   */
  Board(const Board & other);

  ~Board() override;

  /**
   * The operator =
   *
   * @param other The object to be copied.
   *
   * @return A reference to the left operand.
   */
  Board & operator=(const Board & other);

  /**
   * Add a shape to the board, using the current unit factor.
   *
   * @param shape A shape. (Might be a list of shapes, actually!)
   *
   * @return The board itself, as a ShapeList.
   */
  Board & operator<<(const Shape & shape);

  /**
   * Add of shapes from a vector to the shape list.
   */
  template <typename S> //
  Board & operator<<(const std::vector<S> & shapes);

  /**
   * Clears the board with a given background color.
   *
   * @param color The board background color (may be Color::None).
   */
  void clear(const Color & color = Color::Null);

  /**
   * Clears the board and set the background color from an RGB triple.
   *
   * @param red
   * @param green
   * @param blue
   */
  inline void clear(unsigned char red, unsigned char green, unsigned char blue);

  /**
   * @brief Rotate the board by an angle around a point
   * @param angle Rotation angle (in radians)
   * @param center Rotation center
   * @return A reference to the board itself
   */
  Board & rotate(double angle, const Point & center) override;

  /**
   * @brief Rotate the board by an angle around the center of its bounding box
   * @param angle Rotation angle (in radians)
   * @return A reference to the board itself
   */
  Board & rotate(double angle) override;

  /**
   * @brief Translate the board by a given vector
   * @param dx Shift in the x direction
   * @param dy Shift in the y direction
   * @return A reference to the board itself
   */
  Board & translate(double dx, double dy) override;

  /**
   * @brief Scale the board in the x and y direction
   * @param sx Scale factor in the x direction
   * @param sy Scale factor in the y direction
   * @return A reference to the board itself
   */
  Board & scale(double sx, double sy) override;

  /**
   * @brief Scale the board
   * @param s Scale factor
   * @return A reference to the board itself
   */
  Board & scale(double s) override;

  /**
   * Get a rotated copy of the Board around a specified rotation center.
   * @param angle Rotation angle in radian
   * @param center Center of rotation
   * @return Rotated copy of the Board object.
   */
  Board rotated(double angle, const Point & center);

  /**
   * Get a rotated copy of the Board around its bounding box's center.
   *
   * @param angle Rotation angle in radian
   * @return Rotated copy of the Board object.
   */
  Board rotated(double angle);

  /**
   * Get a translated copy of the Board.
   * @param dx x shift value
   * @param dy y shift value
   * @return Translated copy of the Board object.
   */
  Board translated(double dx, double dy);

  /**
   * Get a scaled copy of the Board.
   * @param sx x scaling factor
   * @param sy y scaling factor
   * @return Scaled copy of the Board object.
   */
  Board scaled(double sx, double sy);

  /**
   * Get a scaled copy of the Board.
   * @param s scaling factor
   * @return Scaled copy of the Board object.
   */
  Board scaled(double s);

  /**
   * Draws a dot at coordinates (x,y).
   *
   * @param x First coordinate of the dot.
   * @param y Second coordinate of the dot.
   */
  void drawDot(double x, double y);

  /**
   * Draws a line from (x1,y1) to (x2,y2) using current style parameters.
   * @param x1 First coordinate of the first extremity.
   * @param y1 Second coordinate of the first extremity.
   * @param x2 First coordinate of the second extremity.
   * @param y2 Second coordinate of the second extremity.
   */
  void drawLine(double x1, double y1, double x2, double y2);

  /**
   * Draws a line from p to q using current style parameters.
   *
   * @param p First extremity.
   * @param q Second extremity.
   */
  void drawLine(Point p, Point q);

  /**
   * Draws a line from (x1,y1) to (x2,y2) with an arrow at (x2,y2) using current style parameters.
   *
   * @param x1 First coordinate of the first extremity.
   * @param y1 Second coordinate of the first extremity.
   * @param x2 First coordinate of the second extremity.
   * @param y2 Second coordinate of the second extremity.
   * @param type The arrow extremity type.
   */
  void drawArrow(double x1, double y1, double x2, double y2, Arrow::ExtremityType type = Arrow::ExtremityType::Plain);

  /**
   * Draws a line from p to q with an arrow at (x2,y2).
   *
   * @param p First extremity.
   * @param q Second extremity.
   * @param type The arrow extremity type.
   */
  void drawArrow(Point p, Point q, Arrow::ExtremityType type = Arrow::ExtremityType::Plain);

  /**
   * Draws a triangle.
   *
   * @param x1 First coordinate of the first vertex.
   * @param y1 Second coordinate of the first vertex.
   * @param x2 First coordinate of the second vertex.
   * @param y2 Second coordinate of the second vertex.
   * @param x3 First coordinate of the third vertex.
   * @param y3 Second coordinate of the third vertex.
   */
  void drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3);

  /**
   * Draws a triangle.
   *
   * @param p1 First vertex.
   * @param p2 Second vertex.
   * @param p3 Third vertex.
   */
  void drawTriangle(const Point & p1, const Point & p2, const Point & p3);

  /**
   * Draws a filled triangle.
   *
   * @param x1 First coordinate of the first vertex.
   * @param y1 Second coordinate of the first vertex.
   * @param x2 First coordinate of the second vertex.
   * @param y2 Second coordinate of the second vertex.
   * @param x3 First coordinate of the third vertex.
   * @param y3 Second coordinate of the third vertex.
   */
  void fillTriangle(double x1, double y1, double x2, double y2, double x3, double y3);

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
   */
  void fillGouraudTriangle(const Point & p1, const Color & color1, //
                           const Point & p2, const Color & color2, //
                           const Point & p3, const Color & color3, unsigned char divisions = 3);

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
   */
  inline void fillGouraudTriangle(const double x1, const double y1, const Color & color1, //
                                  const double x2, const double y2, const Color & color2, //
                                  const double x3, const double y3, const Color & color3, unsigned char divisions = 3);

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
   */
  void fillGouraudTriangle(const Point & p1, const float brightness1, //
                           const Point & p2, const float brightness2, //
                           const Point & p3, const float brightness3, unsigned char divisions = 3);

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
   */
  inline void fillGouraudTriangle(const double x1, const double y1, const float brightness1, //
                                  const double x2, const double y2, const float brightness2, //
                                  const double x3, const double y3, const float brightness3, //
                                  unsigned char divisions = 3);

  /**
   * Draws a filled triangle.
   *
   * @param p1 First vertex.
   * @param p2 Second vertex.
   * @param p3 Third vertex.
   */
  void fillTriangle(const Point & p1, const Point & p2, const Point & p3);

  /**
   * Draws a rectangle.
   *
   * @param left First coordinate of the upper left corner.
   * @param top Second coordinate of the upper left corner.
   * @param width Width of the rectangle.
   * @param height Height of the rectangle.
   */
  void drawRectangle(double left, double top, double width, double height);

  /**
   * Draws a rectangle.
   *
   * @param rect Position and size of the rectangle.
   */
  void drawRectangle(const Rect & rect);

  /**
   * Draws a rectangle filled with the current pen color.
   *
   * @param left First coordinate of the upper left corner.
   * @param top Second coordinate of the upper left corner.
   * @param width Width of the rectangle.
   * @param height Height of the rectangle.
   */
  void fillRectangle(double left, double top, double width, double height);

  /**
   * Draws a rectangle filled with the current pen color.
   * @param rect Position and size of the rectangle.
   */
  void fillRectangle(const Rect & rect);

  /**
   * Draws a circle.
   *
   * @param x First coordinate of the circle's center.
   * @param y Second coordinate of the circle's center.
   * @param radius Radius of the circle.
   */
  void drawCircle(double x, double y, double radius);

  /**
   * Draws a circle filled with the current pen color.
   *
   * @param x First coordinate of the circle's center.
   * @param y Second coordinate of the circle's center.
   * @param radius Radius of the circle.
   */
  void fillCircle(double x, double y, double radius);

  /**
   * Draws an ellipse.
   *
   * @param x First coordinate of the circle's center.
   * @param y Second coordinate of the circle's center.
   * @param xRadius Radius of the circle along the x axis.
   * @param yRadius Radius of the circle along the y axis.
   */
  void drawEllipse(double x, double y, double xRadius, double yRadius);

  /**
   * Draws an ellipse filled with the current pen color.
   *
   * @param x First coordinate of the circle's center.
   * @param y Second coordinate of the circle's center.
   * @param xRadius X axis radius of the ellipse.
   * @param yRadius Y axis radius of the ellipse.
   */
  void fillEllipse(double x, double y, double xRadius, double yRadius);

  /**
   * Draws a polygonal line.
   *
   * @param points A vector of points.
   */
  void drawPolyline(const std::vector<Point> & points);

  /**
   * Draws a closed polygonal line.
   *
   * @param points A vector of points.
   */
  void drawClosedPolyline(const std::vector<Point> & points);

  /**
   * Draws a filled polygon.
   *
   * @param points A vector of points.
   */
  void fillPolyline(const std::vector<Point> & points);

  /**
   * Draws a string of text.
   *
   * @param x The first coordinates of the lower left corner.
   * @param y The second coordinates of the lower left corner.
   * @param text The text.
   */
  void drawText(double x, double y, const char * text);

  /**
   * Draws a string of text.
   *
   * @param p Position of the bottom-left corner.
   * @param text The text.
   */
  void drawText(Point p, const char * text);

  /**
   * Draws a string of text.
   *
   * @param x The first coordinates of the lower left corner.
   * @param y The second coordinates of the lower left corner.
   * @param text The text.
   */
  void drawText(double x, double y, const std::string & text);

  /**
   * Draws a string of text.
   *
   * @param p Position of the bottom-left corner.
   * @param str The text.
   */
  void drawText(Point p, const std::string & str);

  /**
   * Changes the current font and font size.
   *
   * @param font The name of the font.
   * @param fontSize The new font size.
   * @return The board itself.
   */
  Board & setFont(const Fonts::Font font, double fontSize);

  /**
   * Changes the font size.
   *
   * @param fontSize The new font size.
   * @return The board itself.
   */
  Board & setFontSize(double fontSize);

  /**
   * Changes the current pen color.
   *
   * @param red Red component [0..255].
   * @param green Green component [0..255].
   * @param blue Blue component [0..255].
   * @param alpha Transparency channel [0..255].
   * @return The board itself.
   */
  Board & setPenColorRGBi(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);

  /**
   * Changes the current pen color.
   *
   * @param red Red component [0..1].
   * @param green Green component [0..1].
   * @param blue Blue component [0..1].
   * @param alpha Transparency [0..1].
   *
   * @return The board itself.
   */
  Board & setPenColorRGBf(float red, float green, float blue, float alpha = 1.0f);

  /**
   * Changes the current pen color.
   *
   * In order to use no pen, one may set the pen color to Color::None.
   * @param color The pen color.
   * @return The board itself.
   */
  Board & setPenColor(const Color & color);

  /**
   * Changes the current fill color.
   *
   * @param red Red component.
   * @param green Green component.
   * @param blue Blue component.
   * @param alpha The opacity.
   * @return The board itself.
   */
  Board & setFillColorRGBi(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);

  /**
   * Changes the current fill color (RGB version).
   *
   * @param red Red component.
   * @param green Green component.
   * @param blue Blue component.
   * @param alpha The opacity.
   * @return The board itself.
   */
  Board & setFillColorRGBf(float red, float green, float blue, float alpha = 1.0f);

  /**
   * Changes the current fill color (HSV version)
   *
   * @param hue
   * @param saturation
   * @param value
   * @param alpha
   * @return
   */
  Board & setFillColorHSV(float hue, float saturation, float value, float alpha = 1.0f);

  /**
   * Changes the current fill color.
   *
   * In order to use no fill color, one may set this color to Color::None.
   * @param color The fill color.
   * @return The board itself.
   */
  Board & setFillColor(const Color & color);

  /**
   * Changes the current line thickness.
   *
   * @param width The new line thickness.
   * @return The board itself.
   */
  Board & setLineWidth(double width);

  /**
   * Changes the current line style.
   *
   * @param style The new line style.
   * @return The board itself.
   */
  Board & setLineStyle(LineStyle style);

  /**
   * Set the line cap style.
   * @param cap The cap-style.
   * @return The board itself.
   */
  inline Board & setLineCap(LineCap cap);

  /**
   * Set the line join style.
   * @param join The join-style
   * @return The board itself.
   */
  inline Board & setLineJoin(LineJoin join);

  /**
   * Changes the background color of the whole drawing.
   *
   * @param color A color (may be Color::None).
   */
  void setBackgroundColor(const Color & color);

  /**
   * Draws the current drawing's bounding box as a rectangle.
   *
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   */
  void drawBoundingBox(LineWidthFlag lineWidthFlag);

  /**
   * Define a clipping rectangle for the whole drawing.
   *
   * @param x
   * @param y
   * @param width
   * @param height
   */
  void setClippingRectangle(double x, double y, double width, double height);

  /**
   * Define a clipping rectangle for the whole drawing.
   *
   * @param rect The clipping rectangle.
   */
  void setClippingRectangle(const Rect & rect);

  /**
   * Define a clipping path for the whole drawing.
   *
   * @param points A path.
   */
  void setClippingPath(const std::vector<Point> & points);

  /**
   * Define a clipping path for the whole drawing.
   * @param path A path.
   */
  void setClippingPath(const Path & path);

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
  void addDuplicates(const Shape & shape, std::size_t times, double dx, double dy, double scale = 1.0);

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
  void addDuplicates(const Shape & shape, std::size_t times, double dx, double dy, double scaleX, double scaleY, double angle = 0.0);

  /**
   * Save the drawing in an EPS, XFIG of SVG file depending
   * on the filename extension. When a size is given (not PageSize::BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename Path of the file to be created.
   * @param size Page size (Either PageSize::BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   */
  void save(const char * filename, PageSize size = PageSize::BoundingBox, double margin = 0.0, Unit unit = Unit::Millimeter) const;

  /**
   * Save the drawing in an EPS, XFIG of SVG file depending
   * on the filename extension. When a size is given (not PageSize::BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename Path of the file to be created.
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   */
  void save(const char * filename, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = Unit::Millimeter) const;

  /**
   * Writes the drawing in a stream as an EPS file. When a size is given (not PageSize::BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param size Page size (Either PageSize::BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   * @param title Document title (Postscript comment).
   */
  void saveEPS(std::ostream & out, PageSize size = PageSize::BoundingBox, double margin = 0.0, Unit unit = Unit::Millimeter, const std::string & title = std::string()) const;

  /**
   * Save the drawing in an EPS file. When a size is given (not PageSize::BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The EPS file name.
   * @param size Page size (Either PageSize::BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   * @param title Document title (Postscript comment).
   */
  void saveEPS(const char * filename, PageSize size = PageSize::BoundingBox, double margin = 0.0, Unit unit = Unit::Millimeter, const std::string & title = std::string()) const;

  /**
   * Write the drawing in a stream as an EPS file. The drawing is scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   * @param title Document title (Postscript comment).
   */
  void saveEPS(std::ostream & out, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = Unit::Millimeter, const std::string & title = std::string()) const;

  /**
   * Save the drawing in an EPS file. The drawing is scaled (up or down) so
   * that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The EPS file name.
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   * @param title Document title (Postscript comment).
   */
  void saveEPS(const char * filename, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = Unit::Millimeter, const std::string & title = std::string()) const;

  /**
   * Save the drawing in an XFig file. When a size is given (not PageSize::BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The name of the FIG file.
   * @param size Page size (Either PageSize::BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   */
  void saveFIG(const char * filename, PageSize size = PageSize::BoundingBox, double margin = 0.0, Unit unit = Unit::Millimeter) const;

  /**
   * Saves the drawing in a stream as an XFig file. When a size is given (not PageSize::BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param size Page size (Either PageSize::BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   */
  void saveFIG(std::ostream & out, PageSize size = PageSize::BoundingBox, double margin = 0.0, Unit unit = Unit::Millimeter) const;

  /**
   * Saves the drawing in an XFig file. When a size is given (not PageSize::BoundingBox), the drawing is scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The XFig file name.
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   */
  void saveFIG(const char * filename, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = Unit::Millimeter) const;

  /**
   * Save the drawing in a stream as an XFig file. The drawing is scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   */
  void saveFIG(std::ostream & out, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = Unit::Millimeter) const;

  /**
   * Save the drawing in an SVG file. When a size is given (not PageSize::BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The name of the file.
   * @param size Page size (Either PageSize::BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   */
  void saveSVG(const char * filename, PageSize size = PageSize::BoundingBox, double margin = 0.0, Unit unit = Unit::Millimeter) const;

  /**
   * Saves the drawing in a stream as an SVG file. When a size is given (not PageSize::BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param size Page size (Either PageSize::BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the margin (default value is millimeter). If size is "BoundingBox", this unit is used for the bounding box as well.
   */
  void saveSVG(std::ostream & out, PageSize size = PageSize::BoundingBox, double margin = 0.0, Unit unit = Unit::Millimeter) const;

  /**
   * Save the drawing in an SVG file. When a size is given (not PageSize::BoundingBox), the drawing is scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The SVG file name.
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   */
  void saveSVG(const char * filename, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = Unit::Millimeter) const;

  /**
   * Save the drawing in a stream as an SVG file. The drawing is scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param pageWidth Width of the page.
   * @param pageHeight Height of the page.
   * @param margin Minimal margin around the figure in the page.
   * @param unit The unit used to express the previous length parameters (default value is millimeter).
   */
  void saveSVG(std::ostream & out, double pageWidth, double pageHeight, double margin = 0.0, Unit unit = Unit::Millimeter) const;

  /**
   * Save the drawing in an TikZ file. When a size is given (not PageSize::BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The name of the file.
   * @param size Page size (Either PageSize::BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page, in millimeters.
   */
  void saveTikZ(const char * filename, PageSize size = PageSize::BoundingBox, double margin = 0.0) const;

  /**
   * Save the drawing in a stream as TikZ file. When a size is given (not PageSize::BoundingBox), the drawing is
   * scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param size Page size (Either PageSize::BoundingBox (default), A4 or Letter).
   * @param margin Minimal margin around the figure in the page, in millimeters.
   */
  void saveTikZ(std::ostream & out, PageSize size = PageSize::BoundingBox, double margin = 0.0) const;

  /**
   * Save the drawing in an TikZ file. When a size is given (not PageSize::BoundingBox), the drawing is scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param filename The name of the file.
   * @param pageWidth Width of the page in millimeters.
   * @param pageHeight Height of the page in millimeters.
   * @param margin Minimal margin around the figure in the page, in millimeters.
   */
  void saveTikZ(const char * filename, double pageWidth, double pageHeight, double margin = 0.0) const;

  /**
   * Save the drawing in a stream as a TikZ file. The drawing is scaled (up or down) so that it fits within the dimension while keeping its aspect ratio.
   *
   * @param out The output stream.
   * @param pageWidth Width of the page in millimeters.
   * @param pageHeight Height of the page in millimeters.
   * @param margin Minimal margin around the figure in the page, in millimeters.
   */
  void saveTikZ(std::ostream & out, double pageWidth, double pageHeight, double margin = 0.0) const;

  /**
   * @brief penColor
   *
   * @return The current pen color.
   */
  inline const Color & penColor() const;

  /**
   * @brief fillColor
   *
   * @return The current fill color.
   */
  inline const Color & fillColor() const;

  /**
   * Return the current style used when drawing shapes with board methods.
   *
   * @return Current style.
   */
  inline const Style & style() const;

  /**
   *  Globally enable linewidth scaling when using scale functions.
   */
  static void enableLineWidthScaling();

  /**
   *  Globally disable linewidth scaling when using scale functions.
   */
  static void disableLineWidthScaling();

  /**
   *  Globally enable/disable linewidth scaling when using scale functions.
   */
  static void setLineWidthScaling(bool);

  /**
   * @brief Standard page size as a rectangle Rect(0,0,width,height)
   * @param size Page size enum
   * @param unit Unit
   * @return Rectangle according to unit
   */
  static Rect pageRect(PageSize size, Unit unit);

protected:
  static double toMillimeter(double x, Unit unit);

  /**
   * Current graphical state for drawings made by the drawSomething() methods.
   */
  struct State {
    Style style;      /**< The style. */
    Fonts::Font font; /**< The font. */
    double fontSize;  /**< The font size. */
    State();
  };
  State _state;           /**< The current state. */
  Color _backgroundColor; /**< The color of the background. */
  Path _clippingPath;
};

// Helper functions

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
 *
 * @return The grid as a group.
 */
Group grid(Point topLeft, size_t columns, size_t rows, double width, double height, //
           Color penColor, Color fillColor,                                         //
           double lineWidth,                                                        //
           const LineStyle style = SolidStyle,                                      //
           const LineCap cap = ButtCap,                                             //
           const LineJoin join = MiterJoin);

/**
 * Build a grid with specified number of rows and columns and a given size.
 *
 * @param topLeft Coordinates of the top left point of the grid.
 * @param columns Number of columns of the grid.
 * @param rows Number of rows of the grid.
 * @param width Width of the grid.
 * @param height Height of the grid.
 * @param style Shape style
 *
 * @return The grid as a group.
 */
Group grid(Point topLeft, size_t columns, size_t rows, double width, double height, const Style & style = Style::defaultStyle());

/**
 * Make a small cross
 * @param p
 * @param style
 * @return A cross made of two lines, centered at point p
 */
Group cross(Point p, const Style & style = Style::defaultStyle());

/**
 * @brief Produce a rectangular color matrix
 * @param topLeft
 * @param colors
 * @param columns
 * @param rows
 * @param pixelWidth
 * @param pixelHeight if zero (default), then it take the same value as pixelHeight
 * @param lineWidth if zero, pixels are drawn with Color::Null outline (i.e. color is "none")
 * @return A group of rows x columns rectangles colored after given vector
 */
Group array(Point topLeft, const std::vector<Color> & colors, unsigned int columns, unsigned int rows, double pixelWidth, double pixelHeight = 0.0, double lineWidth = 0.0);

/**
 * @brief Surround a shape with a rectangular frame
 * @param shape A shape
 * @param margin Space between the shape and the frame
 * @param penColor Frame color
 * @param fillColor Fill color
 * @param lineWidth Line width
 * @param lineStyle Line style
 * @param sketchyCount If set to zero, the frame is a straight one, otherwise a sketchy frame is produced with given repetition count.
 * @return A group of the shape with its frame
 */
Group framed(const Shape & shape,            //
	     double margin = 0.0,            //
             const Color & penColor = Style::defaultPenColor(),   //
             const Color & fillColor = Style::defaultFillColor(), //
             double lineWidth = Style::defaultLineWidth(),        //
             LineStyle lineStyle = Style::defaultLineStyle(),     //
             int sketchyCount = 0);

/**
 * Builds a tiling based on a shape by repeating this shape along its
 * bounding box.
 *
 * @param shape A shape to be repeated.
 * @param topLeftCorner Position of the top left corner of the tiling.
 * @param columns Number of columns of the tiling.
 * @param rows Number of rows of the tiling.
 * @param spacing Spacing between rows and columns.
 * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
 * @return The tiling that has been created, as a Group.
 */
Group tiling(const Shape & shape, Point topLeftCorner, int columns, int rows, double spacing = 0.0, LineWidthFlag lineWidthFlag = UseLineWidth);

/**
 * @brief Surround a shape with a cirle
 * @param shape A shape
 * @param margin Space between the shape and the circle
 * @param penColor Pen color
 * @param fillColor Fill color
 * @param lineWidth Line width
 * @param lineStyle Line style
 * @return A Group of the shape with its surrounding circle
 */
Group circled(const Shape & shape, double margin = 0.0,                                               //
              Color penColor = Style::defaultPenColor(), Color fillColor = Style::defaultFillColor(), //
              double lineWidth = Style::defaultLineWidth(), LineStyle lineStyle = Style::defaultLineStyle());

/**
 * @brief Produce a polyline of the Biezer vertices and control points
 * @param bezier A Bezier
 * @param style Style of the polyline
 * @return A polyline showing the Bezier points and controls
 */
Polyline bezierControls(const Bezier & bezier, const Style & style = Style::defaultStyle());

// Inline methods and functions

inline void Board::clear(unsigned char red, unsigned char green, unsigned char blue)
{
  clear(Color(red, green, blue));
}

inline Board & Board::setLineStyle(LineStyle style)
{
  _state.style.lineStyle = style;
  return *this;
}

inline Board & Board::setLineCap(LineCap cap)
{
  _state.style.lineCap = cap;
  return *this;
}

inline Board & Board::setLineJoin(LineJoin join)
{
  _state.style.lineJoin = join;
  return *this;
}

const Color & Board::penColor() const
{
  return _state.style.penColor;
}

const Color & Board::fillColor() const
{
  return _state.style.fillColor;
}

const Style & Board::style() const
{
  return _state.style;
}

inline void Board::fillGouraudTriangle(const double x1, const double y1, const Color & color1, //
                                       const double x2, const double y2, const Color & color2, //
                                       const double x3, const double y3, const Color & color3, //
                                       unsigned char divisions)
{
  fillGouraudTriangle(Point(x1, y1), color1, Point(x2, y2), color2, Point(x3, y3), color3, divisions);
}

inline void Board::fillGouraudTriangle(const double x1, const double y1, const float brightness1, //
                                       const double x2, const double y2, const float brightness2, //
                                       const double x3, const double y3, const float brightness3, //
                                       unsigned char divisions)
{
  fillGouraudTriangle(Point(x1, y1), brightness1, Point(x2, y2), brightness2, Point(x3, y3), brightness3, divisions);
}

template <typename S> //
Board & Board::operator<<(const std::vector<S> & shapes)
{
  return static_cast<Board &>(ShapeList::operator<<(shapes));
}

} // namespace LibBoard

// TODO : Add rectangle tilling from a vector of colors (2A array) ( https://github.com/c-koi/libboard/pull/7 )

#endif
