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
#include <board/ShapeVisitor.h>
#include <iostream>
#include <Board.h>
#include <board/Shape.h>

namespace LibBoard
{

BoundingBoxExtractor::BoundingBoxExtractor(LibBoard::ShapeList & shapeList) : _shapeList(shapeList) {}

void BoundingBoxExtractor::visit(const Shape & shape)
{
  _shapeList << LibBoard::rectangle(shape.boundingBox(UseLineWidth), Color::Black, Color::Null);
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
  using ::operator<<;
  std::cout << shape.name() << " - " << shape.boundingBox(UseLineWidth) << std::endl;
}

void BoundingBoxViewer::visit(const Shape & shape) const
{
  using ::operator<<;
  std::cout << shape.name() << " - " << shape.boundingBox(UseLineWidth) << std::endl;
}

ShapeCounter::ShapeCounter() : _count(0) {}

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
  if (typeid(shape) != typeid(Group)) {
    ++_count;
  }
}

void ShapeCounter::visit(const Shape &) const
{
  Tools::warning << "ShapeCounter(): Visiting using the const method does not make sense.\n";
}

void ShapeVisitor::visit(ShapeWithStyle & shape)
{
  visit(static_cast<Shape &>(shape));
}

void ShapeVisitor::visit(ShapeWithStyle & shape) const
{
  visit(static_cast<Shape &>(shape));
}

void ShapeVisitor::visit(Dot & dot)
{
  visit(static_cast<Shape &>(dot));
}

void ShapeVisitor::visit(Dot & dot) const
{
  visit(static_cast<Shape &>(dot));
}

void ShapeVisitor::visit(Line & line)
{
  visit(static_cast<Shape &>(line));
}

void ShapeVisitor::visit(Line & line) const
{
  visit(static_cast<Shape &>(line));
}

void ShapeVisitor::visit(Arrow & arrow)
{
  visit(static_cast<Shape &>(arrow));
}

void ShapeVisitor::visit(Arrow & arrow) const
{
  visit(static_cast<Shape &>(arrow));
}

void ShapeVisitor::visit(Polyline & polyline)
{
  visit(static_cast<Shape &>(polyline));
}

void ShapeVisitor::visit(Polyline & polyline) const
{
  visit(static_cast<Shape &>(polyline));
}

void ShapeVisitor::visit(Ellipse & ellipse)
{
  visit(static_cast<Shape &>(ellipse));
}

void ShapeVisitor::visit(Ellipse & ellipse) const
{
  visit(static_cast<Shape &>(ellipse));
}

void ShapeVisitor::visit(Text & text)
{
  visit(static_cast<Shape &>(text));
}

void ShapeVisitor::visit(Text & text) const
{
  visit(static_cast<Shape &>(text));
}

void ShapeVisitor::visit(Bezier & bezier)
{
  visit(static_cast<Shape &>(bezier));
}

void ShapeVisitor::visit(Bezier & bezier) const
{
  visit(static_cast<Shape &>(bezier));
}

ShapeVisitor::~ShapeVisitor() {}

ConstShapeVisitor::~ConstShapeVisitor() {}

void ConstShapeVisitor::visit(const ShapeWithStyle & shape)
{
  visit(static_cast<const Shape &>(shape));
}

void ConstShapeVisitor::visit(const ShapeWithStyle & shape) const
{
  visit(static_cast<const Shape &>(shape));
}

void ConstShapeVisitor::visit(const Dot & dot)
{
  visit(static_cast<const Shape &>(dot));
}

void ConstShapeVisitor::visit(const Dot & dot) const
{
  visit(static_cast<const Shape &>(dot));
}

void ConstShapeVisitor::visit(const Line & line)
{
  visit(static_cast<const Shape &>(line));
}

void ConstShapeVisitor::visit(const Line & line) const
{
  visit(static_cast<const Shape &>(line));
}

void ConstShapeVisitor::visit(const Arrow & arrow)
{
  visit(static_cast<const Shape &>(arrow));
}

void ConstShapeVisitor::visit(const Arrow & arrow) const
{
  visit(static_cast<const Shape &>(arrow));
}

void ConstShapeVisitor::visit(const Polyline & polyline)
{
  visit(static_cast<const Shape &>(polyline));
}

void ConstShapeVisitor::visit(const Polyline & polyline) const
{
  visit(static_cast<const Shape &>(polyline));
}

void ConstShapeVisitor::visit(const Ellipse & ellipse)
{
  visit(static_cast<const Shape &>(ellipse));
}

void ConstShapeVisitor::visit(const Ellipse & ellipse) const
{
  visit(static_cast<const Shape &>(ellipse));
}

void ConstShapeVisitor::visit(const Text & text)
{
  visit(static_cast<const Shape &>(text));
}

