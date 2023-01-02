/* -*- mode: c++ -*- */
/**
 * @file   Rect.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
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
#include <board/Rect.h>
#include <BoardConfig.h>

namespace LibBoard
{

Rect operator||(const Rect & rectA, const Rect & rectB)
{
  if (rectA.isNull() && rectB.isNull()) {
    return Rect();
  } else if (rectA.isNull()) {
    return rectB;
  } else if (rectB.isNull()) {
    return rectA;
  }
  Rect rect;
  rect.top = (rectA.top > rectB.top) ? rectA.top : rectB.top;
  rect.left = (rectA.left < rectB.left) ? rectA.left : rectB.left;
  if (rectA.left + rectA.width > rectB.left + rectB.width) {
    rect.width = rectA.left + rectA.width - rect.left;
  } else {
    rect.width = rectB.left + rectB.width - rect.left;
  }
  if (rectA.top - rectA.height < rectB.top - rectB.height) {
    rect.height = rect.top - (rectA.top - rectA.height);
  } else {
    rect.height = rect.top - (rectB.top - rectB.height);
  }
  return rect;
}

Rect operator&&(const Rect & rectA, const Rect & rectB)
{
  Rect rect;
  rect.top = (rectA.top < rectB.top) ? rectA.top : rectB.top;
  rect.left = (rectA.left > rectB.left) ? rectA.left : rectB.left;
  if (rectA.left + rectA.width < rectB.left + rectB.width)
    rect.width = rectA.left + rectA.width - rect.left;
  else
    rect.width = rectB.left + rectB.width - rect.left;
  if (rectA.top - rectA.height > rectB.top - rectB.height)
    rect.height = rect.top - (rectA.top - rectA.height);
  else
    rect.height = rect.top - (rectB.top - rectB.height);
  if (rect.height < 0)
    rect.height = 0;
  if (rect.width < 0)
    rect.width = 0;
  return rect;
}

Rect & Rect::growToContain(const Point & p)
{
  if (p.x < left) {
    double dw = left - p.x;
    left = p.x;
    width += dw;
  } else if (p.x > left + width) {
    width = p.x - left;
  }
  if (p.y > top) {
    double dh = p.y - top;
    top = p.y;
    height += dh;
  } else if (p.y < top - height) {
    height = top - p.y;
  }
  return *this;
}

Rect & Rect::growToContain(const std::vector<Point> & points)
{
  for (const Point & point : points) {
    growToContain(point);
  }
  return *this;
}

bool Rect::contains(Point p) const
{
  return p.x >= left && p.x <= left + width && p.y <= top && p.y >= top - height;
}

bool Rect::strictlyContains(Point p) const
{
  return p.x > left && p.x < left + width && p.y < top && p.y > top - height;
}

bool Rect::intersects(const Rect & other) const
{
  return contains(other.topLeft()) || contains(other.topRight()) || contains(other.bottomLeft()) || contains(other.bottomRight()) //
         || other.contains(topLeft()) || other.contains(topRight()) || other.contains(bottomLeft()) || other.contains(bottomRight());
}

bool Rect::strictlyIntersects(const Rect & other) const
{
  return strictlyContains(other.topLeft()) || strictlyContains(other.topRight()) || strictlyContains(other.bottomLeft()) || strictlyContains(other.bottomRight()) //
         || other.strictlyContains(topLeft()) || other.strictlyContains(topRight()) || other.strictlyContains(bottomLeft()) || other.strictlyContains(bottomRight());
}

Rect & Rect::grow(double margin)
{
  top += margin;
  left -= margin;
  width += 2 * margin;
  height += 2 * margin;
  return *this;
}

Rect Rect::growed(double margin)
{
  return Rect(*this).grow(margin);
}

} // namespace LibBoard

std::ostream & operator<<(std::ostream & out, const LibBoard::Rect & rect)
{
  out << "Rect(" << rect.left << "," << rect.top << "+" << rect.width << "x" << rect.height << ")";
  return out;
}
