/* -*- mode: c++ -*- */
/**
 * @file   Debug.h
 * @author Sebastien Fourey (GREYC)
 * @date   Nov 2017
 *
 * @brief  The Exception type.
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
#ifndef BOARD_DEBUG_H
#define BOARD_DEBUG_H

#ifdef _BOARD_DEBUG_
#define ENTERING std::cout << "[" << __PRETTY_FUNCTION__ << "] <<Entering>>" << std::endl;
#define LEAVING std::cout << "[" << __PRETTY_FUNCTION__ << "] <<Leaving>>" << std::endl;
#define TRACE std::cout << "[" << __PRETTY_FUNCTION__ << "]" << std::endl;
#define TSHOW(V) std::cout << "[" << __PRETTY_FUNCTION__ << ":" << __LINE__ << "] " << #V << " = " << (V) << std::endl;
#define LSHOW(V) std::cout << "[" << __FILE__ << ":" << __LINE__ << "] " << #V << " = " << (V) << std::endl;
#define MSHOW(MESSAGE, V) std::cout << "[" << __FILE__ << ":" << __LINE__ << "] (" << (MESSAGE) << ") " << #V << " = " << (V) << std::endl;
#define SHOW(V) std::cout << #V << " = " << (V) << std::endl;
#else
#define ENTERING while (false)
#define LEAVING while (false)
#define TRACE                                                                                                                                                                                          \
  while (false)                                                                                                                                                                                        \
  std::cerr << ""
#define TSHOW(V)                                                                                                                                                                                       \
  while (false)                                                                                                                                                                                        \
  std::cerr << ""
#define LSHOW(V)                                                                                                                                                                                       \
  while (false)                                                                                                                                                                                        \
  std::cerr << ""
#define MSHOW(MESSAGE, V)                                                                                                                                                                              \
  while (false)                                                                                                                                                                                        \
  std::cerr << ""
#define SHOW(V)                                                                                                                                                                                        \
  while (false)                                                                                                                                                                                        \
  std::cerr << ""
#endif

#endif // BOARD_EXCEPTION_H
