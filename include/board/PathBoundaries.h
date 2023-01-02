/* -*- mode: c++ -*- */
/**
 * @file   PathBoundaries.h
 * @author Sebastien Fourey (GREYC)
 * @date   Nov 2008
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
#ifndef BOARD_PATH_BOUNDARIES_H
#define BOARD_PATH_BOUNDARIES_H

#include <vector>
#include <board/Point.h>
#include <board/Rect.h>
#include <board/Shape.h>
namespace LibBoard
{

struct Path;

namespace Tools
{

/*
 * An euclidean line a.x + b.y + c = 0
 */
struct EuclideanLine {
  EuclideanLine(double a, double b, double c) : a(a), b(b), c(c) {}
  EuclideanLine(Point p1, Point p2)
  {
    a = p1.y - p2.y;
    b = p2.x - p1.x;
    c = p1.x * (p2.y - p1.y) + p1.y * (p1.x - p2.x);
  }
  double a, b, c;
};

std::vector<Point> pathBoundaryPoints(const Path & path, double strokeWidth, LineCap lineCap, LineJoin lineJoin, double miterLimit = 4.0);

Rect pathBoundingBox(const Path & path, double strokeWidth, LineCap lineCap, LineJoin lineJoin, double miterLimit = 4.0);

} // namespace Tools

} // namespace LibBoard

#endif /* PATH_BOUNDARIES_H */
