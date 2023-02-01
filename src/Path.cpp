/* -*- mode: c++ -*- */
/**
 * @file   Path.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2009
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
#include <board/Path.h>
#include <board/Transforms.h>
#include <iterator>
#include "BoardConfig.h"
#include "board/Transforms.h"

namespace LibBoard
{

Path & Path::pop_back()
{
  _points.pop_back();
  return *this;
}

Path & Path::push_back(const Point & p)
{
  _points.push_back(p);
  return *this;
}

Path & Path::operator<<(const Point & p)
{
  _points.push_back(p);
  return *this;
}

Path & Path::operator<<(const std::vector<Point> & v)
{
  std::copy(v.begin(), v.end(), std::back_inserter(_points));
  return *this;
}

Path::Path(std::initializer_list<Point> points)
{
  _openClosed = Open;
  _points = points;
}

void Path::setOpenClosed(Path::OpenClosed openClosed)
{
  _openClosed = openClosed;
}

Point Path::center() const
{
  return boundingBox().center();
}

Path & Path::rotate(double angle, const Point & center)
{
  std::vector<Point>::iterator i = _points.begin();
  std::vector<Point>::iterator end = _points.end();
  while (i != end) {
    i->rotate(angle, center);
    ++i;
  }
  return *this;
}

Path & Path::rotateDeg(double angle, const Point & center)
{
  return rotate(angle * (M_PI / 180.0), center);
}

Path Path::rotated(double angle, const Point & center) const
{
  Path res(*this);
  std::vector<Point>::iterator i = res._points.begin();
  std::vector<Point>::iterator end = res._points.end();
  while (i != end) {
    i->rotate(angle, center);
    ++i;
  }
  return res;
}

Path Path::rotatedDeg(double angle, const Point & center) const
{
  return rotated(angle * (M_PI / 180.0), center);
}

Path & Path::rotate(double angle)
{
  return Path::rotate(angle, center());
}

Path & Path::rotateDeg(double angle)
{
  return Path::rotate(angle * (M_PI / 180.0), center());
}

Path Path::rotated(double angle) const
{
  return Path(*this).rotate(angle, center());
}

Path Path::rotatedDeg(double angle) const
{
  return Path(*this).rotate(angle * (M_PI / 180.0), center());
}

Path & Path::translate(double dx, double dy)
{
  std::vector<Point>::iterator i = _points.begin();
  std::vector<Point>::iterator end = _points.end();
  Point delta(dx, dy);
  while (i != end) {
    (*i) += delta;
    ++i;
  }
  return *this;
}

Path Path::translated(double dx, double dy) const
{
  Path res(*this);
  std::vector<Point>::iterator i = res._points.begin();
  std::vector<Point>::iterator end = res._points.end();
  Point delta(dx, dy);
  while (i != end) {
    (*i) += delta;
    ++i;
  }
  return res;
}

Path & Path::moveCenter(double x, double y)
{
  Point c = center();
  translate(x - c.x, y - c.y);
  return *this;
}

Path & Path::moveCenter(Point p)
{
  Point c = center();
  translate(p.x - c.x, p.y - c.y);
  return *this;
}

Path & Path::scale(double sx, double sy)
{
  Point c = center();
  translate(-c.x, -c.y);
  for (Point & p : _points) {
    p.scale(sx, sy);
  }
  Point delta = c - center();
  translate(delta.x, delta.y);
  return *this;
}

Path & Path::scale(double s)
{
  return Path::scale(s, s);
}

Path Path::scaled(double sx, double sy) const
{
  return Path(*this).scale(sx, sy);
}

Path Path::scaled(double s) const
{
  return Path(*this).scale(s, s);
}

void Path::scaleAll(double s)
{
  std::vector<Point>::iterator it = _points.begin();
  std::vector<Point>::iterator end = _points.end();
  while (it != end) {
    (*it) *= s;
    ++it;
  }
}

void Path::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  if (_points.empty())
    return;
  std::vector<Point>::const_iterator i = _points.begin();
  std::vector<Point>::const_iterator end = _points.end();

  stream << transform.mapX(i->x) << " " << transform.mapY(i->y) << " m";
  ++i;
  while (i != end) {
    stream << " " << transform.mapX(i->x) << " " << transform.mapY(i->y) << " l";
    ++i;
  }
  stream << (isClosed() ? " cp " : " ");
}

void Path::flushFIG(std::ostream & stream, const TransformFIG & transform) const
{
  if (_points.empty()) {
    return;
  }
  std::vector<Point>::const_iterator i = _points.begin();
  std::vector<Point>::const_iterator end = _points.end();
  while (i != end) {
    stream << " " << static_cast<int>(transform.mapX(i->x)) << " " << static_cast<int>(transform.mapY(i->y));
    ++i;
  }
  if (isClosed()) {
    stream << " " << static_cast<int>(transform.mapX(_points.begin()->x)) << " " << static_cast<int>(transform.mapY(_points.begin()->y));
  }
}

void Path::flushSVGCommands(std::ostream & stream, const TransformSVG & transform) const
{
  if (_points.empty()) {
    return;
  }
  std::vector<Point>::const_iterator i = _points.begin();
  std::vector<Point>::const_iterator end = _points.end();
  int count = 0;

  stream << "M " << transform.mapX(i->x) << " " << transform.mapY(i->y);
  ++i;
  while (i != end) {
    stream << " L " << transform.mapX(i->x) << " " << transform.mapY(i->y);
    ++i;
    count = (count + 1) % 6;
    if (!count)
      stream << "\n                  ";
  }
  if (isClosed()) {
    stream << " Z" << std::endl;
  }
}

void Path::flushSVGPoints(std::ostream & stream, const TransformSVG & transform) const
{
  if (_points.empty()) {
    return;
  }
  std::vector<Point>::const_iterator i = _points.begin();
  std::vector<Point>::const_iterator end = _points.end();
  int count = 0;
  stream << transform.mapX(i->x) << "," << transform.mapY(i->y);
  ++i;
  while (i != end) {
    stream << " " << transform.mapX(i->x) << "," << transform.mapY(i->y);
    ++i;
    count = (count + 1) % 6;
    if (!count)
      stream << "\n                  ";
  }
}

void Path::flushTikZPoints(std::ostream & stream, const TransformTikZ & transform) const
{
  if (_points.empty()) {
    return;
  }
  std::vector<Point>::const_iterator i = _points.begin();
  std::vector<Point>::const_iterator end = _points.end();
  stream << '(' << transform.mapX(i->x) << "," << transform.mapY(i->y) << ')';
  ++i;
  while (i != end) {
    stream << " -- " << '(' << transform.mapX(i->x) << "," << transform.mapY(i->y) << ')';
    ++i;
  }
}

Path Path::transformed(const Transform & transform) const
{
  Path path(_openClosed);
  for (const Point & p : _points) {
    path.push_back(transform.map(p));
  }
  return path;
}

bool Path::isClockwise() const
{
  if (_points.size() < 3) {
    return true;
  }
  double sum = 0.0;
  std::vector<Point>::const_iterator it = _points.begin();
  std::vector<Point>::const_iterator end = _points.end();
  Point previous = _points.front();
  ++it;
  while (it != end) {
    sum += previous.x * (*it).y;
    sum -= previous.y * (*it).x;
    previous = *it;
    ++it;
  }
  sum += previous.x * _points.front().y;
  sum -= previous.y * _points.front().x;
  return sum >= 0;
}

bool Path::isCounterclockwise() const
{
  return !isClockwise();
}

void Path::setClockwise()
{
  if (isCounterclockwise()) {
    std::reverse(_points.begin(), _points.end());
  }
}

void Path::setCounterclockwise()
{
  if (isClockwise()) {
    std::reverse(_points.begin(), _points.end());
  }
}

Path Path::getClockwise() const
{
  Path p(*this);
  p.setClockwise();
  return p;
}

Path Path::getCounterclockwise() const
{
  Path p(*this);
  p.setCounterclockwise();
  return p;
}

Rect Path::boundingBox() const
{
  if (_points.empty()) {
    Tools::warning << "Computing the bounding box of an empty path makes no sense";
    return Rect(0, 0, 0, 0);
  }
  Rect rect(_points.front(), 0.0, 0.0);
  for (const Point & p : _points) {
    rect.growToContain(p);
  }
  return rect;
}

const std::vector<Point> & Path::points() const
{
  return _points;
}

std::ostream & Path::flush(std::ostream & out) const
{
  out << "Path(";
  std::vector<Point>::const_iterator it = _points.begin();
  if (it != _points.end()) {
    out << (*it++);
  }
  while (it != _points.end()) {
    out << "," << (*it++);
  }
  out << ")";
  return out;
}

Path mix(const Path & a, const Path & b, double time)
{
  if (a.size() != b.size()) {
    Tools::error << "Path::mid() cannot interpolate path with different sizes";
    return a;
  }
  Path result(a.openClosed());
  auto ita = a.begin();
  auto itb = b.begin();
  while (ita != a.end()) {
    result.push_back(mix(*ita++, *itb++, time));
  }
  return result;
}

} // namespace LibBoard

std::ostream & operator<<(std::ostream & out, const LibBoard::Path & path)
{
  return path.flush(out);
}
