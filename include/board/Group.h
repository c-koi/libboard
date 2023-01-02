/* -*- mode: c++ -*- */
/**
 * @file   Group.h
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
 *
 * @brief  Group class
 *
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
#ifndef BOARD_GROUP_H
#define BOARD_GROUP_H

#include <board/ShapeList.h>

namespace LibBoard
{

/**
 * The Group structure.
 * @brief A group of shapes. A group is basically a ShapeList except that
 * when rendered in either an SVG of a FIG file, it is a true compound element.
 */
struct Group : public ShapeList {

  Group() : _clippingPath(Path::Closed) {}

  Group(const Group & other) : ShapeList(other), _clippingPath(other._clippingPath) {}

  ~Group() override;

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const override;

  Group & rotate(double angle, const Point & center) override;

  Group & rotate(double angle) override;

  Group rotated(double angle, const Point & center);

  Group rotated(double angle);

  Group & translate(double dx, double dy) override;

  Group translated(double dx, double dy);

  Group & scale(double sx, double sy) override;

  Group & scale(double s) override;

  Group scaled(double sx, double sy);

  Group scaled(double s);

  /**
   * Define a clipping rectangle for the group.
   *
   * @param x
   * @param y
   * @param width
   * @param height
   */
  void setClippingRectangle(double x, double y, double width, double height);

  /**
   * Define a clipping path for the group.
   *
   * @param points A path.
   */
  void setClippingPath(const std::vector<Point> & points);

  /**
   * Define a clipping path for the group.
   * @param path A path.
   */
  void setClippingPath(const Path & path);

  void flushPostscript(std::ostream & stream, const TransformEPS & transform) const override;

  void flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const override;

  void flushSVG(std::ostream & stream, const TransformSVG & transform) const override;

  void flushTikZ(std::ostream & stream, const TransformTikZ & transform) const override;

  Group & operator=(const Group & other);

  Group * clone() const override;

  Rect boundingBox(LineWidthFlag) const override;

  using ShapeList::accept;

  Group * accept(CompositeShapeTransform & transform) const override;

  Group * accept(const CompositeShapeTransform & transform) const override;

private:
  static const std::string _name; /**< The generic name of the shape. */
  Path _clippingPath;
  static std::size_t _clippingCount;
};

// Inline methods and functions

} // namespace LibBoard

#endif /* BOARD_GROUP_H */
