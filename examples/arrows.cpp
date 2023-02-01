/**
 * @file   arrows.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample program to check that arrows are correctly drawn.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 *
 */
#include <Board.h>
#include <board/Tools.h>
#include <sstream>
using namespace LibBoard;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float random_gray()
{
  return Tools::boardRand() / static_cast<float>(RAND_MAX);
}

int main(int, char *[])
{
  Board board;
  board.clear(Color(233, 250, 140));
  board.setLineWidth(0.25);
  Tools::initBoardRand(static_cast<unsigned int>(time(nullptr)));

  double angle = 0.0;
  board << LibBoard::rectangle(-8, 12, 16, 24, Color::Black, Color::Null, 0.1);
  double radius = 10;

  board.setFont(Fonts::CourierBold, 2);
  for (int i = 0; i < 45; ++i) {
    angle = i * (2 * M_PI / 45);

    board.setPenColorRGBf(random_gray(), random_gray(), random_gray());
    board.setFillColor(board.penColor());
    board.drawArrow(0, 0, radius * cos(angle), radius * sin(angle));
    radius += 0.4;

    board.setLineWidth(0.1);
    board.setPenColorRGBi(255, 0, 0);
    board.drawDot(radius * cos(angle), radius * sin(angle));
    std::stringstream s;
    s << i;
    board.drawText(radius * cos(angle), radius * sin(angle), s.str());
  }

  board.setPenColorRGBi(0, 0, 0);
  board.setFont(Fonts::PalatinoRoman, 24);
  board.drawText(0, 15, "Arrows");

  board << LibBoard::rectangle(board.boundingBox(IgnoreLineWidth));

  Rect rect = board.last<Polyline>().boundingBox(UseLineWidth);
  board.setPenColor(Color::Red);
  board.setFillColor(Color::Green);
  board.setLineWidth(1);
  board.drawArrow(rect.topLeft(), rect.bottomRight());

  Rect rect2(10, 90, 40, 40);
  board.drawArrow(rect2.topLeft(), rect2.topRight());
  board.drawArrow(rect2.topRight(), rect2.bottomRight());
  board.drawArrow(rect2.bottomRight(), rect2.bottomLeft());
  board.drawArrow(rect2.bottomLeft(), rect2.topLeft());

  // board.saveEPS( "arrows_A4.eps", PageSize::A4, 2.0, Unit::Centimeter );
  // board.saveSVG( "arrows_A4.svg", PageSize::A4 );
  // board.saveFIG( "arrows.fig" );

  board.scaleToWidth(20, UseLineWidth);
  board.saveSVG("arrows.svg", PageSize::BoundingBox, 0.0, Unit::Centimeter);
  board.saveEPS("arrows.eps", PageSize::BoundingBox, 0.0, Unit::Centimeter);
  board.saveFIG("arrows.fig", PageSize::BoundingBox, 0.0, Unit::Centimeter);
  // system("display arrows.svg");
}
