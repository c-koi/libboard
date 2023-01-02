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
#ifndef BOARD_ARROW_H
#define BOARD_ARROW_H

#include <board/Line.h>

namespace LibBoard
{

/**
 * The arrow structure.
 * @brief A line between two points with an arrow at one extremity.
 */
struct Arrow : public Line {

  /**
   * @brief The ExtremityType enum
   */
  enum class ExtremityType
  {
    Stick,  /**< Two lines  */
    Closed, /**< Closed triangle (pen color, white filled) */
    Plain   /**< Plain triangle (with pen color)  */
  };

  /**
   * Constructs an arrow.
   *
   * @param x1 First coordinate of the start point.
   * @param y1 Second coordinate of the start point.
   * @param x2 First coordinate of the end point.
   * @param y2 Second coordinate of the end point.
   * @param type Arrow type.
   * @param penColor The color of the line.
   * @param fillColor The fill color of the sharp end.
   * @param lineWidth The line thickness.
   * @param lineStyle The line style.
   * @param cap The line cap.
   * @param join The line join.
   */
  inline Arrow(double x1, double y1, double x2, double y2, ExtremityType type = ExtremityType::Plain, Color penColor = Style::defaultPenColor(), Color fillColor = Style::defaultFillColor(),
               double lineWidth = Style::defaultLineWidth(), //
               const LineStyle lineStyle = Style::defaultLineStyle(), const LineCap cap = Style::defaultLineCap(), const LineJoin join = Style::defaultLineJoin());

  /**
   * Constructs an arrow.
   *
   * @param x1 First coordinate of the start point.
   * @param y1 Second coordinate of the start point.
   * @param x2 First coordinate of the end point.
   * @param y2 Second coordinate of the end point.
   * @param type Arrow type.
   * @param style The shape style.
   */
  inline Arrow(double x1, double y1, double x2, double y2, ExtremityType type, const Style & style);

  /**
   * Constructs an arrow.
   *
   * @param p1 Start point.
   * @param p2 End point.
   * @param type Arrow type.
   * @param penColor The color of the line.
   * @param fillColor The fill color of the sharp end.
   * @param lineWidth The line thickness.
   * @param lineStyle The line style.
   * @param cap The line cap.
   * @param join The line join.
   */
  inline Arrow(Point p1, Point p2, ExtremityType type = ExtremityType::Plain, //
               Color penColor = Style::defaultPenColor(),
               Color fillColor = Style::defaultFillColor(),  //
               double lineWidth = Style::defaultLineWidth(), //
               const LineStyle lineStyle = Style::defaultLineStyle(), const LineCap cap = Style::defaultLineCap(), const LineJoin join = Style::defaultLineJoin());

  /**
   * Constructs an arrow.
   *
   * @param p1 Start point.
   * @param p2 End point.
   * @param type Arrow type.
   * @param style The shape style.
   */
  inline Arrow(Point p1, Point p2, ExtremityType type, const Style & style);


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

  Arrow * clone() const override;

  Path extremity() const;

  Arrow(const Arrow &) = default;
  Arrow(Arrow &&) = default;
  Arrow & operator=(Arrow &&) = default;
  Arrow & operator=(const Arrow &) = default;
  ~Arrow() override = default;

  /**
   * @brief The extremity type of the arrow
   * @return The extremity type of the arrow
   */
  inline ExtremityType type() const;

private:
  static const std::string _name; /**< The generic name of the shape. */
  ExtremityType _type;
};

} // namespace LibBoard

/*
 * Inline methods
 */

namespace LibBoard
{
Arrow::Arrow(double x1, double y1, double x2, double y2, ExtremityType type, Color penColor, Color fillColor, double lineWidth, const LineStyle style, const LineCap cap, const LineJoin join)
    : Line(x1, y1, x2, y2, penColor, lineWidth, style, cap, join), _type(type)
{
  // FIXME : Handle fill color of the tip properly
  if (fillColor == Color::Null) {
    _style.fillColor = penColor;
  } else {
    _style.fillColor = fillColor;
  }
}

Arrow::Arrow(double x1, double y1, double x2, double y2, ExtremityType type, const Style & style) //
    : Line(x1, y1, x2, y2, style), _type(type)
{
  if (style.fillColor == Color::Null) {
    _style.fillColor = style.penColor;
  } else {
    _style.fillColor = style.fillColor;
  }
}

Arrow::Arrow(Point p1, Point p2, ExtremityType type,                        //
             Color penColor, Color fillColor, double lineWidth,             //
             const LineStyle style, const LineCap cap, const LineJoin join) //
    : Line(p1, p2, penColor, lineWidth, style, cap, join), _type(type)
{
  if (fillColor == Color::Null) {
    _style.fillColor = penColor;
  } else {
    _style.fillColor = fillColor;
  }
}

Arrow::Arrow(Point p1, Point p2, ExtremityType type, const Style & style) //
    : Line(p1, p2, style), _type(type)
{
  if (style.fillColor == Color::Null) {
    _style.fillColor = style.penColor;
  } else {
    _style.fillColor = style.fillColor;
  }
}

Arrow::ExtremityType Arrow::type() const
{
  return _type;
}
} // namespace LibBoard

#endif /* BOARD_ARROW_H */
