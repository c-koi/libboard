/*
 * @file   arithmetic.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Example that shows the usage of the Board library by
 *         illustating the arithmetic coding principle.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://foureys.users.greyc.fr>
 */
#include <Board.h>
using namespace std;
using namespace LibBoard;

int main(int argc, char * argv[])
{
  Board board;
  if (argc == 2 && !strcmp(argv[1], "-h")) {
    cout << "Usage:  arithmetic [word] \n\n"
            "        Where letters of word are in {a, b, c, d, e}\n\n";
    exit(0);
  }

  char the_string[1024] = "cccccccbb";
  if (argc == 2) {
    Tools::secured_strncpy(the_string, argv[1], 1024);
  }

  double probabilities[100] = {0.2, 0.1, 0.6, 0.05, 0.05};
  double lefts[100] = {0, 0.2, 0.3, 0.9, 0.95};
  Color colors[5] = {Color(0, 0, 255), Color(0, 255, 0), Color(255, 0, 0), Color(0, 255, 255), Color(255, 255, 0)};

  char * pc = the_string;
  int h = 0;
  double thickness = 0.5 / strlen(the_string);

  board.drawRectangle(0, 0, 1.0, 0.5);

  double left = 0;
  double width = 1.0;
  double w = 1.0;
  double n = 1;
  char str[20];

  board.setLineWidth(0.01 * thickness);
  board.setFontSize(0.7 * thickness);
  board.clear(Color(230, 230, 230));
  while (*pc) {
    int i = *pc - 'a';
    left += width * lefts[i];
    width *= probabilities[i];
    board.setPenColorRGBi(100, 100, 100);
    w /= 2.0;
    n *= 2;
    for (int k = 0; k < n; k++) {
      board.drawRectangle(k * w, -h * thickness, w, 0.9 * thickness);
    }
    board.setPenColor(colors[i]);
    board.drawRectangle(left, -h * thickness, width, 0.9 * thickness);
    secured_sprintf(str, 20, "%c", *pc);
    board.drawText((left + width / 2.0), -(h + 0.5) * thickness, str);
    ++h;
    ++pc;
  }

  // Draw the letters rectangles.
  for (int k = 0; k < 5; k++) {
    board.setPenColor(colors[k]);
    board.drawRectangle(lefts[k], thickness, probabilities[k], 0.9 * thickness);
  }

  // board.save("arithmetic_A4.eps", PageSize::A4, 0.5, Unit::Centimeter);
  // board.save("arithmetic.eps", PageSize::BoundingBox, 0.5, Unit::Centimeter);
  // board.save("arithmetic_A4.svg", PageSize::A4, 2, Unit::Centimeter);
  // board.save("arithmetic.fig", PageSize::A4);

  board.scaleToWidth(20, UseLineWidth);
  board.save("arithmetic.svg", PageSize::BoundingBox, 0.0, Unit::Centimeter);
}
