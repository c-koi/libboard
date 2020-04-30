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
#ifndef _BOARD_RECT_H_
#define _BOARD_RECT_H_

#include <iostream>
#include "board/Point.h"

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
   * Rect constructor.
   *
   * @param left
   * @param top
   * @param width
   * @param height
   */
  Rect(double left = 0.0f, double top = 0.0f, double width = 0.0f, double height = 0.0f) : left(left), top(top), width(width), height(height) {}

  /**
   * Rect constructor.
   *
   * @param topLeft The top-left point of the rectangle.
   * @param width
   * @param height
   */
  Rect(Point topLeft, double width = 0.0f, double height = 0.0f) : left(topLeft.x), top(topLeft.y), width(width), height(height) {}

  /**
   * Rect constructor.
   *
   * @param topLeft The top-left point of the rectangle.
   * @param bottomRight The bottom-right point of the rectangle.
   */
  Rect(Point topLeft, Point bottomRight) : left(topLeft.x), top(topLeft.y), width(bottomRight.x - topLeft.x), height(topLeft.y - bottomRight.y) {}

  Point topLeft() const { return Point(left, top); }
  Point topRight() const { return Point(left + width, top); }
  Point bottomLeft() const { return Point(left, top - height); }
  Point bottomRight() const { return Point(left + width, top - height); }
  Point center() const { return Point(left + width / 2.0, top - height / 2.0); }
  Point centerLeft() const { return Point(left, top - height / 2.0); }
  Point centerRight() const { return Point(left + width, top - height / 2.0); }
  Point centerTop() const { return Point(left + width / 2.0, top); }
  Point centerBottom() const { return Point(left + width / 2.0, top - height); }
  double bottom() const { return top - height; }
  double right() const { return left + width; }
  void growToContain(Point);
  bool contains(Point) const;
  Rect & grow(double margin);
};

/**
 * Computes the bounding box of two bounding boxes.
 *
 * @param rectA A first rectangle.
 * @param rectB A second rectangle.
 *
 * @return The smallest rectangle that contains both rectA and rectB.
 */
Rect operator||(const Rect & rectA, const Rect & rectB);

/**
 * Computes the intersection of two bounding boxes.
 *
 * @param rectA A first rectangle.
 * @param rectB A second rectangle.
 *
 * @return The intersecting rectangle of two bounding boxes.
 */
Rect operator&&(const Rect & rectA, const Rect & rectB);

} // mamespace LibBoard

/**
 * Stream output operator for Rect structure.
 *
 * @param out An output stream.
 * @param rect A Rect struct.
 *
 * @return The output stream.
 */
std::ostream & operator<<(std::ostream & out, const LibBoard::Rect & rect);

#endif // _RECT_H_
