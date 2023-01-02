/* -*- mode: c++ -*- */
/**
 * @file   Text.cpp
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
#include <board/Text.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <sstream>
#include <vector>
#include <BoardConfig.h>
#include <board/PSFonts.h>
#include <board/PathBoundaries.h>
#include <board/Rect.h>
#include <board/ShapeVisitor.h>
#include <board/Tools.h>
#include <board/Transforms.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

namespace LibBoard
{

extern const char * XFigPostscriptFontnames[];

const std::string Text::_name("Text");

Text::Text(double x, double y, const std::string & text, const Fonts::Font font, double size, Color color)
    : ShapeWithStyle(color, Color::Null, 1.0, SolidStyle, ButtCap, MiterJoin), //
      _text(text), _font(font), _size(size), _xScale(1.0), _yScale(1.0)
{
  const double width = text.length() * size * 0.71; // Why 0.71? Well, give it a try!
  _box << Point(x, y);
  _box << _box[0] + Point(width, 0);
  _box << _box[0] + Point(width, size);
  _box << _box[0] + Point(0, size);
  _box.close();
}

Text::Text(Point p, const std::string & text, const Fonts::Font font, double size, Color color)
    : ShapeWithStyle(color, Color::Null, 1.0, SolidStyle, ButtCap, MiterJoin), //
      _text(text), _font(font), _size(size), _xScale(1.0), _yScale(1.0)
{
  const double width = text.length() * size * 0.71; // Why 0.71? Well, give it a try!
  _box << p;
  _box << _box[0] + Point(width, 0);
  _box << _box[0] + Point(width, size);
  _box << _box[0] + Point(0, size);
  _box.close();
}

Text::Text(double x, double y, const std::string & text, const Fonts::Font font, const std::string & svgFont, double size, Color color)
    : ShapeWithStyle(color, Color::Null, 1.0, SolidStyle, ButtCap, MiterJoin), //
      _text(text), _font(font), _svgFont(svgFont), _size(size), _xScale(1.0), _yScale(1.0)
{
  const double width = text.length() * size * 0.71; // Why 0.71? Well, give it a try!
  _box << Point(x, y);
  _box << _box[0] + Point(width, 0);
  _box << _box[0] + Point(width, size);
  _box << _box[0] + Point(0, size);
  _box.close();
}

Text::Text(Point p, const std::string & text, const Fonts::Font font, const std::string & svgFont, double size, Color color)
    : ShapeWithStyle(color, Color::Null, 1.0, SolidStyle, ButtCap, MiterJoin), //
      _text(text), _font(font), _svgFont(svgFont), _size(size), _xScale(1.0), _yScale(1.0)
{
  const double width = text.length() * size * 0.71; // Why 0.71? Well, give it a try!
  _box << p;
  _box << _box[0] + Point(width, 0);
  _box << _box[0] + Point(width, size);
  _box << _box[0] + Point(0, size);
  _box.close();
}

const std::string & Text::name() const
{
  return _name;
}

Point Text::center(LineWidthFlag) const
{
  return _box.center();
}

Text & Text::rotate(double angle, const Point & center)
{
  _box.rotate(angle, center);
  //  Point endPos = _position + Point( 10000 * cos( _angle ), 10000 * sin( _angle ) );
  //  _position.rotate( angle, center );
  //  endPos.rotate( angle, center );
  //  Point v = endPos - _position;
  //  v /= v.norm();
  //  if ( v.x >= 0 ) _angle = asin( v.y );
  //  else if ( v.y > 0 ) _angle = (M_PI/2.0) + acos( v.y );
  //  else _angle = (-M_PI/2.0) - acos( -v.y );
  return *this;
}

Text Text::rotated(double angle, const Point & center) const
{
  return Text(*this).rotate(angle, center);
}

Text & Text::rotate(double angle)
{
  _box.rotate(angle);
  //  _angle += angle;
  //  while ( _angle < -M_PI ) {
  //    _angle += 2 * M_PI;
  //  }
  //  while ( _angle > M_PI ) {
  //    _angle -= 2 * M_PI;
  //  }
  return *this;
}

Text Text::rotated(double angle) const
{
  return Text(*this).rotate(angle);
}

Text & Text::translate(double dx, double dy)
{
  _box.translate(dx, dy);
  return *this;
}

Text Text::translated(double dx, double dy) const
{
  return static_cast<Text &>(Text(*this).translate(dx, dy));
}

Text & Text::scale(double sx, double sy)
{
  // TODO Actually handle scaling.
  _xScale *= sx;
  _yScale *= sy;
  _box.scale(sx, sy);
  return *this;
}

Text & Text::scale(double s)
{
  _xScale *= s;
  _yScale *= s;
  _box.scale(s);
  return *this;
}

Text Text::scaled(double sx, double sy) const
{
  return Text(*this).scale(sx, sy);
}

Text Text::scaled(double s) const
{
  return Text(*this).scale(s, s);
}

void Text::scaleAll(double s)
{
  _box.scaleAll(s);
}

Text * Text::clone() const
{
  return new Text(*this);
}

double Text::boxHeight(const Transform & transform) const
{
  Point baseline = (_box[1] - _box[0]);
  Point leftSide = (_box[3] - _box[0]);
  Point transformedLeftSide(transform.map(_box[3]) - transform.map(_box[0]));
  double angle = std::acos(baseline.normalised() * leftSide.normalised());
  if (std::fabs(2 * angle - M_PI) < std::numeric_limits<double>::epsilon()) {
    return transformedLeftSide.norm();
  }
  return std::sin(angle) * transformedLeftSide.norm();
}

double Text::boxLength(const Transform & transform) const
{
  Point transformedBaseline(transform.map(_box[1]) - transform.map(_box[0]));
  return transformedBaseline.norm();
}

double Text::angle() const
{
  Point v = (_box[1] - _box[0]).normalise();
  return std::atan2(v.y, v.x);
}

Point Text::position() const
{
  return _box[0];
}

void Text::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  stream << "\n% Text\n";
  stream << "gs /" << PSFontNames[_font] << " ff " << boxHeight(transform) << " scf sf";
  stream << " " << transform.mapX(position().x) << " " << transform.mapY(position().y) << " m";
  if (angle() != 0.0) {
    stream << " " << (angle() * 180.0 / M_PI) << " rot ";
  }
  stream << " (" << _text << ")"
         << " " << penColor().postscript() << " srgb"
         << " sh gr" << std::endl;
}

void Text::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  const double ppmm = 720.0 / 254.0;
  const double fig_ppmm = 45.0;

  stream << "4 0 ";
  // Color, depth, unused, Font
  stream << colormap[penColor()] << " " << transform.shapeDepth(this) << " -1 " << _font << " ";
  // Font size, Angle, Font flags
  stream << (ppmm * boxHeight(transform) / fig_ppmm) << " " << angle() << " 4 ";
  // Height
  stream << static_cast<int>(boxHeight(transform) /* * 135 / 12.0 */) << " ";
  // Width
  stream << static_cast<int>(boxLength(transform) /* * 135 / 12.0 */) << " ";
  // x y
  stream << static_cast<int>(transform.mapX(position().x)) << " " << static_cast<int>(transform.mapY(position().y)) << " ";
  stream << _text << "\\001\n";
}

