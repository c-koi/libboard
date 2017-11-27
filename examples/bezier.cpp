/**
 * @file   bezier.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Draws a Koch fractal.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 */
#include "Board.h"
#include <cstdlib>
using namespace LibBoard;

int main( int , char *[] )
{
  Board board;
  Shape::disableLineWidthScaling();

  board.setLineWidth(1.0);
  board.setPenColor(Color::Blue);
  board.setFillColor(Color::Red);
  board << Bezier( Point(10,10), Point(10,10) + Point(5,5),
                   Point(30,10), Point(30,10) + Point(-5,5),
                   Color::Red ).rotated(45);

  Bezier b( Point(0,0), Point(0,0) + Point(0,20),
            Point(-30,0), Point(-30,0) + Point(0,-15),
            Color::Green,
            Color::Null,
            1.0);

  ShapeList l;
  l << b << Rectangle(b.boundingBox(Board::UseLineWidth),Color::Black,Color::Null,1.0);

  board << l.rotated(45);
  board << l.rotated(45).translated(100,0).scaled(3);
  board << l.rotated(45).translated(200,0).scaled(3,1);

  board << Polyline(b.discretizedPath()).translated(300,100);

  board.saveSVG( "bezier.svg", Board::BoundingBox );
  board.saveEPS( "bezier.eps", Board::BoundingBox );
}
