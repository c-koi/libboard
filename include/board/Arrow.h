/* -*- mode: c++ -*- */
/**
 * @file   Arrow.h
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
#ifndef _BOARD_ARROW_H_
#define _BOARD_ARROW_H_

#include "board/Line.h"

#if __cplusplus < 201100
#define override
#endif

namespace LibBoard
{

/**
 * The arrow structure.
 * @brief A line between two points with an arrow at one extremity.
 */
struct Arrow : public Line {

  /**
   * Constructs an arrow.
   *
   * @param x1 First coordinate of the start point.
   * @param y1 Second coordinate of the start point.
   * @param x2 First coordinate of the end point.
   * @param y2 Second coordinate of the end point.
   * @param penColor The color of the line.
   * @param fillColor The fill color of the sharp end.
   * @param lineWidth The line thickness.
   * @param depth The depth of the line.
   */
  inline Arrow(double x1, double y1, double x2, double y2, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(), double lineWidth = Shape::defaultLineWidth(),
               const LineStyle lineStyle = Shape::defaultLineStyle(), const LineCap cap = Shape::defaultLineCap(), const LineJoin join = Shape::defaultLineJoin(), int depth = -1);

  /**
   * Constructs an arrow.
   *
   * @param p1 Start point.
   * @param p2 End point.
   * @param penColor The color of the line.
   * @param fillColor The fill color of the sharp end.
   * @param lineWidth The line thickness.
   * @param depth The depth of the line.
   */
  inline Arrow(Point p1, Point p2, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(), double lineWidth = Shape::defaultLineWidth(),
               const LineStyle lineStyle = Shape::defaultLineStyle(), const LineCap cap = Shape::defaultLineCap(), const LineJoin join = Shape::defaultLineJoin(), int depth = -1);

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const override;

  /**
   * Returns a copy of the arrow, rotated around a given rotation center.
   *
   * @param angle The rotation angle.
   * @param center The center of rotation.
   *
   * @return The rotated copy of the line.
   */
  Arrow rotated(double angle, const Point & center) const;

  /**
   * Returns a copy of the arrow, rotated around its center.
   *
   * @param angle The angle of rotation.
   *
   * @return A copy of the lines, rotated around its center by the given angle.
   */
  Arrow rotated(double angle) const;

  /**
   * Returns a translated copy of the arrow.
   *
   * @param dx The shift along the x axis.
   * @param dy The shift along the y axis.
   *
   * @return A translated copy of the line.
   */
  Arrow translated(double dx, double dy) const;

  /**
   * Returns a scaled copy of the arrow.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy scaling factor along the y axis.
   *
   * @return A scaled copy of the arrow.
   */
  Arrow scaled(double sx, double sy) const;

  /**
   * Returns a scaled copy of the arrow.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the arrow.
   */
  Arrow scaled(double s) const;

  /**
   * Returns a resized copy of the arrow.
   *
   * @param w Width of the new arrow.
   * @param h Height of the new arrow.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   * @return A resized copy of the arrow.
   */
  Arrow resized(double w, double h, LineWidthFlag lineWidthFlag) const;

  /**
   * Computes the bounding box of the arrow.
   *
   * @return The rectangle of the bounding box.
   */
  Rect boundingBox(LineWidthFlag) const override;

  void flushPostscript(std::ostream & stream, const TransformEPS & transform) const override;

  void flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const override;

  void flushSVG(std::ostream & stream, const TransformSVG & transform) const override;

  void flushTikZ(std::ostream & stream, const TransformTikZ & transform) const override;

  Arrow * clone() const override;

private:
  static const std::string _name; /**< The generic name of the shape. */
};

} // namespace LibBoard

/*
 * Inline methods
 */

namespace LibBoard
{

Arrow::Arrow(double x1, double y1, double x2, double y2, Color penColor, Color fillColor, double lineWidth, const LineStyle style, const LineCap cap, const LineJoin join, int depth)
    : Line(x1, y1, x2, y2, penColor, lineWidth, style, cap, join, depth)
{

  if (fillColor == Color::Null) {
    Shape::_fillColor = penColor;
  } else {
    Shape::_fillColor = fillColor;
  }
}

Arrow::Arrow(Point p1, Point p2, Color penColor, Color fillColor, double lineWidth, const LineStyle style, const LineCap cap, const LineJoin join, int depth)
    : Line(p1, p2, penColor, lineWidth, style, cap, join, depth)
{
  if (fillColor == Color::Null) {
    Shape::_fillColor = penColor;
  } else {
    Shape::_fillColor = fillColor;
  }
}
}

#if __cplusplus < 201100
#undef override
#endif

#endif /* _BOARD_ARROW_H_ */
