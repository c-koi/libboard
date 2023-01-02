/* -*- mode: c++ -*- */
/**
 * @file   BoardFontText.h
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
 *
 * @brief  Group class
 *
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
#ifndef BOARD_BOARD_FONT_TEXT_H
#define BOARD_BOARD_FONT_TEXT_H

#include <string>
#include <board/Group.h>

namespace LibBoard
{

/**
 * @brief Create a text with board's font
 * @param baselineStart
 * @param text
 * @param size
 * @param penColor
 * @param lineWidth
 * @return The text as a group
 */
Group boardFontText(Point baselineStart, const std::string & text, double size, Color penColor = Style::defaultPenColor(), double lineWidth = 0.0);

} // namespace LibBoard

#endif /* BOARD_BOARD_FONT_TEXT_H */
