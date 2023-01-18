/**
 * @file   xkcd.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample drawing using the board library.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <cstdlib>

using namespace LibBoard;

int main(int, char *[])
{
  Board board;
  const double TEXT_SIZE = 4.0;
  Style::setDefaultLineWidth(0.8);
  const double TEXT_LINEWIDTH = 0.33 * Style::defaultLineWidth();
  Group g;
  // Axis
  g << Line(0, 0, 150, 0, Color::Black);
  g << Line(0, 0, 0, 100, Color::Black);
  // Plot
  g << Line(8, 80, 100, 80, Color("#1f77b4"));
  g << Line(103, 80, 142, 5, Color("#1f77b4"));

  board << framed(makeRough(g), 10, Color::Black, Color::Null, 0.5);

  Group pointText;
  pointText << boardFontText(Point(), "THE DAY I REALIZED", TEXT_SIZE, Color::Black, TEXT_LINEWIDTH);
  pointText.append(boardFontText(Point(), "I COULD COOK BACON", TEXT_SIZE, Color::Black, TEXT_LINEWIDTH), Direction::Bottom, Alignment::Left, 2.0);
  pointText.append(boardFontText(Point(), "WHENEVER I WANTED", TEXT_SIZE, Color::Black, TEXT_LINEWIDTH), Direction::Bottom, Alignment::Left, 2.0);

  // Arrow

  Point translation = Point(78, 58) - pointText.boundingBox(UseLineWidth).topRight();
  pointText.translate(translation.x, translation.y);
  board << pointText;
  board << makeRough(Arrow(pointText.boundingBox(UseLineWidth).topRight(), Point(101, 77), Arrow::ExtremityType::Stick, Color::Black, Color::Null, 0.5 * Style::defaultLineWidth()));

  Group time = boardFontText(Point(), "TIME", TEXT_SIZE, Color::Black, TEXT_LINEWIDTH);
  time.moveCenter(75, -1.25 * TEXT_SIZE);
  board << time;

  Group health = boardFontText(Point(), "MY OVERALL HEALTH", TEXT_SIZE, Color::Black, TEXT_LINEWIDTH);
  health.rotateDeg(90).moveCenter(-1.25 * TEXT_SIZE, 50);
  board << health;

  Group title = boardFontText(Point(), "\"STOVE OWNERSHIP\" FROM XKCD BY RUNDALL MUNROE", TEXT_SIZE, Color::Black, TEXT_LINEWIDTH);

  board.append(title, Direction::Bottom, Alignment::Center, 5.0);

  board.saveSVG("xkcd.svg");
  // system("svgviewer xkcd.svg");
}
