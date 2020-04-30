/**
 * @file   rough.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Illustrate the rough shape filter
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 */
#include <cstdlib>
#include <ctime>
#include "Board.h"
#include "board/RoughVisitor.h"
using namespace LibBoard;

int main(int, char *[])
{
  srand(static_cast<unsigned int>(time(nullptr)));
  Board board;
  Board::disableLineWidthScaling();

  board.setLineWidth(1.0);
  board.setPenColor(Color::Blue);
  board.setFillColor(Color::Red);
  Style::setDefaultPenColor(Color::Red);
  Style::setDefaultFillColor(Color::Green);
  Style::setDefaultLineWidth(0.5);

  Polyline r = rectangle(0, 10, 10, 10);

  Group g;
  g.addTiling(r, Point(0, 100), 4, 4, 0.0, IgnoreLineWidth);

  board << g;
  // board << g << g.translated(36, -6) << g.translated(30, -25);

  board << r.translated(5, 5) << r.translated(8, 8) << r.translated(9, 9);

  board.saveSVG("depth.svg", Board::BoundingBox);
  board.saveFIG("depth.fig", Board::BoundingBox);

  // system("svgviewer depth.svg");
  system("xfig depth.fig");
}
