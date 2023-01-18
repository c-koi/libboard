/**
 * @file   circled_framed.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample drawing using the board library.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <cstdlib>
#include <iostream>
using namespace LibBoard;

int main(int, char *[])
{
  Board board;
  const double TEXT_SIZE = 6.0;
  Style::setDefaultLineWidth(0.5);
  const double TEXT_LINEWIDTH = 0.33 * Style::defaultLineWidth();
  board << circled(boardFontText(Point(), "Hello World!", TEXT_SIZE, Color::Black, TEXT_LINEWIDTH), 2.0);
  board << tiling(circled(boardFontText(Point(), "C", TEXT_SIZE, Color::Black, TEXT_LINEWIDTH), //
                          2.0, Color::Red, Color("#5effa9")),                                   //
                  board.bbox(UseLineWidth).topRight(),                                          //
                  5, 5, 2);
  board << tiling(framed(boardFontText(Point(), "R", TEXT_SIZE, Color::Black, TEXT_LINEWIDTH), //
                         2.0, Color::Red, Color("#5effa9")),                                   //
                  board.bbox(UseLineWidth).topRight(), 5, 5, 2);
  board.saveSVG("circled_framed.svg");
}
