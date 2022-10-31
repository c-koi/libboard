/* -*- mode: c++ -*- */
/**
 * @file   Path.h
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2009
 *
 * @brief
 * \@copyright
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr/>
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
#ifndef BOARD_PATH_H
#define BOARD_PATH_H

#include <initializer_list>
#include <iostream>
#include <vector>
#include "board/Point.h"
#include "board/Rect.h"
#include "board/Transforms.h"

// TODO : Add a resample (uniformly wrt to length) method

namespace LibBoard
{

/**
 * The path structure.
 * @brief A path, according to Postscript and SVG definition.
 */
struct Path {

  typedef std::vector<Point>::size_type size_type;

  enum OpenClosed
  {
    OpenPath,
    ClosedPath
  };

  Path() : _openClosed(OpenPath) {}

  Path(const std::vector<Point> & points, OpenClosed openClosed) : _points(points), _openClosed(openClosed) {}

  Path(std::initializer_list<Point> points);

  // Path(std::initializer_list<Point> points);

  explicit Path(OpenClosed openClosed) : _openClosed(openClosed) {}

  inline void clear();

  inline bool isClosed() const;

  inline bool isOpen() const;

  inline bool empty() const;

  inline std::size_t size() const;

  inline void close();

  inline OpenClosed openClosed() const;

  inline void open();

  inline void setOpenClosed(OpenClosed openClosed);

  /**
   * Center of the bounding box of the path.
   * @return The center of the bounding box of the path.
   */
  Point center() const;

  /**
   * Add a point at the end of the path.
   *
   * @param p A point.
   *
   * @return The path itself.
   */
  Path & operator<<(const Point & p);

  /**
   * Add a vector of points at the end of the path.
   *
   * @param v A vector of points.
   *
   * @return The path itself.
   */
  Path & operator<<(const std::vector<Point> & v);

  /**
   * Remove the last point of the path.
   *
   * @return The path itself.
   */
  Path & pop_back();

  /**
   * Add a point to the path.
   *
   * @return The path itself.
   */
  Path & push_back(const Point & p);

  /**
   * The first point of the path.
   *
   * @return The first point of the path.
   */
  inline const Point & front() const;

  /**
   * The last point of the path.
   *
   * @return The last point of the path.
   */
  inline const Point & back() const;

  /**
   * Get the n-th point of the path.
   *
   * @param n Index of a point in the path.
   *
   * @return A reference to the n-th point.
   */
  inline Point & operator[](const std::size_t n);

  /**
   * Get the n-th point of the path (const version).
   *
   * @param n Index of a point in the path.
   *
   * @return A (const) reference to the n-th point.
   */
  inline const Point & operator[](const std::size_t n) const;

  /**
   * Rotate the path by a given angle and according to a rotation center.
   *
   * @param angle The rotation angle (in radians).
   * @param center The rotation center.
   *
   * @return The path itself.
   */
  Path & rotate(double angle, const Point & center);

  /**
   * Rotate the path by a given angle, in degrees, and according to a rotation
   * center.
   *
   * @param angle The rotation angle (in degrees).
   * @param center The rotation center.
   *
   * @return The path itself.
   */
  Path & rotateDeg(double angle, const Point & center);

  /**
   * Return a rotated copy of the path, thanks to an angle and a rotation
   * center.
   *
   * @param angle The rotation angle (in radians).
   * @param center The rotation center.
   *
   * @return A rotated copy of the path.
   */
  Path rotated(double angle, const Point & center) const;

  /**
   * Return a rotated copy of the path, thanks to an angle and a rotation
   * center.
   *
   * @param angle The rotation angle (in degrees).
   * @param center The rotation center.
   *
   * @return A rotated copy of the path.
   */
  Path rotatedDeg(double angle, const Point & center) const;

  /**
   * Rotate the path by a given angle around the center of its bounding box.
   *
   * @param angle The rotation angle (in radians).
   *
   * @return The path itself.
   */
  Path & rotate(double angle);

  /**
   * Rotate the path by a given angle around the center of its bounding box.
   *
   * @param angle The rotation angle (in degrees).
   *
   * @return The path itself.
   */
  Path & rotateDeg(double angle);

  /**
   * Return a rotated copy of the path, around the center of its bounding box.
   *
   * @param angle The rotation angle (in radians).
   *
   * @return A rotated copy of the point.
   */
  Path rotated(double angle) const;

  /**
   * Return a rotated copy of the path, around the center of its bounding box.
   *
   * @param angle The rotation angle (in degrees).
   *
   * @return A rotated copy of the point.
   */
  Path rotatedDeg(double angle) const;

  /**
   * Translate the path.
   *
   * @param dx The shift along the x axis.
   * @param dy The shift along the y axis.
   *
   * @return The path itself.
   */
  Path & translate(double dx, double dy);

