/* -*- mode: c++ -*- */
/**
 * @file   Exception.h
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
#ifndef BOARD_EXCEPTION_H
#define BOARD_EXCEPTION_H

#include <stdexcept>
#include <string>
#include "BoardConfig.h"

namespace LibBoard
{

class Exception : public std::exception {
public:
  Exception() noexcept;
  Exception(const char * what) noexcept;
  Exception(const std::string & what) noexcept;
  Exception(const Exception &) = default;
  ~Exception() noexcept;
  const char * what() const noexcept;

private:
  std::string _what;
};
} // namespace LibBoard

#endif // BOARD_EXCEPTION_H
