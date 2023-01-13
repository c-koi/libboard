/* -*- mode: c++ -*- */
/**
 * @file   Rect.h
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
#ifndef BOARD_RECT_H
#define BOARD_RECT_H

#include <board/Point.h>
#include <cmath>
#include <iostream>

namespace LibBoard
{

/**
 * The Rectangle structure.
 * @brief Struct representing a rectangle on the plane.
 */
struct Rect {
  double left;   /**< Coordinate of the left side. */
  double top;    /**< Coordinate of the upper side. */
  double width;  /**< Width of the rectangle. */
  double height; /**< Height of the rectangle. */

  /**
   * Rect constructor
   *
   * @param left x coordinate of the left border of the rectangle
   * @param top y coordinate of the top border of the rectangle
   * @param width width of the rectangle
   * @param height height of the rectangle
   */
  Rect(double left = 0.0, double top = 0.0, double width = 0.0, double height = 0.0) : left(left), top(top), width(width), height(height) {}

  /**
   * Rect constructor.
   *
   * @param topLeft The top-left point of the rectangle
   * @param width Width of the rectangle
   * @param height Height of the rectangle
   */
  Rect(Point topLeft, double width = 0.0, double height = 0.0) : left(topLeft.x), top(topLeft.y), width(width), height(height) {}

  /**
   * Rect constructor.
   *
   * @param topLeft The top-left point of the rectangle.
   * @param bottomRight The bottom-right point of the rectangle.
   */
  Rect(Point topLeft, Point bottomRight) : left(topLeft.x), top(topLeft.y), width(bottomRight.x - topLeft.x), height(topLeft.y - bottomRight.y) {}

  /**
   * @brief topLeft Top-left point of the rectangle
   * @return The point at the top-left corner of the rectangle
   */
  Point topLeft() const { return Point(left, top); }

  /**
   * @brief topRight Top-right point of the rectangle
   * @return The point at the top-right corner of the rectangle
   */
  Point topRight() const { return Point(left + width, top); }

  /**
   * @brief bottomLeft Bottom-left point of the rectangle
   * @return The point at the bottom-left corner of the rectangle
   */
  Point bottomLeft() const { return Point(left, top - height); }

  /**
   * @brief bottomRight Bottom-right point of the rectangle
   * @return The point at the bottom-right corner of the rectangle
   */
  Point bottomRight() const { return Point(left + width, top - height); }

  /**
   * @brief center The center of the rectangle
   * @return The point at the center of the rectangle
   */
  Point center() const { return Point(left + width / 2.0, top - height / 2.0); }

  /**
   * @brief centerLeft The center of the left border of the rectangle
   * @return The point at the center of the left border of the rectangle
   */
  Point centerLeft() const { return Point(left, top - height / 2.0); }

  /**
   * @brief centerRight The center of the right border of the rectangle
   * @return The point at the center of the right border of the rectangle
   */
  Point centerRight() const { return Point(left + width, top - height / 2.0); }

  /**
   * @brief centerTop The center of the top border of the rectangle
   * @return The point at the center of the top border of the rectangle
   */
  Point centerTop() const { return Point(left + width / 2.0, top); }

  /**
   * @brief centerBottom The center of the bottom border of the rectangle
   * @return The point at the center of the bottom border of the rectangle
   */
  Point centerBottom() const { return Point(left + width / 2.0, top - height); }

  /**
   * @brief The y coordinate of the bottom of the rectangle
   * @return The y coordinate of the bottom of the rectangle
   */
  double bottom() const { return top - height; }

  /**
   * @brief The x coordinate of the right side of the rectangle
   * @return The x coordinate of the right side of the rectangle
   */
  double right() const { return left + width; }

  /**
   * @brief diameter
   * @return The distance between two opposite corners
   */
  double diameter() const { return std::hypot(width, height); }

  /**
   * @brief Set to a rectangle with zero width and height at position (0,0)
   */
  void clear() { left = top = width = height = 0.0; }

  /**
   * @brief Grow the rectangle so that it contains the point p
   * @param p A point
   * @return A reference to the rectangle itself
   */
  Rect & growToContain(const Point & p);

  /**
   * @brief Grow the rectangle so that it contains a set of points
   * @param points A set of points
   * @return A reference to the rectangle itself
   */
  Rect & growToContain(const std::vector<Point> & points);

  /**
   * @brief Check whether or not a point is contained in the rectangle (including its border)
   * @param p A point
   * @return true if the point is in the rectangle, otherwise false
   */
  bool contains(Point p) const;

  /**
   * @brief Check whether or not a point is strictly contained in the rectangle
   * @param p A point
   * @return true if the point is inside the rectangle, otherwise false
   */
  bool strictlyContains(Point p) const;

  /**
   * @brief Check whether the rectangle intersects another rectangle
   * @param other Another rectangle
   * @return true if the rectangles intersect, otherwise false
   */
  bool intersects(const Rect & other) const;

  /**
   * @brief Check whether the rectangle strictly intersects another rectangle
   * @param other Another rectangle
   * @return true if the rectangles strictly intersect, otherwise false
   */
  bool strictlyIntersects(const Rect & other) const;

  /**
   * @brief Grow the rectangle by a given distance (margin) in each direction
   * @param margin The margin to be added
   * @return A reference to the rectangle itself
   */
  Rect & grow(double margin);

  /**
   * @brief Return the rectangle growed by a given distance (margin) in each direction
   * @param margin The margin to be added
   * @return A new rectangle, result of the growth
   */
  Rect growed(double margin);

  /**
   * @brief Check if a rectangle is null (i.e., zero sized)
   * @return true if the rectangle is null, otherwise false
   */
  inline bool isNull() const;
};

/**
 * Computes the bounding box of two bounding boxes.
 *
 * @param rectA A first rectangle.
 * @param rectB A second rectangle.
 * @return The smallest rectangle that contains both rectA and rectB.
 */
Rect operator||(const Rect & rectA, const Rect & rectB);

/**
 * Computes the intersection of two bounding boxes.
 *
 * @param rectA A first rectangle.
 * @param rectB A second rectangle.
 * @return The intersecting rectangle of two bounding boxes.
 */
Rect operator&&(const Rect & rectA, const Rect & rectB);

} // namespace LibBoard

/**
 * Stream output operator for Rect structure.
 *
 * @param out An output stream.
 * @param rect A Rect struct.
 *
 * @return The output stream.
 */
std::ostream & operator<<(std::ostream & out, const LibBoard::Rect & rect);

// Inline methods

namespace LibBoard
{

bool Rect::isNull() const
{
  return width == 0.0 && height == 0.0;
}

} // namespace LibBoard

#endif // BOARD_RECT_H