void ConstShapeVisitor::visit(const Text & text) const
{
  visit(static_cast<const Shape &>(text));
}

void ConstShapeVisitor::visit(const Bezier & bezier)
{
  visit(static_cast<const Shape &>(bezier));
}

void ConstShapeVisitor::visit(const Bezier & bezier) const
{
  visit(static_cast<const Shape &>(bezier));
}

Shape * CompositeShapeTransform::map(const Dot & dot)
{
  return map(static_cast<const Shape &>(dot));
}

Shape * CompositeShapeTransform::map(const Dot & dot) const
{
  return map(static_cast<const Shape &>(dot));
}

Shape * CompositeShapeTransform::map(const Line & line)
{
  return map(static_cast<const Shape &>(line));
}

Shape * CompositeShapeTransform::map(const Line & line) const
{
  return map(static_cast<const Shape &>(line));
}

Shape * CompositeShapeTransform::map(const Arrow & arrow)
{
  return map(static_cast<const Shape &>(arrow));
}

Shape * CompositeShapeTransform::map(const Arrow & arrow) const
{
  return map(static_cast<const Shape &>(arrow));
}

Shape * CompositeShapeTransform::map(const Polyline & polyline)
{
  return map(static_cast<const Shape &>(polyline));
}

Shape * CompositeShapeTransform::map(const Polyline & polyline) const
{
  return map(static_cast<const Shape &>(polyline));
}

Shape * CompositeShapeTransform::map(const Ellipse & ellipse)
{
  return map(static_cast<const Shape &>(ellipse));
}

Shape * CompositeShapeTransform::map(const Ellipse & ellipse) const
{
  return map(static_cast<const Shape &>(ellipse));
}

Shape * CompositeShapeTransform::map(const Text & text)
{
  return map(static_cast<const Shape &>(text));
}

Shape * CompositeShapeTransform::map(const Text & text) const
{
  return map(static_cast<const Shape &>(text));
}

Shape * CompositeShapeTransform::map(const Bezier & bezier)
{
  return map(static_cast<const Shape &>(bezier));
}

Shape * CompositeShapeTransform::map(const Bezier & bezier) const
{
  return map(static_cast<const Shape &>(bezier));
}

CompositeShapeTransform::~CompositeShapeTransform() {}

PenColorExtractor::PenColorExtractor() {}

void PenColorExtractor::clear()
{
  _colors.clear();
}

void PenColorExtractor::visit(const Shape &) {}

void PenColorExtractor::visit(const Shape &) const
{
  Tools::warning << "PenColorExtractor(): Visiting using the const method does not make sense.\n";
}

void PenColorExtractor::visit(const Dot & dot)
{
  _colors.insert(dot.penColor());
}

void PenColorExtractor::visit(const Line & line)
{
  _colors.insert(line.penColor());
}

void PenColorExtractor::visit(const Arrow & arrow)
{
  _colors.insert(arrow.penColor());
}

void PenColorExtractor::visit(const Polyline & p)
{
  _colors.insert(p.penColor());
}

void PenColorExtractor::visit(const Ellipse & e)
{
  _colors.insert(e.penColor());
}

void PenColorExtractor::visit(const Text & text)
{
  _colors.insert(text.penColor());
}

void PenColorExtractor::visit(const Bezier & bezier)
{
  _colors.insert(bezier.penColor());
}

const std::set<Color> & PenColorExtractor::colors() const
{
  return _colors;
}

FillColorExtractor::FillColorExtractor() {}

void FillColorExtractor::visit(const Shape &) {}

void FillColorExtractor::visit(const Shape &) const
{
  Tools::warning << "FillColorExtractor(): Visiting using the const method does not make sense.\n";
}

void FillColorExtractor::visit(const Dot & dot)
{
  _colors.insert(dot.fillColor());
}

void FillColorExtractor::visit(const Line & line)
{
  _colors.insert(line.fillColor());
}

void FillColorExtractor::visit(const Arrow & arrow)
{
  _colors.insert(arrow.fillColor());
}

void FillColorExtractor::visit(const Polyline & p)
{
  _colors.insert(p.fillColor());
}

void FillColorExtractor::visit(const Ellipse & e)
{
  _colors.insert(e.fillColor());
}

void FillColorExtractor::visit(const Text & text)
{
  _colors.insert(text.fillColor());
}

void FillColorExtractor::visit(const Bezier & bezier)
{
  _colors.insert(bezier.fillColor());
}

const std::set<Color> & FillColorExtractor::colors() const
{
  return _colors;
}

ConstLeafVisitor::ConstLeafVisitor(std::function<void(const Shape &)> f) : _f(f) {}

void ConstLeafVisitor::visit(const Shape &) {}

void ConstLeafVisitor::visit(const Shape &) const {}

void ConstLeafVisitor::visit(const Dot & dot)
{
  _f(dot);
}

