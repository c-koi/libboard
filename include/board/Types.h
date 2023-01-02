/* -*- mode: c++ -*- */
/**
 * @file   Types.h
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
#ifndef BOARD_TYPES_H
#define BOARD_TYPES_H

namespace LibBoard
{

enum SketchFilling
{
  NoFilling,
  PlainFilling,
  StraightHachure,
  CrossingHachure,
  SketchyHachure,
  SketchyCrossingHachure // TODO : Add double crossing hachures
};

} // namespace LibBoard

#endif /* BOARD_TYPES_H */
