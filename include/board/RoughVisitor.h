/* -*- mode: c++ -*- */
/**
 * @file   RoughVisitor.h
 * @author Sebastien Fourey (GREYC)
 * @date   June 2016
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
#ifndef BOARD_ROUGH_VISITOR_H
#define BOARD_ROUGH_VISITOR_H

/*
 *
 * matplotlib.pyplot.xkcd
 *
 * https://shihn.ca/posts/2020/roughjs-algorithms/
 *
 * https://openaccess.city.ac.uk/id/eprint/1274/
 */

#include <cstddef> // For std::size_t
#include <board/Point.h>
#include <board/ShapeVisitor.h>
#include <board/Types.h>

namespace LibBoard
{

struct RoughVisitor : public CompositeShapeTransform {
  RoughVisitor();
  void setRepeat(int n);
  void setFilling(SketchFilling filling);
  void setHachureAngle(double angle);
  void setHachureSpacing(double spacing);
  void setAutomaticHachureSpacing(bool);
  virtual Shape * map(const Shape & shape) override;
  virtual Shape * map(const Shape & shape) const override;
  virtual Shape * map(const Line &) override;
  virtual Shape * map(const Line &) const override;
  virtual Shape * map(const Arrow &) override;
  virtual Shape * map(const Arrow &) const override;
  virtual Shape * map(const Polyline &) override;
  virtual Shape * map(const Polyline &) const override;
  virtual Shape * map(const Ellipse &) override;
  virtual Shape * map(const Ellipse &) const override;
  virtual Shape * map(const Bezier &) override;
  virtual Shape * map(const Bezier &) const override;

  virtual ~RoughVisitor() override;

  /**
   * Randomly slide a point withing a surrounding disk
   *
   * @param point A point.
   * @param radius Maximum radius of the motion.
   */
  static void slideRandomly(Point & point, const double & radius);

  /**
   * Return a point randomly slided withing a surrounding disk
   *
   * @param point A point.
   * @param radius Maximum radius of the motion.
   * @return The slided point, once translated.
   */
  static Point randomlySlided(const Point & point, const double & radius);

  /**
   * Move the point randomly away from an axis
   *
   * @param p Point to be moved
   * @param a First extremity of the segment
   * @param b Second extremity of the segment
   * @param magnitude Maximum distance from segment AB
   */
  static void slideRandomlyAwayFromSegment(Point & p, const Point & a, const Point & b, const double magnitude);

  /**
   * Move the point randomly away from an axis
   *
   * @param p Point to be moved
   * @param a First extremity of the segment
   * @param b Second extremity of the segment
   * @param magnitude Maximum distance from segment AB
   *
   * @return The point once moved.
   */
  static Point randomlySlidedAwayFromSegment(const Point & p, const Point & a, const Point & b, const double magnitude);

  /**
   * Move the point randomly away from an axis, at most 0.5% away from it.
   *
   * @param p Point to be moved
   * @param a First extremity of the segment
   * @param b Second extremity of the segment
   */
  static void slideRandomlyAwayFromSegment(Point & p, const Point & a, const Point & b);

  /**
   * Move the point randomly away from an axis, at most 0.5% away from it.
   *
   * @param p Point to be moved
   * @param a First extremity of the segment
   * @param b Second extremity of the segment
   *
   * @return The point once moved.
   */
  static Point randomlySlidedAwayFromSegment(const Point & p, const Point & a, const Point & b);

  /**
   * Move a point randomly away from an axis within a parallelogram.
   *
   * @param p Point to be moved
   * @param u First unit vector
   * @param uMagnitude Maximum distance along u
   * @param v First unit vector
   * @param vMagnitude Maximum distance along v
   */
  static void slideRandomlyAwayFromSegmentInBox(Point & p, const Point & u, double uMagnitude, const Point & v, double vMagnitude);

  /**
   * Move a point randomly away from an axis within a parallelogram.
   *
   * @param p Point to be moved
   * @param u First unit vector
   * @param uMagnitude Maximum distance along u
   * @param v First unit vector
   * @param vMagnitude Maximum distance along v
   *
   * @return The point once moved.
   */
  static Point randomlySlidedAwayFromSegmentInBox(const Point & p, const Point & u, double uMagnitude, const Point & v, double vMagnitude);

private:
  int _repeat;
  SketchFilling _filling;
  double _hachureAngle;
  double _hachureSpacing;
  bool _automaticHachureSpacing;
};

} // namespace LibBoard

#endif /* BOARD_ROUGH_VISITOR_H */
