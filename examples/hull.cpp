/**
 * @file   hull.cpp
 * @author Sebastien Fourey (GREYC)
 *
 * @brief  Implementation of a naive 2D convex hull algorithm.
 *
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://fourey.users.greyc.fr>
 */
#include <Board.h>
#include <board/Tools.h>
#include <vector>
using namespace LibBoard;

/**
 * @brief isSeparing
 *
 * Check whether a segment [a,b] is on a line separating a vector of points.
 * If not, the points a and b may be swapped so that all points
 * are "on the left" of the vector a->b
 *
 * @param a
 * @param b
 * @param points
 * @return true if the segment [a,b] is on a line separating the given points, otherwise false.
 */
bool isSeparing(Point & a, Point & b, const std::vector<Point> & points)
{
  int sign = 0;
  Point v1 = b - a;
  for (Point p : points) {
    if (p != a && p != b) {
      Point v2 = p - a;
      double wp = v1.x * v2.y - v1.y * v2.x;
      if (std::abs(wp) < 1e-8) {
        if ((a - p) * (b - p) < 0.0) {
          // a, p and b are aligned (in that order)
          return true;
        }
      }
      if (((sign > 0) && (wp < 0.0)) || ((sign < 0) && (wp > 0.0))) {
        return true;
      }
      sign = (wp > 0.0) ? 1 : -1;
    }
  }
  if (sign == -1) {
    std::swap(a, b);
  }
  return false;
}

int main(int, char *[])
{
  Board board;

  Style::setDefaultLineWidth(0.5);
  Style::setDefaultPenColor(Color::Blue);
  Style::setDefaultFillColor(Color::Null);

  std::vector<Point> points;
  int n = 25;
  while (n--) {
    points.push_back(Point(4 * (Tools::boardRand() % 50), 4 * (Tools::boardRand() % 50)));
  }

  typedef std::pair<Point, Point> Segment;
  std::vector<Segment> segments;

  // Compute the set of separating segments
  for (size_t a = 0; a < points.size(); ++a) {
    for (size_t b = a + 1; b < points.size(); ++b) {
      Point pa = points[a];
      Point pb = points[b];
      if (!isSeparing(pa, pb, points)) {
        segments.push_back(Segment(pa, pb));
        // board << Line(pa, pb, Color::Red, 0.1);
      }
    }
  }

  // Build a polyline using the set of segments
  Segment segment = segments.front();
  std::vector<Point> polyline;
  const Point stop = segment.first;
  do {
    polyline.push_back(segment.first);
    for (const Segment & s : segments) {
      if (s.first == segment.second) {
        segment = s;
        break;
      }
    }
  } while (segment.first != stop);

  board << Polyline(polyline, Path::Closed, Color::Cyan, Color("#a0a0c0"), 0.5);

  for (Point p : points) {
    board << Dot(p.x, p.y, Color::Blue, 1.0);
  }

  board.saveSVG("hull.svg");
}
