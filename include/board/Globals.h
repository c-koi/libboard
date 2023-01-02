/* -*- mode: c++ -*- */
/**
 * @file   Globals.h
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
#ifndef BOARD_GLOBALS_H
#define BOARD_GLOBALS_H

namespace LibBoard
{

/**
 * @brief The Direction enum
 */
enum class Direction
{
  Top,
  Right,
  Bottom,
  Left
};

/**
 * @brief The Alignment enum
 */
enum Alignment
{
  Top,
  Bottom,
  Center,
  Left,
  Right
};

/**
 * @brief Page sizes
 */
enum class PageSize
{
  BoundingBox = 0, //!< No page size. The bounding box of the drawing will be used.
  A0,
  A1,
  A2,
  A3,
  A4,
  A5,
  A6,
  A7,
  A8,
  A9,
  A10,
  Letter,
  Legal,
  Executive
};

/**
 * @brief Length units
 */
enum class Unit
{
  Point, //!< A point is 1/72 inch
  Inch,  //!< 25.4 mm
  Centimeter,
  Millimeter
};

/**
 * @brief The ColorSpecification enum
 */
enum class ColorSpecification
{
  PenColor,
  FillColor
};

} // namespace LibBoard

#endif /* BOARD_GLOBALS_H */
