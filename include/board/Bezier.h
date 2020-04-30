/* -*- mode: c++ -*- */
/**
 * @file   Bezier.h
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
#ifndef _BOARD_BEZIER_H_
#define _BOARD_BEZIER_H_

#include "board/Shape.h"

#if __cplusplus < 201100
#define override
#endif

namespace LibBoard
{

/**
 * The Bezier structure.
 * @brief A cubice polygonal line described by a series of 2D points.
 */
struct Bezier : public Shape {

  /**
   * @brief A cubic bezier curve
   *
   * @param points Points of the bezier curve (n points)
   * @param controls Controls points (2(n-1) points)
   * @param penColor
   * @param fillColor
   * @param lineWidth
   * @param lineStyle
   * @param cap
   * @param join
   * @param depth
   */
  Bezier(const std::vector<Point> & points, const std::vector<Point> & controls, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(),
         double lineWidth = Shape::defaultLineWidth(), const LineStyle lineStyle = Shape::defaultLineStyle(), const LineCap cap = Shape::defaultLineCap(),
         const LineJoin join = Shape::defaultLineJoin(), int depth = -1);

  /**
   * @brief Bezier
   * @param p0 Start point
   * @param control0 Start control point
   * @param p1 End point
   * @param control1 End control point
   * @param penColor
   * @param fillColor
   * @param lineWidth
   * @param lineStyle
   * @param cap
   * @param join
   * @param depth
   */
  Bezier(Point p0, Point control0, Point p1, Point control1, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(), double lineWidth = Shape::defaultLineWidth(),
         const LineStyle lineStyle = Shape::defaultLineStyle(), const LineCap cap = Shape::defaultLineCap(), const LineJoin join = Shape::defaultLineJoin(), int depth = -1);

  /**
   * @brief Bezier
   * @param x0 Start point (x coordinate)
   * @param y0 Start point (y coordinate)
   * @param xc0 Start control point (x coordinate)
   * @param yc0 Start control point (y coordinate)
   * @param x1 End point (x coordinate)
   * @param y1 End point (y coordinate)
   * @param xc1 End control point (x coordinate)
   * @param yc1 End control point (y coordinate)
   * @param penColor
   * @param fillColor
   * @param lineWidth
   * @param lineStyle
   * @param cap
   * @param join
   * @param depth
   */
  Bezier(double x0, double y0, double xc0, double yc0, double x1, double y1, double xc1, double yc1, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(),
         double lineWidth = Shape::defaultLineWidth(), const LineStyle lineStyle = Shape::defaultLineStyle(), const LineCap cap = Shape::defaultLineCap(),
         const LineJoin join = Shape::defaultLineJoin(), int depth = -1);

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const override;

  /**
   * @brief Rotate the Bezier curve.
   * @param angle
   * @param center
   * @return The curve itself
   */
  Bezier & rotate(double angle, const Point & center) override;

  Bezier rotated(double angle, const Point & center) const;

  Bezier & rotate(double angle) override;

  /**
   *
   *
   * @param angle
   *
   * @return
   */
  Bezier rotated(double angle) const;

  /**
   * Translate the Bezier by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the Bezier itself.
   */
  Bezier & translate(double dx, double dy) override;

  /**
   * Returns a translated copy of the Bezier.
   *
   * @param dx Shift of the first coordinate.
   * @param dy Shift of the second coordinate.
   *
   * @return A copy of the Bezier, translated.
   */
  Bezier translated(double dx, double dy) const;

  /**
   * Scale the Bezier, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   *
   * @return A reference to the Bezier itself, once scaled.
   */
  Bezier & scale(double sx, double sy) override;

  /**
   * Scale the Bezier, given a scaling factor.
   *
   * @param s Scaling factor.
   *
   * @return A reference to the Bezier itself, once scaled.
   */
  Bezier & scale(double s) override;

  /**
   * Returns a scaled copy of the line.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return A scaled copy of the line.
   */
  Bezier scaled(double sx, double sy) const;

  /**
   * Returns a scaled copy of the line.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the line.
   */
  Bezier scaled(double s) const;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll(double s) override;

  /**
   * Returns a resized copy of the Bezier.
   *
   * @param w Width of the new Bezier.
   * @param h Height of the new Bezier.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   * @return A resized copy of the Bezier.
   */
  Bezier resized(double w, double h, LineWidthFlag lineWidthFlag) const;

  void flushPostscript(std::ostream & stream, const TransformEPS & transform) const override;

  void flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const override;

  void flushSVG(std::ostream & stream, const TransformSVG & transform) const override;

  void flushTikZ(std::ostream & stream, const TransformTikZ & transform) const override;

  Rect boundingBox(LineWidthFlag) const override;

  Bezier * clone() const override;

  inline std::size_t vertexCount() const;

  inline const Path & path() const;

  Path discretizedPath() const;

private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
  Path _path;
  Path _controls;
};

} // namespace LibBoard

#if __cplusplus < 201100
#undef override
#endif

#endif /* _BOARD_BEZIER_H_ */
