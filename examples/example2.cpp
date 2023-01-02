/**
 * @file   example2.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample drawing using the board library.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <cmath>
#include <Board.h>
using namespace LibBoard;

int main(int, char *[])
{
  Board board;
  board.setPenColorRGBi(255, 0, 0);

  std::vector<Point> points;
  for (double x = -1.0; x < 1.0; x += 0.01) {
    points.push_back(Point(100 * x, 100.0 * sin(2 * M_PI * x)));
  }
  board.setLineWidth(0.5);
  board.setPenColorRGBi(0, 0, 255);
  board.drawPolyline(points);

  board.setPenColorRGBi(255, 0, 0);
  for (double x = -100.0; x <= 100.0; x += 10.0) {
    board.setFillColorRGBi(255, 0, 0);
    board.setPenColorRGBi(0, 0, 255);
    board.drawCircle(x, 0.0, 10);
  }
  board.fillCircle(0.0, 0.0, 10);

  board.setPenColorRGBi(0, 0, 0).setFillColorRGBf(0.5f, 1.0f, 0, 0.25);
  board.drawEllipse(50, -50, 40.0, 15.0);

  board.setLineJoin(RoundJoin);
  board.setLineWidth(0.5).setPenColor(Color::Black).setFillColorRGBi(0, 0, 255, 50);
  board.drawRectangle(-100.0, -50.0, 100.0, 25.0);

  board.setLineJoin(MiterJoin).setLineWidth(0.5).setPenColor(Color::Red);
  board.drawTriangle(-50.0, 50.0, 0.0, 50.0, 0.0, 0.0);

  board.setPenColor(Color::Black).setFont(Fonts::HelveticaBold, 12.0);
  board.drawText(0.0, 100.0, "Hello world!");

  Point p = board.last<Text>().boundingBox(IgnoreLineWidth).bottomLeft();

  board << board.last<Text>().rotated(45 * Board::Degree, p);
  board << board.last<Text>().rotated(45 * Board::Degree, p);
  board << board.last<Text>().rotated(45 * Board::Degree, p);
  board << board.last<Text>().rotated(45 * Board::Degree, p);

  board.disableLineWidthScaling();
  board.saveEPS("example2_A4.eps", PageSize::A4);
  board.saveFIG("example2_A4.fig", PageSize::A4);
  board.saveSVG("example2_A4.svg", PageSize::A4);
  board.saveSVG("example2.svg");
}
