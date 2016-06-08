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
#include <cmath>
#include <limits>
#include <vector>
#include <cassert>
using namespace LibBoard;

#include <board/PathBoundaries.h>

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
      list << Circle(a,strokeWidth*0.5,Color::Green,Color::Null,0.5);
    }
  }
  if ( (!path.closed() && lineCap == Shape::RoundCap) || (lineJoin == Shape::RoundJoin) ) {
    Point a = path[path.size()-1];
    list << Circle(a,strokeWidth*0.5,Color::Green,Color::Null,0.5);
  }

  for ( size_t i = 0; i < e.size(); ++i ) {
    Point c = e[i];
    list << Circle(c,10.0,Color::Blue,Color::Null,0.5);
    list << Line(c-Point(10.0,0.0), c+Point(10.0,0),Color::Blue,0.5);
    list << Line(c-Point(0.0,10.0), c+Point(0,10.0),Color::Blue,0.5);
  }
  return list;
}

int main( int , char *[] )
{
  Board board;
  Shape::enableLineWidthScaling();
  board.clear( Color::White);

  // http://www.w3.org/TR/SVG/painting.html#StrokeProperties

  Point p1(30,366);
  Point p2(200,400);
  Point p3(0,0);
  Point p4(-100,20);
  Point p5(-50,-100);

  Path pA(true);
  pA<<p1<<p2<<p3<<p4<<p5;
  Path pB(true);
  pB<<Point(0,0)<<Point(180,0)<<Point(180,300)<<Point(0,300);
  Path pC(true);
  pC << Point(0,0) << Point(200,0) << Point(500,5) << Point(150,200);

  Path losange;
  losange << Point(0,0) << Point(300,-500) << Point(600,0) << Point(300,500);

  ShapeList l;
  l.append( strikeOut(pA,50.0,Shape::RoundCap,Shape::MiterJoin, 3.0 ),
            ShapeList::Right, ShapeList::AlignCenter, 30.0 );
  l.append( strikeOut(pB,50.0,Shape::RoundCap,Shape::BevelJoin, 3.0 ),
            ShapeList::Right, ShapeList::AlignCenter, 30.0 );
  l.append( strikeOut(pC,50.0,Shape::RoundCap,Shape::MiterJoin, 3.0 ),
            ShapeList::Right, ShapeList::AlignCenter, 30.0 );
  l.append( strikeOut(pA,50.0,Shape::RoundCap,Shape::RoundJoin, 3.0 ),
            ShapeList::Right, ShapeList::AlignCenter, 30.0 );
  l.append(Text(0,0,"Round cap, miter/bevel/miter/round join",Fonts::Courier,14.0),ShapeList::Right,ShapeList::AlignCenter);
  board.append(l,ShapeList::Bottom,ShapeList::AlignLeft);

  l.clear();
  l.append( strikeOut(pA,50.0,Shape::ButtCap,Shape::MiterJoin, 3.0 ),
            ShapeList::Right, ShapeList::AlignCenter, 30.0 );
  l.append( strikeOut(pB,50.0,Shape::ButtCap,Shape::BevelJoin, 3.0 ),
            ShapeList::Right, ShapeList::AlignCenter, 30.0 );
  l.append( strikeOut(pC,50.0,Shape::ButtCap,Shape::MiterJoin, 3.0 ),
            ShapeList::Right, ShapeList::AlignCenter, 30.0 );
  l.append( strikeOut(pA,50.0,Shape::ButtCap,Shape::RoundJoin, 3.0 ),
            ShapeList::Right, ShapeList::AlignCenter, 30.0 );
  l.append(Text(0,0,"Butt cap, miter/bevel/miter/round join",Fonts::Courier,14.0),ShapeList::Right,ShapeList::AlignCenter);
  board.append(l,ShapeList::Bottom,ShapeList::AlignLeft);

  l.clear();
  l.append( strikeOut(pA,50.0,Shape::SquareCap,Shape::MiterJoin, 3.0 ),
            ShapeList::Right, ShapeList::AlignCenter, 30.0 );
  l.append( strikeOut(pB,50.0,Shape::SquareCap,Shape::BevelJoin, 3.0 ),
            ShapeList::Right, ShapeList::AlignCenter, 30.0 );
  l.append( strikeOut(pC,50.0,Shape::SquareCap,Shape::MiterJoin, 3.0 ),
            ShapeList::Right, ShapeList::AlignCenter, 30.0 );
  l.append( strikeOut(pA,50.0,Shape::SquareCap,Shape::RoundJoin, 3.0 ),
            ShapeList::Right, ShapeList::AlignCenter, 30.0 );
  l.append(Text(0,0,"Square cap, miter/bevel/miter/round join",Fonts::Courier,14.0),ShapeList::Right,ShapeList::AlignCenter);
  board.append(l,ShapeList::Bottom,ShapeList::AlignLeft);

  //  l.clear();
  //  l.append( strikeOut(losange,20.0,Shape::SquareCap,Shape::MiterJoin ),
  //            ShapeList::Right, ShapeList::AlignCenter, 0.0 );
  //  l.append( strikeOut(losange,20.0,Shape::SquareCap,Shape::MiterJoin ),
  //            ShapeList::Right, ShapeList::AlignCenter, 0.0 );
  //  board.append(l,ShapeList::Bottom,ShapeList::AlignLeft);

  ShapeList list;
  Polyline poly(true,Color::Blue,Color::Null,1.0,Shape::SolidStyle,Shape::ButtCap,Shape::MiterJoin);
  poly << Point(0,0) << Point(30,-50) << Point(60,0) << Point(30,50);
  list.append(poly,ShapeList::Right,ShapeList::AlignCenter);
  list.append(poly,ShapeList::Right,ShapeList::AlignCenter);
  board.append(list,ShapeList::Right,ShapeList::AlignCenter);

  board.append( Rectangle(0,0,100,30).rotateDeg(40).scale(1.5,1.0),
                ShapeList::Right,ShapeList::AlignCenter);

  board.saveEPS( "stroke_path.eps" , Board::A4 );
  board.saveSVG( "stroke_path.svg" );
}
