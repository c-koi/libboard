/* -*- mode: c++ -*- */
/**
 * @file   RoughVisitor.cpp
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
#include <board/RoughVisitor.h>
#include <cassert>
#include <deque>
#include <iostream>
#include <typeinfo>
#include <Board.h>
#include <board/Debug.h>
#include <board/Shape.h>

namespace LibBoard
{

RoughVisitor::RoughVisitor()
{
  _repeat = 1;
  _filling = NoFilling;
  _hachureAngle = 0.0;
  _hachureSpacing = 0.0;
  _automaticHachureSpacing = true;
}

void RoughVisitor::setRepeat(int n)
{
  assert((n > 0) && "RoughVisitor::repeat()");
  _repeat = n;
}

void RoughVisitor::setFilling(SketchFilling filling)
{
  _filling = filling;
}

void RoughVisitor::setHachureAngle(double angle)
{
  _hachureAngle = angle;
}

void RoughVisitor::setHachureSpacing(double spacing)
{
  _hachureSpacing = spacing;
  _automaticHachureSpacing = (spacing == 0.0);
}

void RoughVisitor::setAutomaticHachureSpacing(bool on)
{
  _automaticHachureSpacing = on;
}

Shape * RoughVisitor::map(const Shape & shape)
{
  std::cerr << "Warning: RoughVisitor does not know how to visit a " << typeid(shape).name() << std::endl;
  return nullptr;
}

Shape * RoughVisitor::map(const Shape & shape) const
{
  std::cerr << "Warning: const RoughVisitor does not know how to visit a " << typeid(shape).name() << std::endl;
  return nullptr;
}

Shape * RoughVisitor::map(const Line & line)
{
  return static_cast<const RoughVisitor &>(*this).map(line);
}

Shape * RoughVisitor::map(const Line & line) const
{
  const double length = (line.b() - line.a()).norm();
  const double radius = 0.015 * length;
  Group * group = nullptr;
  Shape * shape = nullptr;

  if (almostEqual(line.a(), line.b())) {
    return line.clone();
  }

  if (_repeat > 1) {
    shape = group = new Group;
  }

  Style style = line.style();
  style.lineCap = RoundCap;
  style.lineJoin = RoundJoin;

  for (int k = 0; k < _repeat; ++k) {
    const Point p0 = randomlySlided(line.a(), radius);
    const Point p1 = randomlySlidedAwayFromSegment(mix(line.a(), line.b(), 0.5), line.a(), line.b());
    const Point ab = line.b() - line.a();
    const Point u = ab.normalised();
    const Point v = u.rotatedPI2();
    const Point p2 = randomlySlidedAwayFromSegmentInBox(mix(line.a(), line.b(), 0.75), u, ab.norm() / 10.0, v, radius);
    const Point p3 = randomlySlided(line.b(), radius);
    if (group) {
      (*group) += Bezier::interpolation(p0, p1, p2, p3, style);
    } else {
      shape = Bezier::interpolation(p0, p1, p2, p3, style).clone();
    }
  }
  return shape;
}

Shape * RoughVisitor::map(const Arrow & arrow)
{
  return static_cast<const RoughVisitor &>(*this).map(arrow);
}

Shape * RoughVisitor::map(const Arrow & arrow) const
{
  const double length = (arrow.b() - arrow.a()).norm();
  const double radius = 0.015 * length;
  Group * group = nullptr;
  Shape * shape = group = new Group;

  Style style = arrow.style();
  style.lineCap = RoundCap;
  style.lineJoin = RoundJoin;
  const Color fillColor = style.fillColor;
  style.fillColor = Color::Null;

  std::vector<Point> points = arrow.extremity().points();
  Point end = (points[0] + points[1] + points[2]) / 3.0;

  for (int k = 0; k < _repeat; ++k) {
    const Point p0 = randomlySlided(arrow.a(), radius);
    const Point p1 = randomlySlidedAwayFromSegment(mix(arrow.a(), arrow.b(), 0.5), arrow.a(), arrow.b());
    const Point ab = arrow.b() - arrow.a();
    const Point u = ab.normalised();
    const Point v = u.rotatedPI2();
    const Point p2 = randomlySlidedAwayFromSegmentInBox(mix(arrow.a(), arrow.b(), 0.75), u, ab.norm() / 10.0, v, radius);
    const Point p3 = randomlySlided(end, radius / 2.0);
    (*group) += Bezier::interpolation(p0, p1, p2, p3, style);
  }

  style.fillColor = fillColor;
  Polyline p(points, (arrow.type() != Arrow::ExtremityType::Stick) ? Path::Closed : Path::Open, style); // FIXME : Take all types into account
  RoughVisitor v;
  v.setRepeat(1);
  Shape * roughExtremity = v.map(p);
  (*group) += *roughExtremity;
  delete roughExtremity;
  return shape;
}

Shape * RoughVisitor::map(const Polyline & polyline)
{
  return static_cast<const RoughVisitor &>(*this).map(polyline);
}

Shape * RoughVisitor::map(const Polyline & polyline) const
{
  assert((polyline.path().size() >= 2) && "RoughVisitor::visit()");
  Shape * shape = nullptr;
  Group * group = nullptr;
  if ((_repeat > 1) || ((_filling != NoFilling && _filling != PlainFilling))) {
    shape = group = new Group;
  }

  // TODO : No contour if penColor is null !

  Style style = polyline.style();
  style.lineCap = RoundCap;
  style.lineJoin = RoundJoin;
  const bool AddHorizontalsHachures = style.penColor.isNull();

  for (int k = 0; k < _repeat; ++k) {
    std::vector<Point> points = polyline.path().points();
    if (polyline.path().isClosed()) {
      points.push_back(points.front());
    }

    Bezier bezier(Point(0, 0), Point(1, 0), Point(2, 0), Point(3, 0), style);
    Point a = points.front();
    double length = (points[1] - a).norm();
    double radius = 0.015 * length;
    slideRandomly(a, radius);
    for (unsigned int segment = 0; segment < points.size() - 1; ++segment) {
      Point b = points[segment + 1];
      length = (points[segment + 1] - points[segment]).norm();
      radius = 0.015 * length;
      const Point ab = b - a;
      const Point u = ab.normalised();
      const Point v = u.rotatedPI2();
      slideRandomly(b, radius);
      const Point p0 = a;
      const Point p1 = randomlySlidedAwayFromSegment(mix(a, b, 0.5), a, b);
      const Point p2 = randomlySlidedAwayFromSegmentInBox(mix(a, b, 0.75), u, ab.norm() / 10.0, v, radius);
      const Point p3 = b;
      a = b;
      if (segment) {
        bezier += Bezier::interpolation(p0, p1, p2, p3, style);
      } else {
        bezier = Bezier::interpolation(p0, p1, p2, p3, style);
      }
    }
    if ((k > 0) || (_filling == NoFilling)) {
      bezier.setFillColor(Color::Null);
    } else if (k == 0 && (_filling != NoFilling && _filling != PlainFilling)) {
      bezier.setFillColor(Color::Null);
      const double spacing = _automaticHachureSpacing ? (std::max(0.1, style.lineWidth * 3)) : _hachureSpacing;
      Style hStyle = style;
      hStyle.penColor = style.fillColor;
      hStyle.fillColor = Color::Null;
      (*group) << hachures(polyline.path(), hStyle, _filling, spacing, _hachureAngle, AddHorizontalsHachures);
    }
    if (group) {
      (*group) += bezier;
    } else {
      shape = bezier.clone();
    }
  }
  return shape;
}

Shape * RoughVisitor::map(const Ellipse & ellipse)
{
  return static_cast<const RoughVisitor &>(*this).map(ellipse);
}

// TODO : Cycle the ellipse path so that the "link part" is always at the top
Shape * RoughVisitor::map(const Ellipse & ellipse) const
{
  Shape * shape = nullptr;
  Group * group = nullptr;
  if (_repeat > 1 || ((_filling != NoFilling && _filling != PlainFilling))) {
    shape = group = new Group;
  }

  Style style = ellipse.style();
  style.lineCap = RoundCap;
  style.lineJoin = RoundJoin;

  for (int k = 0; k < _repeat; ++k) {
    Path path = ellipse.sampledPath(20, Ellipse::SamplingFromTop);
    std::deque<Point> points(path.begin(), path.end());
    Point first = points.front();
    Point last = points.back();
    points.pop_front();
    points.push_back(first);
    points.push_front(last);
    path = Path(std::vector<Point>(points.begin(), points.end()), Path::Open);

    const double radius = ellipse.perimeter() / 160;
    for (unsigned int i = 0; i < path.size(); ++i) {
      slideRandomly(path[i], radius);
    }
    Bezier bezier = Bezier::smoothedPolyline(path, 0.75, style);
    if ((k > 0) || (_filling == NoFilling)) {
      bezier.setFillColor(Color::Null);
    } else if (k == 0 && ((_filling != NoFilling && _filling != PlainFilling))) {
      bezier.setFillColor(Color::Null);
      const double spacing = _automaticHachureSpacing ? (std::max(0.1, style.lineWidth * 3)) : _hachureSpacing;
      Style hStyle = style;
      hStyle.penColor = style.fillColor;
      hStyle.fillColor = Color::Null;
      (*group) << hachures(ellipse, hStyle, _filling, spacing, _hachureAngle);
    }
    if (group) {
      (*group) += bezier;
    } else {
      shape = bezier.clone();
    }
  }
  return shape;
}

Shape * RoughVisitor::map(const Bezier & bezier)
{
  return static_cast<const RoughVisitor &>(*this).map(bezier);
}

Shape * RoughVisitor::map(const Bezier & bezier) const
{
  // TODO : Actually transform a bezier
  std::vector<Point> points = bezier.path().points();
  std::vector<Point> controls = bezier.controls().points();
  const double radius = bezier.lineWidth();
  std::vector<Point>::size_type index = 0;
  for (Point & p : points) {
    Point newP = p;
    RoughVisitor::slideRandomly(newP, radius);
    Point shift = newP - p;
    p = newP;
    if (index > 0) {
      controls[2 * index - 1] += shift;
    }
    if (index < points.size()) {
      controls[2 * index] += shift;
    }
  }
  return new Bezier(points, controls, bezier.style());
}

RoughVisitor::~RoughVisitor() {}

void RoughVisitor::slideRandomly(Point & point, const double & radius)
{
  point = randomlySlided(point, radius);
}

Point RoughVisitor::randomlySlided(const Point & point, const double & radius)
{
  const double r = radius * (0.15 + Tools::boardRandDouble() * 0.85);
  const double angle = Tools::boardRandDouble(0.0, 2 * M_PI);
  return point.translated(r * cos(angle), r * sin(angle));
}

void RoughVisitor::slideRandomlyAwayFromSegment(Point & p, const Point & a, const Point & b, const double magnitude)
{
  const double r = magnitude * (Tools::boardRandDouble() * 2.0 - 1.0);
  const Point normal = (b - a).normalised().rotatedPI2();
  p += r * normal;
}

Point RoughVisitor::randomlySlidedAwayFromSegment(const Point & p, const Point & a, const Point & b, const double magnitude)
{
  Point result = p;
  slideRandomlyAwayFromSegment(result, a, b, magnitude);
  return p;
}

void RoughVisitor::slideRandomlyAwayFromSegment(Point & p, const Point & a, const Point & b)
{
  const Point ab = b - a;
  const double magnitude = ab.norm() / 200.0;
  const double r = magnitude * (Tools::boardRandDouble() * 2.0 - 1.0);
  const Point normal = ab.normalised().rotatedPI2();
  p += r * normal;
}

Point RoughVisitor::randomlySlidedAwayFromSegment(const Point & p, const Point & a, const Point & b)
{
  Point result = p;
  slideRandomlyAwayFromSegment(result, a, b);
  return result;
}

void RoughVisitor::slideRandomlyAwayFromSegmentInBox(Point & p, const Point & u, double uMagnitude, const Point & v, double vMagnitude)
{
  const Point moveU = Tools::boardRandDouble(-uMagnitude, uMagnitude) * u;
  const Point moveV = Tools::boardRandDouble(-vMagnitude, vMagnitude) * v;
  p += moveU;
  p += moveV;
}

Point RoughVisitor::randomlySlidedAwayFromSegmentInBox(const Point & p, const Point & u, double uMagnitude, const Point & v, double vMagnitude)
{
  Point result = p;
  slideRandomlyAwayFromSegmentInBox(result, u, uMagnitude, v, vMagnitude);
  return result;
}

} // namespace LibBoard
