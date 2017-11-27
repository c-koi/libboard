/* -*- mode: c++ -*- */
/**
 * @file   ShapeVisitor.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   June 2016
 *
 * @brief
 * \@copyright
 * This source code is part of the Board project, a C++ library whose
 * purpose is to allow simple drawings in EPS, FIG or SVG files.
 * Copyright (C) 2007 Sebastien Fourey <http://foureys.users.greyc.fr/>
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
#include "ShapeVisitor.h"
#include "Board.h"
#include "board/Shapes.h"
#include <iostream>

namespace LibBoard {

BoundingBoxExtractor::BoundingBoxExtractor( LibBoard::ShapeList & shapeList )
  :_shapeList(shapeList)
{
}

void BoundingBoxExtractor::visit(const Shape & shape)
{
  _shapeList << Rectangle(shape.boundingBox(Board::UseLineWidth),Color::Black,Color::Null);
}

void BoundingBoxExtractor::visit(const Shape &) const
{
  Tools::warning << "BoundingBoxExtractor(): Visiting using the const method does not make sense.\n";
}

const ShapeList & BoundingBoxExtractor::shapeList() const
{
  return _shapeList;
}

void BoundingBoxViewer::visit(const Shape & shape)
{
  std::cout << shape.name() << " - " << shape.boundingBox(Board::UseLineWidth) << " " << std::endl;
}

void BoundingBoxViewer::visit(const Shape &shape) const
{
  std::cout << shape.name() << " - " << shape.boundingBox(Board::UseLineWidth) << std::endl;
}

ShapeCounter::ShapeCounter()
  :_count(0)
{
}

void ShapeCounter::clear()
{
  _count = 0;
}

std::size_t ShapeCounter::value() const
{
  return _count;
}

void ShapeCounter::visit(const Shape & shape)
{
  if ( typeid(shape) != typeid(Group) ) {
    ++_count;
  }
}

void ShapeCounter::visit(const Shape & ) const
{
  Tools::warning << "ShapeCounter(): Visiting using the const method does not make sense.\n";
}

}
