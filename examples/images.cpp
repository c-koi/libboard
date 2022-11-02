/**
 * @file   images.cpp
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

#if (BOARD_HAVE_MAGICKPLUSPLUS == 1)
  Image michel("../resources/saint_michel.jpg", 0, 0, 200);
  Polyline rectangle = LibBoard::rectangle(michel.boundingBox(IgnoreLineWidth), Color::Red, Color::Silver, 1);
  Group g;
  g << rectangle;
  g << michel;

  ShapeList tiling;
  tiling.addTiling(g, Point(0, 0), 5, 4, 0.0, UseLineWidth);
  board << tiling;

  ShapeList avatars;
  Image avatar("../resources/avatar.png", 0, 0, 80);
  avatar.rotateDeg(45);
  avatars.addTiling(avatar, Point(0, 0), 4, 4, 10.0, UseLineWidth);
  avatars.moveCenter(board.center());
  board << avatars;
#else
  Text text(10, -40, "Magick++ is required", Fonts::Helvetica, 10);
  board << LibBoard::rectangle(text.boundingBox(LibBoard::IgnoreLineWidth).grow(5), Color::Red, Color::White, 1.0, SolidStyle, RoundCap, RoundJoin);
  board << text;
#endif

  board.saveEPS("images.eps", 20.0, 30.0, 2.0, Unit::Inch);
  // board.saveEPS( "images.eps", PageSize::BoundingBox, 10.0, Board::UInche );
  board.saveSVG("images_40x60cm.svg", 40.0, 60.0, 10.0, Unit::Centimeter);
  board.saveFIG("images.fig", 20.0, 20.0, 2.0, Unit::Centimeter);
  // board.saveFIG( "images.fig", PageSize::BoundingBox, 10.0, Unit::Centimeter );

  board.scaleToWidth(25, UseLineWidth);
  board.saveSVG("images.svg", PageSize::BoundingBox, 0.0, Unit::Centimeter);
}
