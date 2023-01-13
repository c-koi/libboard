/**
 * @file   ruler.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample program that draws a ruler.
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
  board.clear(Color::White);
  board.setLineWidth(0.01);

  // board << Rectangle( 0, 27.7, 19, 27.7, Color::Black, Color::None, 0.1 );

  Line tiny(0, 0, 0.2, 0, Color::Black, 0.01);
  Line small(0.2, 0, 0.7, 0, Color::Black, 0.01);
  Line large(0.7, 0, 1.5, 0, Color::Black, 0.01);

  // Centimeters
  board.addDuplicates(small, 201, 0, 0.1);
  board.addDuplicates(large, 21, 0, 1);
  board.setFontSize(2);
  board.drawText(board.last<Line>().boundingBox(UseLineWidth).topRight(), "Centimeters");

  // Inches
  tiny.translate(15, 0);
  small.translate(15, 0);
  large.translate(15, 0);
  board.addDuplicates(tiny, 41, 0, 2.54 / 4);
  board.addDuplicates(small, 21, 0, 2.54 / 2);
  board.addDuplicates(large, 11, 0, 2.54);

  board << Text(board.last<Line>().boundingBox(UseLineWidth).topRight(), "Inches", Fonts::CourierBold, 2, Color::Red);

  board.saveEPS("ruler.eps" /*, PageSize::A4 */);
  board.saveFIG("ruler.fig" /*, PageSize::A4 */);

  board.scaleToWidth(25, UseLineWidth);
  board.saveSVG("ruler.svg", PageSize::BoundingBox, 0.0, Unit::Centimeter);
}
