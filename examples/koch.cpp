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

const double dy = 10;

ShapeList strikeOut( const Path & path,
                     double strokeWidth,
                     Shape::LineCap lineCap,
                     Shape::LineJoin lineJoin,
                     double miterLimit = 4.0 )
{
  const double s = 1.0;

  ShapeList list;
  std::vector<Point> e = Tools::pathBoundaryPoints(path,strokeWidth,lineCap, lineJoin, miterLimit );
  size_t limit = (path.closed()?path.size():(path.size()-1));
  for ( size_t i = 0; i < limit; ++i ) {
    Point a = path[i];
    Point b = path[(i+1)%path.size()];
    Point v = b-a;
    Point shift  = 0.5 * strokeWidth * v.rotatedPI2().normalise();
    list << Line(a+shift,b+shift,Color::Black,1.0).scale(s);
    list << Line(a-shift,b-shift,Color::Black,1.0).scale(s);
    list << Line(a,b,Color::Red,1.0);
    if ( (lineCap == Shape::RoundCap) || (lineJoin == Shape::RoundJoin)) {
      list << Circle(a,strokeWidth*0.5,Color::Green,Color::None,0.5);
    }
  }
  if ( (!path.closed() && lineCap == Shape::RoundCap) || (lineJoin == Shape::RoundJoin) ) {
    Point a = path[path.size()-1];
    list << Circle(a,strokeWidth*0.5,Color::Green,Color::None,0.5);
  }

  for ( size_t i = 0; i < e.size(); ++i ) {
    Point c = e[i];
    list << Circle(c,10.0,Color::Blue,Color::None,0.5);
    list << Line(c-Point(10.0,0.0), c+Point(10.0,0),Color::Blue,0.5);
    list << Line(c-Point(0.0,10.0), c+Point(0,10.0),Color::Blue,0.5);
  }
  return list;
}


int coordinate( int width ) {
  return 1 + (int) (width * (rand() / (RAND_MAX + 1.0)));
}

void Koch( ShapeList & board, const Color & color,
           Point p1, Point p2, int depth ) {
  if ( depth <= 0 ) return;

  Point v = p2 - p1;
  Point a = p1 + ( v / 3.0 );
  Point b = p1 + 2 * ( v / 3.0 );
  Point c = b.rotated( 60 * Board::Degree, a );
  Color col = color;

  col.setRGBi( static_cast<unsigned char>( std::floor( col.red() * 0.75 + 0.5 ) ),
               static_cast<unsigned char>( std::floor( col.green() * 0.75 + 0.5 ) ),
               static_cast<unsigned char>( std::floor( col.blue() * 0.75 + 0.5 ) ) );

  board << Triangle( a, c, b, Color::None, color, 0.0 );



  Koch( board, col, p1, a, depth-1 );
  Koch( board, col, a, c, depth-1 );
  Koch( board, col, c, b, depth-1 );
  Koch( board, col, b, p2, depth-1 );
}

int main( int , char *[] )
{
  Board board;
  //board.setUnit(1.0,Board::UPoint);
  Shape::disableLineWidthScaling();
  board.setLineWidth(0.1);
  Shape::setDefaultLineJoin(Shape::RoundJoin);
  Point a( -100, 0 );
  Point c( 100, 0 );
  Point b = c.rotated( 60 * Board::Degree, a );
  Group aux;

  const int recursions = 0;
  Koch( aux, Color::Green, a, b, recursions );
  Koch( aux, Color::Green, b, c, recursions );
  Koch( aux, Color::Green, c, a, recursions );
  aux << Triangle( a, b, c, Color::None, Color::Green, 0 );
  board << aux;

  int n = 3;
  while ( n-- ) {
    board.dup().last().scale( 0.5 );
    board.last()
        .translate( 0.25 * board.last().bbox().width,
                    board.last().bbox().height * 1.5 )
        .rotate( -20 * Board::Degree );
  }

  Path p;
  p << a << b << c;
  board << strikeOut(p,1.0,Shape::defaultLineCap(),Shape::defaultLineJoin());

  //  board << Line( board.bbox().left,
  //                 board.last().bbox().top - 1,
  //                 board.bbox().left + board.bbox().width,
  //                 board.last().bbox().top - 1,
  //                 Color::Blue, 1 );

  board.drawRectangle(board.boundingBox());
  // board << Rectangle(board.boundingBox());

  // board << Text( board.boundingBox().left, board.boundingBox().top,
  //                 "LibBoard Koch's because LibBoard rocks...",
  //                 Fonts::Helvetica,
  //                 "NeverExists, 'Monotype Corsiva', Verdana, Arial",
  //                 24, Color::Blue );
  // board.dup().last().rotate( -90 * Board::Degree ).translate( 0, -0.5 );
  board.saveFIG( "koch.fig", 200, 200 );
  board.saveSVG( "koch.svg", 300, 600 );
  board.saveEPS( "koch.eps", Board::A4 );
  exit(0);
}
