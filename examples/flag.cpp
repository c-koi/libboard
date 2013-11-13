/**
 * @file   flag.cpp
 * @author Sebastien Fourey (GREYC)
 * 
 * @brief  Sample drawing using the board library, demonstrates
 *         the clipping capacity for both SVG and EPS files.
 * 
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 */
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include "Board.h"
#include <vector>
using namespace LibBoard;

const int RAYS = 40;

int main( int, char *[] )
{
  Board board;

  board << Circle( 0, 0, 40, Color::None, Color::Red, 0.0 );

  Line line( 0, 45, 0, 400, Color::Black, 1.0 );
  Line rline(line);
  const double angle = 2 * M_PI / RAYS;
  int n = RAYS;
  while ( n-- ) { 
    board << rline;
    rline.rotate( angle, Point(0,0)  );
  }
  
  board.setClippingRectangle( -100, 100, 350, 250 );
  board.saveEPS( "flag.eps", 210, 297 );
  board.saveSVG( "flag.svg" );                        // Viewport == BoundingBox
  board.saveTikZ( "flag.tikz" );
  exit(0);
}