void Text::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  if (angle() != 0.0) {
    stream << "<g transform=\"translate(" << transform.mapX(position().x) << "," << transform.mapY(position().y) << ")\" >"
           << "<g transform=\"rotate(" << (-angle() * 180.0 / M_PI) << ")\" >"
           << "<text x=\"0\" y=\"0\""
           << " font-family=\"" << (_svgFont.length() ? _svgFont : PSFontNames[_font]) << "\""
           << " font-size=\"" << boxHeight(transform) << "\""
           << " fill=\"" << penColor().svg() << "\"" << fillColor().svgAlpha(" fill") << penColor().svgAlpha(" stroke") << ">" << _text << "</text></g></g>" << std::endl;
  } else {
    stream << "<text x=\"" << transform.mapX(position().x) << "\" y=\"" << transform.mapY(position().y) << "\" "
           << " font-family=\"" << (_svgFont.length() ? _svgFont : PSFontNames[_font]) << "\""
           << " font-size=\"" << boxHeight(transform) << "\""
           << " fill=\"" << penColor().svg() << "\"" << fillColor().svgAlpha(" fill") << penColor().svgAlpha(" stroke") << ">" << _text << "</text>" << std::endl;
  }
  // DEBUG
  // Polyline(_box,Color::Black,Color::None,0.5).flushSVG(stream,transform);
}

