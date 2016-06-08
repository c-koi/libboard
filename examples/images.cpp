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

  Image michel("../resources/saint_michel.jpg",0,0,200);
  Rectangle rectangle( michel.boundingBox(Shape::IgnoreLineWidth), Color::Red, Color::Silver,1);
  Group g;
  g << rectangle;
  g << michel;

  ShapeList tiling;
  tiling.addTiling(g,Point(0,0),5,4,0.0,Board::UseLineWidth);
  board << tiling;

  ShapeList avatars;
  Image avatar("../resources/avatar.png",0,0,80);
  avatar.rotateDeg(45);
  avatars.addTiling(avatar,Point(0,0),4,4,10.0,Board::UseLineWidth);
  avatars.moveCenter(board.center());
  board << avatars;

  //board.saveEPS( "images.eps", 0.0, 0.0, 2.0, Board::UInche );
  board.saveEPS( "images.eps", 20.0, 30.0, 2.0, Board::UInche );
  //board.saveEPS( "images.eps", Board::BoundingBox, 10.0, Board::UInche );
  board.saveSVG( "images_40x60cm.svg", 40.0, 60.0, 10.0, Board::UCentimeter );
  board.saveFIG( "images.fig", 20.0, 20.0, 2.0, Board::UCentimeter );
  // board.saveFIG( "images.fig", Board::BoundingBox, 10.0, Board::UCentimeter );

  board.saveSVG( "images.svg" );
}
