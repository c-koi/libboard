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
using namespace LibBoard;

const int NbColors = 4;
const Color Colors[NbColors] = { Color("#cc0000"),
                                 Color("#00cc00"),
                                 Color("#0000cc"),
                                 Color("#c0c0c0") };

ShapeList generateGroup(int n) {
  if ( !n ) {
    return Group();
  }
  if (n==1) {
    Group g;
    switch (rand()%3) {
    case 0:
      g << Circle(0,0,5,Colors[rand() % NbColors],Color::Null,1.0);
      break;
    case 1:
      g << Rectangle(0,0,10,10,Colors[rand()%NbColors],Color::Null,1.0);
      break;
    case 2:
      g << Rectangle(0,0,10,10,Color::Null,Color::Null,0.0);
      break;
    default:
      break;
    }
    return g;
  }
  int count[4] = {0,0,0,0};
  while (count[0]+count[1]+count[2]+count[3] != n) {
    count[rand()%4] += 1;
  }
  ShapeList topLeft = generateGroup(count[0]);
  ShapeList topRight= generateGroup(count[1]);
  ShapeList bottomLeft = generateGroup(count[2]);
  ShapeList bottomRight = generateGroup(count[3]);
  ShapeList top;
  top << topLeft;
  top.append(topRight,ShapeList::Right,ShapeList::AlignCenter,0.0,ShapeList::UseLineWidth);
  ShapeList bottom;
  bottom << bottomLeft;
  bottom.append(bottomRight,ShapeList::Right,ShapeList::AlignCenter,0.0,ShapeList::UseLineWidth);
  top.append(bottom,ShapeList::Bottom,ShapeList::AlignCenter,0.0,ShapeList::UseLineWidth);

  if ( n >= 4 ) {
    ShapeList list;
    Group group;
    group << top;
    Rect r = top.bbox(Shape::UseLineWidth);

    group << Rectangle(r,Color::Black,Color::Null,0.5,Board::DotStyle);
    std::cout << r << std::endl;
    list << group;
    return list;
  } else {
    ShapeList list;
    list << top;
    return list;
  }

}



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
  //srand(time(0));
  srand(1000);
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
#endif
  board.append(circles,Board::Bottom,Board::AlignRight);

  std::cout << "List of shapes at to level\n";
  std::cout << "==========================\n";
  for (const Shape & s : board) {
    std::cout << s.name() << ", ";
  }
  std::cout << "\n\n";

  std::cout << "Beadth-First traversal of the shapes tree\n";
  std::cout << "=========================================\n";
  ShapeList::BreadthFirstIterator bfi = board.breadthFirstBegin();
  ShapeList::BreadthFirstIterator bfe = board.breadthFirstEnd();
  while (bfi!=bfe) {
    std::cout << (*bfi).name() << ", ";
    ++bfi;
  }
  std::cout << "\n\n";

  std::cout << "Depth-First traversal of the shapes tree\n";
  std::cout << "========================================\n";
  ShapeList::DepthFirstIterator dfi = board.depthFirstBegin();
  ShapeList::DepthFirstIterator dfe = board.depthFirstEnd();
  while (dfi!=dfe) {
    std::cout << (*dfi).name() << ", ";
    ++dfi;
  }
  std::cout << "\n\n";

  std::cout << "Size is " << board.size() << std::endl;
  std::cout << "Deep size is " << board.deepSize() << std::endl;

  board.append(generateGroup(25),ShapeList::Bottom,ShapeList::AlignCenter);

  board.saveSVG( "traversal.svg" );
}
