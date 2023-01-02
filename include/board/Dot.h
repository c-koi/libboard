/* -*- mode: c++ -*- */
/**
 * @file   Dot.h
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
#ifndef BOARD_DOT_H
#define BOARD_DOT_H

#include <board/ShapeWithStyle.h>

namespace LibBoard
{

/**
 * The dot structure. This primitive ha been reported as "missing" by
 * Manuel Peinado from Spain. Manuel also suggested
 * that rendering a dot consists in drawing a line
 * from a point to the same point with a roundcap style.
 *
 * @brief A line between two points.
 */
struct Dot : public ShapeWithStyle {

  inline Dot(double x, double y, Color color = Style::defaultPenColor(), double lineWidth = Style::defaultLineWidth());

  inline Dot(const Point & p, Color color = Style::defaultPenColor(), double lineWidth = Style::defaultLineWidth());

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const override;

  Point center(LineWidthFlag flage = IgnoreLineWidth) const override;

  /**
   * Rotates the dot around a given center of rotation.
   *
   * @param angle The rotation angle.
   * @param center The center of rotation.
   *
   * @return A reference to the Dot itself.
   */
  Dot & rotate(double angle, const Point & center) override;

  /**
   * Returns a rotated copy of the dot around a given rotation center.
   *
   * @param angle Th rotation angle.
   * @param center The center of rotation.
   *
   * @return A rotated copy of the Dot.
   */
  Dot rotated(double angle, const Point & center) const;

  /**
   * Rotates the dot around its center (actually does nothing!).
   *
   * @param angle The rotation angle.
   *
   * @return A reference to the Dot itself.
   */
  Dot & rotate(double angle) override;

  /**
   * Returns a copy of the dot rotated around its center (i.e. left unchanged!)
   *
   * @param angle The rotation angle.
   *
   * @return A copy of the dot.
   */
  Dot rotated(double angle) const;

  /**
   * Translate the dot by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the dot itself.
   */
  Dot & translate(double dx, double dy) override;

  /**
   * Returns a translated copy of the dot.
   *
   * @param dx Shift of the first coordinate.
   * @param dy Shift of the second coordinate.
   *
   * @return A copy of the dot, translated.
   */
  Dot translated(double dx, double dy) const;

  /**
   * Scale the dot, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   *
   * @return A reference to the dot itself, once scaled.
   */
  Dot & scale(double sx, double sy) override;

  /**
   * Scale the dot, given a scaling factor.
   *
   * @param s Scaling factor.
   *
   * @return A reference to the dot itself, once scaled.
   */
  Dot & scale(double s) override;

  /**
   * Returns a scaled copy of the dot, i.e. the dot itself.
   * (Dots are not actually scaled since their diameters is given by
   * the line width.)
   *
   * @param sx The scaling factor around the x axis.
   * @param sy The scaling factor around the y axis.
   *
   * @return A scaled copy of the Dot, i.e. the Dot itself.
   */
  Dot scaled(double sx, double sy) const;

  /**
   * Returns a scaled copy of the dot, i.e. the dot itself.
   * (Dots are not actually scaled since their diameters is given by
   * the line width.)
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the Dot, i.e. the Dot itself.
   */
  Dot scaled(double s) const;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll(double s) override;

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
   * Returns the bounding box of the dot.
   *
   * @return The rectangle of the bounding box.
   */
  Rect boundingBox(LineWidthFlag) const override;

  Dot * clone() const override;

  Dot(const Dot &) = default;
  Dot(Dot &&) = default;
  Dot & operator=(Dot &&) = default;
  Dot & operator=(const Dot &) = default;
  ~Dot() override = default;

private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
  double _x; /**< First coordinate of the dot. */
  double _y; /**< Second coordinate of the dot. */
};
} // namespace LibBoard

/*
 * Inline methods
 */

namespace LibBoard
{
Dot::Dot(double x, double y, Color color, double lineWidth) //
    : ShapeWithStyle(color, Color::Null, lineWidth, SolidStyle, RoundCap, MiterJoin), _x(x), _y(y)
{
}

Dot::Dot(const Point & p, Color color, double lineWidth) //
    : ShapeWithStyle(color, Color::Null, lineWidth, SolidStyle, RoundCap, MiterJoin), _x(p.x), _y(p.y)
{
}
} // namespace LibBoard

#endif /* BOARD_DOT_H */
