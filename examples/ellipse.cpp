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
#include <iostream>

using namespace LibBoard;

int main(int, char *[])
{
  Board board;
  board.clear(Color(200, 255, 200));
  Group g;
  g << LibBoard::rectangle(0, 50, 100, 50, Color::Black, Color::Null, 1);
  g << Line(0, 25, 100, 25, Color(0, 0, 255), 1);
  g << Ellipse(50, 25, 50, 25, Color::Red, Color::Null, 1);
  g.translate(30, 30).rotate(110 * Board::Degree).scale(1.5, 2);
  Rect bbox = g.last<Ellipse>().boundingBox(UseLineWidth);
  board << g;
  board << rectangle(bbox, Color::Purple, Color::Null, 1.0);
  std::cerr << board.boundingBox(UseLineWidth) << std::endl;
  board.saveEPS("ellipse.eps");
  board.saveFIG("ellipse.fig");
  board.saveSVG("ellipse.svg");
}
