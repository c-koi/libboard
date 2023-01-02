/**
 * @file   scale_ellipse.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample program to check that ellipses are correctly scaled.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
using namespace LibBoard;

int main(int, char * [])
{
  Board board;
  board.setLineWidth(0.25);

  Group g;
  g << rectangle(-30, 10, 60, 20, Color::Green, Color::White, 0.1);
  g << Ellipse(0, 0, 30, 10, Color::Red, Color::White, 0.1);
  g << Ellipse(0, 0, 2, 10, Color::Null, Color::Green, 0.1);

  board << g;
  board.addDuplicates(g, 5, 0, -15, 0.8, 0.8, 0.2);
  board.addDuplicates(g, 5, 0, 15, 0.8, 0.8, -0.2);
  board << g.scaled(0.2);

  board.saveEPS("scale_ellipse.eps", PageSize::A4);
  board.saveFIG("scale_ellipse.fig", PageSize::A4);

  board.scaleToWidth(25, UseLineWidth);
  board.saveSVG("scale_ellipse.svg", PageSize::BoundingBox, 0.0, Unit::Centimeter);
}
