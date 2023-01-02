/* -*- mode: c++ -*- */
/**
 * @file   SketchFilter.h
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
#ifndef BOARD_SKETCH_FILTER_H
#define BOARD_SKETCH_FILTER_H

#include <board/Point.h>
#include <board/Shape.h>
#include <board/ShapeList.h>
#include <board/Types.h>
#include <tuple>
#include <vector>

/*
 * matplotlib.pyplot.xkcd
 *
 * https://shihn.ca/posts/2020/roughjs-algorithms/
 *
 * https://openaccess.city.ac.uk/id/eprint/1274/
 */

namespace LibBoard
{
struct Path;
struct Ellipse;

ShapeList makeRough(const Shape & shape, int repeat = 1, SketchFilling filling = PlainFilling, double hachureAngle = 0.0, double hachureSpacing = 0.0);

ShapeList hachuresLinesOrBezier(const std::vector<std::tuple<Point, Point>> & lines, Style style, SketchFilling type);

std::vector<std::tuple<Point, Point>> hachures(const Path & path, double spacing, double angle = 0.0, bool addHorizontals = false);

std::vector<std::tuple<Point, Point>> hachures(const Ellipse & ellipse, double spacing, double angle = 0.0);

ShapeList hachures(const Ellipse & ellipse, Style style, SketchFilling type, double spacing, double angle = 0.0);

ShapeList hachures(const Path & path, Style style, SketchFilling type, double spacing, double angle = 0.0, bool addHorizontals = false);

ShapeList hachures(const Path & path, SketchFilling type, Color color, double width, double spacing, double angle = 0.0);
} // namespace LibBoard

#endif /* BOARD_SKETCH_FILTER_H */
