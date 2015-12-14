/**
 * @file   tilings.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample program that draws a ruler.
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
  board.clear( Color::White);
  board << Board::UPoint;

  Group g;
  g << Circle(10,10,20,Color::Blue,Color::None,1.0);

  Group grid;
  grid << Line(0,0,100,0,Color::Blue,1.0,Shape::SolidStyle,Shape::RoundCap,Shape::RoundJoin);
  grid << Line(0,0,0,-200,Color::Blue,1.0,Shape::SolidStyle,Shape::RoundCap,Shape::RoundJoin);

  Polyline l(false,Color::Blue,Color::None,1.0,Shape::SolidStyle,Shape::RoundCap,Shape::RoundJoin);
  l << Point(-10,0) << Point(0,0) << Point(0,10);
  grid.addTiling(l,Point(0,0),10,20);

  grid.rotateDeg(45);
  board << grid;

  Circle circle(10,10,20,Color::Blue,Color::None,1.0);
  board.append( circle, ShapeList::Right, ShapeList::AlignTop);
  board.append( circle, ShapeList::Right, ShapeList::AlignCenter);
  board.append( circle, ShapeList::Right, ShapeList::AlignBottom);
  board.append( circle, ShapeList::Bottom, ShapeList::AlignLeft);
  board.append( circle, ShapeList::Bottom, ShapeList::AlignCenter);
  board.append( circle, ShapeList::Bottom, ShapeList::AlignRight);

  board.append( Board::makeGrid(Point(0,0),12,6,120,60,Color::Red,Color::None,0.5,Shape::SolidStyle,Shape::ButtCap,Shape::RoundJoin),
                ShapeList::Right, ShapeList::AlignCenter );

  board.saveEPS( "tilings.eps" /*, Board::A4 */ );
  board.saveSVG( "tilings.svg" /*, Board::A4 */ );
  exit(0);
}
