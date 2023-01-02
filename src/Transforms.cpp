/* -*- mode: c++ -*- */
/**
 * @file   Transforms.cpp
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

#include <board/Transforms.h>
#include <cmath>
#include <BoardConfig.h>
#include <board/Rect.h>
#include <board/Shape.h>
#include <board/ShapeList.h>

namespace
{
const double ppmm = 72.0 / 25.4;
const double fig_ppmm = 1143 / 25.4;
} // namespace

namespace LibBoard
{

//
// Transform
//

double Transform::rounded(double x) const
{
  return Transform::round(1000000 * x) / 1000000;
}

Transform::~Transform() {}

double Transform::mapX(double x) const
{
  return rounded(x * _scale + _deltaX);
}

Point Transform::map(const Point & point) const
{
  return Point(mapX(point.x), mapY(point.y));
}

double Transform::scale(double x) const
{
  return rounded(x * _scale);
}

Point Transform::scale(const Point & p) const
{
  return Point(p.x * _scale, p.y * _scale);
}

void Transform::apply(double & x, double & y) const
{
  x = mapX(x);
  y = mapY(y);
}

//
// TransformEPS
//

double TransformEPS::mapY(double y) const
{
  return rounded(y * _scale + _deltaY);
}

double TransformEPS::mapWidth(double w) const
{
  return w * _scale;
}

void TransformEPS::setBoundingBox(const Rect & rect, const double pageWidth, const double pageHeight, const double margin)
{
  Point c = rect.center();
  if (margin < 0.0) {
    if ((rect.height / rect.width) > (pageHeight / pageWidth)) {
      _scale = pageHeight * ppmm / rect.height;
    } else {
      _scale = pageWidth * ppmm / rect.width;
    }
    _deltaX = (0.5 * pageWidth + (-margin)) * ppmm - _scale * c.x;
    _deltaY = (0.5 * pageHeight + (-margin)) * ppmm - _scale * c.y;
    _height = (pageHeight + 2 * (-margin)) * ppmm;
    _pageBoundingBox = Rect(mapX(rect.left) - (-margin) * ppmm, mapY(rect.top) + (-margin) * ppmm, scale(rect.width) + 2 * (-margin) * ppmm, _height);
  } else {
    const double w = pageWidth - 2 * margin;
    const double h = pageHeight - 2 * margin;
    if ((rect.height / rect.width) > (h / w)) {
      _scale = h * ppmm / rect.height;
    } else {
      _scale = w * ppmm / rect.width;
    }
    _deltaX = 0.5 * pageWidth * ppmm - _scale * c.x;
    _deltaY = 0.5 * pageHeight * ppmm - _scale * c.y;
    _height = pageHeight * ppmm;
    _pageBoundingBox = Rect(0, _height, pageWidth * ppmm, pageHeight * ppmm);
  }
}

double TransformEPS::scaleBackMM(double x)
{
  return x / _scale;
}

Rect TransformEPS::pageBoundingBox() const
{
  return _pageBoundingBox;
}

//
// TransformFIG
//

double TransformFIG::rounded(double x) const
{
  return Transform::round(x);
}

double TransformFIG::mapY(double y) const
{
  return rounded(_height - (y * _scale + _deltaY));
}

int TransformFIG::mapWidth(double width) const
{
  // FIG thickness unit is 1/80 inch, reduced to 1/160 when exporting to EPS.
  // Postscript points are 1/72 inch
  if (width == 0.0) {
    return 0;
  }
  int result = static_cast<int>(Transform::round(80 * (width * _postscriptScale / 72.0)));
  return result > 0 ? result : 1;
}

void TransformFIG::setBoundingBox(const Rect & rect, const double pageWidth, const double pageHeight, const double margin)
{
  Point c = rect.center();
  if (margin < 0.0) {
    // Bounding box
    if (rect.height / rect.width > (pageHeight / pageWidth)) {
      _scale = pageHeight * fig_ppmm / rect.height;
      _postscriptScale = pageHeight * ppmm / rect.height;
    } else {
      _scale = pageWidth * fig_ppmm / rect.width;
      _postscriptScale = pageWidth * ppmm / rect.width;
    }
    _deltaX = (0.5 * pageWidth + (-margin)) * fig_ppmm - _scale * c.x;
    _deltaY = (0.5 * pageHeight + (-margin)) * fig_ppmm - _scale * c.y;
    _height = (pageHeight + 2 * (-margin)) * fig_ppmm;
  } else {
    // Fit to pageWidth/pageHeight
    const double w = pageWidth - 2 * margin;
    const double h = pageHeight - 2 * margin;
    if (rect.height / rect.width > (h / w)) {
      _scale = (h * fig_ppmm) / rect.height;
      _postscriptScale = h * ppmm / rect.height;
    } else {
      _scale = (w * fig_ppmm) / rect.width;
      _postscriptScale = w * ppmm / rect.width;
    }
    _deltaX = 0.5 * pageWidth * fig_ppmm - _scale * c.x;
    _deltaY = 0.5 * pageHeight * fig_ppmm - _scale * c.y;
    _height = pageHeight * fig_ppmm;
  }
  // float ppmm = (1200/25.4);
}

unsigned int TransformFIG::shapeDepth(const Shape * shape) const
{
  if (_depthMap) {
    auto it = _depthMap->find(shape);
    if (it != _depthMap->end()) {
      return mapDepth(it->second);
    }
  }
  return 50;
}

unsigned int TransformFIG::mapDepth(unsigned int depth) const
{
  if (depth > _maxDepth) {
    return 999;
  }
  if (_maxDepth - _minDepth > 998) {
    const double range = _maxDepth - _minDepth;
    const unsigned int r = static_cast<unsigned int>(1 + Transform::round(((depth - _minDepth) / range) * 998));
    return r; // r >= 0 ? r : 0;
  } else {
    const unsigned int r = 1 + depth - _minDepth;
    return r; // r >= 0 ? r : 0; // TODO : Check this
  }
}

void TransformFIG::setDepthMap(const std::map<const Shape *, unsigned int> * depthMap, unsigned int min)
{
  _depthMap = depthMap;
  _maxDepth = std::numeric_limits<unsigned int>::max();
  _minDepth = min;
}

//
// TransformSVG
//

double TransformSVG::rounded(double x) const
{
  return Transform::round(100000.0 * x) / 100000.0;
}

double TransformSVG::mapY(double y) const
{
  return rounded(_height - (y * _scale + _deltaY));
}

double TransformSVG::mapWidth(double width) const
{
  return Transform::round(_scale * 1000 * width / ppmm) / 1000.0;
}

void TransformSVG::setBoundingBox(const Rect & rect, const double pageWidth, const double pageHeight, const double margin)
{
  Point c = rect.center();
  if (margin < 0.0) {
    if (rect.height / rect.width > (pageHeight / pageWidth)) {
      _scale = pageHeight * ppmm / rect.height;
    } else {
      _scale = pageWidth * ppmm / rect.width;
    }
    _deltaX = (0.5 * pageWidth + (-margin)) * ppmm - _scale * c.x;
    _deltaY = (0.5 * pageHeight + (-margin)) * ppmm - _scale * c.y;
    _height = (pageHeight + 2 * (-margin)) * ppmm;
  } else {
    const double w = pageWidth - 2 * margin;
    const double h = pageHeight - 2 * margin;
    if (rect.height / rect.width > (h / w)) {
      _scale = h * ppmm / rect.height;
    } else {
      _scale = w * ppmm / rect.width;
    }
    _deltaX = 0.5 * pageWidth * ppmm - _scale * c.x;
    _deltaY = 0.5 * pageHeight * ppmm - _scale * c.y;
    _height = pageHeight * ppmm;
  }
}

double TransformSVG::scaleBackMM(double x)
{
  return x / _scale;
}

TransformMatrix TransformSVG::matrix() const
{
  return TransformMatrix(_scale, 0, _deltaX, 0, -_scale, _height - _deltaY);
}

Point TransformSVG::translation() const
{
  return Point(_deltaX, (_height - _deltaY));
}

double TransformSVG::deltaX() const
{
  return _deltaX;
}

double TransformSVG::deltaY() const
{
  return _height - _deltaY;
}

TransformTikZ::~TransformTikZ() {}

} // namespace LibBoard