  /**
   * Return a translated copy of the path.
   *
   * @param dx The shift along the x axis.
   * @param dy The shift along the y axis.
   *
   * @return A translated copy of the path.
   */
  Path translated(double dx, double dy) const;

  /**
   * @brief Translate the center to a given position.
   * @param x
   * @param y
   * @return The (translated) path itself
   */
  Path & moveCenter(double x, double y);

  /**
   * @brief Translate the center to a given position.
   * @param p
   * @return The (translated) path itself
   */
  Path & moveCenter(Point p);

  /**
   * Apply a scaling factor to the path along each axis.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return The path itself, once scaled.
   */
  Path & scale(double sx, double sy);

  /**
   * Apply a scaling factor to the path.
   *
   * @param s The scaling factor.
   *
   * @return The path itself, once scaled.
   */
  Path & scale(double s);

  /**
   * Return a scaled copy of the path.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return The scaled copy of the path.
   */
  Path scaled(double sx, double sy) const;

  /**
   * Return a scaled copy of the path.
   *
   * @param s The scaling factor along both axis's.
   *
   * @return The scaled copy of the path.
   */
  Path scaled(double s) const;

  /**
   * Scale all the points.
   *
   * @param s The scaling factor.
   */
  void scaleAll(double s);

  void flushPostscript(std::ostream & stream, const TransformEPS & transform) const;

  void flushFIG(std::ostream & stream, const TransformFIG & transform) const;

  void flushSVGPoints(std::ostream & stream, const TransformSVG & transform) const;

  void flushSVGCommands(std::ostream & stream, const TransformSVG & transform) const;

  void flushTikZPoints(std::ostream & stream, const TransformTikZ & transform) const;

  Path transformed(const Transform & transform) const;

  /**
   * @brief Tell if the points of the path are ordered clockwise.
   * @return true if the points are ordered clockwise, otherwise false.
   */
  bool isClockwise() const;

  /**
   * @brief Tell if the points of the path are ordered counterclockwise.
   * @return true if the points are ordered counterclockwise, otherwise false.
   */
  bool isCounterclockwise() const;

  /**
   * @brief Make the path clockwise
   */
  void setClockwise();

  /**
   * @brief Make the path counterclockwise
   */
  void setCounterclockwise();

  /**
   * @brief Return a clockwise copy of the path
   * @return A clockwise copy of the path
   */
  Path getClockwise() const;

  /**
   * @brief Return a counterclockwise copy of the path
   * @return A counterclockwise copy of the path
   */
  Path getCounterclockwise() const;

  /**
   * Compute the bounding box of the path.
   *
   * @return The bounding box of the path.
   */
  Rect boundingBox() const;

  const std::vector<Point> & points() const;

  std::ostream & flush(std::ostream &) const;

  inline std::vector<Point>::const_iterator begin() const;
  inline std::vector<Point>::const_iterator cbegin() const;
  inline std::vector<Point>::const_iterator end() const;
  inline std::vector<Point>::const_iterator cend() const;

protected:
  std::vector<Point> _points;
  OpenClosed _openClosed;
};

/**
 * @brief Interpolate two paths according to a time (0 is a, 1 is b)
 * @param a First path
 * @param b Second path
 * @param time Interpolation time in (0 is a, 1 is b)
 * @return Interpolated path
 */
Path mix(const Path & a, const Path & b, double time);

// Inline methods

void Path::clear()
{
  _points.clear();
}

Point & Path::operator[](const std::size_t n)
{
  return _points[n];
}

const Point & Path::operator[](const std::size_t n) const
{
  return _points[n];
}

std::vector<Point>::const_iterator Path::begin() const
{
  return _points.begin();
}

std::vector<Point>::const_iterator Path::cbegin() const
{
  return _points.begin();
}

std::vector<Point>::const_iterator Path::end() const
{
  return _points.end();
}

std::vector<Point>::const_iterator Path::cend() const
{
  return _points.cend();
}

bool Path::isClosed() const
{
  return _openClosed == ClosedPath;
}

bool Path::isOpen() const
{
  return _openClosed == OpenPath;
}

bool Path::empty() const
{
  return _points.empty();
}

std::size_t Path::size() const
{
  return _points.size();
}

void Path::close()
{
  _openClosed = ClosedPath;
}

Path::OpenClosed Path::openClosed() const
{
  return _openClosed;
}

void Path::open()
{
  _openClosed = OpenPath;
}

const Point & Path::front() const
{
  return _points.front();
}

const Point & Path::back() const
{
  return _points.back();
}

} // namespace LibBoard

std::ostream & operator<<(std::ostream & out, const LibBoard::Path & path);

#endif /* BOARD_PATH_H */
