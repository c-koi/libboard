/**
 * @file   example1.cpp
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
#include <vector>
using namespace LibBoard;

int main(int, char *[])
{
  Board board;
  board.setLineWidth(0.1);
  board.setPenColorRGBi(0, 0, 255);
  std::vector<Point> points;
  for (double x = -1.0; x <= 1.0; x += 0.01) {
    points.push_back(100 * Point(x, sin(2 * x * M_PI)));
  }
  board.setPenColorRGBi(0, 0, 255);
  board.setLineWidth(0.1);
  board.drawPolyline(points);

  board.setPenColor(Color::Black);
  board.drawArrow(board.center(), board.boundingBox(UseLineWidth).topRight());
  board.fillGouraudTriangle(-50, 0, Color(255, 0, 0), //
                            0, 0, Color(0, 255, 0),   //
                            -30, 30, Color(0, 0, 255));

  board.setPenColorRGBi(255, 0, 0);
  board.fillGouraudTriangle(100, 0, 0.1f, 50, 0, 0.8f, 75, -30, 0.1f);
  board.setPenColorRGBi(255, 255, 255);
  board.fillGouraudTriangle(100, 0, 0.1f, 50, 0, 0.8f, 75, 50, 0.1f);

  Board::disableLineWidthScaling();
  board.saveEPS("example1.eps");
  board.saveFIG("example1.fig");
  board.saveSVG("example1.svg");
  board.saveEPS("example1_Letter.eps", PageSize::Letter);
  board.saveFIG("example1_Letter.fig", PageSize::Letter);
  board.saveSVG("example1_Letter.svg", PageSize::Letter);
}
