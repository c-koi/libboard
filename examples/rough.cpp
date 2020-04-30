/**
 * @file   rough.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample program to illustrate the rough shape filter
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 *
 */
#include "Board.h"
using namespace LibBoard;

int main(int, char *[])
{
  Board board;
  board.clear(Color::White);
  board.setLineWidth(0.5);

  int y = 10;

  board.setFillColor(Color::Gray);

  board.setLineStyle(Shape::DashStyle);
  board.drawRectangle(10, y, 200, 10);

  board.saveSVG("rough.svg", Board::BoundingBox, 0.0, Board::UCentimeter);
}
