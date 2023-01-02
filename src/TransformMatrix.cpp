/* -*- mode: c++ -*- */
/**
 * @file   TransformMatrix.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   June 2014
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
#include <board/TransformMatrix.h>
#include <cmath>
#include <BoardConfig.h>
#include <board/Point.h>

namespace LibBoard
{

TransformMatrix TransformMatrix::translation(double dx, double dy)
{
  return TransformMatrix(1.0, 0.0, dx, 0.0, 1.0, dy);
}

TransformMatrix TransformMatrix::translation(const Point & point)
{
  return TransformMatrix(1.0, 0.0, point.x, 0.0, 1.0, point.y);
}

TransformMatrix TransformMatrix::scaling(double sx, double sy)
{
  return TransformMatrix(sx, 0.0, 0.0, 0.0, sy, 0.0);
}

TransformMatrix TransformMatrix::rotation(double angle, Type type)
{
  if (type == SVG) {
    return TransformMatrix(cos(angle), sin(angle), 0.0, -sin(angle), cos(angle), 0.0);
  }
  return TransformMatrix(cos(angle), -sin(angle), 0.0, sin(angle), cos(angle), 0.0);
}

TransformMatrix TransformMatrix::rotation(double angle, const Point & center, Type type)
{
  return TransformMatrix::translation(-center) * TransformMatrix::rotation(angle, type) * TransformMatrix::translation(center);
}

TransformMatrix TransformMatrix::operator*(const TransformMatrix & other) const
{
  return TransformMatrix(_m11 * other._m11 + _m12 * other._m21, _m11 * other._m12 + _m12 * other._m22, _m11 * other._m13 + _m12 * other._m23 + _m13, _m21 * other._m11 + _m22 * other._m21,
                         _m21 * other._m12 + _m22 * other._m22, _m21 * other._m13 + _m22 * other._m23 + _m23);
}

TransformMatrix & TransformMatrix::operator*=(const TransformMatrix & right)
{
  TransformMatrix self(*this);
  _m11 = self._m11 * right._m11 + self._m12 * right._m21;
  _m12 = self._m11 * right._m12 + self._m12 * right._m22;
  _m13 = self._m11 * right._m13 + self._m12 * right._m23 + self._m13;
  _m21 = self._m21 * right._m11 + self._m22 * right._m21;
  _m22 = self._m21 * right._m12 + self._m22 * right._m22;
  _m23 = self._m21 * right._m13 + self._m22 * right._m23 + self._m23;
  return *this;
}

Point TransformMatrix::operator*(const Point & point) const
{
  return Point(_m11 * point.x + _m12 * point.y + _m13, _m21 * point.x + _m22 * point.y + _m23);
}

TransformMatrix TransformMatrix::operator+(const Point & point) const
{
  TransformMatrix result(*this);
  result._m13 += point.x;
  result._m23 += point.y;
  return result;
}

TransformMatrix & TransformMatrix::operator+=(const Point & point)
{
  _m13 += point.x;
  _m23 += point.y;
  return *this;
}

void TransformMatrix::flushSVG(std::ostream & out) const
{
  out << "transform=\"matrix(" << _m11 << "," << _m21 << "," << _m12 << "," << _m22 << "," << _m13 << "," << _m23 << ")\"";
}

void TransformMatrix::flushEPS(std::ostream & out) const
{
  out << "[ " << _m11 << " " << _m21 << " " << _m12 << " " << _m22 << " " << _m13 << " " << _m23 << " ] concat ";
}

} // namespace LibBoard
