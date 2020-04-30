/* -*- mode: c++ -*- */
/**
 * @file   Shape.h
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
#ifndef _BOARD_SHAPE_H_
#define _BOARD_SHAPE_H_

#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "board/Color.h"
#include "board/PSFonts.h"
#include "board/Path.h"
#include "board/Point.h"
#include "board/Rect.h"
#include "board/Tools.h"
#include "board/Transforms.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#if __cplusplus < 201100
#define override
#endif

namespace LibBoard
{

struct ShapeVisitor;
struct ConstShapeVisitor;

/**
 * Shape structure.
 * @brief Abstract structure for a 2D shape.
 */
struct Shape {

  enum LineCap
  {
    ButtCap = 0,
    RoundCap,
    SquareCap
  };
  enum LineJoin
  {
    MiterJoin = 0,
    RoundJoin,
    BevelJoin
  };
  enum LineStyle
  {
    SolidStyle = 0,
    DashStyle,
    DotStyle,
    DashDotStyle,
    DashDotDotStyle,
    DashDotDotDotStyle
  };
  enum LineWidthFlag
  {
    IgnoreLineWidth,
    UseLineWidth
  };

  /**
   * Shape constructor.
   *
   * @param penColor The pen color of the shape.
   * @param fillColor The fill color of the shape.
   * @param lineWidth The line thickness.
   * @param style The line style.
   * @param cap The line cap style.
   * @param join The line join style.
   * @param depth The depth of the shape.
   */
  inline Shape(Color penColor, Color fillColor, double lineWidth, LineStyle style, const LineCap cap, const LineJoin join, int depth);

  /**
   * Shape destructor.
   */
  virtual ~Shape() {}

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
  inline bool filled() const { return _fillColor != Color::Null; }

  /**
   * Returns the center of the shape.
   *
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   * @return The center of the shape, i.e. the center of its bounding box.
   */
  virtual Point center(LineWidthFlag lineWidthFlag = IgnoreLineWidth) const;

  /**
   * Rotate the shape around a given center of rotation.
   *
   * @param angle The rotation angle in radian.
   * @param center The center of rotation.
   *
   * @return A reference to the shape itself.
   */
  virtual Shape & rotate(double angle, const Point & center) = 0;

  /**
   * Rotate the shape around its center.
   *
   * @param angle The rotation angle in radian.
   *
   * @return A reference to the shape itself.
   */
  virtual Shape & rotate(double angle) = 0;

  /**
   * Rotate the shape around a given center of rotation.
   *
   * @param angle The rotation angle in degree.
   * @param center The center of rotation.
   *
   * @return A reference to the shape itself.
   */
  inline Shape & rotateDeg(double angle, const Point & center);

  /**
   * Rotate the shape around its center.
   *
   * @param angle The rotation angle in degree.
   * @param center The center of rotation.
   *
   * @return A reference to the shape itself.
   */
  inline Shape & rotateDeg(double angle);

  /**
   * Translate the shape by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the shape itself.
   */
  virtual Shape & translate(double dx, double dy) = 0;

  /**
   * Move the center of the shape.
   *
   * @param x The new x coordinate of the center.
   * @param y The new y coordinate of the center.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   *
   * @return A reference to the shape itself.
   */
  Shape & moveCenter(double x, double y, LineWidthFlag lineWidthFlag = IgnoreLineWidth);

  /**
   * Move the center of the shape.
   *
   * @param p The new center.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   *
   * @return A reference to the shape itself.
   */
  Shape & moveCenter(Point p, LineWidthFlag lineWidthFlag = IgnoreLineWidth);

  /**
   * Scale the shape along the x an y axis.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return The shape itself.
   */
  virtual Shape & scale(double sx, double sy) = 0;

  /**
   * Scale the shape along both axis.
   *
   * @param s The scaling factor along both axis.
   *
   * @return The shape itself.
   */
  virtual Shape & scale(double s) = 0;

  /**
   * Resize the shape.
   *
   * @param width The new width.
   * @param height The new height.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   *
   * @return The shape itself.
   */
  Shape & resize(double width, double height, LineWidthFlag lineWidthFlag);

  /**
   * Scale the shape to a given width.
   *
   * @param w The requested new width.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   *
   * @return The shape itself.
   */
  Shape & scaleToWidth(double w, LineWidthFlag lineWidthFlag);

  /**
   * Scale the shape to a given height.
   *
   * @param h The requested new height.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   *
   * @return The shape itself.
   */
  Shape & scaleToHeight(double h, LineWidthFlag lineWidthFlag);

  /**
   * Compute the bounding box of the figure.
   *
   * @return The rectangle of the bounding box.
   */
  virtual Rect boundingBox(LineWidthFlag) const = 0;

  /**
   * Compute the bounding box of the figure. (Convenience method to call
   * "boundingBox" with a short name.)
   *
   */
  inline Rect bbox(LineWidthFlag) const;

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
  virtual void scaleAll(double s) = 0;

