/* -*- mode: c++ -*- */
/**
 * @file   Board.cpp
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

#include <Board.h>
#include <algorithm>
#include <board/PSFonts.h>
#include <board/Point.h>
#include <board/Rect.h>
#include <board/Shape.h>
#include <board/ShapeVisitor.h>
#include <board/Tools.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <typeinfo>

#if defined(max)
#undef max
#endif

namespace
{

const double pageSizes[][2] = {
    {0.0, 0.0},                // BoundingBox
    {841.0, 1189.0},           // A0
    {594.0, 841.0},            // A1
    {420.0, 594.0},            // A2
    {297.0, 420.0},            // A3
    {210.0, 297.0},            // A4
    {148.0, 210.0},            // A5
    {105.0, 148.0},            // A6
    {74.0, 105.0},             // A7
    {52.0, 74.0},              // A8
    {37.0, 52.0},              // A9
    {26.0, 37.0},              // A10
    {8.5 * 25.4, 11.0 * 25.4}, // Letter
    {8.5 * 25.4, 14.0 * 25.4}, // Legal
    {7.2 * 25.4, 10.5 * 25.4}  // Executive
};

const double ppmm = 72.0 / 25.4;

struct ShapeRectDepth {
  const LibBoard::Shape * shape;
  const LibBoard::Rect bbox;
  const unsigned int depth;
};

unsigned int findNextDepth(const LibBoard::Rect & bbox, const std::vector<ShapeRectDepth> & depths)
{
  // FIXME : Improve intersection detection

  unsigned int next = std::numeric_limits<unsigned int>::max();
  for (const ShapeRectDepth & srd : depths) {
    if (bbox.strictlyIntersects(srd.bbox) && srd.depth <= next) {
      next = srd.depth - 1;
    }
  }
  return next;
}

} // namespace

namespace LibBoard
{

const double Board::Degree = 3.14159265358979323846 / 180.0;

Board::State::State()
{
  style = Style{Color::Black, Color::Null, 0.5, SolidStyle, ButtCap, MiterJoin};
  font = Fonts::TimesRoman;
  fontSize = 11.0;
}

Board::Board(const Color & backgroundColor) : _backgroundColor(backgroundColor) {}

Board::Board(const Board & other) : ShapeList(other), _state(other._state), _backgroundColor(other._backgroundColor) {}

Board & Board::operator=(const Board & other)
{
  deleteShapes();
  if (!other._shapes.size()) {
    return (*this);
  }
  _shapes.resize(other._shapes.size(), nullptr);
  std::vector<Shape *>::iterator t = _shapes.begin();
  std::vector<Shape *>::const_iterator i = other._shapes.begin();
  std::vector<Shape *>::const_iterator end = other._shapes.end();
  while (i != end) {
    *t = (*i)->clone();
    ++i;
    ++t;
  }
  return *this;
}

Board & Board::operator<<(const Shape & shape)
{
  ShapeList::addShape(shape, 1.0);
  return *this;
}

Board::~Board() {}

void Board::enableLineWidthScaling()
{
  ShapeWithStyle::setLineWidthScaling(true);
}

void Board::disableLineWidthScaling()
{
  ShapeWithStyle::setLineWidthScaling(false);
}

void Board::setLineWidthScaling(bool on)
{
  ShapeWithStyle::setLineWidthScaling(on);
}

void Board::clear(const Color & color)
{
  ShapeList::clear();
  _backgroundColor = color;
}

Board & Board::rotate(double angle, const Point & center)
{
  ShapeList::rotate(angle, center);
  _clippingPath.rotate(angle, center);
  return (*this);
}

Board & Board::rotate(double angle)
{
  ShapeList::rotate(angle);
  _clippingPath.rotate(angle, center());
  return (*this);
}

Board & Board::translate(double dx, double dy)
{
  ShapeList::translate(dx, dy);
  _clippingPath.translate(dx, dy);
  return (*this);
}

Board & Board::scale(double sx, double sy)
{
  if (_clippingPath.size()) {
    Point delta = _clippingPath.center() - center();
    delta.x *= sx;
    delta.y *= sy;
    _clippingPath.scale(sx, sy);
    ShapeList::scale(sx, sy);
    delta = (center() + delta) - _clippingPath.center();
    _clippingPath.translate(delta.x, delta.y);
  } else {
    ShapeList::scale(sx, sy);
  }
  return (*this);
}

Board & Board::scale(double s)
{
  if (_clippingPath.size()) {
    Point delta = _clippingPath.center() - center();
    delta *= s;
    _clippingPath.scale(s);
    ShapeList::scale(s);
    delta = (center() + delta) - _clippingPath.center();
    _clippingPath.translate(delta.x, delta.y);
  } else {
    ShapeList::scale(s);
  }
  return (*this);
}

Board Board::rotated(double angle, const Point & center)
{
  return static_cast<const Board &>(Board(*this).rotate(angle, center));
}

Board Board::rotated(double angle)
{
  return static_cast<const Board &>(Board(*this).rotate(angle));
}

Board Board::translated(double dx, double dy)
{
  return static_cast<const Board &>(Board(*this).translate(dx, dy));
}

Board Board::scaled(double sx, double sy)
{
  return static_cast<const Board &>(Board(*this).scale(sx, sy));
}

Board Board::scaled(double s)
{
  return static_cast<const Board &>(Board(*this).scale(s));
}

Board & Board::setPenColorRGBi(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
  _state.style.penColor.setRGBi(red, green, blue, alpha);
  return *this;
}

Board & Board::setPenColorRGBf(float red, float green, float blue, float alpha)
{
  _state.style.penColor.setRGBf(red, green, blue, alpha);
  return *this;
}

Board & Board::setPenColor(const Color & color)
{
  _state.style.penColor = color;
  return *this;
}

Board & Board::setFillColorRGBi(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
  _state.style.fillColor.setRGBi(red, green, blue, alpha);
  return *this;
}

Board & Board::setFillColorRGBf(float red, float green, float blue, float alpha)
{
  _state.style.fillColor.setRGBf(red, green, blue, alpha);
  return *this;
}

Board & Board::setFillColorHSV(float hue, float saturation, float value, float alpha)
{
  _state.style.fillColor.setHSV(hue, saturation, value, alpha);
  return *this;
}

Board & Board::setFillColor(const Color & color)
{
  _state.style.fillColor = color;
  return *this;
}

Board & Board::setLineWidth(double width)
{
  _state.style.lineWidth = width;
  return *this;
}

Board & Board::setFont(const Fonts::Font font, double fontSize)
{
  _state.font = font;
  _state.fontSize = fontSize;
  return *this;
}

Board & Board::setFontSize(double fontSize)
{
  _state.fontSize = fontSize;
  return *this;
}

void Board::setBackgroundColor(const Color & color)
{
  _backgroundColor = color;
}

void Board::drawDot(double x, double y)
{
  _shapes.push_back(new Dot(x, y, _state.style.penColor, _state.style.lineWidth));
}

void Board::drawLine(double x1, double y1, double x2, double y2)
{
  _shapes.push_back(new Line(x1, y1, x2, y2, _state.style));
}

void Board::drawLine(Point p, Point q)
{
  _shapes.push_back(new Line(p, q, _state.style));
}

void Board::drawArrow(double x1, double y1, double x2, double y2, Arrow::ExtremityType type)
{
  _shapes.push_back(new Arrow(x1, y1, x2, y2, type, _state.style));
}

void Board::drawArrow(Point p, Point q, Arrow::ExtremityType type)
{
  _shapes.push_back(new Arrow(p, q, type, _state.style));
}

void Board::drawRectangle(double left, double top, double width, double height)
{
  _shapes.push_back(LibBoard::rectangle(left, top, width, height, _state.style).clone());
}

void Board::drawRectangle(const Rect & rect)
{
  _shapes.push_back(LibBoard::rectangle(rect.left, rect.top, rect.width, rect.height, _state.style).clone());
}

void Board::fillRectangle(double left, double top, double width, double height)
{
  Style style = _state.style;
  style.lineWidth = 0.0;
  style.fillColor = style.penColor;
  style.penColor = Color::Null;
  _shapes.push_back(LibBoard::rectangle(left, top, width, height, style).clone());
}

void Board::fillRectangle(const Rect & rect)
{
  fillRectangle(rect.left, rect.top, rect.width, rect.height);
}

void Board::drawCircle(double x, double y, double radius)
{
  _shapes.push_back(LibBoard::circle(x, y, radius, _state.style.penColor, _state.style.fillColor, _state.style.lineWidth, _state.style.lineStyle).clone());
}

void Board::fillCircle(double x, double y, double radius)
{
  _shapes.push_back(LibBoard::circle(x, y, radius, Color::Null, _state.style.penColor, 0.0, _state.style.lineStyle).clone());
}

void Board::drawEllipse(double x, double y, double xRadius, double yRadius)
{
  _shapes.push_back(new Ellipse(x, y, xRadius, yRadius, _state.style.penColor, _state.style.fillColor, _state.style.lineWidth, _state.style.lineStyle));
}

void Board::fillEllipse(double x, double y, double xRadius, double yRadius)
{
  _shapes.push_back(new Ellipse(x, y, xRadius, yRadius, Color::Null, _state.style.penColor, 0.0, _state.style.lineStyle));
}

void Board::drawPolyline(const std::vector<Point> & points)
{
  _shapes.push_back(new Polyline(points, Path::Open, _state.style));
}

void Board::drawClosedPolyline(const std::vector<Point> & points)
{
  _shapes.push_back(new Polyline(points, Path::Closed, _state.style));
}

void Board::fillPolyline(const std::vector<Point> & points)
{
  _shapes.push_back(new Polyline(points, Path::Closed, Color::Null, _state.style.penColor, 0.0, _state.style.lineStyle, _state.style.lineCap, _state.style.lineJoin));
}

void Board::drawTriangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
  std::vector<Point> points = {Point(x1, y1), Point(x2, y2), Point(x3, y3)};
  _shapes.push_back(new Polyline(points, Path::Closed, _state.style));
}

void Board::drawTriangle(const Point & p1, const Point & p2, const Point & p3)
{
  std::vector<Point> points = {p1, p2, p3};
  _shapes.push_back(new Polyline(points, Path::Closed, _state.style));
}

void Board::fillTriangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
  std::vector<Point> points = {Point(x1, y1), Point(x2, y2), Point(x3, y3)};
  _shapes.push_back(new Polyline(points, Path::Closed, Color::Null, _state.style.penColor, 0.0, _state.style.lineStyle, _state.style.lineCap, _state.style.lineJoin));
}

void Board::fillTriangle(const Point & p1, const Point & p2, const Point & p3)
{
  std::vector<Point> points = {p1, p2, p3};
  _shapes.push_back(new Polyline(points, Path::Closed, Color::Null, _state.style.penColor, 0.0, _state.style.lineStyle, _state.style.lineCap, _state.style.lineJoin));
}

void Board::fillGouraudTriangle(const Point & p1, const Color & color1, const Point & p2, const Color & color2, const Point & p3, const Color & color3, unsigned char divisions)
{
  _shapes.push_back(new GouraudTriangle(p1, color1, p2, color2, p3, color3, divisions));
}

void Board::fillGouraudTriangle(const Point & p1, const float brightness1, const Point & p2, const float brightness2, const Point & p3, const float brightness3, unsigned char divisions)
{
  Color color1(_state.style.penColor);
  Color color2(_state.style.penColor);
  Color color3(_state.style.penColor);
  color1.red(static_cast<unsigned char>(std::min(255.0f, color1.red() * brightness1)));
  color1.green(static_cast<unsigned char>(std::min(255.0f, color1.green() * brightness1)));
  color1.blue(static_cast<unsigned char>(std::min(255.0f, color1.blue() * brightness1)));
  color2.red(static_cast<unsigned char>(std::min(255.0f, color2.red() * brightness2)));
  color2.green(static_cast<unsigned char>(std::min(255.0f, color2.green() * brightness2)));
  color2.blue(static_cast<unsigned char>(std::min(255.0f, color2.blue() * brightness2)));
  color3.red(static_cast<unsigned char>(std::min(255.0f, color3.red() * brightness3)));
  color3.green(static_cast<unsigned char>(std::min(255.0f, color3.green() * brightness3)));
  color3.blue(static_cast<unsigned char>(std::min(255.0f, color3.blue() * brightness3)));
  fillGouraudTriangle(p1, color1, p2, color2, p3, color3, divisions);
}

void Board::drawText(double x, double y, const char * text)
{
  _shapes.push_back(new Text(x, y, text, _state.font, _state.fontSize, _state.style.penColor));
}

void Board::drawText(Point p, const char * text)
{
  _shapes.push_back(new Text(p, text, _state.font, _state.fontSize, _state.style.penColor));
}

void Board::drawText(double x, double y, const std::string & text)
{
  _shapes.push_back(new Text(x, y, text, _state.font, _state.fontSize, _state.style.penColor));
}

void Board::drawText(Point p, const std::string & str)
{
  _shapes.push_back(new Text(p, str, _state.font, _state.fontSize, _state.style.penColor));
}

void Board::drawBoundingBox(LineWidthFlag lineWidthFlag)
{
  Rect bbox = boundingBox(lineWidthFlag);
  Polyline rectangle = LibBoard::rectangle(bbox.left, bbox.top, bbox.width, bbox.height, _state.style);
  _shapes.push_back(rectangle.clone());
}

void Board::setClippingRectangle(double x, double y, double width, double height)
{
  _clippingPath.clear();
  _clippingPath << Point(x, y);
  _clippingPath << Point(x + width, y);
  _clippingPath << Point(x + width, y - height);
  _clippingPath << Point(x, y - height);
}

void Board::setClippingRectangle(const Rect & rect)
{
  setClippingRectangle(rect.left, rect.top, rect.width, rect.height);
}

void Board::setClippingPath(const std::vector<Point> & points)
{
  _clippingPath.clear();
  std::vector<Point>::const_iterator it = points.begin();
  std::vector<Point>::const_iterator end = points.end();
  while (it != end) {
    _clippingPath << *it;
    ++it;
  }
}

void Board::setClippingPath(const Path & path)
{
  _clippingPath = path;
  _clippingPath.close();
  if (_clippingPath.size() > 1) {
    if (_clippingPath[0] == _clippingPath[_clippingPath.size() - 1]) {
      _clippingPath.pop_back();
    }
  }
}

void Board::addDuplicates(const Shape & shape, std::size_t times, double dx, double dy, double scale)
{
  Shape * s = shape.clone();
  while (times--) {
    (*this) << (*s);
    if (scale != 1.0) {
      s->scale(scale);
    }
    s->translate(dx, dy);
  }
  delete s;
}

void Board::addDuplicates(const Shape & shape, std::size_t times, double dx, double dy, double scaleX, double scaleY, double angle)
{
  Shape * s = shape.clone();
  while (times--) {
    (*this) << (*s);
    if (scaleX != 1.0 || scaleY != 1.0) {
      s->scale(scaleX, scaleY);
    }
    if (dx != 0.0 || dy != 0.0) {
      s->translate(dx, dy);
    }
    if (angle != 0.0) {
      s->rotate(angle);
    }
  }
  delete s;
}

void Board::saveEPS(const char * filename, PageSize size, double margin, Unit unit, const std::string & title) const
{
  if (size == PageSize::BoundingBox) {
    if (title == std::string()) {
      saveEPS(filename, 0.0, 0.0, margin, unit, filename);
    } else {
      saveEPS(filename, 0.0, 0.0, margin, unit, title);
    }
  } else {
    if (title == std::string()) {
      saveEPS(filename, pageSizes[int(size)][0], pageSizes[int(size)][1], toMillimeter(margin, unit), Unit::Millimeter, filename);
    } else {
      saveEPS(filename, pageSizes[int(size)][0], pageSizes[int(size)][1], toMillimeter(margin, unit), Unit::Millimeter, title);
    }
  }
}

void Board::saveEPS(std::ostream & out, PageSize size, double margin, Unit unit, const std::string & title) const
{
  if (size == PageSize::BoundingBox) {
    saveEPS(out, 0.0, 0.0, margin, unit, title);
  } else {
    saveEPS(out, pageSizes[int(size)][0], pageSizes[int(size)][1], toMillimeter(margin, unit), Unit::Millimeter, title);
  }
}

void Board::saveEPS(std::ostream & out, double pageWidth, double pageHeight, double margin, Unit unit, const std::string & title) const
{
  out << "%!PS-Adobe-2.0 EPSF-2.0" << std::endl;
  out << "%%Title: " << title << std::endl;
  out << "%%Creator: Board library (v" << BOARD_VERSION_STRING << ") Copyleft 2007 Sebastien Fourey" << std::endl;
  {
    time_t t = time(nullptr);
    char str_time[255];
    Tools::secured_ctime(str_time, &t, 255);
    out << "%%CreationDate: " << str_time;
  }

  Rect bbox = boundingBox(UseLineWidth);
  bool clipping = _clippingPath.size() > 2;
  if (clipping) {
    bbox = bbox && _clippingPath.boundingBox();
  }
  TransformEPS transform;
  if (pageWidth == 0.0 && pageHeight == 0.0) { // Fit to bounding box using given unit.
    transform.setBoundingBox(bbox, toMillimeter(bbox.width, unit), toMillimeter(bbox.height, unit), -toMillimeter(margin, unit));

    Rect page = transform.pageBoundingBox();
    out << "%%BoundingBox: " << std::setprecision(8) << page.left << " " << page.bottom() << " " << page.right() << " " << page.top << std::endl;

  } else {
    transform.setBoundingBox(bbox, toMillimeter(pageWidth, unit), toMillimeter(pageHeight, unit), toMillimeter(margin, unit));
    Rect page = transform.pageBoundingBox();
    out << "%%BoundingBox: " << std::setprecision(8) << page.left << " " << page.bottom() << " " << page.right() << " " << page.top << std::endl;
  }

  out << "%Magnification: 1.0000" << std::endl;
  out << "%%EndComments" << std::endl;

  out << "\n"
         "/cp {closepath} bind def\n"
         "/ef {eofill} bind def\n"
         "/gr {grestore} bind def\n"
         "/gs {gsave} bind def\n"
         "/sa {save} bind def\n"
         "/rs {restore} bind def\n"
         "/l {lineto} bind def\n"
         "/ct {curveto} bind def\n"
         "/m {moveto} bind def\n"
         "/rm {rmoveto} bind def\n"
         "/n {newpath} bind def\n"
         "/s {stroke} bind def\n"
         "/sh {show} bind def\n"
         "/slc {setlinecap} bind def\n"
         "/slj {setlinejoin} bind def\n"
         "/slw {setlinewidth} bind def\n"
         "/srgb {setrgbcolor} bind def\n"
         "/rot {rotate} bind def\n"
         "/sc {scale} bind def\n"
         "/sd {setdash} bind def\n"
         "/ff {findfont} bind def\n"
         "/sf {setfont} bind def\n"
         "/scf {scalefont} bind def\n"
         "/sw {stringwidth} bind def\n"
         "/sd {setdash} bind def\n"
         "/tr {translate} bind def\n"
         " 0.5 setlinewidth\n";

  if (clipping) {
    out << " newpath ";
    _clippingPath.flushPostscript(out, transform);
    out << " 0 slw clip " << std::endl;
  }

  // Draw the background color if needed.
  if (_backgroundColor != Color::Null) {
    LibBoard::rectangle(bbox, Color::Null, _backgroundColor, 0.0).flushPostscript(out, transform);
  }

  for (const Shape * shape : _shapes) {
    shape->flushPostscript(out, transform);
  }
  out << "showpage" << std::endl;
  out << "%%Trailer" << std::endl;
  out << "%EOF" << std::endl;
}

void Board::saveEPS(const char * filename, double pageWidth, double pageHeight, double margin, Unit unit, const std::string & title) const
{
  std::ofstream out(filename);
  saveEPS(out, pageWidth, pageHeight, margin, unit, title);
  out.close();
}

void Board::saveFIG(const char * filename, PageSize size, double margin, Unit unit) const
{
  if (size == PageSize::BoundingBox) {
    saveFIG(filename, 0.0, 0.0, margin, unit);
  } else {
    saveFIG(filename, pageSizes[int(size)][0], pageSizes[int(size)][1], toMillimeter(margin, unit), Unit::Millimeter);
  }
}

void Board::saveFIG(std::ostream & out, PageSize size, double margin, Unit unit) const
{
  if (size == PageSize::BoundingBox) {
    saveFIG(out, 0.0, 0.0, margin, unit);
  } else {
    saveFIG(out, pageSizes[int(size)][0], pageSizes[int(size)][1], toMillimeter(margin, unit), Unit::Millimeter);
  }
}

void Board::saveFIG(std::ostream & out, double pageWidth, double pageHeight, double margin, Unit unit) const
{
  Rect bbox = boundingBox(UseLineWidth);
  TransformFIG transform;

  if (pageWidth == 0.0 && pageHeight == 0.0) {
    transform.setBoundingBox(bbox, toMillimeter(bbox.width, unit), toMillimeter(bbox.height, unit), -toMillimeter(margin, unit));
  } else {
    transform.setBoundingBox(bbox, toMillimeter(pageWidth, unit), toMillimeter(pageHeight, unit), toMillimeter(margin, unit));
  }

  out << "#FIG 3.2  Produced by the Board library (v" BOARD_VERSION_STRING ") Copyleft 2007 Sebastien Fourey\n";
  out << "Portrait\n"
         "Center\n"
         "Metric\n"
         "A4\n"
         "100.00\n"
         "Single\n"
         "-2\n"
         "1200 2\n";

  std::map<Color, int> colormap;
  int maxColor = 32;

  colormap[Color(0, 0, 0)] = 0;
  colormap[Color(0, 0, 255)] = 1;
  colormap[Color(0, 255, 0)] = 2;
  colormap[Color(0, 255, 255)] = 0;
  colormap[Color(255, 0, 0)] = 4;
  colormap[Color(255, 0, 255)] = 0;
  colormap[Color(255, 255, 0)] = 6;
  colormap[Color(255, 255, 255)] = 7;

  PenColorExtractor penColorExtractor;
  accept(penColorExtractor);
  FillColorExtractor fillColorExtractor;
  accept(fillColorExtractor);

  // TODO : Color quantization

  for (const Color & color : penColorExtractor.colors()) {
    if (colormap.find(color) == colormap.end() && color.valid()) {
      colormap[color] = maxColor++;
    }
  }
  for (const Color & color : fillColorExtractor.colors()) {
    if (colormap.find(color) == colormap.end() && color.valid()) {
      colormap[color] = maxColor++;
    }
  }

  if (colormap.find(_backgroundColor) == colormap.end() && _backgroundColor.valid()) {
    colormap[_backgroundColor] = maxColor++;
  }

  // Write the colormap
  std::map<Color, int>::const_iterator iColormap = colormap.begin();
  std::map<Color, int>::const_iterator endColormap = colormap.end();
  char colorString[255];
  while (iColormap != endColormap) {
    secured_sprintf(colorString, 255, "0 %d #%02x%02x%02x\n", iColormap->second, iColormap->first.red(), iColormap->first.green(), iColormap->first.blue());
    if (iColormap->second >= 32) {
      out << colorString;
    }
    ++iColormap;
  }

  // Draw the background color if needed.
  if (_backgroundColor != Color::Null) {
    Polyline r = LibBoard::rectangle(bbox, Color::Null, _backgroundColor, 0.0);
    r.flushFIG(out, transform, colormap);
  }

  // Draw the shapes.
  // FIXME : Handle depth

  std::vector<ShapeRectDepth> depths;
  unsigned int minDepth = std::numeric_limits<unsigned int>::max();
  ConstLeafVisitor::Function countShapes = [&depths, &minDepth](const Shape & shape) { //
    const Rect bbox = shape.bbox(UseLineWidth);
    const unsigned int depth = findNextDepth(bbox, depths);
    const ShapeRectDepth srd{&shape, bbox, depth};
    depths.emplace_back(srd);
    if (depth < minDepth) {
      minDepth = depth;
    }
  };
  ConstLeafVisitor visitor(countShapes);
  accept(visitor);

  std::map<const Shape *, unsigned int> depthMap;
  for (const auto & srd : depths) {
    depthMap[srd.shape] = srd.depth;
  }
  depths.clear();
  transform.setDepthMap(&depthMap, minDepth);

  // std::cout << depthMap.size() << " leaves\n";

  for (const Shape * shape : _shapes) {
    shape->flushFIG(out, transform, colormap);
  }
}

void Board::saveFIG(const char * filename, double pageWidth, double pageHeight, double margin, Unit unit) const
{
  std::ofstream out(filename);
  saveFIG(out, pageWidth, pageHeight, margin, unit);
  out.close();
}

void Board::saveSVG(const char * filename, PageSize size, double margin, Unit unit) const
{
  if (size == PageSize::BoundingBox) {
    saveSVG(filename, 0.0, 0.0, margin, unit);
  } else {
    saveSVG(filename, pageSizes[int(size)][0], pageSizes[int(size)][1], toMillimeter(margin, unit), Unit::Millimeter);
  }
}

void Board::saveSVG(std::ostream & out, PageSize size, double margin, Unit unit) const
{
  if (size == PageSize::BoundingBox) {
    saveSVG(out, 0.0, 0.0, margin, unit);
  } else {
    saveSVG(out, pageSizes[int(size)][0], pageSizes[int(size)][1], toMillimeter(margin, unit), Unit::Millimeter);
  }
}

void Board::saveSVG(const char * filename, double pageWidth, double pageHeight, double margin, Unit unit) const
{
  std::ofstream out(filename);
  saveSVG(out, pageWidth, pageHeight, margin, unit);
  out.close();
}

void Board::saveSVG(std::ostream & out, double pageWidth, double pageHeight, double margin, Unit unit) const
{
  Rect bbox = boundingBox(UseLineWidth);
  TransformSVG transform;
  bool clipping = _clippingPath.size() > 2;
  if (clipping) {
    bbox = bbox && _clippingPath.boundingBox();
  }

  out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?>" << std::endl;
  out << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"" << std::endl;
  out << " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;

  if (pageWidth == 0.0 && pageHeight == 0.0) {
    transform.setBoundingBox(bbox, toMillimeter(bbox.width, unit), toMillimeter(bbox.height, unit), -toMillimeter(margin, unit));
    out << "<svg width=\"" << toMillimeter(bbox.width + 2 * margin, unit) << "mm"
        << "\" height=\"" << toMillimeter(bbox.height + 2 * margin, unit) << "mm"
        << "\" " << std::endl;
    out << "     viewBox=\"0 0 " << toMillimeter(bbox.width + 2 * margin, unit) * ppmm << " " << toMillimeter(bbox.height + 2 * margin, unit) * ppmm << "\" " << std::endl;
    out << "     xmlns=\"http://www.w3.org/2000/svg\""
        << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
        << " version=\"1.1\" >" << std::endl;
  } else {
    transform.setBoundingBox(bbox, toMillimeter(pageWidth, unit), toMillimeter(pageHeight, unit), toMillimeter(margin, unit));

    out << "<svg width=\"" << toMillimeter(pageWidth, unit) << "mm\" height=\"" << toMillimeter(pageHeight, unit) << "mm\" " << std::endl;
    out << "     viewBox=\"0 0 " << toMillimeter(pageWidth, unit) * ppmm << " " << toMillimeter(pageHeight, unit) * ppmm << "\" " << std::endl;
    out << "     xmlns=\"http://www.w3.org/2000/svg\""
        << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
        << " version=\"1.1\" >" << std::endl;
  }

  out << "<desc>"
         "Drawing created with the Board library (v"
      << BOARD_VERSION_STRING
      << ") Copyright Sebastien Fourey"
         "</desc>"
      << std::endl;

  if (clipping) {
    out << "<g clip-rule=\"nonzero\">\n"
           " <clipPath id=\"GlobalClipPath\">\n"
           "  <path clip-rule=\"evenodd\"  d=\"";
    _clippingPath.flushSVGCommands(out, transform);
    out << "\" />\n"
           " </clipPath>\n"
           "<g clip-path=\"url(#GlobalClipPath)\">\n";
  }

  // Draw the background color if needed.
  if (_backgroundColor != Color::Null) {
    Polyline r = LibBoard::rectangle(bbox, Color::Null, _backgroundColor, 0.0);
    r.flushSVG(out, transform);
  }

  // Draw the shapes.
  for (const Shape * shape : _shapes) {
    shape->flushSVG(out, transform);
  }

  if (clipping) {
    out << "</g>\n</g>";
  }
  out << "</svg>" << std::endl;
}

void Board::saveTikZ(const char * filename, PageSize size, double margin) const
{
  saveTikZ(filename, pageSizes[int(size)][0], pageSizes[int(size)][1], margin);
}

void Board::saveTikZ(std::ostream & out, PageSize size, double margin) const
{
  saveTikZ(out, pageSizes[int(size)][0], pageSizes[int(size)][1], margin);
}

void Board::saveTikZ(std::ostream & out, double pageWidth, double pageHeight, double margin) const
{
  TransformTikZ transform;
  Rect box = boundingBox(UseLineWidth);
  bool clipping = _clippingPath.size() > 2;
  if (clipping)
    box = box && _clippingPath.boundingBox();
  transform.setBoundingBox(box, pageWidth, pageHeight, margin);

  out << "\\begin{tikzpicture}[anchor=south west,text depth=0,x={(1pt,0pt)},y={(0pt,-1pt)}]" << std::endl;

  if (clipping) {
    out << "\\clip ";
    _clippingPath.flushTikZPoints(out, transform);
    out << "\n";
  }

  // Draw the background color if needed.
  if (_backgroundColor != Color::Null) {
    LibBoard::rectangle(box, Color::Null, _backgroundColor, 0.0).flushTikZ(out, transform);
  }

  // Draw the shapes.
  for (const Shape * shape : _shapes) {
    shape->flushTikZ(out, transform);
  }

  out << "\\end{tikzpicture}" << std::endl;
}

Rect Board::pageRect(PageSize size, Unit unit)
{
  const double width = pageSizes[int(size)][0];
  const double height = pageSizes[int(size)][1];
  switch (unit) {
  case Unit::Point:
    return {0, 0, (width / 25.4) * 72.0, (height / 25.4) * 72.0};
  case Unit::Millimeter:
    return {0, 0, width, height};
  case Unit::Centimeter:
    return {0, 0, width / 10.0, height / 10.0};
  case Unit::Inch:
    return {0, 0, width / 25.4, height / 25.4};
  }
  Tools::error << "Board::pageRect(): bad unit (" << int(unit) << ")\n";
  return Rect{0, 0, 0, 0};
}

double Board::toMillimeter(double x, Unit unit)
{
  // enum class Unit { Point, Inch, Centimeter, Millimeter };
  switch (unit) {
  case Unit::Point:
    return x * 25.4 / 72.0;
  case Unit::Inch:
    return x * 25.4;
  case Unit::Centimeter:
    return x * 10.0;
  case Unit::Millimeter:
    return x;
  }
  Tools::error << "toMillimeter(): bad unit (" << int(unit) << ")\n";
  return 0;
}

void Board::saveTikZ(const char * filename, double pageWidth, double pageHeight, double margin) const
{
  std::ofstream out(filename);
  saveTikZ(out, pageWidth, pageHeight, margin);
  out.close();
}

void Board::save(const char * filename, double pageWidth, double pageHeight, double margin, Unit unit) const
{
  if (Tools::stringEndsWith(filename, ".eps", Tools::CaseInsensitive)) {
    saveEPS(filename, pageWidth, pageHeight, margin, unit);
    return;
  }
  if (Tools::stringEndsWith(filename, ".fig", Tools::CaseInsensitive)) {
    saveFIG(filename, pageWidth, pageHeight, margin, unit);
    return;
  }
  if (Tools::stringEndsWith(filename, ".svg", Tools::CaseInsensitive)) {
    saveSVG(filename, pageWidth, pageHeight, margin, unit);
    return;
  }
  if (Tools::stringEndsWith(filename, ".tikz", Tools::CaseInsensitive)) {
    saveTikZ(filename, pageWidth, pageHeight, margin);
    return;
  }
}

void Board::save(const char * filename, PageSize size, double margin, Unit unit) const
{
  if (size == PageSize::BoundingBox) {
    save(filename, 0.0, 0.0, margin, unit);
  } else {
    save(filename, pageSizes[int(size)][0], pageSizes[int(size)][1], toMillimeter(margin, unit), Unit::Millimeter);
  }
}

Group grid(Point topLeft, size_t columns, size_t rows, //
           double width, double height,                //
           Color penColor, Color fillColor, double lineWidth, const LineStyle lineStyle, const LineCap cap, const LineJoin join)
{
  Group group;
  double cellSide = width / double(columns);
  group << rectangle(topLeft.x, topLeft.y, width, height, penColor, fillColor, lineWidth, lineStyle, cap, join);
  Line vLine(topLeft.x + cellSide, topLeft.y, topLeft.x + cellSide, topLeft.y - height, penColor, lineWidth, lineStyle, RoundCap, join);
  while (--columns) {
    group << vLine;
    vLine.translate(cellSide, 0);
  }
  cellSide = height / double(rows);
  Line hLine(topLeft.x, topLeft.y - cellSide, topLeft.x + width, topLeft.y - cellSide, penColor, lineWidth, lineStyle, RoundCap, join);
  while (--rows) {
    group << hLine;
    hLine.translate(0, -cellSide);
  }
  return group;
}

Group grid(Point topLeft, size_t columns, size_t rows, //
           double width, double height,                //
           const Style & style)
{
  return grid(topLeft, columns, rows, width, height, style.penColor, style.fillColor, style.lineWidth, style.lineStyle, style.lineCap, style.lineJoin);
}

Group tiling(const Shape & shape, Point topLeftCorner, int columns, int rows, double spacing, LineWidthFlag lineWidthFlag)
{
  Group group;
  if (columns && rows) {
    Rect box = shape.boundingBox(lineWidthFlag);
    Shape * topLeft = shape.clone();
    topLeft->translate(topLeftCorner.x - box.left, topLeftCorner.y - box.top);
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < columns; ++c) {
        Shape * shape = topLeft->clone();
        shape->translate(c * (box.width + spacing), -r * (box.height + spacing));
        group << (*shape);
        delete shape;
      }
    }
    delete topLeft;
  }
  return group;
}

Group circled(const Shape & shape, double margin, Color penColor, Color fillColor, double lineWidth, LineStyle lineStyle)
{
  Group g;
  Rect r = shape.boundingBox(UseLineWidth);
  g << circle(r.center(), r.diameter() * 0.5 + lineWidth + margin, penColor, fillColor, lineWidth, lineStyle);
  g << shape;
  return g;
}

Group cross(Point p, const Style & style)
{
  Group cross;
  double half = std::max(style.lineWidth * 2.5, 0.1);
  cross << Line(p.translated(-half, 0), p.translated(half, 0), style) << Line(p.translated(0, -half), p.translated(0, half), style);
  return cross;
}

Polyline bezierControls(const Bezier & bezier, const Style & style)
{
  std::vector<Point> path;
  std::vector<Point>::const_iterator iPoint = bezier.path().points().begin();
  std::vector<Point>::const_iterator endPoint = bezier.path().points().end();
  std::vector<Point>::const_iterator iControl = bezier.controls().begin();
  Point a = *iPoint;
  ++iPoint;
  while (iPoint != endPoint) {
    Point b = *iPoint;
    path.push_back(a);
    path.push_back(iControl[0]);
    path.push_back(iControl[1]);
    ++iPoint;
    iControl += 2;
    a = b;
  }
  path.push_back(bezier.path().back());
  return Polyline(path, Path::Open, style);
}

Group array(Point topLeft, const std::vector<Color> & colors, //
            unsigned int columns, unsigned int rows,          //
            double pixelWidth, double pixelHeight, double lineWidth)
{
  if (colors.size() < rows * columns) {
    throw Exception("array(): not enough colors cells for requested array size");
  }
  if (pixelHeight == 0.0) {
    pixelHeight = pixelWidth;
  }
  Group result;
  Style style(Color::Null, Color::Null, lineWidth, SolidStyle, SquareCap, MiterJoin);
  const bool WithLine = (lineWidth != 0.0);
  for (unsigned int i = 0; i < rows; i++) {
    const double y = topLeft.y - i * pixelHeight;
    const unsigned int skip_lines = i * columns;
    for (unsigned int j = 0; j < columns; j++) {
      const double x = topLeft.x + j * pixelWidth;
      style.fillColor = colors[j + skip_lines];
      if (WithLine) {
        style.penColor = style.fillColor;
      }
      result << rectangle(x, y, pixelWidth, pixelHeight, style);
    }
  }
  return result;
}

Group framed(const Shape & shape, double margin, //
             const Color & penColor,             //
             const Color & fillColor,            //
             double lineWidth,                   //
             LineStyle lineStyle,                //
             int sketchyCount)
{
  Group result;
  Rect bbox = shape.bbox(UseLineWidth);
  if (margin != 0.0) {
    bbox.grow(margin);
  }
  Polyline r = rectangle(bbox, penColor, fillColor, lineWidth, lineStyle);
  if (sketchyCount > 0) {
    result << makeRough(r, sketchyCount, NoFilling);
  } else {
    result << r;
  }
  result << shape;
  return result;
}

} // namespace LibBoard

/**
 * @example examples/arithmetic.cpp
 * @example examples/array.cpp
 * @example examples/arrows.cpp
 * @example examples/bezier.cpp
 * @example examples/board_font_text.cpp
 * @example examples/circled_framed.cpp
 * @example examples/clipping.cpp
 * @example examples/ellipse.cpp
 * @example examples/example1.cpp
 * @example examples/example2.cpp
 * @example examples/example3.cpp
 * @example examples/example4.cpp
 * @example examples/flag.cpp
 * @example examples/graph.cpp
 * @example examples/holes.cpp
 * @example examples/Huffman.cpp
 * @example examples/hull.cpp
 * @example examples/images.cpp
 * @example examples/interpolate.cpp
 * @example examples/Julia.cpp
 * @example examples/koch.cpp
 * @example examples/line_segment.cpp
 * @example examples/line_style.cpp
 * @example examples/logo.cpp
 * @example examples/rough.cpp
 * @example examples/ruler.cpp
 * @example examples/sandbox.cpp
 * @example examples/scale_ellipse.cpp
 * @example examples/sierpinski.cpp
 * @example examples/stroke_path.cpp
 * @example examples/test_arrow.cpp
 * @example examples/test_depth.cpp
 * @example examples/tilings.cpp
 * @example examples/traversal.cpp
 * @example examples/triangles.cpp
 * @example examples/xkcd.cpp
 */

