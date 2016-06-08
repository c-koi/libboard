/**
 * @file   linestyle.cpp
 * @author Sebastien Fourey (GREYC)
 * 
 * @brief  Sample program to check line styles.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 *
 */
#include <cstdlib>
#include <iostream>
#include "Board.h"
using namespace LibBoard;

int main( int , char *[] )
{
  Board board;
  board.clear( Color::White );
  board.setLineWidth(0.5);

  int y = 10;

  board.setFillColor( Color::Gray );

  board.setLineStyle( Shape::DashStyle );
  board.drawRectangle( 10, y, 200, 10 );
  y+=20;
  board.setLineStyle( Shape::DotStyle );
  board.drawRectangle( 10, y, 200, 10 );
  y+=20;
  board.setLineStyle( Shape::DashDotStyle );
  board.drawRectangle( 10, y, 200, 10 );
  y+=20;
  // board.setLineStyle( Shape::DashDotDotStyle );
  board << Rectangle( 10, y, 200, 10, Color::Black, Color::Gray, 0.1, Shape::DashDotDotStyle );
  y+=20;
  board.setLineStyle( Shape::DashDotDotDotStyle );
  //  board.drawArrow( 10, y, 200, y );
  board << Rectangle( 10, y, 200, 10, Color::Black, Color::Gray, 0.1, Shape::DashDotDotStyle );
  y+=20;
  board.setLineStyle( Shape::SolidStyle );
  // board.drawArrow( 10, y, 200, y );
  board << Rectangle( 10, y, 200, 10, Color::Black, Color::Red, 0.1, Shape::SolidStyle );
  y+=20;

  board.saveEPS( "line_style.eps" );
  board.saveFIG( "line_style.fig" );
  board.saveSVG( "line_style.svg" );
}
