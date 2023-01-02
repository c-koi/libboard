/* -*- mode: c++ -*- */
/**
 * @file   TransformMatrix.h
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
#ifndef BOARD_TRANSFORM_MATRIX_H
#define BOARD_TRANSFORM_MATRIX_H

#include <board/Point.h>
#include <cmath>
#include <iostream>

namespace LibBoard
{

class TransformMatrix {
public:
  enum Type
  {
    SVG,
    Postscript
  };

  inline TransformMatrix();
  inline TransformMatrix(double m11, double m12, double m13, double m21, double m22, double m23);

  static TransformMatrix translation(double dx, double dy);
  static TransformMatrix translation(const Point &);
  static TransformMatrix scaling(double sx, double sy);
  static TransformMatrix rotation(double angle, Type type);
  static TransformMatrix rotation(double angle, const Point & center, Type type);

  TransformMatrix operator*(const TransformMatrix &) const;
  TransformMatrix & operator*=(const TransformMatrix &);
  Point operator*(const Point & point) const;
  TransformMatrix operator+(const Point &) const;
  TransformMatrix & operator+=(const Point &);
  void flushSVG(std::ostream &) const;
  void flushEPS(std::ostream &) const;

private:
  double _m11, _m12, _m13;
  double _m21, _m22, _m23;
};

// inline methods

TransformMatrix::TransformMatrix()
{
  _m11 = 1.0;
  _m12 = 0.0;
  _m13 = 0.0;
  _m21 = 0.0;
  _m22 = 1.0;
  _m23 = 0.0;
}

TransformMatrix::TransformMatrix(double m11, double m12, double m13, double m21, double m22, double m23) : _m11(m11), _m12(m12), _m13(m13), _m21(m21), _m22(m22), _m23(m23) {}

} // namespace LibBoard

#endif /* BOARD_TRANSFORM_MATRIX_H */
