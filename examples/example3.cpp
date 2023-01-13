/**
 * @file   example3.cpp
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

  board.clear(Color(0, 120, 0));
  board.setPenColorRGBi(255, 0, 0);

  board.fillGouraudTriangle(-50, 100, Color(255, 0, 0), 0, 100, Color(0, 255, 0), -30, 130, Color(0, 120, 255));

  board.saveEPS("example3.eps", 210, 297);
  board.saveEPS("example3_15x10.eps", 210, 297, 25);
  board.saveFIG("example3.fig");
  board.saveFIG("example3_A4.fig", PageSize::A4);

  // Viewport == BoundingBox
  board.saveSVG("example3.svg");

  // Centered on an A4 paper, with a 50mm margin.
  board.saveSVG("example3_A4.svg", PageSize::A4, 50);

  board.scale(10);
  board.saveSVG("example3_x10.svg");
  board.saveFIG("example3_x10.fig");
}
