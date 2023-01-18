/**
 * @file   board_font_text.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample use of Board's font
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <board/BoardFontText.h>
#include <board/Tools.h>
#include <ctime>
using namespace LibBoard;

Group text()
{
  Group g;
  std::string s;
  for (int c = '!'; c <= '~'; ++c) {
    s.push_back((char)c);
  }
  g = boardFontText(Point(10, 20), s, 40);
  g << Line(Point(10, 20), Point(10 + g.boundingBox(LineWidthFlag::UseLineWidth).width, 20), Color::Black);

  g.accept(ShapeWithStyleVisitor(Color::Red, Color::Null));
  g.append(makeRough(boardFontText(Point(0, 0), s, 40), 1), Direction::Bottom, Alignment::Left);
  g << Dot(0, 0);
  return framed(g, 10.0, Color::Black, Color::White, 1.0);
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

  board.saveSVG("board_font_text.svg", PageSize::BoundingBox);
  // system("svgviewer board_font_text.svg");
}
