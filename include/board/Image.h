/* -*- mode: c++ -*- */
/**
 * @file   Image.h
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
#ifndef _BOARD_IMAGE_H_
#define _BOARD_IMAGE_H_

#include "board/Shapes.h"
#include "board/TransformMatrix.h"
#include <string>

namespace LibBoard {

/**
 * Image structure.
 * @brief Structure for a bitmap image shape.
 */
struct Image : public Shape {

   inline Image( const char * filename,
                 double left, double top, double width, double height=0.0,
                 int depth = -1 );

   inline Image( const char * filename, const Rect & rect, int depth = -1 );

   /**
   * Image destructor.
   */
   ~Image() { }

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
   * Returns the gravity center of the shape.
   *
   * @return The center of the shape.
   */
   Point center() const;

   /**
   * Rotate the shape around a given center of rotation.
   *
   * @param angle The rotation angle in radian.
   * @param center The center of rotation.
   *
   * @return A reference to the shape itself.
   */
   Shape & rotate( double angle, const Point & center );

   /**
   * Rotate the shape around its center.
   *
   * @param angle The rotation angle in radian.
   *
   * @return A reference to the shape itself.
   */
   Shape & rotate( double angle );

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
   Image rotated(double angle, const Point & ) const;

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
   Shape & translate( double dx, double dy );

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
   Shape & scale( double sx, double sy );

   /**
   * Scale the shape along both axis.
   *
   * @param s The scale factor along both axis.
   *
   * @return The shape itself.
   */
   Shape & scale( double s );

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
   * @return The rectangle of the bounding box.
   */
   Rect boundingBox() const;


   /**
   * Returns the bounding box of the figure. (Convenience method to call "boundingBox" with a short name.)
   *
   */
   inline Rect bbox();


   /**
   * Decrement the depth of the shape. (Pull the shape toward the foreground.)
   *
   * @return
   */
   inline Shape & operator--();

   /**
   * Increment the depth of the shape. (Push the shape toward the background.)
   *
   * @return
   */
   inline Shape & operator++();


   /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
   void scaleAll( double s );


   /**
   * Writes the EPS code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
   void flushPostscript( std::ostream & stream,
                         const TransformEPS & transform ) const;

   /**
   * Writes the FIG code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
   void flushFIG( std::ostream & stream,
                  const TransformFIG & transform,
                  std::map<Color,int> & colormap ) const;

   /**
   * Writes the SVG code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
   void flushSVG( std::ostream & stream,
                  const TransformSVG & transform ) const;

   /**
   * Writes the TikZ code of the shape in a stream according
   * to a transform.
   *
   * @param stream The output stream.
   * @param transform A 2D transform to be applied.
   */
   void flushTikZ( std::ostream & stream,
                   const TransformTikZ & transform ) const;

   inline int depth() const;

   const Color & penColor() const;

   const Color & fillColor() const;

private:

   static const std::string _name; /**< The generic name of the shape. */

   Rectangle _rectangle;

   Rectangle _originalRectangle;

   TransformMatrix _transformMatrixSVG;

   TransformMatrix _transformMatrixEPS;

   std::string _filename;
};

Image::Image(const char * filename,
             double left, double top, double width, double height, int depth)
   : Shape(Color::None, Color::None, 0.0, SolidStyle, ButtCap, MiterJoin, depth),
     _rectangle(left, top, width, height, Color::Black, Color::None, 0.1, SolidStyle, ButtCap, MiterJoin, depth ),
     _originalRectangle(left, top, width, height, Color::Black, Color::None, 0.1, SolidStyle, ButtCap, MiterJoin, depth ),
     _filename( filename )
{
}

Image::Image(const char * filename,
             const Rect & rect, int depth)
   : Shape(Color::None, Color::None, 0.0, SolidStyle, ButtCap, MiterJoin, depth),
     _rectangle(rect, Color::Black, Color::None, 0.1, SolidStyle, ButtCap, MiterJoin, depth ),
     _originalRectangle(rect, Color::Black, Color::None, 0.1, SolidStyle, ButtCap, MiterJoin, depth ),
     _filename( filename )
{
}

Rect Image::bbox()
{
  return _rectangle.bbox();
}


} // namespace LibBoard

#endif /* _SHAPE_H_ */

