/**
 * @file   example1.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample drawing using the board library.
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
#include "PathBoundaries.h"
using namespace LibBoard;

int main( int, char *[] )
{
  Board board;

  // Default unit is the Postscript dot (1/72 inch).
  board.setLineWidth( 0.1 );
  board.setPenColorRGBi( 0, 0, 255 );
  std::vector<Point> points;
  for ( double x = -1.0; x <= 1.0; x += 0.01 ) {
    points.push_back( 100 * Point( x, sin(2*x*M_PI) ) );
  }
  board.setPenColorRGBi( 0, 0, 255 );
  board.setLineWidth(0.1);
  board.drawPolyline( points );

  Polyline p = board.last<Polyline>();

  board.setPenColor(Color::Black);
  board.drawArrow( board.center(), board.boundingBox().topRight(), true );

  board.fillGouraudTriangle( -50, 0, Color( 255, 0, 0 ),
                             0, 0, Color( 0, 255, 0 ),
                             -30, 30, Color( 0, 0, 255 ) );

  GouraudTriangle t = board.last<GouraudTriangle>();

  board.setPenColorRGBi( 255, 0, 0 );
  board.fillGouraudTriangle( 100, 0, 0.1f,
                             50, 0, 0.8f,
                             75, -30, 0.1f );
  board.setPenColorRGBi( 255, 255, 255 );
  board.fillGouraudTriangle( 100, 0, 0.1f,
                             50, 0, 0.8f,
                             75, 50, 0.1f );

  Board::disableLineWidthScaling();
  board.saveEPS( "draw1.eps" );
  board.saveFIG( "draw1.fig" );
  board.saveSVG( "draw1.svg" );
  board.saveEPS( "draw1_Letter.eps", Board::Letter );
  board.saveFIG( "draw1_Letter.fig", Board::Letter );
  board.saveSVG( "draw1_Letter.svg", Board::Letter );
  exit(0);
}
