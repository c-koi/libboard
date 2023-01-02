/* -*- mode: c++ -*- */
/**
 * @file   Point.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   Dec 2015
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
#include <board/Point.h>
#include <iomanip>
#include <limits>
#include <BoardConfig.h>
#include <board/Tools.h>

namespace LibBoard
{
Point Point::Infinity(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());

Point mix(const Point & a, const Point & b, double t)
{
  return (1 - t) * a + t * b;
}

bool orthogonal(const Point & a, const Point & b)
{
  return Tools::almostEqual(a * b, 0.0);
}

} // namespace LibBoard

std::ostream & operator<<(std::ostream & out, const LibBoard::Point & p)
{
  return out << "Point(" << std::setprecision(15) << p.x << "," << p.y << ")";
}

std::ostream & operator<<(std::ostream & out, const std::vector<LibBoard::Point> & v)
{
  out << "[";
  auto it = v.cbegin();
  if (it != v.end()) {
    out << *it++;
  }
  while (it != v.end()) {
    out << "," << *it++;
  }
  return out << "]";
}
