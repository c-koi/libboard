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

Group text()
{
  Group g;
  std::string s = "C'EST ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  s.clear();
  for (int c = '!'; c <= '~'; ++c) {
    s.push_back((char)c);
  }
  g = boardFontText(Point(0, 0), s, 40); // TODO : Baseline !

  // g << Line(0, 0, 100, 0, Color::Black); // TODO : Baseline !
  g.accept(ShapeWithStyleVisitor(Color::Red, Color::Null));
  g.append(makeRough(boardFontText(Point(0, 0), s, 40), 1), Board::Bottom, Board::AlignLeft);
  return g;
}

int main(int, char *[])
{
  auto t = time(nullptr);
  Tools::initBoardRand(static_cast<unsigned long>(t));
  Board board;
  Board::disableLineWidthScaling();

  Style::setDefaultLineWidth(1);
  board.setLineWidth(0.5);
  board.setPenColor(Color::Blue);
  board.setFillColor(Color::Red);

  board << text();

  //  std::vector<Point> p = {Point(50, 50), Point(70, 75), Point(80, 75)};
  //  std::vector<Point> q = {mid(p[0], p[1], 0.25), //
  //                          mid(p[1], p[0], 0.25), //
  //                          (p[1] + Point(3, -4)), //
  //                          (p[2] + Point(-3, 2))};

  //  board << rectangle(0, 0, 20, 20);
  //  board << rectangle(100, 100, 20, 20);
  //  board << Bezier(p, q, Color::Black);
  //  board << Bezier(p, q, Color::Black).scaled(1.5);
  //  board << bezierControls(Bezier(p, q, Color::Black), Style().withLineWidth(0.2));
  //  board << bezierControls(Bezier(p, q, Color::Black).scaled(1.5), Style().withLineWidth(0.2));

  board.saveSVG("board_font_text.svg", Board::BoundingBox);

  system("svgviewer board_font_text.svg");
}
