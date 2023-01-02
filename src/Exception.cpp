/* -*- mode: c++ -*- */
/**
 * @file   Exception.cpp
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
#include <board/Exception.h>

namespace LibBoard
{

Exception::Exception() noexcept {}

Exception::Exception(const char * what) noexcept : _what(what) {}

Exception::Exception(const std::string & what) noexcept : _what(what) {}

Exception::~Exception() noexcept {}

const char * Exception::what() const noexcept
{
  return _what.c_str();
}
} // namespace LibBoard