void Text::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  // FIXME: honor font-size
  static const char BOLD_FONT = 0x01;
  static const char ITALIC_FONT = 0x02;
  static const char MONOSPACE_FONT = 0x04;
  static const char SANSSERIF_FONT = 0x08;
  char fontTraits[] = {
      0,                                        // TimesRoman,
      ITALIC_FONT,                              // TimesItalic,
      BOLD_FONT,                                // TimesBold,
      BOLD_FONT | ITALIC_FONT,                  // TimesBoldItalic,
      SANSSERIF_FONT,                           // AvantGardeBook,
      SANSSERIF_FONT | ITALIC_FONT,             // AvantGardeBookOblique,
      SANSSERIF_FONT,                           // AvantGardeDemi,
      SANSSERIF_FONT | ITALIC_FONT,             // AvantGardeDemiOblique,
      0,                                        // BookmanLight,
      ITALIC_FONT,                              // BookmanLightItalic,
      0,                                        // BookmanDemi,
      ITALIC_FONT,                              // BookmanDemiItalic,
      MONOSPACE_FONT,                           // Courier,
      MONOSPACE_FONT | ITALIC_FONT,             // CourierOblique,
      MONOSPACE_FONT | BOLD_FONT,               // CourierBold,
      MONOSPACE_FONT | BOLD_FONT | ITALIC_FONT, // CourierBoldOblique,
      SANSSERIF_FONT,                           // Helvetica,
      SANSSERIF_FONT | ITALIC_FONT,             // HelveticaOblique,
      SANSSERIF_FONT | BOLD_FONT,               // HelveticaBold,
      SANSSERIF_FONT | BOLD_FONT | ITALIC_FONT, // HelveticaBoldOblique,
      SANSSERIF_FONT,                           // HelveticaNarrow,
      SANSSERIF_FONT | ITALIC_FONT,             // HelveticaNarrowOblique,
      SANSSERIF_FONT | BOLD_FONT,               // HelveticaNarrowBold,
      SANSSERIF_FONT | BOLD_FONT | ITALIC_FONT, // HelveticaNarrowBoldOblique,
      0,                                        // NewCenturySchoolbookRoman,
      ITALIC_FONT,                              // NewCenturySchoolbookItalic,
      BOLD_FONT,                                // NewCenturySchoolbookBold,
      BOLD_FONT | ITALIC_FONT,                  // NewCenturySchoolbookBoldItalic,
      0,                                        // PalatinoRoman,
      ITALIC_FONT,                              // PalatinoItalic,
      BOLD_FONT,                                // PalatinoBold,
      BOLD_FONT | ITALIC_FONT,                  // PalatinoBoldItalic,
      0,                                        // Symbol,
      ITALIC_FONT,                              // ZapfChanceryMediumItalic,
      0                                         // ZapfDingbats
  };

  stream << "\\path[" << _style.tikzProperties(transform) << "] (" << transform.mapX(position().x) << ',' << transform.mapY(position().y) << ") node {"
         << (fontTraits[_font] & ITALIC_FONT ? "\\itshape " : "") << (fontTraits[_font] & BOLD_FONT ? "\\bfseries " : "") << (fontTraits[_font] & MONOSPACE_FONT ? "\\ttfamily " : "")
         << (fontTraits[_font] & SANSSERIF_FONT ? "\\sffamily " : "") << _text << "};" << std::endl;
}

void Text::accept(ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Text::accept(const ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Text::accept(ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

void Text::accept(const ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

Shape * Text::accept(CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Shape * Text::accept(const CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Rect Text::boundingBox(LineWidthFlag) const
{
  // TODO Get correct values!
  return _box.boundingBox();
}

} // namespace LibBoard
