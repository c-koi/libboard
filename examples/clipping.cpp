/**
 * @file   clipping.cpp
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
  Group g;
  Path clip = {Point(-10, 5), Point(0, 5), Point(10, 0), Point(0, -5), Point(-10, -5)};
  g.setClippingPath(clip);
  g << circle(0, 0, 10, Color::Black, Color::Red, 1.0);

  board.addDuplicates(g, 10, 18, 0, 0.9, 1.05);
  board.addDuplicates(g, 10, 18, -18, 1.2, 1);

  Group cross;
  clip.clear();
  clip << Point(5, 15);
  clip << Point(5, 5);
  clip << Point(15, 5);
  clip << Point(15, -5);
  clip << Point(5, -5);
  clip << Point(5, -15);
  clip << Point(-5, -15);
  clip << Point(-5, -5);
  clip << Point(-15, -5);
  clip << Point(-15, 5);
  clip << Point(-5, 5);
  clip << Point(-5, 15);

  cross.setClippingPath(clip);

  Ellipse cropedC = LibBoard::circle(0, 0, 10, Color::Black, Color(100, 255, 100), 1.0);
  cross << cropedC;

  board << cross.scaled(3);
  board.addDuplicates(cross.translated(0, -60).scaled(2), 18, 0, 0, 0.85, 0.85, 0.1);

  board.saveEPS("clipping.eps", 210, 297);

  // Centered on an A4 paper with a 50mm margin.
  board.saveSVG("clipping_A4.svg", PageSize::A4, 50);

  // Viewport == BoundingBox (default)
  board.scaleToWidth(20, UseLineWidth);
  board.saveSVG("clipping.svg", PageSize::BoundingBox, 0.0, Unit::Centimeter);
}
