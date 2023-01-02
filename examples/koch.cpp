/**
 * @file   koch.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Draws a Koch fractal.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
using namespace LibBoard;

void Koch(Polyline & curve, Point p1, Point p2, int depth)
{
  if (depth > 0) {
    Point v = p2 - p1;
    Point a = p1 + (v / 3.0);
    Point b = p1 + 2 * (v / 3.0);
    Point c = b.rotated(60 * Board::Degree, a);
    Koch(curve, p1, a, depth - 1);
    Koch(curve, a, c, depth - 1);
    Koch(curve, c, b, depth - 1);
    Koch(curve, b, p2, depth - 1);
  } else {
    curve << p2;
  }
}

int main(int, char *[])
{
  Board board;
  Board::disableLineWidthScaling();
  board.setLineWidth(0.0);
  board.setPenColor(Color::Null);

  Point a(-100, 0);
  Point c(100, 0);
  Point b = c.rotated(60 * Board::Degree, a);

  Polyline curve(Path::Closed, Color::Null, Color::Green, 0.0, SolidStyle, RoundCap, RoundJoin);

  const int recursions = 5;
  Koch(curve, a, b, recursions);
  Koch(curve, b, c, recursions);
  Koch(curve, c, a, recursions);
  board << curve;

  Tools::notice << curve.vertexCount() << " points in the curve after " << recursions << " recursions.\n";

  board.saveFIG("koch.fig", 200, 200);
  board.saveEPS("koch.eps", PageSize::A4);

  board.scaleToWidth(25, UseLineWidth);
  board.saveSVG("koch.svg", PageSize::BoundingBox, 0.0, Unit::Centimeter);
}
