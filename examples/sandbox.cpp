/**
 * @file   example4.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample drawing using the board library.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>

using namespace LibBoard;

int main(int, char *[])
{
  Board board;
  Polyline p = rectangle(0, 0, 10, 4, Color::Pink);
  Point corner = p.boundingBox(IgnoreLineWidth).bottomLeft();
  for (float a = 0; a < 200; a += 10) {
    p = p.translated(10, 0).rotated(10 * Board::Degree);
    board << p;
  }
  board << cross(corner);
  board.saveSVG("sandbox.svg");
  // system("svgviewer sandbox.svg");
}
