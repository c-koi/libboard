/**
 * @file   bezier.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <cstdlib>
#include <Board.h>
using namespace LibBoard;

int main(int, char *[])
{
  Board board;
  Board::disableLineWidthScaling();

  board.setLineWidth(1.0);
  board.setPenColor(Color::Blue);
  board.setFillColor(Color::Red);
  board << Bezier(Point(10, 10), Point(10, 10) + Point(5, 5), Point(30, 10), Point(30, 10) + Point(-5, 5), Color::Red).rotated(45);

  Bezier b(Point(0, 0), Point(0, 0) + Point(0, 20), Point(-30, 0), Point(-30, 0) + Point(0, -15), Color::Green, Color::Null, 1.0);

  ShapeList l;
  l << b; // << LibBoard::rectangle(b.boundingBox(UseLineWidth), Color::Black, Color::Null, 1.0);

  board.append(b);
  board.append(b.scaled(2, 2));
  board.append(Polyline(b.discretizedPath()));
  board.append(b.rotated(45 * Board::Degree));
  ShapeList lb;
  Bezier b3 = b.rotated(45 * Board::Degree).scaled(5);
  lb << b3;
  lb << rectangle(lb.boundingBox(UseLineWidth));

  auto ex = b3.pathThroughLocalExtremums();
  std::cout << Path(ex, Path::Open) << std::endl;
  for (const Point & p : ex) {
    lb << cross(p);
  }
  lb << bezierControls(b3);
  lb << Polyline(Path(ex, Path::Open), Color::Magenta);

  board.append(lb);

  board.append(l.rotated(45 * Board::Degree).translated(100, 0).scaled(5));
  board.append(l.rotated(45 * Board::Degree).translated(200, 0).scaled(3, 2));

  l.clear();
  l << b;
  l << b.rotated(45 * Board::Degree);
  l << LibBoard::rectangle(b.rotated(45 * Board::Degree).boundingBox(UseLineWidth), Color::Black, Color::Null, 1.0);

  Polyline dp(b.discretizedPath().rotated(45 * Board::Degree), Color::Blue);
  l << dp;
  board.append(l);

  Path p = {Point(0, 0), Point(100, 0)};
  std::vector<Point> c = {Point(125, 75), Point(-25, 75)};
  l.clear();
  Bezier b2(p.points(), c, Color::Green);
  l << b2;
  ex = b2.pathThroughLocalExtremums();
  std::cout << Path(ex, Path::Open);
  for (const Point & p : ex) {
    l << cross(p);
  }
  l << bezierControls(b2);
  l << Polyline(ex, Path::Open, Color::Magenta);
  board.append(l);

  Point y0(0, 0);
  Point y1(10, 20);
  Point y2(20, 3);
  Point y3(25, -5);
  {
    l.clear();
    l << cross(y0);
    l << cross(y1);
    l << cross(y2);
    l << cross(y3);
    Bezier b = Bezier::interpolation(y0, y1, y2, y3);
    l << b;
    board.append(l);
    Path p = {y0, y1, y2, y3};
    board.append(Bezier::smoothedPolyline(p, 0.5));
  }

  board.saveSVG("bezier.svg", PageSize::BoundingBox);
  // system("svgviewer bezier.svg");
}
