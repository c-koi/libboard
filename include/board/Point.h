/* -*- mode: c++ -*- */
/**
 * @file   Point.h
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
 *
 * @brief  The Point structure.
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
#ifndef BOARD_POINT_H
#define BOARD_POINT_H

#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
#include <board/Tools.h>

namespace LibBoard
{

/**
 * The Point structure.
 * @brief Struct representing a 2D point.
 */
struct Point {
  double x; /**< The point's first coordinate */
  double y; /**< The point's second coordinate */

  /**
   * Point constructor.
   */
  Point() : x(0.0), y(0.0) {}

  /**
   * Point copy.
   * @param other
   */
  Point(const Point & other) : x(other.x), y(other.y) {}

  /**
   * Point assignment operator.
   * @param other
   */
  inline Point & operator=(const Point & other);

  /**
   * Point constructor.
   *
   * @param x The point's first coordinate.
   * @param y The point's second coordinate.
   */
  Point(double x, double y) : x(x), y(y) {}

  /**
   * Get the values of the x and y values.
   *
   * @param x Reference to the x to be set.
   * @param y Reference to the y to be set.
   */
  inline void get(double & x, double & y) const;

  /**
   * Rotate the point with a given angle around the origin (0,0)
   *
   * @param angle The rotation angle.
   */
  inline Point & rotate(double angle);

  /**
   * Get a rotated copy of the point, around the origin, by a given angle.
   *
   * @param angle The rotation angle (in radians).
   *
   * @return The rotated point.
   */
  inline Point rotated(double angle) const;

  /**
   * Rotate the point, given a rotation center and an angle.
   *
   * @param angle The rotation angle (in radians).
   * @param center The rotation center.
   *
   * @return The point itself (once rotated).
   */
  inline Point & rotate(double angle, const Point & center);

  /**
   * Get a rotated copy of the point, given a rotation center and an angle.
   *
   * @param angle The rotation angle (in radians).
   * @param center The rotation center.
   *
   * @return A rotated copy of the point.
   */
  inline Point rotated(double angle, const Point & center) const;

  /**
   * Get a PI/2 rotated copy of the point around (0,0), counterclockwise.
   *
   * @return A rotated copy of the point.
   */
  inline Point rotatedPI2() const;

  /**
   * Move the point given a translation vector (given as another Point).
   *
   * @param other A point seen as a translation vector.
   *
   * @return The point itself, once translated.
   */
  inline Point & operator+=(const Point & other);

  /**
   * Translate the point
   *
   * @param dx Translation along the x axis.
   * @param dy Translation along the y axis.
   * @return The point itself, once translated.
   */
  inline Point & translate(double dx, double dy);

  /**
   * Return a translated copy of the point
   *
   * @param dx Translation along the x axis.
   * @param dy Translation along the y axis.
   * @return The translated point.
   */
  inline Point translated(double dx, double dy) const;

  /**
   * Scale the point
   *
   * @param sx x factor.
   * @param sy y factor.
   * @return The point itself, once scaled.
   */
  inline Point & scale(double sx, double sy);

  /**
   * Return a scaled copy of a point
   *
   * @param sx x factor.
   * @param sy y factor.
   * @return The new point, once scaled.
   */
  inline Point scaled(double sx, double sy);

  /**
   * Move (backward) the point given a translation vector (given as another Point).
   *
   * @param other A point seen as a translation vector.
   *
   * @return The point itself, once translated.
   */
  inline Point & operator-=(const Point & other);

  /**
   * Scale the point's coordinates.
   *
   * @param s A scaling factor.
   *
   * @return The point itself, once scaled.
   */
  inline Point & operator*=(double s);

  /**
   * Downscale the point's coordinates.
   *
   * @param s A (down)scaling factor.
   *
   * @return The point itself, once scaled.
   */
  inline Point & operator/=(double s);

  /**
   * Get the opposite copy of the point.
   *
   * @return The opposite copy of the point.
   */
  inline Point operator-() const;

  /**
   * Compute the norm of the point (seen as a vector).
   *
   * @return The norm of the point.
   */
  inline double norm() const;

  /**
   * Make the point (seen as a vector) a unit vector.
   *
   * @return The point itself.
   */
  inline Point & normalise();

  /**
   * Return a normalized copy of the point (seen as a vector).
   *
   * @return A unit vector.
   */
  inline Point normalised() const;

  /**
   * Return the argument of the point (seen as a complex number).
   *
   * @return The argument of the point.
   */
  inline double argument() const;

  /**
   * Returns true if point is (Inf,Inf).
   *
   * @return true if point is (Inf,Inf).
   */
  inline bool isInf() const;

  static Point Infinity;
};

/**
 * @brief Return an interpolated point between two points at 'time' t.
 * @param a First point.
 * @param b Second point.
 * @param t Interpolation time in [0,1]. 0 is a, 1 is b.
 * @return The interpolated point.
 */
Point mix(const Point & a, const Point & b, double t);

