/**
 * @file   fern.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Draws a Fern fractal.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 */
#include <cstdlib>
#include "Board.h"
using namespace LibBoard;

void drawFern(Board & b, unsigned int iterations)
{
  double x = 0.0;
  double y = 0.0;
  double nx, ny;
  double radius = 2.0;

  while (iterations--) {
    b.drawCircle(200 * x, 200 * y, radius);
    double g = rand() / static_cast<double>(RAND_MAX);
    if (g < .01) { // Stem
      nx = 0;
      ny = 0.16 * y;
    } else if (g < .08) { // F3 : mirror & flip
      nx = 0.2 * x - 0.26 * y;
      ny = 0.23 * x + 0.22 * y + 1.6;
    } else if (g < 0.15) { // F4 : mirror (no flip)
      nx = -0.15 * x + 0.28 * y;
      ny = 0.26 * x + 0.24 * y + 0.44;
    } else { // F2 : mirror & scale
      radius /= 2.0;
      nx = 0.85 * x + 0.04 * y;
      ny = -0.04 * x + 0.85 * y + 1.6;
    }
    x = nx;
    y = ny;
  }
}

int main(int, char *[])
{
  srand(static_cast<unsigned int>(time(nullptr)));
  Board board;
  Board::disableLineWidthScaling();
  board.setLineWidth(0.0);
  board.setPenColor(Color::Null);
  board.setFillColor(Color::Red);
  drawFern(board, 1 << 17);
  board.saveEPS("fern.eps", Board::A4);
  board.saveSVG("fern.svg", Board::A4);
}
