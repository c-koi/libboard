/* -*- mode: c++ -*- */
/**
 * @file   Line.h
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
#ifndef _BOARD_LINE_H_
#define _BOARD_LINE_H_

#include "board/Shape.h"

#if __cplusplus < 201100
#define override
#endif

namespace LibBoard
{

/**
 * The line structure.
 * @brief A line between two points.
 */
struct Line : public Shape {

  /**
   * Constructs a line.
   *
   * @param x1 First coordinate of the start point.
   * @param y1 Second coordinate of the start point.
   * @param x2 First coordinate of the end point.
   * @param y2 Second coordinate of the end point.
   * @param color The color of the line.
   * @param lineWidth The line thickness.
   * @param depth The depth of the line.
   */
  inline Line(double x1, double y1, double x2, double y2, Color color, double lineWidth = Shape::defaultLineWidth(), const LineStyle lineStyle = Shape::defaultLineStyle(),
              const LineCap cap = Shape::defaultLineCap(), const LineJoin join = Shape::defaultLineJoin(), int depth = -1);

  /**
   * Constructs a line.
   *
   * @param a First extremity.
   * @param b Second extremity.
   * @param color The color of the line.
   * @param lineWidth The line thickness.
   * @param depth The depth of the line.
   */
  inline Line(Point a, Point b, Color color, double lineWidth = Shape::defaultLineWidth(), const LineStyle lineStyle = Shape::defaultLineStyle(), const LineCap cap = Shape::defaultLineCap(),
              const LineJoin join = Shape::defaultLineJoin(), int depth = -1);

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const override;

  Line & rotate(double angle, const Point & center) override;

  /**
   * Returns a copy of the line, rotated around a given rotation center.
   *
   * @param angle The rotation angle.
   * @param center The center of rotation.
   *
   * @return The rotated copy of the line.
   */
  Line rotated(double angle, const Point & center) const;

  Line & rotate(double angle) override;

  /**
   * Returns a copy of the line, rotated around its center.
   *
   * @param angle The angle of rotation.
   *
   * @return A copy of the lines, rotated around its center by the given angle.
   */
  Line rotated(double angle) const;

  /**
   * Translate the line by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the line itself.
   */
  Line & translate(double dx, double dy) override;

  /**
   * Returns a translated copy of the line.
   *
   * @param dx The shift along the x axis.
   * @param dy The shift along the y axis.
   *
   * @return A translated copy of the line.
   */
  Line translated(double dx, double dy) const;

  /**
   * Scale the line, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   *
   * @return A reference to the line itself, once scaled.
   */
  Line & scale(double sx, double sy) override;

  /**
   * Scale the line, given a scaling factor.
   *
   * @param s Scaling factor.
   *
   * @return A reference to the line itself, once scaled.
   */
  Line & scale(double s) override;

  /**
   * Returns a scaled copy of the line.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy scaling factor along the y axis.
   *
   * @return A scaled copy of the line.
   */
  Line scaled(double sx, double sy) const;

  /**
   * Returns a scaled copy of the line.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the line.
   */
  Line scaled(double s) const;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll(double s) override;

  /**
   * Returns a resized copy of the line.
   *
   * @param w Width of the new line.
   * @param h Height of the new line.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   * @return A resized copy of the line.
   */
  Line resized(double w, double h, LineWidthFlag lineWidthFlag) const;

  /**
   * Returns the bounding box of the line.
   *
   * @return The rectangle of the bounding box.
   */
  Rect boundingBox(LineWidthFlag) const override;

  Line * clone() const override;

  void flushPostscript(std::ostream & stream, const TransformEPS & transform) const override;

  void flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const override;

  void flushSVG(std::ostream & stream, const TransformSVG & transform) const override;

  void flushTikZ(std::ostream & stream, const TransformTikZ & transform) const override;

private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
  double _x1; /**< First coordinate of the start point. */
  double _y1; /**< Second coordinate of the start point. */
  double _x2; /**< First coordinate of the end point. */
  double _y2; /**< Second coordinate of the end point. */
};
}

/*
 * Inline methods
 */

namespace LibBoard
{

Line::Line(double x1, double y1, double x2, double y2, Color color, double lineWidth, const LineStyle style, const LineCap cap, const LineJoin join, int depth)
    : Shape(color, Color::Null, lineWidth, style, cap, join, depth), _x1(x1), _y1(y1), _x2(x2), _y2(y2)
{
}

Line::Line(Point a, Point b, Color color, double lineWidth, const LineStyle style, const LineCap cap, const LineJoin join, int depth)
    : Shape(color, Color::Null, lineWidth, style, cap, join, depth), _x1(a.x), _y1(a.y), _x2(b.x), _y2(b.y)
{
}
}

#if __cplusplus < 201100
#undef override
#endif

#endif /* _BOARD_LINE_H_ */
