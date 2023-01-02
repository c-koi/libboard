/* -*- mode: c++ -*- */
/**
 * @file   SketchFilter.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   June 2016
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
#include <board/SketchFilter.h>
#include <algorithm>
#include <deque>
#include <set>
#include <vector>
#include <board/Ellipse.h>
#include <board/Line.h>
#include <board/Path.h>
#include <board/Point.h>
#include <board/RoughVisitor.h>
#include <board/Tools.h>

namespace
{
using LibBoard::Point;
struct Edge {
  Edge(const Point & a, const Point & b) //
  {
    if (a.y < b.y) {
      _a = a;
      _b = b;
      _slopeInverse = LibBoard::Tools::almostEqual(a.y, b.y) ? std::numeric_limits<double>::infinity() : ((b.x - a.x) / (b.y - a.y));
    } else {
      _a = b;
      _b = a;
      _slopeInverse = LibBoard::Tools::almostEqual(a.y, b.y) ? std::numeric_limits<double>::infinity() : ((a.x - b.x) / (a.y - b.y));
    }
    _yMax = _b.y;
    _xScanline = _a.x;
    _xMin = std::min(a.x, b.x);
    _yMin = std::min(a.y, b.y);
  }

  Edge(const Edge & other)                                          //
      : _a(other._a), _b(other._b),                                 //
        _xMin(other._xMin), _yMin(other._yMin), _yMax(other._yMax), //
        _xScanline(other._xScanline), _slopeInverse(other._slopeInverse)
  {
  }
  //  Edge(Edge && other)                                               //
  //      : _a(other._a), _b(other._b),                                 //
  //        _xMin(other._xMin), _yMin(other._yMin), _yMax(other._yMax), //
  //        _xScanline(other._xScanline), _slopeInverse(other._slopeInverse)
  //  {
  // }
  //  Edge & operator=(Edge && other)
  //  {
  //    _a = other._a;
  //    _b = other._b;
  //    _xMin = other._xMin;
  //    _yMin = other._yMin;
  //    _yMax = other._yMax;
  //    _xScanline = other._xScanline;
  //    _slopeInverse = other._slopeInverse;
  //    return *this;
  //  }
  Edge & operator=(const Edge & other)
  {
    _a = other._a;
    _b = other._b;
    _xMin = other._xMin;
    _yMin = other._yMin;
    _yMax = other._yMax;
    _xScanline = other._xScanline;
    _slopeInverse = other._slopeInverse;
    return *this;
  }

  bool horizontal() const { return _slopeInverse == std::numeric_limits<double>::infinity(); }
  const Point & a() const { return _a; }
  const Point & b() const { return _b; }
  double xMin() const { return _xMin; }
  double yMin() const { return _yMin; }
  double yMax() const { return _yMax; }
  double & xScanline() { return _xScanline; }
  double xScanline() const { return _xScanline; }
  void updateXfromY(double y) { _xScanline = _a.x + (y - _yMin) * _slopeInverse; }

private:
  Point _a;
  Point _b;
  double _xMin;
  double _yMin;
  double _yMax;
  double _xScanline;
  double _slopeInverse;
}; // namespace

// std::ostream & operator<<(std::ostream & out, const Edge & e)
//{
//  using ::operator<<;
//  return out << "E(" << e.a() << ", " << e.b() << " x:" << e.xScanline() << " ymax:" << e.yMax() << ")";
//}

// std::ostream & operator<<(std::ostream & out, const std::deque<Edge> & edges)
//{
//  using ::operator<<;
//  out << "[";
//  bool first = true;
//  for (const auto & e : edges) {
//    if (first) {
//      first = false;
//    } else {
//      out << ";";
//    }
//    out << e;
//  }
//  out << "]";
//  return out;
//}

bool edgeLesserX(const Edge & a, const Edge & b)
{
  return a.xScanline() < b.xScanline();
}

bool edgeLesserYmin(const Edge & a, const Edge & b)
{                                 //
  return (a.yMin() < b.yMin()) || //
         (LibBoard::Tools::almostEqual(a.yMin(), b.yMin()) && a.xMin() < b.xMin());
}

inline void updateXfromYAndSort(std::deque<Edge> & edges, double y)
{
  for (Edge & e : edges) {
    e.updateXfromY(y);
  }
  std::sort(edges.begin(), edges.end(), edgeLesserX);
}

inline void rotate(std::vector<std::tuple<Point, Point>> & result, double angle, Point center)
{
  for (auto & t : result) {
    std::get<0>(t).rotate(angle, center);
    std::get<1>(t).rotate(angle, center);
  }
}

} // namespace

namespace LibBoard
{

// TODO : Better automatic smoot Bezier (Calman-*)

ShapeList makeRough(const Shape & shape, int repeat, SketchFilling filling, double hachureAngle, double hachureSpacing)
{
  ShapeList result;
  RoughVisitor visitor;
  visitor.setFilling(filling);
  visitor.setHachureAngle(hachureAngle);
  visitor.setRepeat(repeat);
  visitor.setHachureSpacing(hachureSpacing);
  result.push_back(shape.accept(visitor));
  return result;
}

std::vector<std::tuple<Point, Point>> hachures(const Path & path, double spacing, double angle, bool addHorizontals)
{
  std::vector<std::tuple<Point, Point>> result;
  std::deque<Edge> edges; // EdgeLesserYmin

  Path rotatedPath = (angle == 0.0) ? path : path.rotated(-angle);
  Point center = path.center();

  Path::size_type n = path.size();
  for (Path::size_type i = 0; i < n; ++i) {
    Edge e(rotatedPath[i], rotatedPath[(i + 1) % n]);
    if (!e.horizontal()) {
      edges.emplace_back(e);
    } else if (addHorizontals) {
      result.push_back(std::make_tuple(e.a(), e.b()));
    }
  }
  std::sort(edges.begin(), edges.end(), edgeLesserYmin);

  double y = edges.begin()->yMin() + spacing;
  std::deque<Edge> activeEdges; // EdgeLesserX
  while (!activeEdges.empty() || !edges.empty()) {

    //    std::cout << "y=" << y << " E(" << edges.size() << ") "
    //              << "AE1(" << activeEdges.size() << ") = " << activeEdges << std::endl;

    //    (a) Move from ET bucket y to the AET edges whose ymin <= y.
    while (!edges.empty() && edges.front().yMin() <= y) {
      if (!edges.front().horizontal()) {
        activeEdges.emplace_back(edges.front());
      }
      edges.pop_front();
    }

    // std::cout << "AE2(" << activeEdges.size() << ") = " << activeEdges << std::endl;

    //   (b) Remove from AET entries where y = ymax, then sort the AET on x.
    std::deque<Edge> newActiveEdges;
    for (const Edge & e : activeEdges) {
      if (y < e.yMax()) {
        newActiveEdges.emplace_back(e);
      }
    }
    activeEdges = newActiveEdges;
    updateXfromYAndSort(activeEdges, y);

    // std::cout << "AE3(" << activeEdges.size() << ") = " << activeEdges << std::endl;

    //   (c) Fill in pixels on scanline y by using pairs of x coordinates from the AET.

    if (!(activeEdges.size() % 2)) {
      auto ite = activeEdges.begin();
      while (ite != activeEdges.end()) {
        Point a(ite->xScanline(), y);
        ++ite;
        Point b(ite->xScanline(), y);
        ++ite;
        // std::cout << "Draw " << Edge(a, b) << std::endl;
        result.push_back(std::make_tuple(a, b));
      }
    }

    //   (d) Increment y by appropriate value defined by the hachure density, i.e. the next scanline.
    y += spacing;

    //   (e) For each non-vertical edge remaining in the AET, update x for the new y
    //   (edge.x = edge.x + edge.iSlope)

    updateXfromYAndSort(activeEdges, y);

    // std::cout << "AE4(" << activeEdges.size() << ") = " << activeEdges << std::endl;
  }
  if (angle != 0.0) {
    rotate(result, angle, center);
  }
  return result;
}

ShapeList hachuresLinesOrBezier(const std::vector<std::tuple<Point, Point>> & lines, Style style, SketchFilling type)
{
  ShapeList list;
  style.fillColor = Color::Null;
  for (const auto & t : lines) {
    if (type == SketchyHachure) {
      list << LibBoard::makeRough(Line(std::get<0>(t), std::get<1>(t), style), 1);
    } else {
      list << Line(std::get<0>(t), std::get<1>(t), style);
    }
  }
  return list;
}

ShapeList hachures(const Path & path, Style style, SketchFilling type, double spacing, double angle, bool addHorizontals)
{
  std::vector<std::tuple<Point, Point>> lines; // TODO : Refactor this (DRY)
  if (type == CrossingHachure || type == SketchyCrossingHachure) {
    lines = hachures(path, spacing, angle, addHorizontals);
    for (const auto & p : hachures(path, spacing, angle + M_PI_2)) {
      lines.emplace_back(p);
    }
    if (type == CrossingHachure) {
      type = StraightHachure;
    } else {
      type = SketchyHachure;
    }
  } else {
    lines = hachures(path, spacing, angle, addHorizontals);
  }
  return hachuresLinesOrBezier(lines, style, type);
}

ShapeList hachures(const Path & path, SketchFilling type, Color color, double width, double spacing, double angle)
{
  Style style;
  style.lineStyle = SolidStyle;
  style.lineCap = RoundCap;
  style.lineJoin = RoundJoin;
  style.fillColor = Color::Null;
  style.penColor = color;
  style.lineWidth = width;
  return hachures(path, style, type, spacing, angle);
}

std::vector<std::tuple<Point, Point>> hachures(const Ellipse & ellipse, double spacing, double angle)
{
  Ellipse e = ellipse;
  e.moveCenter(0, 0);
  if (angle != 0.0) {
    e.rotate(-angle);
  }
  Point t = ellipse.center(IgnoreLineWidth);

  Rect box = e.boundingBox(IgnoreLineWidth);
  double yMax = box.top;
  double yMin = box.bottom();
  const double alpha = e.angle();
  const double a = Tools::square(std::cos(alpha) / e.xRadius()) + Tools::square(std::sin(alpha) / e.yRadius());
  const double bYFactor = (std::sin(2 * alpha) / Tools::square(e.xRadius())) - (std::sin(2 * alpha) / Tools::square(e.yRadius()));
  const double cYFactor = Tools::square(std::sin(alpha) / e.xRadius()) + Tools::square(std::cos(alpha) / e.yRadius());

  std::vector<std::tuple<Point, Point>> lines;
  double x1, x2;
  for (double y = yMin + spacing; y < yMax; y += spacing) { // FIXME : Start at yMin + spacing !
    double b = y * bYFactor;
    double c = -1.0 + Tools::square(y) * cYFactor;
    if (Tools::solveQuadratic(a, b, c, x1, x2)) {
      lines.push_back(std::make_tuple(Point(x1, y) + t, Point(x2, y) + t));
    }
  }

  if (angle != 0.0) {
    rotate(lines, angle, t);
  }
  return lines;
}

ShapeList hachures(const Ellipse & ellipse, Style style, SketchFilling type, double spacing, double angle)
{
  std::vector<std::tuple<Point, Point>> lines;
  if (type == CrossingHachure || type == SketchyCrossingHachure) {
    lines = hachures(ellipse, spacing, angle);
    const std::vector<std::tuple<Point, Point>> ortogonals = hachures(ellipse, spacing, angle + M_PI_2);
    for (const auto & p : ortogonals) {
      lines.emplace_back(p);
    }
    if (type == CrossingHachure) {
      type = StraightHachure;
    } else {
      type = SketchyHachure;
    }
  } else {
    lines = hachures(ellipse, spacing, angle);
  }
  return hachuresLinesOrBezier(lines, style, type);
}

} // namespace LibBoard
