/**
 * @file   array.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Illustrate the rough shape filter
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
using namespace LibBoard;

Group space_invader()
{
  const Color b = Color::White;
  const Color w = Color::Green;
  std::vector<Color> v = {
      b, b, b, b, b, b, b, b, b, b, b, b, b, //
      b, b, b, w, b, b, b, b, b, w, b, b, b, //
      b, b, b, b, w, b, b, b, w, b, b, b, b, //
      b, b, b, w, w, w, w, w, w, w, b, b, b, //
      b, b, w, w, b, w, w, w, b, w, w, b, b, //
      b, w, w, w, w, w, w, w, w, w, w, w, b, //
      b, w, b, w, w, w, w, w, w, w, b, w, b, //
      b, w, b, w, b, b, b, b, b, w, b, w, b, //
      b, b, b, b, w, w, b, w, w, b, b, b, b, //
      b, b, b, b, b, b, b, b, b, b, b, b, b, //
  };
  return array(Point(0, 0), v, 13, 10, 4, 4, 0.1);
}

int main(int, char *[])
{
  Board board;
  Board::disableLineWidthScaling();
  board.setLineWidth(1.0);
  board << space_invader();
  board.append(makeRough(board.last<Group>(), 2, LibBoard::SketchyHachure, 10.0 * Board::Degree));
  board.saveSVG("array.svg", PageSize::BoundingBox);
  // system("svgviewer array.svg");
}
