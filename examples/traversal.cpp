/**
 * @file   traversal.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample code showing the use of shape iterators as well as
 *         the ShapeList::topLevelFindLast<T>(n) method.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <ctime>
#include <iostream>
using namespace LibBoard;

const int NbColors = 4;
const Color Colors[NbColors] = {Color("#cc0000"), Color("#00cc00"), Color("#0000cc"), Color("#c0c0c0")};

ShapeList generateGroup(int n)
{
  if (!n) {
    return ShapeList() << (Group() << LibBoard::rectangle(0, 0, 10, 10, Color("#404040"), Color::Null, 0.1, DashStyle));
  }
  if (n == 1) {
    switch (Tools::boardRand() % 2) {
    case 0:
      return ShapeList() << circle(0, 0, 5, Colors[Tools::boardRand() % NbColors], Color::Null, 1.0);
    case 1:
      return ShapeList() << rectangle(0, 0, 10, 10, Colors[Tools::boardRand() % NbColors], Color::Null, 1.0);
    }
  }
  int count[4] = {0, 0, 0, 0};
  while (count[0] + count[1] + count[2] + count[3] != n) {
    count[Tools::boardRand() % 4] += 1;
  }
  ShapeList topLeft = generateGroup(count[0]);
  ShapeList topRight = generateGroup(count[1]);
  ShapeList bottomLeft = generateGroup(count[2]);
  ShapeList bottomRight = generateGroup(count[3]);
  ShapeList top;
  top << topLeft;
  top.append(topRight, Direction::Right, Alignment::Center, 0.0, UseLineWidth);
  ShapeList bottom;
  bottom << bottomLeft;
  bottom.append(bottomRight, Direction::Right, Alignment::Center, 0.0, UseLineWidth);
  top.append(bottom, Direction::Bottom, Alignment::Center, 0.0, UseLineWidth);
  if (n >= 2) {
    ShapeList list;
    Group group;
    group << top;
    Rect r = top.bbox(UseLineWidth);
    group << LibBoard::rectangle(r, Color::Black, Color::Null, 0.1);
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
template <typename T> ShapeList findAll(ShapeList & list)
{
  ShapeList result;
  try {
    ShapeList::size_type n = 0;
    while (true) {
      result.append(list.topLevelFindLast<T>(n), Direction::Right, Alignment::Center);
      ++n;
    }
  } catch (Exception &) {
  }
  return result;
}

int main(int, char *[])
{
  // srand(time(0));
  Tools::initBoardRand(1000);
  Board board;

  Style::setDefaultLineWidth(0.5);
  Style::setDefaultPenColor(Color::Blue);
  Style::setDefaultFillColor(Color::Null);

  Group cross;
  cross << Line(0, 0, 10, 10, Color::Green) << Line(0, 10, 10, 0, Color::Green);

  // Build a random list of shapes
  int n = 25;
  while (n--) {
    int r = Tools::boardRand() % 3;
    switch (r) {
    case 0:
      board.append(circle(0, 0, 5.0), Direction::Right, Alignment::Center);
      break;
    case 1:
      board.append(rectangle(0, 0, 10, 10), Direction::Right, Alignment::Center);
      break;
    case 2:
      board.append(cross, Direction::Right, Alignment::Center);
      break;
    }
  }

  // Extract circles, rectangles, and crosses in distinct lists
  ShapeList circles = findAll<Ellipse>(board);
  ShapeList rectangles = findAll<Polyline>(board);
  ShapeList crosses = findAll<Group>(board);

  board.append(Text(0, 0, "Everything", LibBoard::Fonts::Helvetica, 7), Direction::Right, Alignment::Center, 10.0);

  board.append(circles, Direction::Bottom, Alignment::Left);
  board.append(rectangles, Direction::Bottom, Alignment::Left);
  board.append(crosses, Direction::Bottom, Alignment::Left);

  for (Shape & s : circles) {
    s.scale(1, 2);
  }
  board.append(circles, Direction::Bottom, Alignment::Right);

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
  while (bfi != bfe) {
    std::cout << (*bfi).name() << ", ";
    ++bfi;
  }
  std::cout << "\n\n";

  std::cout << "Depth-First traversal of the shapes tree\n";
  std::cout << "========================================\n";
  ShapeList::DepthFirstIterator dfi = board.depthFirstBegin();
  ShapeList::DepthFirstIterator dfe = board.depthFirstEnd();
  while (dfi != dfe) {
    std::cout << (*dfi).name() << ", ";
    ++dfi;
  }
  std::cout << "\n\n";

  board.append(generateGroup(50), Direction::Bottom, Alignment::Center);

  std::cout << "Size is " << board.size() << std::endl;
  std::cout << "Board deep size is " << board.deepSize() << std::endl;

  ShapeList & tree = board.last<ShapeList>();
  std::size_t deepSize = tree.deepSize();
  std::cout << "Tree deep size is " << deepSize << std::endl;

  dfi = tree.depthFirstBegin();
  dfe = tree.depthFirstEnd();

  std::size_t counter = 0;
  ShapeList points;
  Color shade;
  Path path;
  while (dfi != dfe) {
    Shape * r = dynamic_cast<Polyline *>(dfi.pointer());
    if (!r) {
      r = dynamic_cast<Ellipse *>(dfi.pointer());
    }
    if (r) {
      shade.setRGBf(0.2f + 0.8f * (counter / (float)deepSize), 0, 0, 1.0);
      points << LibBoard::circle(r->center(), 1, Color::Null, shade, 0.0);
      path << r->center();
    }
    ++dfi;
    ++counter;
  }
  std::cout << counter << " shapes visited.\n";
  board << points;
  board << Polyline(path, Color("#00c000"), Color::Null, 0.5, SolidStyle, RoundCap, RoundJoin);
  board.saveSVG("traversal.svg");
}