/**
 * @mainpage LibBoard - A C++ library for simple Postscript, SVG, and XFig drawings
 *
 * <table border="0"><tr><td><img align=left src="https://foureys.users.greyc.fr/board/doc/LibBoardLogoII_Small.png"/></td>
 * <td>(Copyleft) 2007 S&eacute;bastien Fourey - GREYC ENSICAEN</td></tr></table>
 *
 * @section intro_sec Introduction
 *
 * The board library allows simple drawings in:
 * <ul>
 *  <li>Encapsulated Postcript files (EPS) ;
 *  <li>XFig files (FIG) ;
 *  <li>Scalable Vector Graphics files (SVG).
 * </ul>
 *
 * The main class of the library is the #LibBoard#Board class. It is intented to
 * be as simple as possible so that it can be used quickly in programs to
 * generate the kind of figure one would rather not draw by hand, but which can
 * be easily drawn by a computer (C++) program.
 *
 * @section examples_sec Code examples
 *
 * <ul>
 * <li>See the "Examples" tab above or visit
 *     <a href="https://github.com/c-koi/libboard/blob/v0.9.6/EXAMPLES.md">dedicated page on GitHub</a>.
 * </ul>
 *
 * @section links_sec Links
 *
 * <ul>
 * <li>Visit the <a href="https://github.com/c-koi/libboard">official source code repository</a>.</li>
 * </ul>
 */
