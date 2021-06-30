/**
 * @file   rough.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Illustrate the rough shape filter
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 */
#include <cstdlib>
#include <ctime>
#include "Board.h"
#include "board/BoardFontText.h"
#include "board/Debug.h"
using namespace LibBoard;

Group interpolate()
{
  Group g;
  Path a = {Point(0, 0), Point(50, 0), Point(100, 0), Point(150, 0), Point(200, 0)};
  Path b = {Point(0, 100), Point(50, 150), Point(100, 100), Point(150, 150), Point(200, 100)};
  Polyline pa(a, Color::Red);
  Polyline pb(b, Color::Green);
  g << pa << pb;
  for (double t = 0.05; t < 1.0; t += 0.05) {
    g << mix(pa, pb, t);
  }

  for (decltype(a.size()) i = 0; i < a.size(); ++i) {
    g << Line(a[i], b[i], Color::Gray);
  }
  return g;
}

Group mustache()
{
  Group g;
  Path p = {Point(100, 100), Point(150, 150), Point(200, 100)};
  Color c0 = Color::fromHueColormap(0.0f);
  Color c1 = Color::fromHueColormap(0.999f);
  Style style = Style::defaultStyle();
  for (double r = 0.0; r <= 2.0; r += 0.1) {
    const float t = static_cast<float>(r) / 2.0f;
    style.penColor = Color::mixHSV(c0, c1, t);
    g << Bezier::smoothedPolyline(p.points(), r, style);
  }
  return g;
}

int main(int, char *[])
{
  Board board;
  Board::disableLineWidthScaling();

  board.setLineWidth(0.1);
  board.setPenColor(Color::Blue);
  board.setFillColor(Color::Red);

  board.append(interpolate(), Board::Right, Board::AlignCenter, 5);
  board.append(mustache(), Board::Right, Board::AlignCenter, 5);

  board.saveSVG("interpolate.svg", Board::BoundingBox);

  system("svgviewer interpolate.svg");
}
