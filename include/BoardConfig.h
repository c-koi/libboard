/* -*- mode: c++ -*- */
/**
 * @file   Board.h
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Declaration of the Board class.
 *
 * \@copyright
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr/>
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
#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#define _BOARD_HAVE_MAGICKPLUSPLUS_ 1

#define _BOARD_WIN32_ 0

#define _BOARD_VERSION_ 0.9.5

#define BOARD_STRINGIFY( X ) # X
#define BOARD_XSTRINGIFY( X ) BOARD_STRINGIFY( X )

#define _BOARD_VERSION_STRING_  BOARD_XSTRINGIFY( _BOARD_VERSION_ )

#if __cplusplus<201100
#define BOARD_NOEXCEPT throw()
#else
#define BOARD_NOEXCEPT noexcept
#endif

#endif
