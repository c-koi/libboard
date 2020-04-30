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
#include "board/BoardFontText.h"
#include "board/Debug.h"
using namespace LibBoard;

Group space_invader()
{
  Group g;
  const Color b = Color::Black;
  const Color w = Color::White;
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
  g << array(Point(0, 0), v, 13, 10, 4, 4, 0.01);
  return g;
}

int main(int, char *[])
{
  Board board;
  Board::disableLineWidthScaling();
  board.setLineWidth(1.0);
  board.append(space_invader(), Board::Right, Board::AlignCenter);
  board.saveSVG("array.svg", Board::BoundingBox);
}
