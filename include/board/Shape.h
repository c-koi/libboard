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
#ifndef BOARD_SHAPE_H
#define BOARD_SHAPE_H

#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <board/Color.h>
#include <board/Globals.h>
#include <board/PSFonts.h>
#include <board/Path.h>
#include <board/Point.h>
#include <board/Rect.h>
#include <board/Style.h>
#include <board/Tools.h>
#include <board/Transforms.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

namespace LibBoard
{

struct ShapeVisitor;
struct ConstShapeVisitor;
struct CompositeShapeTransform;

/**
 * Shape structure.
 * @brief Abstract structure for a 2D shape.
 */
struct Shape {

  /**
   * Shape constructor.
   */
  inline Shape();

  /**
   * Shape destructor.
   */
  virtual ~Shape();

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
   * @param colormap
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

  /**
   * @brief Accept a composite shape transform.
   *
   * @param transform A composite shape transform object.
   */
  virtual Shape * accept(CompositeShapeTransform & transform) const;

  /**
   * @brief Accept a constant composite shape transform.
   *
   * @param transform A constant composite shape transform object..
   */
  virtual Shape * accept(const CompositeShapeTransform & transform) const;

  Shape(const Shape & other);

private:
  static const std::string _name; /**< The generic name of the shape. */
};

} // namespace LibBoard

/*
 * Inline methods
 */

namespace LibBoard
{
Shape::Shape() {}

Rect Shape::bbox(LineWidthFlag flag) const
{
  return boundingBox(flag);
}

Shape & Shape::rotateDeg(double angle, const Point & center)
{
  return rotate(angle * (M_PI / 180.0), center);
}

Shape & Shape::rotateDeg(double angle)
{
  return rotate(angle * (M_PI / 180.0), center());
}

} // namespace LibBoard

#endif /* BOARD_SHAPE_H */
