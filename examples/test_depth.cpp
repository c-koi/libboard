/**
 * @file   test_depth.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Illustrate the rough shape filter
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <ctime>
using namespace LibBoard;

int main(int, char *[])
{
  Tools::initBoardRand(static_cast<unsigned int>(time(nullptr)));
  Board board;
  Board::disableLineWidthScaling();

  Style::setDefaultPenColor(Color::Red);
  Style::setDefaultFillColor(Color::Green);
  Style::setDefaultLineWidth(0.5);

  Polyline r = rectangle(0, 10, 10, 10);
  board << tiling(r, Point(0, 100), 4, 4, 0.0, IgnoreLineWidth);
  board << r.translated(5, 5) << r.translated(8, 8) << r.translated(9, 9);

  board.saveSVG("test_depth.svg", PageSize::BoundingBox);
  // board.saveFIG("depth.fig", PageSize::BoundingBox);
  // system("svgviewer depth.svg");
  // system("xfig depth.fig");
}
