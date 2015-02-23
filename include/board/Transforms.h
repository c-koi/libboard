/* -*- mode: c++ -*- */
/**
 * @file   Transforms.h
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
#ifndef _BOARD_TRANSFORMS_H_
#define _BOARD_TRANSFORMS_H_

#include <limits>
#include <vector>
#include <cmath>
#include "TransformMatrix.h"

namespace LibBoard {

struct Rect;
struct Shape;
struct ShapeList;

/**
 * The base class for transforms.
 * @brief
 */
struct Transform {
public:
  inline Transform();
  virtual ~Transform() { };
  virtual double mapX( double x ) const;
  virtual double mapY( double y ) const = 0;
  Point map( const Point & ) const;
  virtual void apply( double & x, double & y ) const;
  virtual double scale( double x ) const;
  virtual double rounded( double x ) const;
  virtual void setBoundingBox( const Rect & rect,
                               const double pageWidth,
                               const double pageHeight,
                               const double margin ) = 0;

  static inline double round( const double & x );

protected:
  double _scale;
  double _deltaX;
  double _deltaY;
  double _height;
};

/**
 * The TransformEPS structure.
 * @brief Structure representing a scaling and translation
 * suitable for an EPS output.
 */
struct TransformEPS : public Transform {
public:
  double mapY( double y ) const;
  void setBoundingBox( const Rect & rect,
                       const double pageWidth,
                       const double pageHeight,
                       const double margin );
};

/**
 * The TransformFIG structure.
 * @brief Structure representing a scaling and translation
 * suitable for an XFig output.
 */
struct TransformFIG : public Transform {
public:
  inline TransformFIG();
  double rounded( double x ) const;
  double mapY( double y ) const;
  int mapWidth( double width ) const;
  void setBoundingBox( const Rect & rect,
                       const double pageWidth,
                       const double pageHeight,
                       const double margin );
  void setDepthRange( const ShapeList & shapes );
  int mapDepth( int depth ) const;
private:
  int _maxDepth;
  int _minDepth;
};

/**
 * The TransformSVG structure.
 * @brief Structure representing a scaling and translation
 * suitable for an SVG output.
 */
struct TransformSVG : public Transform {
public:
  double rounded( double x ) const;
  double mapY( double y ) const;
  double mapWidth( double width ) const;
  void setBoundingBox( const Rect & rect,
                       const double pageWidth,
                       const double pageHeight,
                       const double margin );
  TransformMatrix matrix() const;
  Point translation() const;
  double deltaX() const;
  double deltaY() const;

};

/**
 * The TransformTikZ structure.
 * @brief Structure representing a scaling and translation
 * suitable for an TikZ output.
 */
struct TransformTikZ : public TransformSVG {
};


#include "Transforms.ih"

} // namespace LibBoard

#endif /* _TRANSFORMS_H_ */
