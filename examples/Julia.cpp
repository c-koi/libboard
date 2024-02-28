/**
 * @file   Julia.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Sample drawing using the board library.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <algorithm>
#include <board/Debug.h>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <set>
#include <vector>
using namespace LibBoard;
using namespace std;

using Complex = std::complex<double>;

inline double normalized(double phi)
{
  while (phi <= -M_PI) {
    phi += 2 * M_PI;
  }
  while (phi > M_PI) {
    phi -= 2 * M_PI;
  }
  return phi;
}

inline Point point(Complex z)
{
  return Point(40 * z.real(), 40 * z.imag());
}

Group julia(Complex c)
{
  Group result;
  std::vector<Complex> complexes;
  std::vector<Complex> new_complexes;
  const double STEP = 0.05;
  for (double alpha = M_PI; alpha > -M_PI; alpha -= STEP) {
    complexes.push_back(std::polar<double>(2, alpha));
  }
  int n = 6;
  while (n--) {
    Group g;
    Color cLine = Color::fromRGBf(Tools::boardRandDouble(0, 1), //
                                  Tools::boardRandDouble(0, 1), //
                                  Tools::boardRandDouble(0, 1));
    for (Complex & z : complexes) {
      Complex zc = z - c;
      Complex z2 = std::polar(std::sqrt(std::abs(zc)), normalized(std::arg(zc)) / 2.0);
      new_complexes.push_back(z2);
      new_complexes.push_back(-z2);
    }

    for (const Complex & zBefore : complexes) {
      const Point p1 = point(zBefore);
      double distanceMin = numeric_limits<double>::max();
      Point closest;
      for (const Complex & zAfter : new_complexes) {
        const Point p2 = point(zAfter);
        const double d = (p2 - p1).norm();
        if (d < distanceMin) {
          closest = p2;
          distanceMin = (p2 - p1).norm();
        }
      }
      g << Line(p1, closest, cLine);
    }

    complexes = new_complexes;
    std::cout << complexes.size() << std::endl;
    new_complexes.clear();
    Color color(Tools::boardRandDouble(0, 1), Tools::boardRandDouble(0, 1), Tools::boardRandDouble(0, 1));
    for (const Complex & z : complexes) {
      g << Dot(point(z), color, 0.2);
    }
    result << g;
  }
  return result;
}

int main(int, char *[])
{
  Board board;
  Style::setDefaultLineWidth(0.1);
  Tools::initBoardRand(time(nullptr));
  board << julia(Complex{0.25, 0.75});
  board.saveSVG("Julia.svg");
  // system("display Julia.svg");
}
