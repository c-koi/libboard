/* -*- mode: c++ -*- */
/**
 * @file   Path.h
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2009
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
#ifndef _BOARD_PATH_H_
#define _BOARD_PATH_H_

#include "board/Point.h"
#include "board/Rect.h"
#include "board/Transforms.h"
#include <vector>
#include <iostream>

namespace LibBoard {

/**
 * The path structure.
 * @brief A path, according to Postscript and SVG definition.
 */
struct Path { 

  Path() : _closed( false ) { }

  Path( const std::vector<Point> & points, bool closed )
    : _points( points ), _closed( closed ) { }

  Path( bool closed ) : _closed( closed ) { }

  inline void clear();

  inline bool closed() const;

  inline bool empty() const;

  inline std::size_t size() const;

  inline void setClosed( bool closed  );

  /**
   * Center of the bounding box of the path.
   * @return The center of the bounding box of the path.
   */
  Point center() const;

  /**
   * Add a point at the end of the path.
   *
   * @param p A point.
   *
   * @return The path itself.
   */
  Path & operator<<( const Point & p );

  /**
   * Remove the last point of the path.
   *
   * @return The path itself.
   */
  Path & pop_back();

  /**
   * Get the n-th point of the path.
   *
   * @param n Index of a point in the path.
   *
   * @return A reference to the n-th point.
   */
  inline Point & operator[]( const std::size_t n );

  /**
   * Get the n-th point of the path (const version).
   *
   * @param n Index of a point in the path.
   *
   * @return A (const) reference to the n-th point.
   */
  inline const Point & operator[]( const std::size_t n ) const;

  /**
   * Rotate the path by a given angle and according to a rotation center.
   *
   * @param angle The rotation angle (in radians).
   * @param center The rotation center.
   *
   * @return The path itself.
   */
  Path & rotate( double angle, const Point & center );

  /**
   * Return a rotated copy of the path, thanks to an angle and a rotation
   * center.
   *
   * @param angle The rotation angle (in radians).
   * @param center The rotation center.
   *
   * @return A rotated copy of the path.
   */
  Path rotated( double angle, const Point & center ) const;

  /**
   * Rotate the path by a given angle around the center of its bounding box.
   *
   * @param angle The rotation angle (in radians).
   *
   * @return The path itself.
   */
  Path & rotate( double angle );

  /**
   * Return a rotated copy of the path, around the center of its bounding box.
   *
   * @param angle The rotation angle (in radians).
   *
   * @return A rotated copy of the point.
   */
  Path rotated( double angle ) const;

  /**
   * Translate the path.
   *
   * @param dx The shift along the x axis.
   * @param dy The shift along the y axis.
   *
   * @return The path itself.
   */
  Path & translate( double dx, double dy );

  /**
   * Return a translated copy of the path.
   *
   * @param dx The shift along the x axis.
   * @param dy The shift along the y axis.
   *
   * @return A translated copy of the path.
   */
  Path translated( double dx, double dy ) const;

  /**
   * Apply a scaling factor to the path along each axis.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return The path itself, once scaled.
   */
  Path & scale( double sx, double sy );

  /**
   * Apply a scaling factor to the path.
   *
   * @param s The scaling factor.
   *
   * @return The path itself, once scaled.
   */
  Path & scale( double s );


  /**
   * Return a scaled copy of the path.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return The scaled copy of the path.
   */
  Path scaled( double sx, double sy )  const;


  /**
   * Return a scaled copy of the path.
   *
   * @param s The scaling factor along both axis's.
   *
   * @return The scaled copy of the path.
   */
  Path scaled( double s )  const;

  /**
   * Scale all the points.
   *
   * @param s The scaling factor.
   */
  void scaleAll( double s );

  void flushPostscript( std::ostream & stream,
                        const TransformEPS & transform ) const;

  void flushFIG( std::ostream & stream,
                 const TransformFIG & transform ) const;

  void flushSVGPoints( std::ostream & stream,
                       const TransformSVG & transform ) const;

  void flushSVGCommands( std::ostream & stream,
                         const TransformSVG & transform ) const;

  void flushTikZPoints( std::ostream & stream,
                        const TransformTikZ & transform ) const;

  /**
   * Compute the bounding box of the path.
   *
   * @return The bounding box of the path.
   */
  Rect boundingBox() const;

protected:
  std::vector<Point> _points;
  bool _closed;
};

void
Path::clear()
{
  _points.clear();
}

Point &
Path::operator[]( const std::size_t n )
{
  return _points[ n ];
}

const Point &
Path::operator[]( const std::size_t n ) const {
  return _points[ n ];
}

bool
Path::closed() const
{
  return _closed;
}

bool
Path::empty() const
{
  return _points.empty();
}

std::size_t
Path::size() const
{
  return _points.size();
}

void
Path::setClosed( bool closed )
{
  _closed = closed;
}

} // namespace LibBoard  

#endif /* _PATH_H_ */

