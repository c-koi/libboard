/* -*- mode: c++ -*- */
/**
 * @file   Bezier.cpp
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
#include <algorithm>
#include <board/Bezier.h>
#include <board/Debug.h>
#include <board/PSFonts.h>
#include <board/PathBoundaries.h>
#include <board/Rect.h>
#include <board/ShapeVisitor.h>
#include <board/Tools.h>
#include <board/Transforms.h>
#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>
#include <set>
#include <sstream>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

namespace
{
inline LibBoard::Point BezierEval(const LibBoard::Point & p0, const LibBoard::Point & p1, const LibBoard::Point & p2, const LibBoard::Point & p3, double t)
{
  const double t2 = t * t;
  const double t3 = t2 * t;
  const double tc = (1 - t);
  const double tc2 = tc * tc;
  const double tc3 = tc2 * tc;
  return tc3 * p0 + 3 * tc2 * t * p1 + 3 * tc * t2 * p2 + t3 * p3;
}
} // namespace

namespace LibBoard
{

const std::string Bezier::_name("Bezier");

Bezier::Bezier(const std::vector<Point> & points, const std::vector<Point> & controls, Color penColor, Color fillColor, double lineWidth, const LineStyle lineStyle, const LineCap cap,
               const LineJoin join)
    : ShapeWithStyle(penColor, fillColor, lineWidth, lineStyle, cap, join), _path(points, Path::Open), _controls(controls, Path::Open)
{
}

Bezier::Bezier(const std::vector<Point> & points, const std::vector<Point> & controls, const Style & style) //
    : ShapeWithStyle(style), _path(points, Path::Open), _controls(controls, Path::Open)
{
}

Bezier::Bezier(Point p0, Point control0, Point p1, Point control1, //
               Color penColor, Color fillColor, double lineWidth, const LineStyle lineStyle, const LineCap cap, const LineJoin join)
    : ShapeWithStyle(penColor, fillColor, lineWidth, lineStyle, cap, join)
{
  _path << p0 << p1;
  _controls << control0 << control1;
}

Bezier::Bezier(Point p0, Point control0, Point p1, Point control1, const Style & style) //
    : ShapeWithStyle(style)
{
  _path << p0 << p1;
  _controls << control0 << control1;
}

Bezier::Bezier(double x0, double y0, double xc0, double yc0, double x1, double y1, double xc1, double yc1, Color penColor, Color fillColor, double lineWidth, const LineStyle lineStyle,
               const LineCap cap, const LineJoin join)
    : ShapeWithStyle(penColor, fillColor, lineWidth, lineStyle, cap, join)
{
  _path << Point(x0, y0) << Point(x1, y1);
  _controls << Point(xc0, yc0) << Point(xc1, yc1);
}

Bezier::Bezier(double x0, double y0, double xc0, double yc0, double x1, double y1, double xc1, double yc1, const Style & style) //
    : ShapeWithStyle(style)
{
  _path << Point(x0, y0) << Point(x1, y1);
  _controls << Point(xc0, yc0) << Point(xc1, yc1);
}

const std::string & Bezier::name() const
{
  return _name;
}

Bezier & Bezier::rotate(double angle, const Point & center)
{
  _path.rotate(angle, center);
  _controls.rotate(angle, center);
  return *this;
}

Bezier Bezier::rotated(double angle, const Point & center) const
{
  return Bezier(*this).rotate(angle, center);
}

Bezier & Bezier::rotate(double angle)
{
  _path.rotate(angle, center());
  _controls.rotate(angle, center());
  return *this;
}

Bezier Bezier::rotated(double angle) const
{
  return Bezier(*this).rotate(angle, center());
}

Bezier & Bezier::translate(double dx, double dy)
{
  _path.translate(dx, dy);
  _controls.translate(dx, dy);
  return *this;
}

Bezier Bezier::translated(double dx, double dy) const
{
  return Bezier(*this).translate(dx, dy);
}

Bezier & Bezier::scale(double sx, double sy)
{
  // Compute relative control vectors
  const std::vector<Point> & points = _path.points();
  std::vector<Point> controls;
  std::vector<Point> rControls;
  auto itC = _controls.begin();
  for (decltype(points.size()) i = 0; i < points.size() - 1; ++i) {
    const auto & a = points[i];
    const auto & b = points[i + 1];
    const auto & c0 = itC[0];
    const auto & c1 = itC[1];
    rControls.emplace_back((c0 - a).scaled(sx, sy));
    rControls.emplace_back((c1 - b).scaled(sx, sy));
    itC += 2;
  }
  _path.scale(sx, sy);
  // Compute new controls from scaled relative ones
  controls.clear();
  auto itRC = rControls.begin();
  for (decltype(points.size()) i = 0; i < points.size() - 1; i += 1) {
    controls.emplace_back(points[i] + itRC[0]);
    controls.emplace_back(points[i + 1] + itRC[1]);
    itRC += 2;
  }
  _controls = Path(controls, Path::Open);
  updateLineWidth(std::max(sx, sy));
  return *this;
}

Bezier & Bezier::scale(double s)
{
  Bezier::scale(s, s);
  return *this;
}

Bezier Bezier::scaled(double sx, double sy) const
{
  return Bezier(*this).scale(sx, sy);
}

Bezier Bezier::scaled(double s) const
{
  return Bezier(*this).scale(s, s);
}

void Bezier::scaleAll(double s)
{
  _path.scaleAll(s);
  _controls.scaleAll(s);
}

Bezier Bezier::resized(double w, double h, LineWidthFlag lineWidthFlag) const
{
  return static_cast<Bezier &>(Bezier(*this).resize(w, h, lineWidthFlag));
}

Bezier * Bezier::clone() const
{
  return new Bezier(*this);
}

// TODO : test another digitization process to minimize the number of segments
// TODO : Compute bounding box numerically according to derivative

// Path Bezier::discretizedPath() const
//{
//  Path path;

//  std::vector<Point>::const_iterator iPoint = _path.points().begin();
//  std::vector<Point>::const_iterator endPoint = _path.points().end();
//  std::vector<Point>::const_iterator iControl = _controls.points().begin();
//  Point p0(iPoint->x, iPoint->y);
//  Point p1;
//  ++iPoint;
//  while (iPoint != endPoint) {
//    p1 = Point(iPoint->x, iPoint->y);
//    Point t0 = 3 * (Point(iControl[0].x, iControl[0].y) - p0);
//    Point t1 = 3 * (p1 - Point(iControl[1].x, iControl[1].y));

//    //
//    // Discretize a Bezier segment
//    //
//    const double a0 = 2 * p0.x - 2 * p1.x + t0.x + t1.x;
//    const double b0 = -3 * p0.x + 3 * p1.x - 2 * t0.x - t1.x;
//    const double c0 = t0.x;
//    const double d0 = p0.x;
//    const double a1 = 2 * p0.y - 2 * p1.y + t0.y + t1.y;
//    const double b1 = -3 * p0.y + 3 * p1.y - 2 * t0.y - t1.y;
//    const double c1 = t0.y;
//    const double d1 = p0.y;
//    // Compute optimal step
//    double previousLength = 0.0;
//    double length = std::numeric_limits<double>::max();
//    double step = 1.0;
//    while (step > 1 / 512.0 && fabs(length - previousLength) > 0.00002) {
//      step /= 2.0;
//      previousLength = length;
//      // Compute length
//      Point previous(d0, d1);
//      length = 0.0;
//      for (double t = step; t <= 1.0; t += step) {
//        Point current(a0 * t * t * t + b0 * t * t + c0 * t + d0, a1 * t * t * t + b1 * t * t + c1 * t + d1);
//        length += (current - previous).norm();
//        previous = current;
//      }
//    }
//    // Add points
//    for (double t = 0; t <= 1.0; t += step) {
//      //      std::cout << Point( (a0 * t * t * t + b0 * t * t + c0 * t + d0),
//      //                     (a1 * t * t * t + b1 * t * t + c1 * t + d1) ) << std::endl;
//      path << Point((a0 * t * t * t + b0 * t * t + c0 * t + d0), (a1 * t * t * t + b1 * t * t + c1 * t + d1));
//    }

//    ++iPoint;
//    iControl += 2;
//    p0 = p1;
//  }
//  return path;
//}

std::vector<Point> Bezier::pathThroughLocalExtremums() const
{
  std::set<double> tValues;

  std::vector<Point> points;
  std::vector<Point>::const_iterator iPoint = _path.points().begin();
  std::vector<Point>::const_iterator endPoint = _path.points().end();
  std::vector<Point>::const_iterator iControl = _controls.points().begin();
  Point p0(iPoint->x, iPoint->y);
  Point p3;
  ++iPoint;
  Path::size_type interval = 0;
  while (iPoint != endPoint) {
    tValues.clear();
    tValues.insert(0.0); // First point
    p3 = Point(iPoint->x, iPoint->y);
    const Point & p1 = iControl[0];
    const Point & p2 = iControl[1];

    // Derivative coefficients in this interval, to solve B(t)' = at^2+bt+c =? 0
    const Point a = 3 * (p1 - p0) - 6 * (p2 - p1) + 3 * (p3 - p2);
    const Point b = -6 * (p1 - p0) + 6 * (p2 - p1);
    const Point c = 3 * (p1 - p0);
    double t1, t2;
    if (Tools::solveQuadratic(a.x, b.x, c.x, t1, t2)) {
      if ((0.0 < t1) && (t1 < 1.0)) {
        tValues.insert(t1);
      }
      if (!Tools::almostEqual(t1, t2) && (0.0 < t2) && (t2 < 1.0)) {
        tValues.insert(t2);
      }
    }
    if (Tools::solveQuadratic(a.y, b.y, c.y, t1, t2)) {
      if ((0.0 < t1) && (t1 < 1.0)) {
        tValues.insert(t1);
      }
      if (!Tools::almostEqual(t1, t2) && (0.0 < t2) && (t2 < 1.0)) {
        tValues.insert(t2);
      }
    }
    for (const double & t : tValues) {
      points.emplace_back(eval(p0, p1, p2, p3, t));
    }
    tValues.clear();
    ++iPoint;
    ++interval;
    if (iPoint == endPoint) {
      points.emplace_back(eval(p0, p1, p2, p3, 1.0));
    }
    iControl += 2;
    p0 = p3;
  }

  points.emplace_back(_path.back());

  return points;
} // namespace LibBoard

Path Bezier::discretizedPath() const
{
  Path path;
  std::vector<Point>::const_iterator iPoint = _path.points().begin();
  std::vector<Point>::const_iterator endPoint = _path.points().end();
  std::vector<Point>::const_iterator iControl = _controls.points().begin();
  Point p0(iPoint->x, iPoint->y);
  Point p1;
  ++iPoint;
  while (iPoint != endPoint) {
    p1 = Point(iPoint->x, iPoint->y);
    const Point & c0 = iControl[0];
    const Point & c1 = iControl[1];

    // Add points
    for (double t = 0; t <= 1.0; t += 0.01) {
      Point p = std::pow(1 - t, 3.0) * p0 + 3 * std::pow(1 - t, 2.0) * t * c0 + 3 * (1 - t) * std::pow(t, 2.0) * c1 + std::pow(t, 3) * p1;
      path << p;
    }

    ++iPoint;
    iControl += 2;
    p0 = p1;
  }
  return path;
}

Bezier & Bezier::operator+=(const Bezier & other)
{
  Point end = _path.back();
  Point start = other.path().front();
  bool contiguous = (start == end);
  if (!contiguous) {
    Point diff = start - end;
    const double len = diff.norm();
    Point orientation = diff.normalised();
    _controls.push_back(end + (len * 0.33 * orientation));
    _controls.push_back(start - (len * 0.33 * orientation));
  }
  {
    std::vector<Point>::const_iterator it = other.path().points().begin();
    std::vector<Point>::const_iterator limit = other.path().points().end();
    if (contiguous) {
      ++it;
    }
    while (it != limit) {
      _path.push_back(*it++);
    }
  }
  {
    std::vector<Point>::const_iterator it = other.controls().points().begin();
    std::vector<Point>::const_iterator limit = other.controls().points().end();
    while (it != limit) {
      _controls.push_back(*it++);
    }
  }
  return *this;
}

Bezier Bezier::operator+(const Bezier & other) const
{
  return Bezier(*this) += other;
}

Bezier Bezier::smoothedPolyline(const std::vector<Point> & path, double roundness, const Style & style)
{
  assert(path.size() >= 2 && "Bezier::smoothedPolyline() need at least two points");
  std::vector<Point> points;
  std::vector<Point> controls;
  points.push_back(path[0]);
  controls.push_back(mix(path[0], path[1], 0.3 * roundness));
  for (std::vector<Point>::size_type i = 1; i < path.size() - 1; ++i) {
    Point previous = path[i - 1];
    Point current = path[i];
    Point next = path[i + 1];
    Point orientation = (next - previous).normalised();
    points.push_back(current);
    controls.push_back(current - ((current - previous).norm() * 0.3 * roundness) * orientation);
    controls.push_back(current + ((next - current).norm() * 0.3 * roundness) * orientation);
  }
  Point previous = path[path.size() - 2];
  Point current = path.back();
  points.push_back(current);
  controls.push_back(mix(current, previous, 0.3 * roundness));
  return Bezier(points, controls, style);
}

Bezier Bezier::smoothedPolyline(const Path & path, double roundness, const Style & style)
{
  if (path.isClosed()) {
    std::vector<Point> closedPath = path.points();
    closedPath.push_back(closedPath.front());
    std::vector<Point> points;
    std::vector<Point> controls;
    const std::vector<Point>::size_type len = closedPath.size();

    Point previous = closedPath[len - 2];
    Point current = closedPath[0];
    Point next = closedPath[1];
    Point orientation = (next - previous).normalised();
    points.push_back(current);
    controls.push_back(current + ((next - current).norm() * 0.3 * roundness) * orientation);

    for (std::vector<Point>::size_type i = 1; i < len - 1; ++i) {
      Point previous = closedPath[i - 1];
      Point current = closedPath[i];
      Point next = closedPath[i + 1];
      Point orientation = (next - previous).normalised();
      points.push_back(current);
      controls.push_back(current - ((current - previous).norm() * 0.3 * roundness) * orientation);
      controls.push_back(current + ((next - current).norm() * 0.3 * roundness) * orientation);
    }
    points.push_back(current);
    controls.push_back(current - ((current - previous).norm() * 0.3 * roundness) * orientation);
    return Bezier(points, controls, style);
  } else {
    return smoothedPolyline(path.points(), roundness, style);
  }
}

Bezier Bezier::interpolation(const Point & a, const Point & b, const Point & c, const Point & d, const Style & style)
{
  const Point & p0 = a;
  const Point p1 = (-5 * a + 18 * b - 9 * c + 2 * d) / 6.0;
  const Point p2 = (2 * a - 9 * b + 18 * c - 5 * d) / 6.0;
  const Point & p3 = d;
  std::vector<Point> points = {p0, p3};
  std::vector<Point> controls = {p1, p2};
  return Bezier(points, controls, style);
}

Point Bezier::eval(const Point & p0, const Point & p1, const Point & p2, const Point & p3, double t) const
{
  return BezierEval(p0, p1, p2, p3, t);
}

Point Bezier::eval(Path::size_type interval, double t) const
{
  assert((_path.size() && (interval < _path.size() - 1)) && "Interval is out of limits");
  Point p0 = _path[interval];
  Point p1 = _controls[2 * interval];
  Point p2 = _controls[2 * interval + 1];
  Point p3 = _path[interval + 1];
  return BezierEval(p0, p1, p2, p3, t);
}

void Bezier::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  if (_path.empty()) {
    return;
  }
  if (_controls.size() != 2 * (_path.size() - 1)) {
    Tools::error << "Bezier::flushPostscript: wrong number of control points\n";
    return;
  }

  stream << "\n% Bezier\n";
  if (filled()) {
    stream << "n ";

    // Fill
    std::vector<Point>::const_iterator iPoint = _path.points().begin();
    std::vector<Point>::const_iterator endPoint = _path.points().end();
    std::vector<Point>::const_iterator iControl = _controls.points().begin();
    int count = 0;
    stream << transform.mapX(iPoint->x) << " " << transform.mapY(iPoint->y) << " m";
    ++iPoint;
    while (iPoint != endPoint) {
      stream << " " << transform.mapX(iControl[0].x) << " " << transform.mapY(iControl[0].y) << " " << transform.mapX(iControl[1].x) << " " << transform.mapY(iControl[1].y) << " "
             << transform.mapX(iPoint->x) << " " << transform.mapY(iPoint->y) << " ct";
      ++iPoint;
      iControl += 2;
      count = (count + 1) % 6;
      if (!count)
        stream << "\n                  ";
    }
    stream << " ";

    fillColor().flushPostscript(stream);
    stream << " " << _style.postscriptProperties(transform);
    stream << " fill" << std::endl;
  }
  if (penColor() != Color::Null && lineWidth() != 0.0) {
    stream << " " << _style.postscriptProperties(transform) << "\n";
    stream << "n ";

    // Stroke
    std::vector<Point>::const_iterator iPoint = _path.points().begin();
    std::vector<Point>::const_iterator endPoint = _path.points().end();
    std::vector<Point>::const_iterator iControl = _controls.points().begin();
    int count = 0;
    stream << transform.mapX(iPoint->x) << " " << transform.mapY(iPoint->y) << " m";
    ++iPoint;
    while (iPoint != endPoint) {
      stream << " " << transform.mapX(iControl[0].x) << " " << transform.mapY(iControl[0].y) << " " //
             << transform.mapX(iControl[1].x) << " " << transform.mapY(iControl[1].y) << " "        //
             << transform.mapX(iPoint->x) << " " << transform.mapY(iPoint->y) << " ct";
      ++iPoint;
      iControl += 2;
      count = (count + 1) % 6;
      if (!count)
        stream << "\n                  ";
    }

    stream << " ";
    penColor().flushPostscript(stream);
    stream << " stroke" << std::endl;
  }
}

void Bezier::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  // FIXME : Complete this (convert Bezier to X-spline)
  Tools::unused(stream);
  Tools::unused(transform);
  Tools::unused(colormap);
  Tools::warning << "Bezier::flushFIG not supported yet\n";
}

void Bezier::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  if (_path.empty()) {
    return;
  }
  if (_controls.size() != 2 * (_path.size() - 1)) {
    Tools::error << "Bezier::flushSVG: wrong number of control points\n";
    return;
  }
  stream << "<path";
  stream << _style.svgProperties(transform) << std::endl;
  stream << "          d=\"";
  std::vector<Point>::const_iterator iPoint = _path.points().begin();
  std::vector<Point>::const_iterator endPoint = _path.points().end();
  std::vector<Point>::const_iterator iControl = _controls.points().begin();
  int count = 0;
  stream << "M" << transform.mapX(iPoint->x) << "," << transform.mapY(iPoint->y);
  ++iPoint;
  stream << " C";
  while (iPoint != endPoint) {
    stream << " " << transform.mapX(iControl[0].x) << "," << transform.mapY(iControl[0].y) << " " //
           << transform.mapX(iControl[1].x) << "," << transform.mapY(iControl[1].y) << " "        //
           << transform.mapX(iPoint->x) << "," << transform.mapY(iPoint->y);
    ++iPoint;
    iControl += 2;
    count = (count + 1) % 6;
    if (!count) {
      stream << "\n                  ";
    }
  }
  stream << "\" />" << std::endl;
}

void Bezier::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  Tools::unused(stream);
  Tools::unused(transform);
  // Not supported yet
  Tools::warning << "Bezier::flushTikZ not supported yet\n";
}

void Bezier::accept(ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Bezier::accept(const ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Bezier::accept(ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

void Bezier::accept(const ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

Shape * Bezier::accept(CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Shape * Bezier::accept(const CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Rect Bezier::boundingBox(LineWidthFlag lineWidthFlag) const
{
  // Path path = discretizedPath(); // DONE: Make it more "analytical".
  Path path(pathThroughLocalExtremums(), Path::Open);
  Rect result;
  switch (lineWidthFlag) {
  case UseLineWidth:
    result = Tools::pathBoundingBox(path, lineWidth(), lineCap(), RoundJoin);
    break;
  case IgnoreLineWidth:
    result = path.boundingBox();
    break;
  }
  using ::operator<<;
  LSHOW(_path.points());
  LSHOW(result);
  return result;
}

} // namespace LibBoard
