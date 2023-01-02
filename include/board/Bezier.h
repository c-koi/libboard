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
#ifndef BOARD_BEZIER_H
#define BOARD_BEZIER_H

#include <board/ShapeWithStyle.h>

// TODO : Add a merge (+=) operation to link to bezier curve

namespace LibBoard
{

/**
 * The Bezier structure.
 *
 * @brief A Bezier curve described by two series of
 * 2D points (curve points and control points).
 */
struct Bezier : public ShapeWithStyle {

  /**
   * @brief A Bezier curve described by two series of
   * 2D points (curve points and control points).
   *
   * @param points Points of the bezier curve (n points)
   * @param controls Controls points (2(n-1) points)
   * @param penColor
   * @param fillColor
   * @param lineWidth
   * @param lineStyle
   * @param cap
   * @param join
   */
  Bezier(const std::vector<Point> & points, const std::vector<Point> & controls,                              //
         Color penColor = Style::defaultPenColor(), Color fillColor = Style::defaultFillColor(),              //
         double lineWidth = Style::defaultLineWidth(), const LineStyle lineStyle = Style::defaultLineStyle(), //
         const LineCap cap = Style::defaultLineCap(), const LineJoin join = Style::defaultLineJoin());

  /**
   * @brief A cubic bezier curve
   *
   * @param points Points of the bezier curve (n points)
   * @param controls Controls points (2(n-1) points)
   * @param style Shape style.
   */
  Bezier(const std::vector<Point> & points, const std::vector<Point> & controls, //
         const Style & style);

  /**
   * @brief A bezier curve defined by its two extremities and two control points
   *
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
   */
  Bezier(Point p0, Point control0, Point p1, Point control1,                                                  //
         Color penColor = Style::defaultPenColor(), Color fillColor = Style::defaultFillColor(),              //
         double lineWidth = Style::defaultLineWidth(), const LineStyle lineStyle = Style::defaultLineStyle(), //
         const LineCap cap = Style::defaultLineCap(), const LineJoin join = Style::defaultLineJoin());

  /**
   * @brief A bezier curve defined by its two extremities and two control points
   *
   * @param p0 Start point
   * @param control0 Start control point
   * @param p1 End point
   * @param control1 End control point
   * @param style Shape style.
   */
  Bezier(Point p0, Point control0, Point p1, Point control1, //
         const Style & style);

  /**
   * @brief A bezier curve defined by its two extremities and two control points
   *
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
   */
  Bezier(double x0, double y0, double xc0, double yc0, double x1, double y1, double xc1, double yc1,          //
         Color penColor = Style::defaultPenColor(), Color fillColor = Style::defaultFillColor(),              //
         double lineWidth = Style::defaultLineWidth(), const LineStyle lineStyle = Style::defaultLineStyle(), //
         const LineCap cap = Style::defaultLineCap(), const LineJoin join = Style::defaultLineJoin());

  /**
   * @brief A bezier curve defined by its two extremities and two control points
   *
   * @param x0 Start point (x coordinate)
   * @param y0 Start point (y coordinate)
   * @param xc0 Start control point (x coordinate)
   * @param yc0 Start control point (y coordinate)
   * @param x1 End point (x coordinate)
   * @param y1 End point (y coordinate)
   * @param xc1 End control point (x coordinate)
   * @param yc1 End control point (y coordinate)
   * @param style Shape style.
   */
  Bezier(double x0, double y0, double xc0, double yc0, double x1, double y1, double xc1, double yc1, //
         const Style & style);

  /**
   * The generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return The generic name of the shape
   */
  const std::string & name() const override;

  /**
   * @brief Rotate the Bezier curve.
   *
   * @param angle The rotation angle.
   * @param center Center of the rotation.
   * @return The curve itself.
   */
  Bezier & rotate(double angle, const Point & center) override;

  /**
   * @brief Rotated copy of the curve.
   * @param angle A rotation angle (in radians)
   * @param center The center of rotation
   * @return A rotated copy of the curve
   */
  Bezier rotated(double angle, const Point & center) const;

  /**
   * @brief Rotate the Bezier curve around its bounding box center
   * @param angle The rotation angle.
   * @return The curve itself.
   */
  Bezier & rotate(double angle) override;

  /**
   * @brief Return a rotated copy of the curve around its bounding box center
   * @param angle The rotation angle.
   * @return A rotated copy of the curve.
   */
  Bezier rotated(double angle) const;

