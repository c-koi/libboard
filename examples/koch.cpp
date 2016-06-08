/**
 * @file   koch.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Draws a Koch fractal.
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
#include <vector>
#include <utility>
#include "Board.h"
#include "PathBoundaries.h"
using namespace std;
using namespace LibBoard;

void Koch( Polyline & curve, Point p1, Point p2, int depth ) {
  if ( depth > 0 ) {
    Point v = p2 - p1;
    Point a = p1 + ( v / 3.0 );
    Point b = p1 + 2 * ( v / 3.0 );
    Point c = b.rotated( 60 * Board::Degree, a );
    Koch( curve, p1, a, depth-1 );
    Koch( curve, a, c, depth-1 );
    Koch( curve, c, b, depth-1 );
    Koch( curve, b, p2, depth-1 );
  } else {
    curve << p2;
  }
}

int main( int , char *[] )
{
  Board board;
  Shape::disableLineWidthScaling();
  board.setLineWidth(0.0);
  board.setPenColor(Color::Null);

  Point a( -100, 0 );
  Point c( 100, 0 );
  Point b = c.rotated( 60 * Board::Degree, a );

  Polyline curve(true,Color::Null,Color::Green,0.0,Shape::SolidStyle,Shape::RoundCap,Shape::RoundJoin);

  // curve << a;  // Not required here.
  const int recursions = 5;
  Koch( curve, a, b, recursions );
  Koch( curve, b, c, recursions );
  Koch( curve, c, a, recursions );
  board << curve;

  board.saveFIG( "koch.fig", 200, 200 );
  board.saveEPS( "koch.eps", Board::A4 );

  board.scaleToWidth(25,Board::UseLineWidth);
  board.saveSVG( "koch.svg", Board::BoundingBox, 0.0, Board::UCentimeter );
}