/**
 * @brief Check if two vectors are orthogonals
 * @param a A vector
 * @param b A vector
 * @return true if the two vectors are orthogonal, otherwise false
 */
bool orthogonal(const Point & a, const Point & b);

/**
 * @brief Compute the sum of two vectors
 * @param a A vector
 * @param b A vector
 * @return The sum of a and b
 */
inline Point operator+(const Point & a, const Point & b)
{
  return Point(a.x + b.x, a.y + b.y);
}

/**
 * @brief Compute the difference between two vectors
 * @param a A vector
 * @param b A vector
 * @return The vector a-b
 */
inline Point operator-(const Point & a, const Point & b)
{
  return Point(a.x - b.x, a.y - b.y);
}

/**
 * @brief Compute the scalar product of two vectors
 * @param a A vector
 * @param b A vector
 * @return The scalar product of a and b
 */
inline double operator*(const Point & a, const Point & b)
{
  return a.x * b.x + a.y * b.y;
}

/**
 * @brief Compute the product of a vector and a scalar
 * @param p A vector
 * @param s A scalar
 * @return The product of p and s
 */
inline Point operator*(const Point & p, double s)
{
  return Point(p.x * s, p.y * s);
}

/**
 * @brief Compute the product of a vector and a scalar
 * @param s A scalar
 * @param p A vector
 * @return The product of s and p
 */
inline Point operator*(double s, const Point & p)
{
  return Point(s * p.x, s * p.y);
}

/**
 * @brief Compute the division of a vector by a scalar
 * @param p A vector
 * @param s A scalar
 * @return The division of p by s
 */
inline Point operator/(const Point & p, double s)
{
  return Point(p.x / s, p.y / s);
}

/**
 * @brief Check if two points are equal
 * @param a A point
 * @param b A point
 * @return true if the two points are equal, otherwise false
 */
inline bool operator==(const Point & a, const Point & b)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
  return (a.x == b.x) && (a.y == b.y);
#pragma clang diagnostic pop
}

/**
 * @brief Check if two points are different
 * @param a A point
 * @param b A point
 * @return true if the two points are different, otherwise false
 */
inline bool operator!=(const Point & a, const Point & b)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
  return (a.x != b.x) || (a.y != b.y);
#pragma clang diagnostic push
}

/**
 * @brief Check if two points are almost equal according to Tools::almostEqual
 * @param a A point
 * @param b A point
 * @return true if a and b are almost equal
 */
inline bool almostEqual(const Point & a, const Point & b)
{
  return Tools::almostEqual((a - b).norm(), 0.0);
}

// Inline methods

inline Point & Point::operator+=(const Point & other)
{
  x += other.x;
  y += other.y;
  return *this;
}

inline Point & Point::operator-=(const Point & other)
{
  x -= other.x;
  y -= other.y;
  return *this;
}

inline Point & Point::operator*=(double s)
{
  x *= s;
  y *= s;
  return *this;
}

inline Point & Point::operator/=(double s)
{
  x /= s;
  y /= s;
  return *this;
}

Point & Point::operator=(const Point & other)
{
  x = other.x;
  y = other.y;
  return *this;
}

void Point::get(double & x, double & y) const
{
  x = Point::x;
  y = Point::y;
}

Point & Point::rotate(double angle)
{
  const double x = cos(angle) * Point::x - sin(angle) * Point::y;
  const double y = sin(angle) * Point::x + cos(angle) * Point::y;
  Point::x = x;
  Point::y = y;
  return *this;
}

Point Point::rotated(double angle) const
{
  return Point(*this).rotate(angle);
}

Point & Point::rotate(double angle, const Point & center)
{
  (*this) -= center;
  (*this).rotate(angle);
  (*this) += center;
  return *this;
}

Point Point::rotated(double angle, const Point & center) const
{
  return Point(*this).rotate(angle, center);
}

Point Point::rotatedPI2() const
{
  return Point(-y, x);
}

Point & Point::translate(double dx, double dy)
{
  x += dx;
  y += dy;
  return *this;
}

Point Point::translated(double dx, double dy) const
{
  return Point(x + dx, y + dy);
}

Point & Point::scale(double sx, double sy)
{
  x *= sx;
  y *= sy;
  return *this;
}

Point Point::scaled(double sx, double sy)
{
  return Point(*this).scale(sx, sy);
}

double Point::norm() const
{
  return std::hypot(x, y);
}

Point Point::normalised() const
{
  return (*this) / norm();
}

Point & Point::normalise()
{
  double n = norm();
  x /= n;
  y /= n;
  return *this;
}

double Point::argument() const
{
  return std::atan2(y, x);
}

bool Point::isInf() const
{
  return (*this) == Point::Infinity;
}

Point Point::operator-() const
{
  return Point(-x, -y);
}

} // namespace LibBoard

std::ostream & operator<<(std::ostream & out, const LibBoard::Point &);

std::ostream & operator<<(std::ostream & out, const std::vector<LibBoard::Point> &);

#endif // BOARD_POINT_H
