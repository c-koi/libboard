/**
 * @file   logo.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample drawing using the board library.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 *
 */
#include "Board.h"
#include <cmath>
using namespace LibBoard;

int main( int, char *[] )
{
  Board board;
  Shape::disableLineWidthScaling();

  board.setLineWidth( 1 ).setPenColorRGBi( 255, 100, 0 );
  board.setLineStyle( Shape::SolidStyle );
  board.setLineJoin( Shape::MiterJoin );
  board.setLineCap( Shape::RoundCap );
  board.drawLine( -50, -27, 50, -27 );
  board.addDuplicates( board.last(), 10, 0, -2 );

  Point p = board.last<Line>().boundingBox(Shape::IgnoreLineWidth).centerLeft();
  int n = 20;
  double angle = -M_PI/(2*n);
  while ( n-- )
    board << board.last<Line>().rotated( angle, p );

  n = 30;
  angle = -M_PI/(n);
  while ( n-- ) {
    board << board.last<Line>().scaled( 0.95 ).rotated( angle, p );
  }

  double textHeight = board.last<Line>().boundingBox(Shape::UseLineWidth).height;
  board << Text( -45, -20, "LibBoard",
                 Fonts::Helvetica,
                 "'Bookman Old Style',Verdana",
                 textHeight * 1.2,
                 Color::Green );

  Shape::enableLineWidthScaling();
  board.saveEPS( "logo_A4.eps", Board::A4 );
  board.saveFIG( "logo_A4.fig", Board::A4  );

  board.scaleToWidth(25,Board::UseLineWidth);
  board.saveSVG( "logo.svg", Board::BoundingBox, 0.0, Board::UCentimeter );
}
