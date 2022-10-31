/**
 * @file   graph.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief   Draw a random complete graph
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 */
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "Board.h"

using namespace std;
using namespace LibBoard;

int coordinate(int width)
{
  return 1 + (int)(width * (rand() / (RAND_MAX + 1.0)));
}

int main(int, char *[])
{
  Board board;
  board.clear(Color::White);
  vector<Point> points;
  vector<Point>::iterator i1, i2, end;
  int n = 22;
  while (n--) {
    points.push_back(Point(coordinate(40), coordinate(40)));
  }

  end = points.end();
  i1 = points.begin();
  while (i1 != end) {
    i2 = i1;
    Color pen = Color::fromHueColormap(static_cast<float>(Tools::boardRandDouble()));
    while (i2 != end) {
      if (i1 != i2 && !(rand() % 6)) {
        board << Arrow(*i1, *i2, Arrow::Plain, pen, pen, 0.1);
        Point v = (*i2 - *i1);
        Point vn = v.normalised();
        const double norm = v.norm();
        board << Arrow(*i1, (*i1) + ((norm > 8) ? (vn * 8.0) : v), Arrow::Plain, pen, pen, 0.1).translated(45, 0);
      }
      ++i2;
    }
    ++i1;
  }

  i1 = points.begin();
  while (i1 != end) {
    board << LibBoard::circle(*i1, 0.25, Color::Black, Color::Black, 0);
    board << LibBoard::circle(*i1, 0.25, Color::Black, Color::Black, 0).translated(45, 0);
    ++i1;
  }

  board.saveEPS("graph.eps", 100, 100);
  board.saveFIG("graph.fig", 100, 100);
  board.scaleToWidth(25, UseLineWidth);
  board.saveSVG("graph.svg", Board::PageSize::BoundingBox, 2.0, Board::Unit::Centimeter);
}
