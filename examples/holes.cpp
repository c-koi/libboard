/**
 * @file   holes.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample program that draws a ruler.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <board/Tools.h>
#include <ctime>
#include <set>
#include <vector>
using namespace LibBoard;

int main(int, char *[])
{
  Board board;
  Board::enableLineWidthScaling();
  board.clear(Color::White);

  Tools::initBoardRand(static_cast<unsigned int>(time(nullptr)));

  Path pA(Path::Closed);
  pA << Point(0, 0) << Point(180, 0) << Point(180, 300) << Point(0, 300);
  Path pB(Path::Closed);
  pB << Point(0, 0) << Point(200, 0) << Point(500, 5) << Point(150, 200);

  Path losange;
  losange.close();
  losange << Point(0, 0) << Point(300, -500) << Point(600, 0) << Point(300, 500);

  board << Line(-50, 0, 650, 0, Color::Red);
  board << Line(-50, 300, 650, -300, Color::Red);
  board << board.last<Line>();
  board.last<Line>().rotate(90 * Board::Degree);
  Polyline p(losange, Style::defaultPenColor(), Color::Green);
  losange.scale(0.5);
  p.addHole(losange);
  board << p;

  board.rotate(45 * Board::Degree);

  board.append(board.last<Polyline>(), Direction::Right, Alignment::Center);

  Polyline square(Path::Closed, Color::Black, Color(70, 70, 130), 0.1);
  square << Point(-110, 110) << Point(110, 110) << Point(110, -110) << Point(-110, -110);
  int n = 80;
  std::set<std::pair<int, int>> holes;
  while (n--) {
    holes.insert(std::make_pair((int)(Tools::boardRand() % 20) - 10, (int)(Tools::boardRand() % 20) - 10));
  }
  std::set<std::pair<int, int>>::iterator it = holes.begin();
  while (it != holes.end()) {
    Path hole;
    Point p(it->first * 10, it->second * 10);
    hole << Point(p.x - 5, p.y + 5) << Point(p.x + 5, p.y + 5) << Point(p.x + 5, p.y - 5) << Point(p.x - 5, p.y - 5);
    square.addHole(hole);
    ++it;
  }
  board << square.scaled(3).translated(1000, 0).rotated(45 * Board::Degree);

  board.saveEPS("holes.eps", PageSize::A4);
  board.scaleToWidth(25, UseLineWidth);
  board.saveSVG("holes.svg", PageSize::BoundingBox, 0.0, Unit::Centimeter);
}
