/* -*- mode: c++ -*- */
/**
 * @file   Color.cpp
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
#include <BoardConfig.h>
#include <board/Color.h>
#include <board/Tools.h>
#include <cmath>
#include <cstdio>
using std::string;

namespace LibBoard
{

const Color Color::Null(nullptr);
const Color Color::Black((unsigned char)0, (unsigned char)0, (unsigned char)0);
const Color Color::Brown((unsigned char)0xA5, (unsigned char)0x2A, (unsigned char)0x2A);
const Color Color::Pink((unsigned char)0xFF, (unsigned char)0xC0, (unsigned char)0xCB);
const Color Color::Gray((unsigned char)128, (unsigned char)128, (unsigned char)128);
const Color Color::White((unsigned char)255, (unsigned char)255, (unsigned char)255);
const Color Color::Red((unsigned char)255, (unsigned char)0, (unsigned char)0);
const Color Color::DarkGreen((unsigned char)0, (unsigned char)0x64, (unsigned char)0);
const Color Color::Green((unsigned char)0, (unsigned char)128, (unsigned char)0);
const Color Color::Lime((unsigned char)0, (unsigned char)255, (unsigned char)0);
const Color Color::Blue((unsigned char)0, (unsigned char)0, (unsigned char)255);
const Color Color::Cyan((unsigned char)0, (unsigned char)255, (unsigned char)255);
const Color Color::DarkCyan((unsigned char)0, (unsigned char)0x8B, (unsigned char)0x8B);
const Color Color::Magenta((unsigned char)255, (unsigned char)0, (unsigned char)255);
const Color Color::Yellow((unsigned char)255, (unsigned char)255, (unsigned char)0);
const Color Color::Silver((unsigned char)190, (unsigned char)190, (unsigned char)190);
const Color Color::Purple((unsigned char)160, (unsigned char)32, (unsigned char)240);
const Color Color::Navy((unsigned char)0, (unsigned char)0, (unsigned char)128);
const Color Color::Aqua((unsigned char)0, (unsigned char)255, (unsigned char)255);

Color & Color::setHSV(float hue, float saturation, float value, float alpha)
{
  float c = value * saturation;
  float hp = hue / 60.0f;
  float x = c * (1 - std::abs(fmodf(hp, 2) - 1));
  float results[][3] = {{c, x, 0}, {x, c, 0}, {0, c, x}, {0, x, c}, {x, 0, c}, {c, 0, x}, {c, x, 0}};
  float * color = results[int(hp)];
  float m = value - c;
  _red = static_cast<int>(255 * (color[0] + m));
  _green = static_cast<int>(255 * (color[1] + m));
  _blue = static_cast<int>(255 * (color[2] + m));
  _alpha = static_cast<int>(255 * alpha);
  return *this;
}

Color & Color::setRGBf(float red, float green, float blue, float alpha)
{
  Tools::clamp(red, 0.0f, 1.0f);
  Tools::clamp(green, 0.0f, 1.0f);
  Tools::clamp(blue, 0.0f, 1.0f);
  Tools::clamp(alpha, 0.0f, 1.0f);
  _red = static_cast<unsigned char>(255 * red);
  _green = static_cast<unsigned char>(255 * green);
  _blue = static_cast<unsigned char>(255 * blue);
  _alpha = static_cast<unsigned char>(255 * alpha);
  return *this;
}

bool Color::operator==(const Color & other) const
{
  return _red == other._red && _green == other._green && _blue == other._blue && _alpha == other._alpha;
}

bool Color::operator!=(const Color & other) const
{
  return _red != other._red || _green != other._green || _blue != other._blue || _alpha != other._alpha;
}

bool Color::operator<(const Color & other) const
{
  if (_red < other._red)
    return true;
  if (_red == other._red) {
    if (_green < other._green)
      return true;
    if (_green == other._green) {
      if (_blue < other._blue)
        return true;
      if (_blue == other._blue)
        return _alpha < other._alpha;
    }
  }
  return false;
}

void Color::flushPostscript(std::ostream & stream) const
{
  stream << (_red / 255.0) << " " << (_green / 255.0) << " " << (_blue / 255.0) << " srgb\n";
}

string Color::postscript() const
{
  char buffer[255];
  secured_sprintf(buffer, 255, "%.4f %.4f %.4f", _red / 255.0, _green / 255.0, _blue / 255.0);
  return buffer;
}

string Color::svg() const
{
  char buffer[255];
  if (*this == Color::Null)
    return "none";
  secured_sprintf(buffer, 255, "rgb(%d,%d,%d)", _red, _green, _blue);
  return buffer;
}

string Color::svgAlpha(const char * prefix) const
{
  char buffer[255];
  if (_alpha == 255 || *this == Color::Null) {
    return "";
  }
  secured_sprintf(buffer, 255, " %s-opacity=\"%f\"", prefix, static_cast<double>(_alpha) / 255.0);
  return buffer;
}

string Color::tikz() const
{
  // see tex/generic/pgf/utilities/pgfutil-plain.def for color definitions
  char buffer[255];
  if (*this == Color::Null)
    return "none";
  if (*this == Color::Black)
    return "black";
  if (*this == Color::Gray)
    return "gray";
  if (*this == Color::White)
    return "white";
  if (*this == Color::Red)
    return "red";
  if (*this == Color::Green)
    return "green!50!black";
  if (*this == Color::Lime)
    return "green";
  if (*this == Color::Blue)
    return "blue";
  if (*this == Color::Silver)
    return "white!75!black";
  if (*this == Color::Purple)
    return "{rgb,255:red,160;green,32;blue,240}";
  if (*this == Color::Navy)
    return "blue!50!black";
  secured_sprintf(buffer, 255, "{rgb,255:red,%d;green,%d;blue,%d}", _red, _green, _blue);
  return buffer;
}

void Color::toHSV(float & hue, float & saturation, float & value, float & alpha) const
{
  float r = static_cast<float>(_red) / 255.0f;
  float g = static_cast<float>(_green) / 255.0f;
  float b = static_cast<float>(_blue) / 255.0f;
  alpha = static_cast<float>(_alpha) / 255.0f;
  value = std::max(r, std::max(g, b));
  const float & xMax = value;
  const float xMin = std::min(r, std::min(g, b));
  const float c = xMax - xMin;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
  if (c < 1e-5f) {
    hue = 0.0f;
  } else if (value == r) {
    hue = 60.0f * (g - b) / c;
  } else if (value == g) {
    hue = 60.0f * (2 + (b - r) / c);
  } else if (value == b) {
    hue = 60.0f * (4 + (r - g) / c);
  }
#pragma clang diagnostic pop
  if (hue < 0.0f) {
    hue += 360.0f;
  }
  saturation = (value < 1e-5f) ? 0.0f : (c / value);
}

Color Color::fromHSV(float hue, float saturation, float value, float alpha)
{
  Color c;
  c.setHSV(hue, saturation, value, alpha);
  return c;
}

Color Color::mixRGB(const Color & a, const Color & b, float t)
{
  float red = (a.red() + t * (static_cast<float>(b.red()) - a.red())) / 255.0f;
  float green = (a.green() + t * (static_cast<float>(b.green()) - a.green())) / 255.0f;
  float blue = (a.blue() + t * (static_cast<float>(b.blue()) - a.blue())) / 255.0f;
  float alpha = (a.alpha() + t * (static_cast<float>(b.alpha()) - a.alpha())) / 255.0f;
  return Color::fromRGBf(red, green, blue, alpha);
}

Color Color::mixHSV(const Color & a, const Color & b, float t)
{
  float ha, sa, va, aa;
  float hb, sb, vb, ab;
  a.toHSV(ha, sa, va, aa);
  b.toHSV(hb, sb, vb, ab);
  return fromHSV(ha + (hb - ha) * t, //
                 sa + (sb - sa) * t, //
                 va + (vb - va) * t, //
                 aa + (ab - aa) * t);
}

Color Color::fromHueColormap(float t)
{
  return Color::fromHSV(360.0f * t, 0.75f, 0.7f);
}

std::ostream & Color::flush(std::ostream & out) const
{
  if (isNull()) {
    return out << "Color(Null)";
  }
  float h, s, v, a;
  toHSV(h, s, v, a);
  out << "Color(RGBA:" << _red << "," << _green << "," << _blue << "," << _alpha << " HSV:" << h << "," << s << "," << v << ")";
  return out;
}

std::ostream & operator<<(std::ostream & out, const Color & color)
{
  return color.flush(out);
}

} // namespace LibBoard
