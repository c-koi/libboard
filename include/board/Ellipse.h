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
#ifndef BOARD_ELLIPSE_H
#define BOARD_ELLIPSE_H

#include <board/ShapeWithStyle.h>

namespace LibBoard
{

/**
 * The ellipse structure.
 * @brief An ellipse.
 */
struct Ellipse : public ShapeWithStyle {

  enum SamplingStart
  {
    SamplingFromRight,
    SamplingFromTop,
    SamplingFromLeft,
    SamplingFromBottom
  };

  inline Ellipse(double x, double y, double xRadius, double yRadius,                                     //
                 Color penColor = Style::defaultPenColor(), Color fillColor = Style::defaultFillColor(), //
                 double lineWidth = Style::defaultLineWidth(), LineStyle lineStyle = Style::defaultLineStyle());

  inline Ellipse(Point center, double xRadius, double yRadius,                                           //
                 Color penColor = Style::defaultPenColor(), Color fillColor = Style::defaultFillColor(), //
                 double lineWidth = Style::defaultLineWidth(), const LineStyle lineStyle = Style::defaultLineStyle());

  inline Ellipse(Point center, double xRadius, double yRadius, Style style);

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const override;

  Point center(LineWidthFlag lineWidthFlag = IgnoreLineWidth) const override;

  inline double xRadius() const;
  inline double yRadius() const;
  inline double angle() const;

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

  /**
   * @brief Accepts a visitor object.
   *
   * @param visitor A visitor object.
   */
  virtual void accept(ShapeVisitor & visitor) override;

  /**
   * @brief Accepts a visitor object.
   *
   * @param visitor A visitor object.
   */
  virtual void accept(const ShapeVisitor & visitor) override;

  /**
   * @brief Accepts a const-shape visitor object.
   *
   * @param visitor A const-shape visitor object.
   */
  virtual void accept(ConstShapeVisitor & visitor) const override;

  /**
   * @brief Accepts a const-shape visitor object.
   *
   * @param visitor A const-shape visitor object.
   */
  virtual void accept(const ConstShapeVisitor & visitor) const override;

  /**
   * @brief Accept a composite shape transform.
   *
   * @param transform A composite shape transform object.
   */
  virtual Shape * accept(CompositeShapeTransform & transform) const override;

  /**
   * @brief Accept a constant composite shape transform.
   *
   * @param transform A constant composite shape transform object..
   */
  virtual Shape * accept(const CompositeShapeTransform & transform) const override;

  Rect boundingBox(LineWidthFlag) const override;

  Ellipse * clone() const override;

  /**
   * Set flag to save the ellipse as a circle as soon as radii are equal.
   */
  void setCircleFlag();

  /**
   * Estimated perimeter of the ellipse.
   * @return The estimated perimeter.
   */
  double perimeter() const;

  /**
   * Return uniformly sampled points along the ellipse (counterclockwise).
   * @param n Number of sample points
   * @param start Position of the first point
   * @return A path of the samples points
   */
  Path sampledPath(int n, SamplingStart start = SamplingFromRight) const;

  Ellipse(const Ellipse &) = default;
  Ellipse(Ellipse &&) = default;
  Ellipse & operator=(Ellipse &&) = default;
  Ellipse & operator=(const Ellipse &) = default;
  ~Ellipse() override = default;

private:
  static const std::string _name; /**< The generic name of the shape. */
  bool isACircle() const;
  inline void normaliseAngle();

protected:
  Point _center;
  double _xRadius;
  double _yRadius;
  double _angle;
  bool _isCreatedAsCircle;
};

Ellipse circle(double x, double y, double radius,                                                      //
               Color penColor = Style::defaultPenColor(), Color fillColor = Style::defaultFillColor(), //
               double lineWidth = Style::defaultLineWidth(), const LineStyle lineStyle = Style::defaultLineStyle());

Ellipse circle(Point center, double radius,                                                            //
               Color penColor = Style::defaultPenColor(), Color fillColor = Style::defaultFillColor(), //
               double lineWidth = Style::defaultLineWidth(), const LineStyle lineStyle = Style::defaultLineStyle());

Ellipse circle(Point center, double radius, Style style);

// Inline methods

Ellipse::Ellipse(double x, double y, double xRadius, double yRadius, Color penColor, Color fillColor, double lineWidth, LineStyle lineStyle)
    : ShapeWithStyle(penColor, fillColor, lineWidth, lineStyle, ButtCap, MiterJoin), //
      _center(x, y), _xRadius(xRadius), _yRadius(yRadius), _angle(0.0), _isCreatedAsCircle(false)
{
  normaliseAngle();
}

Ellipse::Ellipse(Point center, double xRadius, double yRadius, Color penColor, Color fillColor, double lineWidth, const LineStyle lineStyle)
    : ShapeWithStyle(penColor, fillColor, lineWidth, lineStyle, ButtCap, MiterJoin), //
      _center(center), _xRadius(xRadius), _yRadius(yRadius), _angle(0.0), _isCreatedAsCircle(false)
{
  normaliseAngle();
}

inline Ellipse::Ellipse(Point center, double xRadius, double yRadius, Style style) //
    : ShapeWithStyle(style),                                                       //
      _center(center), _xRadius(xRadius), _yRadius(yRadius), _angle(0.0), _isCreatedAsCircle(false)
{
  normaliseAngle();
}

inline void Ellipse::normaliseAngle()
{
  while (_angle > M_PI_2) {
    _angle -= M_PI;
  }
  while (_angle < -M_PI_2) {
    _angle += M_PI;
  }
}

double Ellipse::xRadius() const
{
  return _xRadius;
}

double Ellipse::yRadius() const
{
  return _yRadius;
}

double Ellipse::angle() const
{
  return _angle;
}

} // namespace LibBoard

#endif /* BOARD_ELLIPSE_H */
