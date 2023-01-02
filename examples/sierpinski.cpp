/**
 * @file   sierpinski.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Draws a Sierpinky triangle.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
using namespace LibBoard;

void Sierpinksi(Polyline & curve, Point p1, Point p2, int depth, int side)
{
  if (depth == 0) {
    curve << p2;
  } else {
    Point c = mix(p1, p2, 0.5);
    Point a = c.rotated(side * 60 * Board::Degree, p1);
    Point b = c.rotated(side * -60 * Board::Degree, p2);
    Sierpinksi(curve, p1, a, depth - 1, -1 * side);
    Sierpinksi(curve, a, b, depth - 1, side);
    Sierpinksi(curve, b, p2, depth - 1, -1 * side);
  }
}

int main(int, char *[])
{
  Board board;
  Point a(0, 0);
  Point b(1000, 0);
  Polyline curve(Path::Open, Color::gray(10), Color::Null, 0.1, SolidStyle, RoundCap, RoundJoin);
  curve << a;
  const int recursions = 11;
  Sierpinksi(curve, a, b, recursions, 1);
  board << curve;
  Tools::notice << curve.vertexCount() << " points in the curve after " << recursions << " recursions.\n";
  board.saveFIG("sierpinski.fig", 200, 200);
  board.saveEPS("sierpinski.eps", PageSize::A4);
  board.scaleToWidth(25, UseLineWidth);
  board.saveSVG("sierpinski.svg", PageSize::BoundingBox, 0.0, Unit::Centimeter);
}
