/**
 * @file   rough.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Illustrate the rough shape filter
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <board/Debug.h>
#include <ctime>
using namespace LibBoard;

Group slide()
{
  Group g;
  Point a(30, 10);
  Point b(60, 50);
  Point v1(5, 5);
  Point v2(-5, 5);
  g << Bezier(a, a + v1, b, b + v2, Color::Red).rotated(45);

  RoughVisitor::slideRandomly(a, 1);
  RoughVisitor::slideRandomly(b, 1);
  g << Bezier(a, a + v1, b, b + v2, Color::Green).rotated(45);

  RoughVisitor v;
  v.setRepeat(1);
  Shape * rline = Line(0, 0, 20, 20, Color::Red).accept(v);
  try {
    g << *rline;
  } catch (const char * message) {
    std::cerr << message << std::endl;
  }
  delete rline;
  return g;
}

Group diamond()
{
  Path p = {Point(100, 100), Point(150, 150), Point(200, 100)};
  Path p2 = {Point(100, 100), Point(150, 50), Point(200, 100)};
  Group g;
  for (double r = 0.0; r <= 1.0; r += 0.1) {
    g << mix(Polyline(p, Color::Red), Polyline(p2, Color::Yellow), r);
  }
  return g;
}

Group rough()
{
  ShapeList l;
  Polyline poly = rectangle(0, 0, 50, 50, Color::Gray, Color::Green);
  l.append(poly, Direction::Right, Alignment::Center);
  Arrow arrow(0, 0, 60, 60, Arrow::ExtremityType::Stick, Color::Red, Color::White);
  l.append(arrow, Direction::Right, Alignment::Center);
  Ellipse e(0, 0, 15, 25, Color::Red, Color::Brown);
  e.rotate(10 * Board::Degree);
  l.append(e, Direction::Right, Alignment::Center);
  Ellipse c = circle(10, 10, 15, Color::Green, Color::Lime);
  l.append(c);
  Group g;
  g.append(l, Direction::Right, Alignment::Center);
  g.append(makeRough(l, 1, SketchyCrossingHachure, 25 * Board::Degree), Direction::Bottom, Alignment::Center);
  g.append(framed(boardFontText(Point(0, 0), "Hello world!", 10, Color::DarkGreen, 1.5), 5.0, Color::Black, Color::Null, 2.0), Direction::Right, Alignment::Center, 5);
  return g;
}

Group hachures()
{
  Group g;

  Style::setDefaultLineWidth(2);

  Polyline p = rectangle(0, 0, 100, 100);
  g << p;

  Group square;
  square << p;
  square << hachures(p.path(), StraightHachure, Color::Red, 1, 3, 15 * Board::Degree);
  g.append(square, Direction::Right, Alignment::Center, 3);

  p.rotate(10 * Board::Degree);
  square.clear();
  square << p;
  square << hachures(p.path(), StraightHachure, Color::Red, 1, 3);
  g.append(square, Direction::Right, Alignment::Center, 3);

  square.clear();

  Path path = p.path();
  std::swap(path[1], path[2]);

  square << Polyline(path);
  ShapeList h = hachures(path, StraightHachure, Color::Red, 1, 3);
  square << h;

  g.append(square, Direction::Right, Alignment::Center, 3);

  path = {Point(0, 0), Point(0, 100), Point(40, 70), Point(100, 0), Point(100, 100), Point(40, 70)};
  path.close();

  return g;
}

Group hachuresBis()
{
  Group g;
  Path path = {Point(0, 0), Point(50, 100), Point(75, 75), Point(100, 90), Point(150, 50), Point(200, 0), Point(180, 100)};
  path.close();
  g << hachures(path, StraightHachure, Color::Red, 0.5, 2, 30 * Board::Degree);
  return g;
}

Group polygons()
{
  Group g;

  Style::setDefaultLineWidth(2.5);
  Style::setDefaultFillColor(Color::Cyan);
  Style::setDefaultPenColor(Color(20, 20, 20));

  Polyline p = rectangle(0, 0, 100, 100).rotate(10 * Board::Degree);
  g << p;

  Path path = p.path();
  std::swap(path[1], path[2]);

  g.append(Polyline(path).rotate(25 * Board::Degree));

  path = {Point(0, 0), Point(0, 100), Point(40, 70), Point(100, 0), Point(100, 100), Point(40, 70)};
  path.close();

  g.append(Polyline(path).rotate(12 * Board::Degree));

  return g;
}

Group hachuresTer()
{
  Group g;
  Path path = {Point(0, 0), Point(0, 100), Point(40, 70), Point(100, 0), Point(100, 100), Point(40, 70)};
  path.close();
  g << hachures(path, StraightHachure, Color::Red, 0.5, 2, 0 * Board::Degree);
  return g;
}

Group ell()
{
  Group g;
  Style::setDefaultLineWidth(0.5);
  Ellipse e(0, 0, 60, 30, Color::Red, Color::Green);
  e.rotate(35 * Board::Degree);
  // g << e;
  // g << rectangle(e.boundingBox(UseLineWidth), Color::Red, Color::Null, 2.0);

  //  Polyline r = rectangle(0, 0, 50, 40, Color::Blue, Color::Aqua, 2.0);
  //  g.append(r);
  //  g.append(makeRough(r, 2, NoFilling, 10 * Board::Degree, 0.0), Direction::Right);
  //  g.append(makeRough(r, 2, StraightHachure, 0.0 * Board::Degree, 0.0), Direction::Right);
  //  g.append(makeRough(r, 2, SketchyHachure, 0.0 * Board::Degree, 0.0), Direction::Right);
  //  g.append(makeRough(r, 2, SketchyCrossingHachure, 14.0 * Board::Degree, 2.0), Direction::Right);
  //  r.setPenColor(Color::Null);
  //  g.append(makeRough(r, 2, StraightHachure, 0.0 * Board::Degree, 0.0), Direction::Right);
  //  g.append(makeRough(r, 2, SketchyCrossingHachure, 0.0 * Board::Degree, 2.5), Direction::Right);
  //  g.append(makeRough(e, 1, PlainFilling, 10 * Board::Degree, 0.0), Direction::Right);

  Group eh;
  Style s = e.style();
  s.penColor = Color::Green;
  // eh << hachures(e, s, StraightHachure, 4, 0 * Board::Degree);
  e.setPenColor(Color::Black);
  e.setFillColor(Color::Brown);
  eh << e;
  g.append(eh, Direction::Right);

  // g.append(makeRough(e, 2, StraightHachure, 10 * Board::Degree, 0.0), Direction::Right);
  // g.append(makeRough(e, 2, CrossingHachure, 10 * Board::Degree, 0.0), Direction::Right);
  g.append(makeRough(e, 2, SketchyHachure, 10 * Board::Degree, 0.0), Direction::Right);
  g.append(makeRough(e, 1, SketchyCrossingHachure, 45 * Board::Degree, 3.0));

  // g.append(e);
  return g;
}

int main(int, char *[])
{
  auto t = time(nullptr);
  // t = 1589104684;
  Tools::initBoardRand(static_cast<unsigned long>(t));
  Board board;
  Board::disableLineWidthScaling();

  board.setLineWidth(1.0);
  board.setPenColor(Color::Blue);
  board.setFillColor(Color::Red);
  // Style::setDefaultLineWidth(0.5);

  Path r = rectangle(0, 0, 10, 10).path();
  std::cout << Style::defaultStyle() << std::endl;
  // Style::setDefaultPenColor(Color::Blue);
  // board.append(Bezier::smoothedPolyline(r, 1.5), Direction::Right, Alignment::Center);

  // board.append(slide(), Direction::Right, Alignment::Center);
  // board.append(diamond(), Direction::Right, Alignment::Center);

  board.append(hachures(), Direction::Right, Alignment::Center);

  board.append(polygons(), Direction::Bottom, Alignment::Center);
  board.append(makeRough(polygons(), 1, PlainFilling), Direction::Bottom, Alignment::Center);
  board.append(makeRough(polygons(), 2, PlainFilling), Direction::Bottom, Alignment::Center);
  board.append(makeRough(polygons(), 2, SketchyHachure, 20 * Board::Degree, 2), Direction::Bottom, Alignment::Center);

  board.append(rough(), Direction::Right, Alignment::Center);
  board.append(framed(ell()), Direction::Right, Alignment::Center);

  board.append(hachuresBis());
  board.append(hachuresTer(), Direction::Bottom, Alignment::Center);

  //  board.append(framed(boardFontText(Point(0, 0), "Hello", 24), Color::Red, 2, 5, 1));
  //  board.append(makeRough(framed(boardFontText(Point(0, 0), "Hello", 24), Color::Red, 2, 5), 1));
  //  board.append(makeRough(framed(boardFontText(Point(0, 0), "HELLO", 24), Color::Red, 2, 5), 1));

  // board.append(framed(board), Direction::Bottom);

  board.saveSVG("rough.svg", PageSize::BoundingBox);
  // board.saveFIG("rough.fig", PageSize::BoundingBox);
  // system("svgviewer rough.svg");
}
