/* -*- mode: c++ -*- */
/**
 * @file   Ellipse.h
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
#ifndef _BOARD_ELLIPSE_H_
#define _BOARD_ELLIPSE_H_

#include "board/Shape.h"

#if __cplusplus < 201100
#define override
#endif

namespace LibBoard
{

/**
 * The ellipse structure.
 * @brief An ellipse.
 */
struct Ellipse : public Shape {

  Ellipse(double x, double y, double xRadius, double yRadius, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(), double lineWidth = Shape::defaultLineWidth(),
          const LineStyle lineStyle = Shape::defaultLineStyle(), int depth = -1)
      : Shape(penColor, fillColor, lineWidth, lineStyle, ButtCap, MiterJoin, depth), _center(x, y), _xRadius(xRadius), _yRadius(yRadius), _angle(0.0), _isCreatedAsCircle(false)
  {
    while (_angle > M_PI_2)
      _angle -= M_PI;
    while (_angle < -M_PI_2)
      _angle += M_PI;
  }

  Ellipse(Point center, double xRadius, double yRadius, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(), double lineWidth = Shape::defaultLineWidth(),
          const LineStyle lineStyle = Shape::defaultLineStyle(), int depth = -1)
      : Shape(penColor, fillColor, lineWidth, lineStyle, ButtCap, MiterJoin, depth), _center(center), _xRadius(xRadius), _yRadius(yRadius), _angle(0.0), _isCreatedAsCircle(false)
  {
    while (_angle > M_PI_2)
      _angle -= M_PI;
    while (_angle < -M_PI_2)
      _angle += M_PI;
  }

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const override;

  Point center(LineWidthFlag lineWidthFlag = IgnoreLineWidth) const override;

  Ellipse & rotate(double angle, const Point & center) override;

  /**
   *
   *
   * @param angle
   * @param center
   *
   * @return
   */
  Ellipse rotated(double angle, const Point & center) const;

  Ellipse & rotate(double angle) override;

  /**
   *
   *
   * @param angle
   *
   * @return
   */
  Ellipse rotated(double angle) const;

  /**
   * Translate the ellipse by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the ellipse itself.
   */
  Ellipse & translate(double dx, double dy) override;

  /**
   * Returns a translated copy of the ellipse.
   *
   * @param dx Shift of the first coordinate.
   * @param dy Shift of the second coordinate.
   *
   * @return A copy of the ellipse, translated.
   */
  Ellipse translated(double dx, double dy) const;

  /**
   * Scale the polyline, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   *
   * @return A reference to the polyline itself, once scaled.
   */
  Ellipse & scale(double sx, double sy) override;

  /**
   * Scale the ellipse, given a scaling factor.
   *
   * @param s Scaling factor.
   *
   * @return A reference to the ellipse itself, once scaled.
   */
  Ellipse & scale(double s) override;

  /**
   * Returns a scaled copy of the ellipse.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return A scaled copy of the ellipse.
   */
  Ellipse scaled(double sx, double sy) const;

  /**
   * Returns a scaled copy of the ellipse.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the ellipse.
   */
  Ellipse scaled(double s) const;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll(double s) override;

  /**
   * Returns a resized copy of the ellipse.
   *
   * @param w Width of the new ellipse.
   * @param h Height of the new ellipse.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   * @return A resized copy of the ellipse.
   */
  Ellipse resized(double w, double h, LineWidthFlag lineWidthFlag) const;

  void flushPostscript(std::ostream & stream, const TransformEPS & transform) const override;

  void flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const override;

  void flushSVG(std::ostream & stream, const TransformSVG & transform) const override;

  void flushTikZ(std::ostream & stream, const TransformTikZ & transform) const override;

  Rect boundingBox(LineWidthFlag) const override;

  Ellipse * clone() const override;

  /**
   * Set flag to save the ellipse as a circle as soon as radii are equal.
   */
  void setCircleFlag();

private:
  static const std::string _name; /**< The generic name of the shape. */
  bool isACircle() const;

protected:
  Point _center;
  double _xRadius;
  double _yRadius;
  double _angle;
  bool _isCreatedAsCircle;
};

Ellipse circle(double x, double y, double radius, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(), double lineWidth = Shape::defaultLineWidth(),
               const Shape::LineStyle lineStyle = Shape::defaultLineStyle(), int depth = -1);

Ellipse circle(Point center, double radius, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(), double lineWidth = Shape::defaultLineWidth(),
               const Shape::LineStyle lineStyle = Shape::defaultLineStyle(), int depth = -1);

} // namespace LibBoard

#if __cplusplus < 201100
#undef override
#endif

#endif /* _BOARD_ELLIPSE_H_ */
