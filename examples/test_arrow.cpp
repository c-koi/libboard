/**
 * @file   test_arrow.cpp
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
  Style::setDefaultLineWidth(1.0);

  board << Arrow(Point(0, 0), Point(100, 0), Arrow::Stick);
  board << board.last<Arrow>().rotated(180 * Board::Degree).translated(-30, 10);
  board << Arrow(Point(0, 50), Point(100, 50), Arrow::Closed);
  board << board.last<Arrow>().rotated(180 * Board::Degree).translated(-30, 10);
  board << Arrow(Point(0, 100), Point(100, 100), Arrow::Plain);
  board << board.last<Arrow>().rotated(180 * Board::Degree).translated(-30, 10);

  board.saveSVG("test_arrow.svg", Board::PageSize::BoundingBox);
  // board.saveEPS("test_arrow.eps", Board::PageSize::BoundingBox);
  // board.saveFIG("test_arrow.fig", Board::PageSize::BoundingBox);
  // system("svgviewer test_arrow.svg");
}
