/* -*- mode: c++ -*- */
/**
 * @file   PathBoundaries.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   Nov 2008
 *
 * @brief
 * \@copyright
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <https://foureys.users.greyc.fr/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <BoardConfig.h>
#include <board/PathBoundaries.h>
#include <board/Shape.h>
namespace LibBoard
{

namespace Tools
{

bool isASharpCorner(Point p1, Point p2, Point p3)
{
  return (p3 - p2) * (p2 - p1) < 0.0;
}

double sharpCornerAngle(Point p1, Point p2, Point p3)
{
  Point v1 = (p1 - p2).normalised();
  Point v2 = (p3 - p2).normalised();
  return acos(v1 * v2);
}

Point intersection(EuclideanLine l1, EuclideanLine l2)
{
  double det = l1.a * l2.b - l2.a * l1.b;
  if (fabs(det) < 1.0 / 256) {
    return Point::Infinity;
  }
  Point res;
  //
  //   AX=Y   X = A^{1}Y
  //
  //      [ l1.a  l1.b ]    [ res.x ]    [ -l1.c ]
  //  A = [ l2.a  l2.b ]  X=[ res.y ]  Y=[ -l2.c ]
  //
  //                                                         [  l2.b  -l1.b ]
  //  A^{-1} = (1/det(A))*tranpose(comatrix(A)) = (1/det(A))*[ -l2.a   l1.a ]
  //
  res.x = (-l1.c * l2.b + l2.c * l1.b) / det;
  res.y = (l1.c * l2.a - l2.c * l1.a) / det;
  return res;
}

Point exteriorMiterIntersection(Point p1, Point p2, Point p3, double strokeWidth)
{
  Point va(p2 - p1);
  Point vb(p3 - p2);
  Point shiftA = 0.5 * strokeWidth * va.rotatedPI2().normalise();
  Point shiftB = 0.5 * strokeWidth * vb.rotatedPI2().normalise();
  if (va.rotatedPI2() * vb >= 0.0) {
    // Left turn
    shiftA = -shiftA;
    shiftB = -shiftB;
  }
  EuclideanLine la(p1 + shiftA, p2 + shiftA);
  EuclideanLine lb(p2 + shiftB, p3 + shiftB);
  Point result = intersection(la, lb);
  if (result.isInf()) {
    return p2;
  }
  return result;
}

Point interiorMiterIntersection(Point p1, Point p2, Point p3, double strokeWidth)
{
  Point va(p2 - p1);
  Point vb(p3 - p2);
  Point shiftA = 0.5 * strokeWidth * va.rotatedPI2().normalise();
  Point shiftB = 0.5 * strokeWidth * vb.rotatedPI2().normalise();
  if (va.rotatedPI2() * vb >= 0.0) {
    // Left turn
    shiftA = -shiftA;
    shiftB = -shiftB;
  }
  EuclideanLine la(p1 - shiftA, p2 - shiftA);
  EuclideanLine lb(p2 - shiftB, p3 - shiftB);
  return intersection(la, lb);
}

void exteriorBevelIntersection(Point p1, Point p2, Point p3, double strokeWidth, Point & a, Point & b)
{
  Point va(p2 - p1);
  Point vb(p3 - p2);
  Point shiftA, shiftB;
  if (va.rotatedPI2() * vb > 0.0) {
    // Left turn
    shiftA = -0.5 * strokeWidth * va.rotatedPI2().normalise();
    shiftB = -0.5 * strokeWidth * vb.rotatedPI2().normalise();
  } else {
    // Right turn
    shiftA = 0.5 * strokeWidth * va.rotatedPI2().normalise();
    shiftB = 0.5 * strokeWidth * vb.rotatedPI2().normalise();
  }
  a = p2 + shiftA;
  b = p2 + shiftB;
}

std::vector<Point> exteriorRoundIntersection(Point p1, Point p2, Point p3, double strokeWidth)
{
  std::vector<Point> result;
  Point va(p2 - p1);
  Point vb(p3 - p2);
  Point shiftA, shiftB;
  bool leftTurn = (va.rotatedPI2() * vb > 0.0);
  if (leftTurn) {
    shiftA = -0.5 * strokeWidth * va.rotatedPI2().normalise();
    shiftB = -0.5 * strokeWidth * vb.rotatedPI2().normalise();
  } else {
    shiftA = 0.5 * strokeWidth * va.rotatedPI2().normalise();
    shiftB = 0.5 * strokeWidth * vb.rotatedPI2().normalise();
  }
  double angleFirst = leftTurn ? shiftA.normalised().argument() : shiftB.normalised().argument();
  double angleSecond = leftTurn ? shiftB.normalised().argument() : shiftA.normalised().argument();

  if ((angleFirst >= 0 && angleSecond >= 0 && angleFirst >= angleSecond) || (angleFirst <= 0 && angleSecond <= 0 && angleFirst >= angleSecond) || (angleFirst < 0 && angleSecond >= 0) ||
      (angleFirst <= 0 && angleSecond > 0)) {
    result.push_back(p2 + Point(0.5 * strokeWidth, 0));
  }

  angleFirst -= M_PI_2;
  angleSecond -= M_PI_2;
  if (angleFirst < -M_PI)
    angleFirst += 2 * M_PI;
  if (angleSecond < -M_PI)
    angleSecond += 2 * M_PI;
  if ((angleFirst >= 0 && angleSecond >= 0 && angleFirst >= angleSecond) || (angleFirst <= 0 && angleSecond <= 0 && angleFirst >= angleSecond) || (angleFirst < 0 && angleSecond >= 0) ||
      (angleFirst <= 0 && angleSecond > 0)) {
    result.push_back(p2 + Point(0, 0.5 * strokeWidth));
  }

  angleFirst -= M_PI_2;
  angleSecond -= M_PI_2;
  if (angleFirst < -M_PI)
    angleFirst += 2 * M_PI;
  if (angleSecond < -M_PI)
    angleSecond += 2 * M_PI;
  if ((angleFirst >= 0 && angleSecond >= 0 && angleFirst >= angleSecond) || (angleFirst <= 0 && angleSecond <= 0 && angleFirst >= angleSecond) || (angleFirst < 0 && angleSecond >= 0) ||
      (angleFirst <= 0 && angleSecond > 0)) {
    result.push_back(p2 + Point(-0.5 * strokeWidth, 0));
  }

  angleFirst -= M_PI_2;
  angleSecond -= M_PI_2;
  if (angleFirst < -M_PI)
    angleFirst += 2 * M_PI;
  if (angleSecond < -M_PI)
    angleSecond += 2 * M_PI;
  if ((angleFirst >= 0 && angleSecond >= 0 && angleFirst >= angleSecond) || (angleFirst <= 0 && angleSecond <= 0 && angleFirst >= angleSecond) || (angleFirst < 0 && angleSecond >= 0) ||
      (angleFirst <= 0 && angleSecond > 0)) {
    result.push_back(p2 + Point(0, -0.5 * strokeWidth));
  }
  return result;
}

void butCapExtremities(Point p1, Point p2, double strokeWidth, Point & a, Point & b)
{
  Point v(p2 - p1);
  Point shift;
  shift = 0.5 * strokeWidth * v.rotatedPI2().normalise();
  a = p2 + shift;
  b = p2 - shift;
}

void squareCapExtremities(Point p1, Point p2, double strokeWidth, Point & a, Point & b)
{
  Point v(p2 - p1);
  Point halfShift = 0.5 * strokeWidth * v.normalised();
  Point shift;
  shift = 0.5 * strokeWidth * v.rotatedPI2().normalise();
  a = p2 + shift + halfShift;
  b = p2 - shift + halfShift;
}

std::vector<Point> roundCapExtremities(Point p1, Point p2, double strokeWidth)
{
  std::vector<Point> result;
  Point halfX(strokeWidth * 0.5, 0);
  Point halfY(0.0, strokeWidth * 0.5);
  Point v(p2 - p1);
  v.normalise();

  if (v * Point(1.0, 0.0) >= 0.0) {
    result.push_back(p2 + halfX);
  }
  if (v * Point(-1.0, 0.0) >= 0.0) {
    result.push_back(p2 - halfX);
  }
  if (v * Point(0.0, 1.0) >= 0.0) {
    result.push_back(p2 + halfY);
  }
  if (v * Point(0.0, -1.0) >= 0.0) {
    result.push_back(p2 - halfY);
  }
  return result;
}

std::vector<Point> pathBoundaryPoints(const Path & path, double strokeWidth, LineCap lineCap, LineJoin lineJoin, double miterLimit)
{
  if (strokeWidth == 0.0) {
    return path.points();
  }
  std::vector<Point> result;
  std::vector<Point> unclosedPath = path.points();
  if (path.isClosed()) {
    while (unclosedPath.size() > 1 && (unclosedPath.front() == unclosedPath.back())) {
      unclosedPath.pop_back();
    }
  }
  if (unclosedPath.size() == 1) {
    Point p = unclosedPath.front();
    double shift = strokeWidth * 0.5;
    result.push_back(Point(p.x - shift, p.y + shift));
    result.push_back(Point(p.x + shift, p.y + shift));
    result.push_back(Point(p.x + shift, p.y - shift));
    result.push_back(Point(p.x - shift, p.y - shift));
    return result;
  }
  std::vector<Point> simplePath;
  simplePath.push_back(unclosedPath.front());
  for (size_t i = 1; i < unclosedPath.size(); ++i) {
    if (unclosedPath[i] != simplePath.back()) {
      simplePath.push_back(unclosedPath[i]);
    }
  }
  if (unclosedPath.size() == 1) {
    Point p = unclosedPath.front();
    double shift = strokeWidth * 0.5;
    result.push_back(Point(p.x - shift, p.y + shift));
    result.push_back(Point(p.x + shift, p.y + shift));
    result.push_back(Point(p.x + shift, p.y - shift));
    result.push_back(Point(p.x - shift, p.y - shift));
    return result;
  }

  if (simplePath.size() == 1) { // TODO Handle this case
    result.push_back(simplePath[0]);
    return result;
  }
  size_t limit = path.isClosed() ? simplePath.size() : (simplePath.size() - 2);
  for (size_t i = 0; i < limit; ++i) {
    Point p0 = simplePath[i];
    Point p1 = simplePath[(i + 1) % simplePath.size()];
    Point p2 = simplePath[(i + 2) % simplePath.size()];
    switch (lineJoin) {
    case MiterJoin: {
      Point exterior = exteriorMiterIntersection(p0, p1, p2, strokeWidth);
      // Point interior = interiorMiterIntersection(p0,p1,p2,strokeWidth);
      // const double miterLength = (exterior-interior).norm();
      // const double ratio = miterLength / strokeWidth;
      // miterLength / stroke-width = 1 / sin ( theta / 2 )
      const double ratio = (1 / sin(sharpCornerAngle(p0, p1, p2) / 2.0));
      if (isASharpCorner(p0, p1, p2)) {
        if (ratio > miterLimit) { // Fallback to BevelJoin
          Point a, b;
          exteriorBevelIntersection(p0, p1, p2, strokeWidth, a, b);
          result.push_back(a);
          result.push_back(b);
        } else {
          result.push_back(exterior);
        }
      } else {
        result.push_back(exterior);
      }
    } break;
    case BevelJoin: {
      Point a, b;
      exteriorBevelIntersection(p0, p1, p2, strokeWidth, a, b);
      result.push_back(a);
      result.push_back(b);
    } break;
    case RoundJoin: {
      std::vector<Point> points = exteriorRoundIntersection(p0, p1, p2, strokeWidth);
      std::copy(points.begin(), points.end(), std::back_inserter(result));
    } break;
    }
  }

  // Extremities if not closed
  if (!path.isClosed()) {
    Point a, b;
    switch (lineCap) {
    case ButtCap: {
      Point p = simplePath[1];
      Point q = simplePath[0];
      butCapExtremities(p, q, strokeWidth, a, b);
      result.push_back(a);
      result.push_back(b);
      p = simplePath[simplePath.size() - 2];
      q = simplePath[simplePath.size() - 1];
      butCapExtremities(p, q, strokeWidth, a, b);
      result.push_back(a);
      result.push_back(b);
    } break;
    case SquareCap: {
      Point p = simplePath[1];
      Point q = simplePath[0];
      squareCapExtremities(p, q, strokeWidth, a, b);
      result.push_back(a);
      result.push_back(b);
      p = simplePath[simplePath.size() - 2];
      q = simplePath[simplePath.size() - 1];
      squareCapExtremities(p, q, strokeWidth, a, b);
      result.push_back(a);
      result.push_back(b);
    } break;
    case RoundCap: {
      std::vector<Point> points;
      Point p = simplePath[1];
      Point q = simplePath[0];
      points = roundCapExtremities(p, q, strokeWidth);
      std::copy(points.begin(), points.end(), std::back_inserter(result));
      p = simplePath[simplePath.size() - 2];
      q = simplePath[simplePath.size() - 1];
      points = roundCapExtremities(p, q, strokeWidth);
      std::copy(points.begin(), points.end(), std::back_inserter(result));
    } break;
    }
  }
  return result;
}

Rect pathBoundingBox(const Path & path, double strokeWidth, LineCap lineCap, LineJoin lineJoin, double miterLimit)
{
  if (strokeWidth == 0.0) {
    return path.boundingBox();
  }
  std::vector<Point> v = pathBoundaryPoints(path, strokeWidth, lineCap, lineJoin, miterLimit);
  std::vector<Point>::iterator it = v.begin();
  if (it == v.end()) {
    Tools::warning << "Computing the bounding box of an empty path makes no sense";
    return Rect();
  }
  Rect result(*it);
  while (it != v.end()) {
    result.growToContain(*it++);
  }
  return result;
}

} // namespace Tools

} // namespace LibBoard
