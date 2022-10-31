/**
 * @file   tilings.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample program that draws a ruler.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr>
 */
#include "Board.h"
using namespace LibBoard;

int main(int, char *[])
{
  Board board;
  board.clear(Color::White);

  Style::setDefaultLineWidth(0.1);
  Style::setDefaultLineStyle(SolidStyle);
  Style::setDefaultLineCap(RoundCap);
  Style::setDefaultLineJoin(RoundJoin);

  Group g;
  g << circle(10, 10, 20, Color::Blue, Color::Null);

  Group grid;
  grid << Line(0, 0, 100, 0, Color::Blue);
  grid << Line(0, 0, 0, -200, Color::Blue);

  Polyline l(Path::OpenPath, Color::Blue, Color::Null);
  l << Point(-10, 0) << Point(0, 0) << Point(0, 10);
  grid.addTiling(l, Point(0, 0), 10, 20, 0.0, IgnoreLineWidth);

  grid.rotateDeg(45);
  board << grid;

  Ellipse circle = LibBoard::circle(10, 10, 20, Color::Blue, Color::Null, 1.0);
  Rect bbox = board.boundingBox(UseLineWidth);
  Line separator(bbox.topRight(), bbox.bottomRight(), Color::Black);
  board.append(separator, ShapeList::Right, ShapeList::AlignCenter);
  board.append(circle, ShapeList::Right, ShapeList::AlignTop);
  board.append(separator, ShapeList::Right, ShapeList::AlignCenter);
  board.append(circle, ShapeList::Right, ShapeList::AlignCenter);
  board.append(separator, ShapeList::Right, ShapeList::AlignCenter);
  board.append(circle, ShapeList::Right, ShapeList::AlignBottom);

  bbox = board.boundingBox(UseLineWidth);
  separator = Line(bbox.bottomLeft(), bbox.bottomRight(), Color::Black);

  board.append(separator, ShapeList::Bottom, ShapeList::AlignLeft);
  board.append(circle, ShapeList::Bottom, ShapeList::AlignLeft);
  board.append(separator, ShapeList::Bottom, ShapeList::AlignLeft);
  board.append(circle, ShapeList::Bottom, ShapeList::AlignCenter);
  board.append(separator, ShapeList::Bottom, ShapeList::AlignLeft);
  board.append(circle, ShapeList::Bottom, ShapeList::AlignRight);
  board.append(separator, ShapeList::Bottom, ShapeList::AlignLeft);

  bbox = board.boundingBox(UseLineWidth);
  separator = Line(bbox.topRight(), bbox.bottomRight(), Color::Black);
  board.append(separator, ShapeList::Right, ShapeList::AlignCenter);

  board.append(LibBoard::grid(Point(0, 0), 12, 6, 120, 60, Color::Red, Color::Null, 0.5, SolidStyle, ButtCap, RoundJoin), ShapeList::Right, ShapeList::AlignCenter);

  board.saveEPS("tilings.eps");

  board.scaleToWidth(25, UseLineWidth);
  board.saveSVG("tilings.svg", Board::PageSize::BoundingBox, 0.0, Board::Unit::Centimeter);
}
