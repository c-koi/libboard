/**
 * @file   ruler.cpp
 * @author Sebastien Fourey (GREYC)
 * 
 * @brief  Sample program that draws a ruler.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 */
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <string>
#include "Board.h"
using namespace LibBoard;

int main( int , char *[] )
{
  Board board;
  board.clear( Color::Navy );
  board.setLineWidth(1);

  board << Board::UCentimeter;

  Group g;
  g << Rectangle( 30,30,30,20, Color::Red, Color::Silver,2);
  g << Image("../resources/saint_michel.jpg",32,28,30);
  ShapeList row(g,4,35,0,1.0,1.0,0.0);
  ShapeList page(row,4,0,25,1.0,1.0,0.0);

  board << page;

  Point c = page.boundingBox().center();

  board << Image("../resources/avatar.png",c.x,c.y,20);

  board << Image("../resources/avatar.png",c.x,c.y,20).rotated(M_PI_2,c);
  board << Image("../resources/avatar.png",c.x,c.y,20).rotated(M_PI,c);
  board << Image("../resources/avatar.png",c.x,c.y,20).rotated(3*M_PI_2,c);

  board.saveEPS( "images.eps" /*, Board::A4 */ );
  board.saveFIG( "images.fig" /*, Board::A4 */ );
  board.saveSVG( "images.svg" /*, Board::A4 */ );

  exit(0);
}
