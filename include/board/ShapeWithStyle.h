/* -*- mode: c++ -*- */
/**
 * @file   ShapeWithStyle.h
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
#ifndef BOARD_SHAPE_WITH_STYLE_H
#define BOARD_SHAPE_WITH_STYLE_H

#include <board/Shape.h>

namespace LibBoard
{

/**
 * ShapeWithStyle structure.
 * @brief Abstract structure for a 2D shape.
 */
struct ShapeWithStyle : public Shape {

  /**
   * ShapeWithStyle constructor.
   *
   * @param penColor The pen color of the shape.
   * @param fillColor The fill color of the shape.
   * @param lineWidth The line thickness.
   * @param style The line style.
   * @param cap The line cap style.
   * @param join The line join style.
   */
  inline ShapeWithStyle(Color penColor, Color fillColor, double lineWidth, LineStyle style, const LineCap cap, const LineJoin join);

  /**
   * ShapeWithStyle constructor.
   *
   * @param style Style properties
   */
  inline ShapeWithStyle(const Style & style);

  ShapeWithStyle(const ShapeWithStyle & other);

  ShapeWithStyle & operator=(const ShapeWithStyle & other);

  /**
   * Shape destructor.
   */
  ~ShapeWithStyle();

  /**
   * @return "ShapeWithStyle"
   */
  const std::string & name() const;

  Shape * clone() const = 0;

  inline const Color & penColor() const;

  inline ShapeWithStyle & setPenColor(const Color &);

  inline const Color & fillColor() const;

  inline ShapeWithStyle & setFillColor(const Color &);

  inline double lineWidth() const;

  inline ShapeWithStyle & setLineWidth(double);

  inline LineStyle lineStyle() const;

  inline ShapeWithStyle & setLineStyle(LineStyle);

  inline LineCap lineCap() const;

  inline ShapeWithStyle & setLineCap(LineCap);

  inline LineJoin lineJoin() const;

  inline ShapeWithStyle & setLineJoin(LineJoin);

  inline Style style() const;

  inline ShapeWithStyle & setStyle(const Style &);

  /**
   *  Globally enable/disable linewidth scaling when using scale functions.
   */
  static void setLineWidthScaling(bool);

  /**
   * Checks whether a shape is filled with a color or not.
   *
   * @return true if the shape is filled.
   */
  inline bool filled() const;

private:
  static const std::string _name; /**< The generic name of the shape. */
  static bool _lineWidthScaling;

protected:
  inline void updateLineWidth(double s);

  Style _style;
};

} // namespace LibBoard

/*
 * Inline methods
 */

namespace LibBoard
{
ShapeWithStyle::ShapeWithStyle(Color penColor, Color fillColor, double lineWidth, LineStyle lineStyle, const LineCap lineCap, const LineJoin lineJoin)
    : _style{penColor, fillColor, lineWidth, lineStyle, lineCap, lineJoin}
{
}

ShapeWithStyle::ShapeWithStyle(const Style & style) //
    : _style(style)
{
}

const Color & ShapeWithStyle::penColor() const
{
  return _style.penColor;
}

ShapeWithStyle & ShapeWithStyle::setPenColor(const Color & color)
{
  _style.penColor = color;
  return *this;
}

const Color & ShapeWithStyle::fillColor() const
{
  return _style.fillColor;
}

ShapeWithStyle & ShapeWithStyle::setFillColor(const Color & color)
{
  _style.fillColor = color;
  return *this;
}

double ShapeWithStyle::lineWidth() const
{
  return _style.lineWidth;
}

ShapeWithStyle & ShapeWithStyle::setLineWidth(double width)
{
  _style.lineWidth = width;
  return *this;
}

LineStyle ShapeWithStyle::lineStyle() const
{
  return _style.lineStyle;
}

ShapeWithStyle & ShapeWithStyle::setLineStyle(LineStyle style)
{
  _style.lineStyle = style;
  return *this;
}

LineCap ShapeWithStyle::lineCap() const
{
  return _style.lineCap;
}

ShapeWithStyle & ShapeWithStyle::setLineCap(LineCap cap)
{
  _style.lineCap = cap;
  return *this;
}

LineJoin ShapeWithStyle::lineJoin() const
{
  return _style.lineJoin;
}

ShapeWithStyle & ShapeWithStyle::setLineJoin(LineJoin join)
{
  _style.lineJoin = join;
  return *this;
}

ShapeWithStyle & ShapeWithStyle::setStyle(const Style & style)
{
  _style = style;
  return *this;
}

Style ShapeWithStyle::style() const
{
  return _style;
}

bool ShapeWithStyle::filled() const
{
  return fillColor() != Color::Null;
}

void ShapeWithStyle::updateLineWidth(double s)
{
  if (_lineWidthScaling) {
    _style.lineWidth *= s;
  }
}

} // namespace LibBoard

#endif /* BOARD_SHAPE_WITH_STYLE_H */
