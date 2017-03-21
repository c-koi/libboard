/**
 * @file   holes.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample program that draws a ruler.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 */
#include "Board.h"
#include <vector>
#include <set>
#include <ctime>
#include <cstdlib>
using namespace LibBoard;

int main( int , char *[] )
{
  Board board;
  Shape::enableLineWidthScaling();
  board.clear(Color::White);

  srand(time(0));


  Path pA(true);
  pA<<Point(0,0)<<Point(180,0)<<Point(180,300)<<Point(0,300);
  Path pB(true);
  pB << Point(0,0) << Point(200,0) << Point(500,5) << Point(150,200);

  Path losange;
  losange.setClosed(true);
  losange << Point(0,0) << Point(300,-500) << Point(600,0) << Point(300,500);

  board << Line(-50,0,650,0,Color::Red);
  board << Line(-50,300,650,-300,Color::Red);
  board << board.last<Line>();
  board.last<Line>().rotate(90*Board::Degree);
  Polyline p(losange,Shape::defaultPenColor(),Color::Green);
  losange.scale(0.5);
  p.addHole(losange);
  board << p;


  board.rotate(45*Board::Degree);

  board.append(board.last<Polyline>(),Board::Right,Board::AlignCenter);


  Polyline square(true,Color::Black,Color(10,20,130),0.1);
  square << Point(-110,110) << Point(110,110) << Point(110,-110) << Point(-110,-110);
  int n = 40 ;
  std::set<std::pair<int,int>> holes;
  while ( n-- ) {
    holes.insert(std::make_pair((int)(rand()%20)-10,(int)(rand()%20)-10));
  }
  std::set<std::pair<int,int>>::iterator it = holes.begin();
  while (it != holes.end()) {
    Path hole;
    Point p(it->first*10,it->second*10);
    hole << Point(p.x-4.5,p.y+4.5) << Point(p.x+4.5,p.y+4.5) << Point(p.x+4.5,p.y-4.5) << Point(p.x-4.5,p.y-4.5);
    square.addHole(hole);
    ++it;
  }
  board << square.scaled(2);

  //  board << Polyline(losange,Shape::defaultPenColor(),Color::White);

  board.saveEPS( "holes.eps" , Board::A4 );
  board.scaleToWidth(25,Board::UseLineWidth);
  board.saveSVG( "holes.svg", Board::BoundingBox, 0.0, Board::UCentimeter );
}
