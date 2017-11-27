/**
 * @file   traversal.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample code showing the use of shape iterators as well as
 *         the ShapeList::topLevelFindLast<T>(n) method.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 */
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Board.h"
#include "Exception.h"
using namespace LibBoard;

/**
 * Extract all shapes of a given type (T) thanks to the topLevelFindLast<>()
 * method.
 */
template<typename T>
ShapeList findAll(ShapeList & list)
{
  ShapeList result;
  try {
    int n = 0;
    while ( true ) {
      result.append(list.topLevelFindLast<T>(n),Board::Right,Board::AlignCenter);
      ++n;
    }
  } catch (Exception & ) {
  }
  return result;
}

int main( int, char *[] )
{
  srand(time(0));
  Board board;

  Shape::setDefaultLineWidth(0.5);
  Shape::setDefaultPenColor(Color::Blue);
  Shape::setDefaultFillColor(Color::Null);

  Group cross;
  cross << Line(0,0,10,10,Color::Green) << Line(0,10,10,0,Color::Green);

  // Build a random list of shapes
  int n = 25;
  while ( n-- ) {
    int r = rand() % 3;
    switch ( r ) {
    case 0:
      board.append(Circle(0,0,5.0),Board::Right,Board::AlignCenter);
      break;
    case 1:
      board.append(Rectangle(0,0,10,10),Board::Right,Board::AlignCenter);
      break;
    case 2:
      board.append(cross,Board::Right,Board::AlignCenter);
      break;
    }
  }

  // Extract circles, rectangles, and crosses in distinct lists
  ShapeList circles = findAll<Circle>(board);
  ShapeList rectangles = findAll<Rectangle>(board);
  ShapeList crosses = findAll<Group>(board);

  board.append(Text(0,0,"Everything",LibBoard::Fonts::Helvetica,7),
               Board::Right,
               Board::AlignCenter,
               10.0);

  board.append(circles,Board::Bottom,Board::AlignLeft);
  board.append(rectangles,Board::Bottom,Board::AlignLeft);
  board.append(crosses,Board::Bottom,Board::AlignLeft);

#if __cplusplus<201100
  // Use an iterator
  ShapeList::TopLevelIterator it = circles.begin();
  while ( it != circles.end() ) {
    (*it).scale(1,2);
    ++it;
  }
#else
  // Use a C++11 for loop
  for (Shape & s : circles) {
    s.scale(1,2);
  }
  for (const Shape & s : board) {
    std::cout << s.name() << std::endl;
  }
#endif

  board.append(circles,Board::Bottom,Board::AlignRight);

  std::cout << "Size is " << board.size() << std::endl;
  std::cout << "Deep size is " << board.deepSize() << std::endl;

  board.saveSVG( "traversal.svg" );
}
