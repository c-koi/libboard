/**
 * @file   tilings.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample program that draws a ruler.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <vector>
using namespace LibBoard;

#include <board/PathBoundaries.h>

ShapeList strikeOut(const Path & path, double strokeWidth, LineCap lineCap, LineJoin lineJoin, double miterLimit = 4.0)
{
  const double s = 1.0;
  ShapeList list;
  std::vector<Point> e = Tools::pathBoundaryPoints(path, strokeWidth, lineCap, lineJoin, miterLimit);
  size_t limit = (path.isClosed() ? path.size() : (path.size() - 1));
  for (size_t i = 0; i < limit; ++i) {
    Point a = path[i];
    Point b = path[(i + 1) % path.size()];
    Point v = b - a;
    Point shift = 0.5 * strokeWidth * v.rotatedPI2().normalise();
    list << Line(a + shift, b + shift, Color::Black, strokeWidth * 0.01).scale(s);
    list << Line(a - shift, b - shift, Color::Black, strokeWidth * 0.01).scale(s);
    list << Line(a, b, Color::Red, strokeWidth * 0.01);
    if ((lineCap == RoundCap) || (lineJoin == RoundJoin)) {
      list << circle(a, strokeWidth * 0.5, Color::Green, Color::Null, strokeWidth * 0.01);
    }
  }
  if ((!path.isClosed() && lineCap == RoundCap) || (lineJoin == RoundJoin)) {
    Point a = path[path.size() - 1];
    list << circle(a, strokeWidth * 0.5, Color::Green, Color::Null, strokeWidth * 0.01);
  }

  for (size_t i = 0; i < e.size(); ++i) {
    Point c = e[i];
    list << circle(c, 10.0, Color::Blue, Color::Null, strokeWidth * 0.01);
    list << Line(c - Point(10.0, 0.0), c + Point(10.0, 0), Color::Blue, strokeWidth * 0.01);
    list << Line(c - Point(0.0, 10.0), c + Point(0, 10.0), Color::Blue, strokeWidth * 0.01);
  }
  return list;
}

int main(int, char *[])
{
  Board board;
  Board::enableLineWidthScaling();
  board.clear(Color::White);

  // http://www.w3.org/TR/SVG/painting.html#StrokeProperties

  Point p1(30, 366);
  Point p2(200, 400);
  Point p3(0, 0);
  Point p4(-100, 20);
  Point p5(-50, -100);

  Path pA(Path::Closed);
  pA << p1 << p2 << p3 << p4 << p5;
  Path pB(Path::Closed);
  pB << Point(0, 0) << Point(180, 0) << Point(180, 300) << Point(0, 300);
  Path pC(Path::Closed);
  pC << Point(0, 0) << Point(200, 0) << Point(500, 5) << Point(150, 200);

  Path losange;
  losange << Point(0, 0) << Point(300, -500) << Point(600, 0) << Point(300, 500);

  ShapeList l;
  l.append(strikeOut(pA, 50.0, RoundCap, MiterJoin, 3.0), Direction::Right, Alignment::Center, 30.0);
  l.append(strikeOut(pB, 50.0, RoundCap, BevelJoin, 3.0), Direction::Right, Alignment::Center, 30.0);
  l.append(strikeOut(pC, 50.0, RoundCap, MiterJoin, 3.0), Direction::Right, Alignment::Center, 30.0);
  l.append(strikeOut(pA, 50.0, RoundCap, RoundJoin, 3.0), Direction::Right, Alignment::Center, 30.0);
  l.append(Text(0, 0, "Round cap, miter/bevel/miter/round join", Fonts::Courier, 14.0), Direction::Right, Alignment::Center);
  board.append(l, Direction::Bottom, Alignment::Left);

  l.clear();
  l.append(strikeOut(pA, 50.0, ButtCap, MiterJoin, 3.0), Direction::Right, Alignment::Center, 30.0);
  l.append(strikeOut(pB, 50.0, ButtCap, BevelJoin, 3.0), Direction::Right, Alignment::Center, 30.0);
  l.append(strikeOut(pC, 50.0, ButtCap, MiterJoin, 3.0), Direction::Right, Alignment::Center, 30.0);
  l.append(strikeOut(pA, 50.0, ButtCap, RoundJoin, 3.0), Direction::Right, Alignment::Center, 30.0);
  l.append(Text(0, 0, "Butt cap, miter/bevel/miter/round join", Fonts::Courier, 14.0), Direction::Right, Alignment::Center);
  board.append(l, Direction::Bottom, Alignment::Left);

  l.clear();
  l.append(strikeOut(pA, 50.0, SquareCap, MiterJoin, 3.0), Direction::Right, Alignment::Center, 30.0);
  l.append(strikeOut(pB, 50.0, SquareCap, BevelJoin, 3.0), Direction::Right, Alignment::Center, 30.0);
  l.append(strikeOut(pC, 50.0, SquareCap, MiterJoin, 3.0), Direction::Right, Alignment::Center, 30.0);
  l.append(strikeOut(pA, 50.0, SquareCap, RoundJoin, 3.0), Direction::Right, Alignment::Center, 30.0);
  l.append(Text(0, 0, "Square cap, miter/bevel/miter/round join", Fonts::Courier, 14.0), Direction::Right, Alignment::Center);
  board.append(l, Direction::Bottom, Alignment::Left);

  ShapeList list;
  Polyline poly(Path::Closed, Color::Blue, Color::Null, 1.0, SolidStyle, ButtCap, MiterJoin);
  poly << Point(0, 0) << Point(30, -50) << Point(60, 0) << Point(30, 50);
  list.append(poly, Direction::Right, Alignment::Center);
  list.append(poly, Direction::Right, Alignment::Center);
  board.append(list, Direction::Right, Alignment::Center);
  board.append(LibBoard::rectangle(0, 0, 100, 30).rotateDeg(40).scale(1.5, 1.0), Direction::Right, Alignment::Center);

  board.saveEPS("stroke_path.eps", PageSize::A4);
  board.scaleToWidth(25, UseLineWidth);
  board.saveSVG("stroke_path.svg", PageSize::BoundingBox, 0.0, Unit::Centimeter);
}