  /**
   * Translate the Bezier by a given offset.
   *
   * @param dx Shift of the first coordinate.
   * @param dy Shift of the second coordinate.
   * @return A reference to the Bezier itself.
   */
  Bezier & translate(double dx, double dy) override;

  /**
   * Returns a translated copy of the Bezier.
   *
   * @param dx Shift of the first coordinate.
   * @param dy Shift of the second coordinate.
   * @return A copy of the Bezier, translated.
   */
  Bezier translated(double dx, double dy) const;

  /**
   * Scale the Bezier, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   * @return A reference to the Bezier itself, once scaled.
   */
  Bezier & scale(double sx, double sy) override;

  /**
   * Scale the Bezier, given a scaling factor.
   *
   * @param s Scaling factor.
   * @return A reference to the Bezier itself, once scaled.
   */
  Bezier & scale(double s) override;

  /**
   * Returns a scaled copy of the line.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   * @return A scaled copy of the line.
   */
  Bezier scaled(double sx, double sy) const;

  /**
   * Returns a scaled copy of the line.
   *
   * @param s The scaling factor.
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

  /**
   * @brief Return the bounding box of the curve.
   * @return The bounding box of the curve.
   */
  Rect boundingBox(LineWidthFlag) const override;

  /**
   * @brief Return a copy of the curve.
   * @return a copy of the curve.
   */
  Bezier * clone() const override;

  /**
   * @brief The series of points defining the curve, as a path.
   * @return The series of points defining the curve, as a path.
   */
  inline const Path & path() const;

  /**
   * @brief The series of control points points defining the curve, as a path.
   * @return The series of control points points defining the curve, as a path.
   */
  inline const Path & controls() const;

  /**
   * @brief A sequence of properly spaced points approximating the curve.
   * @return A path of properly spaced points approximating the curve.
   */
  Path discretizedPath() const;

  /**
   * @brief A path through local extremums.
   * @return A path through local extremums.
   */
  std::vector<Point> pathThroughLocalExtremums() const;

  /**
   * @brief Add points and crontrol points of another curve.
   * @param other A Bezier curve.
   * @return This Bezier curve, appended.
   */
  Bezier & operator+=(const Bezier & other);

  /**
   * @brief Concatenate two Bezier curves.
   * @param other A Bezier curve.
   * @return A new Bezier curve, concatenation of this curve with other.
   */
  Bezier operator+(const Bezier & other) const;

  /**
   * @brief Build a Bezier curve as a 'rounded' version of a Polyline.
   * @param path A sequence of points defining a polyline.
   * @param roundness
   * @param style
   * @return A bezier curve
   */
  static Bezier smoothedPolyline(const std::vector<Point> & path, double roundness, const Style & style = Style::defaultStyle());

  /**
   * @brief Build a Bezier curve as a 'rounded' version of a Polyline.
   * @param path A path defining a polyline.
   * @param roundness
   * @param style
   * @return A bezier curve
   */
  static Bezier smoothedPolyline(const Path & path, double roundness, const Style & style = Style::defaultStyle());

  /**
   * @brief Bezier interpolation passing through 4 points
   * @param a
   * @param b
   * @param c
   * @param d
   * @param style
   * @return A Bezier interpolation passing through the 4 points a, b, c, d
   */
  static Bezier interpolation(const Point & a, const Point & b, const Point & c, const Point & d, const Style & style = Style::defaultStyle());

  /**
   * @brief Get a point of the curve given its interval between two curve points and a position in [0..1].
   * @param interval Position of the preceding curve point.
   * @param t Time position within the interval, in [0..1].
   * @return
   */
  Point eval(Path::size_type interval, double t) const;

  Bezier(const Bezier &) = default;
  Bezier(Bezier &&) = default;
  Bezier & operator=(Bezier &&) = default;
  Bezier & operator=(const Bezier &) = default;
  ~Bezier() override = default;

private:
  Point eval(const Point & p0, const Point & p1, const Point & p2, const Point & p3, double t) const;
  static const std::string _name; /**< The generic name of the shape. */

protected:
  Path _path;
  Path _controls;
};

// Inline methods

const Path & Bezier::path() const
{
  return _path;
}

const Path & Bezier::controls() const
{
  return _controls;
}

} // namespace LibBoard

#endif /* BOARD_BEZIER_H */
