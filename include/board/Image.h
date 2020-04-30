/* -*- mode: c++ -*- */
/**
 * @file   Image.h
 * @author Sebastien Fourey (GREYC)
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
#ifndef _BOARD_IMAGE_H_
#define _BOARD_IMAGE_H_

#include <string>
#include "board/Polyline.h"
#include "board/Shape.h"
#include "board/TransformMatrix.h"

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
   * @param depth
   *
   */
  Image(const char * filename, double left, double top, double width, double height = 0.0, int depth = -1);

  /**
   *
   * Construct an Image given a filename, a size and (possibly) a depth.
   *
   * @param filename The image filename
   * @param rect The position and size of the image
   * @param depth The depth (default -1)
   *
   */
  Image(const char * filename, const Rect & rect, int depth = -1);

  /**
   * Image destructor.
   */
  inline ~Image();

  /**
   * Returns the generic name of the shape (Image)
   *
   * @return
   */
  const std::string & name() const;

  /**
   * Return a copy of the shape.
   *
   * @return
   */
  Image * clone() const;

  /**
   * Rotate the shape around a given center of rotation.
   *
   * @param angle The rotation angle in radian.
   * @param center The center of rotation.
   *
   * @return A reference to the shape itself.
   */
  Shape & rotate(double angle, const Point & center);

  /**
   * Rotate the shape around its center.
   *
   * @param angle The rotation angle in radian.
   *
   * @return A reference to the shape itself.
   */
  Shape & rotate(double angle);

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
  Shape & translate(double dx, double dy);

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
  Shape & scale(double sx, double sy);

  /**
   * Scale the shape along both axis.
   *
   * @param s The scale factor along both axis.
   *
   * @return The shape itself.
   */
  Shape & scale(double s);

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
  Rect boundingBox(LineWidthFlag lineWidthFlag) const;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll(double s);

  /**
   * Writes the EPS code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  void flushPostscript(std::ostream & stream, const TransformEPS & transform) const;

  /**
   * Writes the FIG code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  void flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const;

  /**
   * Writes the SVG code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  void flushSVG(std::ostream & stream, const TransformSVG & transform) const;

  /**
   * Writes the TikZ code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
  void flushTikZ(std::ostream & stream, const TransformTikZ & transform) const;

private:
  static const std::string _name; /**< The generic name of the shape. */
  Polyline _rectangle;
  Polyline _originalRectangle;
  TransformMatrix _transformMatrixSVG;
  TransformMatrix _transformMatrixEPS;
  std::string _filename;
};

Image::~Image() {}

} // namespace LibBoard

#endif /* _SHAPE_H_ */
