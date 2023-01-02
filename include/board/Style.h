/* -*- mode: c++ -*- */
/**
 * @file   Style.h
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
#ifndef BOARD_STYLE_H
#define BOARD_STYLE_H
#include <board/Color.h>
#include <iostream>
#include <stack>

namespace LibBoard
{
enum LineCap
{
  ButtCap = 0,
  RoundCap,
  SquareCap
};
enum LineJoin
{
  MiterJoin = 0,
  RoundJoin,
  BevelJoin
};
enum LineStyle
{
  SolidStyle = 0,
  DashStyle,
  DotStyle,
  DashDotStyle,
  DashDotDotStyle,
  DashDotDotDotStyle
};

enum LineWidthFlag
{
  IgnoreLineWidth,
  UseLineWidth
};

struct TransformTikZ;
struct TransformEPS;
struct TransformSVG;

/**
 * Structure describing shape style properties.
 */
struct Style {
  Color penColor;
  Color fillColor;
  double lineWidth;
  LineStyle lineStyle;
  LineCap lineCap;
  LineJoin lineJoin;

  /**
   * Create a style with (current) default values.
   */
  Style();

  Style(Color penColor, Color fillColor, double lineWidth, LineStyle linestyle, LineCap lineCap, LineJoin lineJoin);

  /**
   * Return a string of the svg properties lineWidth, opacity, penColor, fillColor,
   * lineCap, and lineJoin.
   *
   * @return A string of the properties suitable for inclusion in an svg tag.
   */
  std::string svgProperties(const TransformSVG & transform) const;

  /**
   * Return a string of the properties lineWidth, penColor, lineCap, and lineJoin
   * as Postscript commands.
   * @return A string of the Postscript commands.
   */
  std::string postscriptProperties(const TransformEPS & transform) const;

  /**
   * Return a string of the properties lineWidth, penColor, lineCap, and lineJoin
   * as TikZ commands.
   * @return A string of the TikZ commands.
   */
  std::string tikzProperties(const TransformTikZ & transform) const;

  static void setDefaultStyle(const Style &);

  static void setDefaultLineWidth(double);

  static void setDefaultPenColor(Color);

  static void setDefaultFillColor(Color);

  static void setDefaultLineStyle(LineStyle);

  static void setDefaultLineCap(LineCap);

  static void setDefaultLineJoin(LineJoin);

  /**
   * Returns a copy of the style, with modified pen color.
   * @param color New pen color.
   * @return A copy of the style, with modified pen color.
   */
  inline Style withPenColor(const Color & color) const;

  /**
   * Returns a copy of the style, with modified fill color.
   * @param color New fill color.
   * @return A copy of the style, with modified fill color.
   */
  inline Style withFillColor(const Color & color) const;

  /**
   * Returns a copy of the style, with modified line width.
   * @param width New line width.
   * @return A copy of the style, with modified line width.
   */
  inline Style withLineWidth(double width) const;

  /**
   * Returns a copy of the style, with modified line style.
   * @param lineStyle New line style.
   * @return A copy of the style, with modified line style.
   */
  inline Style withLineStyle(LineStyle lineStyle) const;

  /**
   * Returns a copy of the style, with modified line cap.
   * @param lineCap New line cap.
   * @return A copy of the style, with modified line cap.
   */
  inline Style withLineCap(LineCap lineCap) const;

  /**
   * Returns a copy of the style, with modified line join.
   * @param lineJoin New line join.
   * @return A copy of the style, with modified line join.
   */
  inline Style withLineJoin(LineJoin lineJoin) const;

  /**
   * @brief defaultStyle
   * @return Default shape style.
   */
  static inline const Style & defaultStyle();

  /**
   * @brief defaultLineWidth
   * @return Default lineWidth value for shapes (intialized as 1.0).
   */
  static inline const double & defaultLineWidth();

  /**
   * @brief defaultPenColor
   * @return Default pen color for shapes (initialized as Black)
   */
  static inline const Color & defaultPenColor();

  /**
   * @brief defaultFillColor
   * @return Default fill color for shapes (initialized as Color::None)
   */
  static inline const Color & defaultFillColor();

  /**
   * @brief defaultLineStyle
   * @return Default line style for shapes (initialized as SolidStyle)
   */
  static inline const LineStyle & defaultLineStyle();

  /**
   * @brief defaultLineCap
   * @return Default line cap for shapes (initialized as ButtCap)
   */
  static inline const LineCap & defaultLineCap();

  /**
   * @brief defaultLineJoin
   * @return Default line join for shapes (initialized as MiterJoin)
   */
  static inline const LineJoin & defaultLineJoin();

  /**
   * Push the current default style on top of the style stack.
   */
  static void push();

  /**
   * Restore the top of the style stack as the new default style (and pop it).
   */
  static void pop();

private:
  static Style _defaultStyle;
  static std::stack<Style> _styleStack;
};

std::ostream & operator<<(std::ostream & out, const Style & style);

extern const char * xFigDashStylesPS[];
extern const char * xFigDashStylesSVG[];
extern const char * xFigDashStylesTikZ[];

// Inline methods

Style Style::withPenColor(const Color & color) const
{
  Style style(*this);
  style.penColor = color;
  return style;
}

Style Style::withFillColor(const Color & color) const
{
  Style style(*this);
  style.fillColor = color;
  return style;
}

Style Style::withLineWidth(double width) const
{
  Style style(*this);
  style.lineWidth = width;
  return style;
}

Style Style::withLineStyle(LineStyle lineStyle) const
{
  Style style(*this);
  style.lineStyle = lineStyle;
  return style;
}

Style Style::withLineCap(LineCap lineCap) const
{
  Style style(*this);
  style.lineCap = lineCap;
  return style;
}

Style Style::withLineJoin(LineJoin lineJoin) const
{
  Style style(*this);
  style.lineJoin = lineJoin;
  return style;
}

const Style & Style::defaultStyle()
{
  return _defaultStyle;
}

const double & Style::defaultLineWidth()
{
  return _defaultStyle.lineWidth;
}

const Color & Style::defaultPenColor()
{
  return _defaultStyle.penColor;
}

const Color & Style::defaultFillColor()
{
  return _defaultStyle.fillColor;
}

const LineStyle & Style::defaultLineStyle()
{
  return _defaultStyle.lineStyle;
}

const LineCap & Style::defaultLineCap()
{
  return _defaultStyle.lineCap;
}

const LineJoin & Style::defaultLineJoin()
{
  return _defaultStyle.lineJoin;
}

} // namespace LibBoard

#endif /* BOARD_STYLE_H */
