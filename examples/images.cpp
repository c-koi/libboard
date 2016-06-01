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
  board << Board::UCentimeter;  //  Or: board.setUnit(Board::UCentimeter);

  Group g;
  Image michel("../resources/saint_michel.jpg",0,0,5);
  Rectangle rectangle( michel.boundingBox(), Color::Red, Color::Silver,2);
  g << rectangle;
  g << michel;

  board << g;

  //  ShapeList row;
  //  row.repeat(g,4,g.boundingBox().width*1.1,0);
  //  ShapeList page;
  //  page.repeat(row,4,0,g.boundingBox().height);
  //  board << page;

  //  Point c = page.boundingBox().center();
  //  Image avatar("../resources/avatar.png",c.x,c.y,3);
  //  board << avatar;
  //  board << avatar.rotated(M_PI_2,c);
  //  board << avatar.rotated(M_PI,c);
  //  board << avatar.rotated(3*M_PI_2,c);



  board.saveEPS( "images.eps" /*, Board::A4 */ );
  board.saveFIG( "images.fig" /*, Board::A4 */ );
  board.saveSVG( "images.svg" , Board::A4  );

  exit(0);
}
