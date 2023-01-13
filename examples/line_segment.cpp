/**
 * @file   line_segment.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample drawing using the board library.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <cmath>
using namespace LibBoard;

const int PIXEL_WIDTH = 30;
const int IMAGE_HALF_SIDE = 15; // In pixels...

namespace
{

int myRound(double x)
{
  return (x >= 0.0) ? static_cast<int>(floor(x + 0.5)) : static_cast<int>(ceil(x - 0.5));
}

template <typename T> void mySwap(T & a, T & b)
{
  T tmp = a;
  a = b;
  b = tmp;
}
} // namespace

void drawPixel(Board & board, int x, int y)
{
  board.drawRectangle(PIXEL_WIDTH * (x - 0.5), PIXEL_WIDTH * (y + 0.5), PIXEL_WIDTH, PIXEL_WIDTH);
}

/*
 * Draws a rasterization of a line segment, i.e., the set of pixels
 * that intersect the segment (and the Euclidean line segment as well).
 */
void drawLine(Board & board, double x1, double y1, double x2, double y2)
{
  // Draw the Euclidean line in the real plane.
  board.setPenColorRGBi(255, 0, 0);
  board.drawArrow(x1 * PIXEL_WIDTH, y1 * PIXEL_WIDTH, x2 * PIXEL_WIDTH, y2 * PIXEL_WIDTH);
  board.fillCircle(x1 * PIXEL_WIDTH, y1 * PIXEL_WIDTH, 2);
  board.fillCircle(x2 * PIXEL_WIDTH, y2 * PIXEL_WIDTH, 2);
  board.setPenColorRGBi(0, 0, 0);

  // Draw the rasterized line.
  bool permute = false;
  double slope = (y2 - y1) / (x2 - x1);
  if (std::abs(slope) > 1) {
    permute = true;
    mySwap(x1, y1);
    mySwap(x2, y2);
    slope = (y2 - y1) / (x2 - x1);
  }
  if (myRound(x1) == myRound(x2)) {
    drawPixel(board, myRound(x1), myRound(y1));
    drawPixel(board, myRound(x2), myRound(y2));
    return;
  }
  int xStep = (x1 < x2) ? +1 : -1;
  int x = myRound(x1);
  int xStop = myRound(x2);
  // Draw pixel(s) for x=round(x1)
  if (permute) {
    drawPixel(board, myRound(y1), x);
  } else {
    drawPixel(board, x, myRound(y1));
  }
  int y = myRound(slope * ((x + xStep * 0.5) - x1) + y1);
  if (y != myRound(y1)) {
    if (permute) {
      drawPixel(board, y, x);
    } else {
      drawPixel(board, x, y);
    }
  }
  // Traverse all xs
  if (permute) {
    do {
      x += xStep;
      int yA = myRound(slope * ((x - 0.5) - x1) + y1);
      drawPixel(board, yA, x);
      int yB = myRound(slope * ((x + 0.5) - x1) + y1);
      if (yB != yA) {
        drawPixel(board, yB, x);
      }
    } while (x != xStop);
  } else {
    do {
      x += xStep;
      int yA = myRound(slope * ((x - 0.5) - x1) + y1);
      drawPixel(board, x, yA);
      int yB = myRound(slope * ((x + 0.5) - x1) + y1);
      if (yB != yA) {
        drawPixel(board, x, yB);
      }
    } while (x != xStop);
  }
}

int main(int, char *[])
{
  Board board;
  const int side = PIXEL_WIDTH * IMAGE_HALF_SIDE;

  board.setPenColorRGBi(0, 0, 0);
  board.setLineWidth(1.0);
  board.setLineCap(RoundCap);
  board.drawLine(-side, 0, side, 0);
  board.drawLine(0, -side, 0, side);

  // Slope < 1, forward and backward
  drawLine(board, -12.25, 1.2, 5.34, 9.23);
  drawLine(board, 0.3, -3.2, 8.4, -6.7);
  drawLine(board, -2.3, -3.2, -12.4, -7.7);
  // Slope > 1
  drawLine(board, 7.6, 1.2, 10.5, 12.1);

  board.saveEPS("line_segment.eps");

  board.scaleToWidth(25, UseLineWidth);
  board.saveSVG("line_segment.svg", PageSize::BoundingBox, 0.0, Unit::Centimeter);
}
