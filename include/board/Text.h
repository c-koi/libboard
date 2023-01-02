/* -*- mode: c++ -*- */
/**
 * @file   Text.h
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
#ifndef BOARD_TEXT_H
#define BOARD_TEXT_H

#include <board/ShapeWithStyle.h>

// TODO : Add a sketchy line text font

namespace LibBoard
{

/**
 * The text structure.
 * @brief A piece of text.
 */
struct Text : public ShapeWithStyle {

  /**
   * Create a Text sctucture.
   *
   * @param x x-coordinate of the bottom-left corner.
   * @param y y-coordinate of the bottom-left corner.
   * @param text
   * @param font
   * @param size
   * @param color
   */
  Text(double x, double y, const std::string & text, const Fonts::Font font, double size, Color color = Color::Black);

  /**
   * Create a Text sctucture.
   *
   * @param p Position of the bottom-left corner.
   * @param text
   * @param font
   * @param size
   * @param color
   */
  Text(Point p, const std::string & text, const Fonts::Font font, double size, Color color = Color::Black);

  /**
   * Create a Text sctucture.
   *
   * @param x x-coordinate of the bottom-left corner.
   * @param y y-coordinate of the bottom-left corner.
   * @param text
   * @param font
   * @param svgFont The font family for an SVG file. (E.g. "Verdana, Arial" or "'Time New Roman', Serif" )
   * @param size
   * @param color
   */
  Text(double x, double y, const std::string & text, const Fonts::Font font, const std::string & svgFont, double size, Color color = Color::Black);

  /**
   * Create a Text sctucture.
   *
   * @param p Position of the bottom-left corner.
   * @param text
   * @param font
   * @param svgFont The font family for an SVG file. (E.g. "Verdana, Arial" or "'Time New Roman', Serif" )
   * @param size
   * @param color
   */
  Text(Point p, const std::string & text, const Fonts::Font font, const std::string & svgFont, double size, Color color = Color::Black);

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const override;

  Point center(LineWidthFlag lineWidthFlag = IgnoreLineWidth) const override;

  Text & rotate(double angle, const Point & center) override;

  Text rotated(double angle, const Point & center) const;

  Text & rotate(double angle) override;

  Text rotated(double angle) const;

  /**
   * Translate the text by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the text itself.
   */
  Text & translate(double dx, double dy) override;

  /**
   * Returns a translated copy of the text.
   *
   * @param dx Shift of the first coordinate.
   * @param dy Shift of the second coordinate.
   *
   * @return A copy of the text, translated.
   */
  Text translated(double dx, double dy) const;

  /**
   * Scale the text, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   *
   * @return A reference to the text itself, once scaled.
   */
  Text & scale(double sx, double sy) override;

  /**
   * Scale the text, given a scaling factor.
   *
   * @param s Scaling factor.
   *
   * @return A reference to the text itself, once scaled.
   */
  Text & scale(double s) override;

  /**
   * Returns a scaled copy of the text.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return A scaled copy of the text.
   */
  Text scaled(double sx, double sy) const;

  /**
   * Returns a scaled copy of the text.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the text.
   */
  Text scaled(double s) const;

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

  Text * clone() const override;

  Text(const Text &) = default;
  Text(Text &&) = default;
  Text & operator=(Text &&) = default;
  Text & operator=(const Text &) = default;
  ~Text() override = default;

private:
  static const std::string _name; /**< The generic name of the shape. */

  double boxHeight(const Transform &) const;
  double boxLength(const Transform &) const;

  double angle() const;

  Point position() const;

protected:
  std::string _text;
  Fonts::Font _font;
  std::string _svgFont;
  double _size;
  double _xScale;
  double _yScale;
  Path _box; // Rectangle around the text
};

} // namespace LibBoard

#endif /* BOARD_TEXT_H */