void ConstLeafVisitor::visit(const Line & line)
{
  _f(line);
}

void ConstLeafVisitor::visit(const Arrow & arrow)
{
  _f(arrow);
}

void ConstLeafVisitor::visit(const Polyline & polyline)
{
  _f(polyline);
}

void ConstLeafVisitor::visit(const Ellipse & ellipse)
{
  _f(ellipse);
}

void ConstLeafVisitor::visit(const Text & text)
{
  _f(text);
}

void ConstLeafVisitor::visit(const Bezier & bezier)
{
  _f(bezier);
}

LeafVisitor::LeafVisitor(LeafVisitor::Function f) : _f(f) {}

void LeafVisitor::visit(Shape &) {}

void LeafVisitor::visit(Shape &) const {}

void LeafVisitor::visit(Dot & dot)
{
  _f(dot);
}

void LeafVisitor::visit(Line & line)
{
  _f(line);
}

void LeafVisitor::visit(Arrow & arrow)
{
  _f(arrow);
}

void LeafVisitor::visit(Polyline & polyline)
{
  _f(polyline);
}

void LeafVisitor::visit(Ellipse & ellipse)
{
  _f(ellipse);
}

void LeafVisitor::visit(Text & text)
{
  _f(text);
}

void LeafVisitor::visit(Bezier & bezier)
{
  _f(bezier);
}

ShapeWithStyleVisitor::ShapeWithStyleVisitor(Style style)
{
  _f = [style](ShapeWithStyle & shape) { shape.setStyle(style); };
}

ShapeWithStyleVisitor::ShapeWithStyleVisitor(Color color, ColorSpecification spec)
{
  if (spec == ColorSpecification::PenColor) {
    _f = [color](ShapeWithStyle & shape) { shape.setPenColor(color); };
  } else {
    _f = [color](ShapeWithStyle & shape) { shape.setFillColor(color); };
  }
}

ShapeWithStyleVisitor::ShapeWithStyleVisitor(Color penColor, Color fillColor)
{
  _f = [penColor, fillColor](ShapeWithStyle & shape) {
    shape.setPenColor(penColor);
    shape.setFillColor(fillColor);
  };
}

ShapeWithStyleVisitor::ShapeWithStyleVisitor(double lineWidth)
{
  _f = [lineWidth](ShapeWithStyle & shape) { shape.setLineWidth(lineWidth); };
}

ShapeWithStyleVisitor::ShapeWithStyleVisitor(LineStyle lineStyle)
{
  _f = [lineStyle](ShapeWithStyle & shape) { shape.setLineStyle(lineStyle); };
}

ShapeWithStyleVisitor::ShapeWithStyleVisitor(LineCap lineCap)
{
  _f = [lineCap](ShapeWithStyle & shape) { shape.setLineCap(lineCap); };
}

ShapeWithStyleVisitor::ShapeWithStyleVisitor(LineJoin lineJoin)
{
  _f = [lineJoin](ShapeWithStyle & shape) { shape.setLineJoin(lineJoin); };
}

void ShapeWithStyleVisitor::visit(Shape &) {}

void ShapeWithStyleVisitor::visit(Shape &) const {}

void ShapeWithStyleVisitor::visit(ShapeWithStyle & shape)
{
  _f(shape);
}

void ShapeWithStyleVisitor::visit(ShapeWithStyle & shape) const
{
  _f(shape);
}

void ShapeWithStyleVisitor::visit(Dot & dot)
{
  _f(dot);
}

void ShapeWithStyleVisitor::visit(Dot & dot) const
{
  _f(dot);
}

void ShapeWithStyleVisitor::visit(Line & line)
{
  _f(line);
}

void ShapeWithStyleVisitor::visit(Line & line) const
{
  _f(line);
}

void ShapeWithStyleVisitor::visit(Arrow & arrow)
{
  _f(arrow);
}

void ShapeWithStyleVisitor::visit(Arrow & arrow) const
{
  _f(arrow);
}

void ShapeWithStyleVisitor::visit(Polyline & polyline)
{
  _f(polyline);
}

void ShapeWithStyleVisitor::visit(Polyline & polyline) const
{
  _f(polyline);
}

void ShapeWithStyleVisitor::visit(Ellipse & ellipse)
{
  _f(ellipse);
}

void ShapeWithStyleVisitor::visit(Ellipse & ellipse) const
{
  _f(ellipse);
}

void ShapeWithStyleVisitor::visit(Text & text)
{
  _f(text);
}

void ShapeWithStyleVisitor::visit(Text & text) const
{
  _f(text);
}

void ShapeWithStyleVisitor::visit(Bezier & bezier)
{
  _f(bezier);
}

void ShapeWithStyleVisitor::visit(Bezier & bezier) const
{
  _f(bezier);
}

} // namespace LibBoard