  /**
   * Write the EPS code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  virtual void flushPostscript(std::ostream & stream, const TransformEPS & transform) const = 0;

  /**
   * Write the FIG code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  virtual void flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const = 0;

  /**
   * Write the SVG code of the shape in a stream according to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  virtual void flushSVG(std::ostream & stream, const TransformSVG & transform) const = 0;

  /**
   * Write the TikZ code of the shape in a stream according to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  virtual void flushTikZ(std::ostream & stream, const TransformTikZ & transform) const = 0;

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

  inline int depth() const;

  virtual void depth(int);

  virtual void shiftDepth(int shift);

  inline const Color & penColor() const;

  inline const Color & fillColor() const;

  static void setDefaultLineWidth(double);
  static void setDefaultPenColor(Color);
  static void setDefaultFillColor(Color);
  static void setDefaultLineStyle(Shape::LineStyle);
  static void setDefaultLineCap(Shape::LineCap);
  static void setDefaultLineJoin(Shape::LineJoin);

  /**
   * @brief defaultLineWidth
   * @return Default lineWidth value for shapes (intialized as 1.0).
   */
  static double defaultLineWidth();

  /**
   * @brief defaultPenColor
   * @return Default pen color for shapes (initialized as Black)
   */
  static Color defaultPenColor();

  /**
   * @brief defaultFillColor
   * @return Default fill color for shapes (initialized as Color::None)
   */
  static Color defaultFillColor();

  /**
   * @brief defaultLineStyle
   * @return Default line style for shapes (initialized as SolidStyle)
   */
  static Shape::LineStyle defaultLineStyle();

  /**
   * @brief defaultLineCap
   * @return Default line cap for shapes (initialized as ButtCap)
   */
  static Shape::LineCap defaultLineCap();

  /**
   * @brief defaultLineJoin
   * @return Default line join for shapes (initialized as MiterJoin)
   */
  static Shape::LineJoin defaultLineJoin();

  /**
   * @brief Accepts a visitor object.
   *
   * @param visitor A visitor object.
   */
  virtual void accept(ShapeVisitor & visitor);

  /**
   * @brief Accepts a visitor object.
   *
   * @param visitor A visitor object.
   */
  virtual void accept(const ShapeVisitor & visitor);

  /**
   * @brief Accepts a const-shape visitor object.
   *
   * @param visitor A const-shape visitor object.
   */
  virtual void accept(ConstShapeVisitor & visitor) const;

  /**
   * @brief Accepts a const-shape visitor object.
   *
   * @param visitor A const-shape visitor object.
   */
  virtual void accept(const ConstShapeVisitor & visitor) const;

private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
  static bool _lineWidthScaling; /**< Linewidth should be scaled by
                                      scaling functions. */

  static double _defaultLineWidth;
  static Color _defaultPenColor;
  static Color _defaultFillColor;
  static Shape::LineStyle _defaultLineStyle;
  static Shape::LineCap _defaultLineCap;
  static Shape::LineJoin _defaultLineJoin;

  inline void updateLineWidth(double s);

  int _depth;           /**< The depth of the shape. */
  Color _penColor;      /**< The color of the shape. */
  Color _fillColor;     /**< The color of the shape. */
  double _lineWidth;    /**< The line thickness. */
  LineStyle _lineStyle; /**< The line style (solid, dashed, etc.). */
  LineCap _lineCap;     /**< The linecap attribute. (The way line terminates.) */
  LineJoin _lineJoin;   /**< The linejoin attribute. (The shape of line junctions.) */

  /**
   * Return a string of the svg properties lineWidth, opacity, penColor, fillColor,
   * lineCap, and lineJoin.
   *
   * @return A string of the properties suitable for inclusion in an svg tag.
   */
  std::string svgProperties(const TransformSVG & transform) const;

  /**
   * Return a string of the properties lineWidth, penColor, lineCap, and lineJoin
   * as Postscript commands.
   * @return A string of the Postscript commands.
   */
  std::string postscriptProperties(const TransformEPS & transform) const;

  /**
   * Return a string of the properties lineWidth, penColor, lineCap, and lineJoin
   * as TikZ commands.
   * @return A string of the TikZ commands.
   */
  std::string tikzProperties(const TransformTikZ & transform) const;
};

/**
 * Compares two shapes according to their depths.
 *
 * @param s1 A pointer to a first shape.
 * @param s2 A pointer to a second shape.
 *
 * @return
 */
bool shapeGreaterDepth(const Shape * s1, const Shape * s2);

extern const char * xFigDashStylesPS[];
extern const char * xFigDashStylesSVG[];
extern const char * xFigDashStylesTikZ[];

} // namespace LibBoard

/*
 * Inline methods
 */

namespace LibBoard
{
Shape::Shape(Color penColor, Color fillColor, double lineWidth, LineStyle style, const LineCap cap, const LineJoin join, int depth)
    : _depth(depth), _penColor(penColor), _fillColor(fillColor), _lineWidth(lineWidth), _lineStyle(style), _lineCap(cap), _lineJoin(join)
{
}

Rect Shape::bbox(LineWidthFlag flag) const
{
  return boundingBox(flag);
}

Shape & Shape::operator++()
{
  ++_depth;
  return *this;
}

Shape & Shape::operator--()
{
  --_depth;
  return *this;
}

int Shape::depth() const
{
  return _depth;
}

const Color & Shape::penColor() const
{
  return _penColor;
}

const Color & Shape::fillColor() const
{
  return _fillColor;
}

Shape & Shape::rotateDeg(double angle, const Point & center)
{
  return rotate(angle * (M_PI / 180.0), center);
}

Shape & Shape::rotateDeg(double angle)
{
  return rotate(angle * (M_PI / 180.0), center());
}

void Shape::updateLineWidth(double s)
{
  if (_lineWidthScaling) {
    _lineWidth *= s;
  }
}
}

#if __cplusplus < 201100
#undef override
#endif

#endif /* _BOARD_SHAPE_H_ */
