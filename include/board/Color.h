/* -*- mode: c++ -*- */
/**
 * @file   Color.h
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
 *
 * @brief  The Color structure.
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
#ifndef BOARD_COLOR_H
#define BOARD_COLOR_H

#include <board/Tools.h>
#include <cstddef>
#include <cstdio>
#include <ostream>
#include <string>

namespace LibBoard
{

/**
 * Color structure.
 *
 * @brief Structure representing an RGB triple.
 */
class Color {

public:
  inline Color();
  inline Color(const unsigned int rgb, unsigned char alpha = 255);
  inline Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255);
  inline Color(unsigned char gray, unsigned char alpha = 255);

  static inline Color fromRGBf(float red, float green, float blue, float alpha = 1.0f);
  static inline Color gray(unsigned char grayLevel, unsigned char alpha = 255);
  /**
   * @brief Build a color from an HTML color string (\#RRGGBB)
   * @param htmlColor An HTML color string
   * @param alpha The alpha value
   */
  inline Color(const std::string & htmlColor, unsigned char alpha);

  /**
   * @brief Build a color from an HTML color string (\#RRGGBB or \#RRGGBBAA)
   * @param htmlColor An HTML color string
   */
  inline Color(const std::string & htmlColor);

  inline Color(const char * htmlColor, unsigned char alpha = 255);

  /**
   * Create a null color
   */
  inline Color(std::nullptr_t);

  inline void red(unsigned char red);
  inline void green(unsigned char green);
  inline void blue(unsigned char blue);
  inline void alpha(unsigned char alpha);

  inline unsigned char red() const;
  inline unsigned char green() const;
  inline unsigned char blue() const;
  inline unsigned char alpha() const;

  /**
   * @brief setRGBi
   * @param red
   * @param green
   * @param blue
   * @param alpha
   * @return
   */
  inline Color & setRGBi(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha = 255);

  /**
   * @brief setHSV
   * @param hue
   * @param saturation
   * @param value
   * @param alpha
   * @return
   */
  Color & setHSV(float hue, float saturation, float value, float alpha = 1.0);

  Color & setRGBf(float red, float green, float blue, float alpha = 1.0);

  bool operator==(const Color & other) const;

  bool operator!=(const Color & other) const;

  bool operator<(const Color & other) const;

  void flushPostscript(std::ostream &) const;

  std::string svg() const;

  inline bool isNull() const;

  inline bool isValid() const;

  /**
   * Return a an SVG parameter string for the opacity value.
   *
   * @param prefix A prefix string to be appended to the returned string if not empty.
   *
   * @return An empty string if alpha == 255, otherwise the string \<prefix\>-opacity="\<alpha-value\>".
   */
  std::string svgAlpha(const char * prefix) const;

  std::string postscript() const;

  /**
   * Return a string representation of the color usable in TikZ commands.
   * Use the corresponding named color (or a mixture of a named color and black)
   * for predefined colors. Use a mixture of red, green and blue for general
   * colors.
   *
   * @return a string representation of the color usable in TikZ commands.
   */
  std::string tikz() const;

  void toHSV(float & hue, float & saturation, float & value, float & alpha) const;

  inline bool valid() const { return (*this) != Color::Null; }

  static Color fromHSV(float hue, float saturation, float value, float alpha = 1.0f);

  static Color mixRGB(const Color & a, const Color & b, float t);

  static Color mixHSV(const Color & a, const Color & b, float t);

  static Color fromHueColormap(float t);

  std::ostream & flush(std::ostream & out) const;

public:
  static const Color Null;
  static const Color Black;
  static const Color Brown;
  static const Color Pink;
  static const Color Gray;
  static const Color White;
  static const Color Red;
  static const Color Green;
  static const Color DarkGreen;
  static const Color Lime;
  static const Color Blue;
  static const Color Cyan;
  static const Color DarkCyan;
  static const Color Magenta;
  static const Color Yellow;
  static const Color Silver;
  static const Color Purple;
  static const Color Navy;
  static const Color Aqua;

private:
  int _red;   /**< The red component. */
  int _green; /**< The green component. */
  int _blue;  /**< The blue component. */
  int _alpha; /**< The opacity. */
};

std::ostream & operator<<(std::ostream & out, const Color & color);

// Inline methods

Color & Color::setRGBi(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha)
{
  _red = red;
  _green = green;
  _blue = blue;
  _alpha = alpha;
  return *this;
}

bool Color::isNull() const
{
  return _red == -1 && _green == -1 && _blue == -1;
}

bool Color::isValid() const
{
  return !isNull();
}

Color::Color() : _red(0), _green(0), _blue(0), _alpha(255) {}

Color::Color(std::nullptr_t)
{
  _red = _green = _blue = -1;
  _alpha = 255;
}

Color::Color(const unsigned int rgb, unsigned char alpha) : _alpha(alpha)
{
  _red = (rgb & 0xFF0000u) >> 16;
  _green = (rgb & 0xFF00u) >> 8;
  _blue = rgb & 0xFF;
}

Color::Color(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) : _red(red), _green(green), _blue(blue), _alpha(alpha) {}

Color::Color(unsigned char gray, unsigned char alpha) : _red(gray), _green(gray), _blue(gray), _alpha(alpha) {}

Color Color::fromRGBf(float red, float green, float blue, float alpha)
{
  return Color(static_cast<unsigned char>(red * 255), static_cast<unsigned char>(green * 255), static_cast<unsigned char>(blue * 255), static_cast<unsigned char>(alpha * 255));
}

Color Color::gray(unsigned char grayLevel, unsigned char alpha)
{
  return Color(grayLevel, grayLevel, grayLevel, alpha);
}

// FIXME : Make alpha parameter take precedence over AA in html string
Color::Color(const std::string & htmlColor, unsigned char alpha) : Color(htmlColor.c_str(), alpha) {}

Color::Color(const std::string & htmlColor) : Color(htmlColor.c_str(), 255) {}

Color::Color(const char * htmlColor, unsigned char alpha) : _alpha(alpha)
{
  unsigned int r, g, b, a = alpha;
  if (!htmlColor || *htmlColor == '\0') {
    *this = Color::Null;
    return;
  }
  if (strlen(htmlColor) == 7 && sscanf(htmlColor, "#%2x%2x%2x", &r, &g, &b) == 3) {
    _red = (int)r;
    _green = (int)g;
    _blue = (int)b;
    _alpha = (int)a;
  } else if (strlen(htmlColor) == 9 && sscanf(htmlColor, "#%2x%2x%2x%2x", &r, &g, &b, &a) == 4) {
    _red = (int)r;
    _green = (int)g;
    _blue = (int)b;
    _alpha = (int)a;
  } else {
    *this = Color::Null;
    Tools::error << "Color::Color(htmlcolor): cannot parse color string\n";
  }
}

void Color::red(const unsigned char red)
{
  _red = red;
}

void Color::green(unsigned char green)
{
  _green = green;
}

void Color::blue(unsigned char blue)
{
  _blue = blue;
}

void Color::alpha(unsigned char alpha)
{
  _alpha = alpha;
}

unsigned char Color::red() const
{
  return static_cast<unsigned char>(_red);
}

unsigned char Color::green() const
{
  return static_cast<unsigned char>(_green);
}

unsigned char Color::blue() const
{
  return static_cast<unsigned char>(_blue);
}

unsigned char Color::alpha() const
{
  return static_cast<unsigned char>(_alpha);
}
} // namespace LibBoard

#endif // BOARD_COLOR_H
