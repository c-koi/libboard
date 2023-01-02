/* -*- mode: c++ -*- */
/**
 * @file   Shape.cpp
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
#include <board/Shape.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <sstream>
#include <vector>
#include <BoardConfig.h>
#include <board/PSFonts.h>
#include <board/PathBoundaries.h>
#include <board/Rect.h>
#include <board/ShapeVisitor.h>
#include <board/Tools.h>
#include <board/Transforms.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

namespace LibBoard
{

extern const char * XFigPostscriptFontnames[];

const std::string Shape::_name("AbstractShape");

Shape::~Shape() {}

const std::string & Shape::name() const
{
  return _name;
}

Point Shape::center(LineWidthFlag lineWidthFlag) const
{
  return boundingBox(lineWidthFlag).center();
}

Shape & Shape::moveCenter(double x, double y, LineWidthFlag lineWidthFlag)
{
  Point c = center(lineWidthFlag);
  translate(x - c.x, y - c.y);
  return *this;
}

Shape & Shape::moveCenter(Point p, LineWidthFlag lineWidthFlag)
{
  Point c = center(lineWidthFlag);
  translate(p.x - c.x, p.y - c.y);
  return *this;
}

Shape & Shape::resize(double width, double height, LineWidthFlag lineWidthFlag)
{
  Rect bbox = boundingBox(lineWidthFlag);
  scale(width / bbox.width, height / bbox.height);
  return *this;
}

Shape & Shape::scaleToWidth(double w, LineWidthFlag lineWidthFlag)
{
  double factor = w / boundingBox(lineWidthFlag).width;
  scale(factor);
  return *this;
}

Shape & Shape::scaleToHeight(double h, LineWidthFlag lineWidthFlag)
{
  double factor = h / boundingBox(lineWidthFlag).height;
  scale(factor);
  return *this;
}

void Shape::accept(ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Shape::accept(const ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Shape::accept(ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

void Shape::accept(const ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

Shape * Shape::accept(CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Shape * Shape::accept(const CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Shape::Shape(const Shape &) {}

} // namespace LibBoard
