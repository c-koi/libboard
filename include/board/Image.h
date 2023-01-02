/* -*- mode: c++ -*- */
/**
 * @file   Image.h
 * @author Sebastien Fourey (GREYC)
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
#ifndef BOARD_IMAGE_H
#define BOARD_IMAGE_H

#include <string>
#include <board/Polyline.h>
#include <board/Shape.h>
#include <board/TransformMatrix.h>

namespace LibBoard
{

/**
 * Image structure.
 *
 * @brief Structure for a bitmap image shape.
 */
struct Image : public Shape {

  /**
   *
   * Construct an Image given a filename, a size and (possibly) a depth.
   * If the height parameter is 0.0, then the correct height is computed
   * according to the actual aspect ratio of the bitmap image, obtained
   * from the file.
   *
   * @param filename
   * @param left
   * @param top
   * @param width
   * @param height
   *
   */
  Image(const char * filename, double left, double top, double width, double height = 0.0);

  /**
   *
   * Construct an Image given a filename, a size and (possibly) a depth.
   *
   * @param filename The image filename
   * @param rect The position and size of the image
   */
  Image(const char * filename, const Rect & rect);

  /**
   * Returns the generic name of the shape (Image)
   *
   * @return
   */
  const std::string & name() const override;

  /**
   * Return a copy of the shape.
   *
   * @return
   */
  Image * clone() const override;

  /**
   * Rotate the shape around a given center of rotation.
   *
   * @param angle The rotation angle in radian.
   * @param center The center of rotation.
   *
   * @return A reference to the shape itself.
   */
  Shape & rotate(double angle, const Point & center) override;

  /**
   * Rotate the shape around its center.
   *
   * @param angle The rotation angle in radian.
   *
   * @return A reference to the shape itself.
   */
  Shape & rotate(double angle) override;

  /**
   * Returns a rotated copy of the image.
   *
   * @param angle The rotation angle in radian.
   *
   * @return A rotated copy of the image.
   */
  Image rotated(double angle) const;

  /**
   * Returns a rotated copy of the image.
   *
   * @param angle The rotation angle in radian.
   * @param center The center of rotation.
   *
   * @return A rotated copy of the image.
   */
  Image rotated(double angle, const Point &) const;

  /**
   * Translate the shape by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the shape itself.
   */
  Shape & translate(double dx, double dy) override;

  /**
   * Returns a translated copy of the image.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A translated copy of the image.
   */
  Image translated(double dx, double dy) const;

  /**
   * Scale the shape along the x an y axis.
   *
   * @param sx The scale factor along the x axis.
   * @param sy The scale factor along the y axis.
   *
   * @return The shape itself.
   */
  Shape & scale(double sx, double sy) override;

  /**
   * Scale the shape along both axis.
   *
   * @param s The scale factor along both axis.
   *
   * @return The shape itself.
   */
  Shape & scale(double s) override;

  /**
   * Returns a scaled copy of the image.
   *
   * @param sx Scale factor along the x axis.
   * @param sy Scale factor along the y axis.
   *
   * @return A scaled copy of the image.
   */
  Image scaled(double sx, double sy);

  /**
   * Returns the bounding box of the figure.
   *
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   * @return The rectangle of the bounding box.
   */
  Rect boundingBox(LineWidthFlag lineWidthFlag) const override;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll(double s) override;

  /**
   * Writes the EPS code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  void flushPostscript(std::ostream & stream, const TransformEPS & transform) const override;

  /**
   * Writes the FIG code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   * @param colormap
   */
  void flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const override;

  /**
   * Writes the SVG code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  void flushSVG(std::ostream & stream, const TransformSVG & transform) const override;

  /**
   * Writes the TikZ code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  void flushTikZ(std::ostream & stream, const TransformTikZ & transform) const override;

  /**
   * @brief Accepts a visitor object.
   *
   * @param visitor A visitor object.
   */
  virtual void accept(ShapeVisitor & visitor) override;

  /**
   * @brief Accepts a visitor object.
   *
   * @param visitor A visitor object.
   */
  virtual void accept(const ShapeVisitor & visitor) override;

  /**
   * @brief Accepts a const-shape visitor object.
   *
   * @param visitor A const-shape visitor object.
   */
  virtual void accept(ConstShapeVisitor & visitor) const override;

  /**
   * @brief Accepts a const-shape visitor object.
   *
   * @param visitor A const-shape visitor object.
   */
  virtual void accept(const ConstShapeVisitor & visitor) const override;

  /**
   * @brief Accept a composite shape transform.
   *
   * @param transform A composite shape transform object.
   */
  virtual Shape * accept(CompositeShapeTransform & transform) const override;

  /**
   * @brief Accept a constant composite shape transform.
   *
   * @param transform A constant composite shape transform object..
   */
  virtual Shape * accept(const CompositeShapeTransform & transform) const override;

  Image(const Image &) = default;
  Image(Image &&) = default;
  Image & operator=(Image &&) = default;
  ~Image() override = default;

private:
  static const std::string _name; /**< The generic name of the shape. */
  Polyline _rectangle;
  Polyline _originalRectangle;
  TransformMatrix _transformMatrixSVG;
  TransformMatrix _transformMatrixEPS;
  std::string _filename;
};

} // namespace LibBoard

#endif /* SHAPE_H */
