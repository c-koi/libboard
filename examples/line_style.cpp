/**
 * @file   line_style.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample program to check line styles.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 *
 */
#include <Board.h>
using namespace LibBoard;

int main(int, char *[])
{
  Board board;
  board.clear(Color::White);
  board.setLineWidth(0.5);

  int y = 10;

  board.setFillColor(Color::Gray);
  board.setLineStyle(DashStyle);
  board.drawRectangle(10, y, 200, 10);
  y += 20;
  Style style = board.style();
  board << rectangle(10, y, 200, 10, style.withLineStyle(DotStyle));
  y += 20;
  board << rectangle(10, y, 200, 10, style.withLineStyle(DashDotStyle));
  y += 20;
  board << rectangle(10, y, 200, 10, Color::Black, Color::Gray, 0.1, DashDotDotStyle);
  y += 20;
  board.setLineStyle(DashDotDotDotStyle);
  //  board.drawArrow( 10, y, 200, y );
  board << rectangle(10, y, 200, 10, Color::Black, Color::Gray, 0.1, DashDotDotStyle);
  y += 20;
  board.setLineStyle(SolidStyle);
  // board.drawArrow( 10, y, 200, y );
  board << rectangle(10, y, 200, 10, Color::Black, Color::Red, 0.1, SolidStyle);
  y += 20;

  board.saveEPS("line_style.eps");
  board.saveFIG("line_style.fig");

  board.scaleToWidth(25, UseLineWidth);
  board.saveSVG("line_style.svg", PageSize::BoundingBox, 0.0, Unit::Centimeter);
}
