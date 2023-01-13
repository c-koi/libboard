/**
 * @file   triangles.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Illustrate the rough shape filter
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <cassert>
#include <ctime>
using namespace LibBoard;

enum Division
{
  DivTriangle,
  DivVertexA,
  DivVertexB,
  DivVertexC
};

std::vector<Polyline> divided(const Polyline & t, Division division)
{
  std::vector<Polyline> result;
  assert(t.vertexCount() == 3);
  assert(t.path().isClosed());
  const Point a = t.path()[0];
  const Point b = t.path()[1];
  const Point c = t.path()[2];
  if (division == DivTriangle) {
    const Point ab = mix(a, b, 0.5);
    const Point bc = mix(b, c, 0.5);
    const Point ca = mix(c, a, 0.5);
    result.push_back(triangle(ab, bc, ca));
    result.push_back(triangle(a, ab, ca));
    result.push_back(triangle(b, bc, ab));
    result.push_back(triangle(c, ca, bc));
  } else if (division == DivVertexA) {
    const Point bc = mix(b, c, 0.5);
    result.push_back(triangle(b, bc, a));
    result.push_back(triangle(c, a, bc));
  } else if (division == DivVertexB) {
    const Point ac = mix(a, c, 0.5);
    result.push_back(triangle(c, ac, b));
    result.push_back(triangle(a, b, ac));
  } else if (division == DivVertexC) {
    const Point ab = mix(a, b, 0.5);
    result.push_back(triangle(a, ab, c));
    result.push_back(triangle(b, c, ab));
  }
  return result;
}

std::vector<Polyline> divided(const std::vector<Polyline> & v, Division division)
{
  std::vector<Polyline> result;
  for (const Polyline & p : v) {
    auto div = divided(p, division);
    for (const Polyline & t : div) {
      result.push_back(t);
    }
  }
  return result;
}

int main(int, char *[])
{
  Board board;
  Board::disableLineWidthScaling();

  Style::setDefaultFillColor(Color::Null);
  Style::setDefaultLineCap(RoundCap);
  Style::setDefaultLineWidth(1);

  board.setLineWidth(0.1);
  board.setPenColor(Color::Blue);
  board.setFillColor(Color::Red);

  std::vector<Polyline> result;
  //  result.push_back(triangle(Point(0, 0), Point(500, 500), Point(0, 1000)));
  //  result.push_back(triangle(Point(1000, 0), Point(500, 500), Point(0, 00)));
  //  result.push_back(triangle(Point(1000, 1000), Point(500, 500), Point(1000, 0)));
  //  result.push_back(triangle(Point(0, 1000), Point(500, 500), Point(1000, 1000)));

  result.push_back(triangle(Point(0, 0), Point(500, 500), Point(0, 1000)));
  result.push_back(triangle(Point(500, 500), Point(0, 0), Point(1000, 0)));
  result.push_back(triangle(Point(1000, 1000), Point(500, 500), Point(1000, 0)));
  result.push_back(triangle(Point(500, 500), Point(1000, 1000), Point(0, 1000)));
  auto seed = time(nullptr);
  std::cout << "SEED: " << seed << std::endl;
  Tools::initBoardRand(seed);
  int n = 3;
  while (n--) {
    result = divided(result, Division(Tools::boardRand() % 4));
  }
  // board << divided(t, DivTriangle);
  // board << divided(t, DivVertexA);

  board << result;

  board.saveSVG("triangles.svg", PageSize::BoundingBox);
  // system("svgviewer triangles.svg");
}
