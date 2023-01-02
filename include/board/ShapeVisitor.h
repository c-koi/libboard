/* -*- mode: c++ -*- */
/**
 * @file   ShapeVisitor.h
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
#ifndef BOARD_SHAPE_VISITOR_H
#define BOARD_SHAPE_VISITOR_H

#include <board/Color.h>
#include <board/Globals.h>
#include <board/Style.h>
#include <cstddef> // For std::size_t
#include <functional>
#include <set>

namespace LibBoard
{

struct Shape;
struct ShapeWithStyle;
struct ShapeList;
struct Board;
struct Dot;
struct Line;
struct Arrow;
struct Polyline;
struct Ellipse;
struct Text;
struct Bezier;

/**
 * @brief A ShapeVisitor visits all shapes in a composite shape tree in back-to-front order.
 */
struct ShapeVisitor {
  virtual void visit(Shape & shape) = 0;
  virtual void visit(Shape & shape) const = 0;
  virtual void visit(ShapeWithStyle & shape);
  virtual void visit(ShapeWithStyle & shape) const;
  virtual void visit(Dot &);
  virtual void visit(Dot &) const;
  virtual void visit(Line &);
  virtual void visit(Line &) const;
  virtual void visit(Arrow &);
  virtual void visit(Arrow & arrow) const;
  virtual void visit(Polyline &);
  virtual void visit(Polyline &) const;
  virtual void visit(Ellipse &);
  virtual void visit(Ellipse &) const;
  virtual void visit(Text &);
  virtual void visit(Text &) const;
  virtual void visit(Bezier &);
  virtual void visit(Bezier &) const;
  virtual ~ShapeVisitor();
};

/**
 * @brief A ConstShapeVisitor may visit const shapes of a composite shape tree in back-to-front order.
 */
struct ConstShapeVisitor {
  virtual void visit(const Shape & shape) = 0;
  virtual void visit(const Shape & shape) const = 0;
  virtual void visit(const ShapeWithStyle & shape);
  virtual void visit(const ShapeWithStyle & shape) const;
  virtual void visit(const Dot &);
  virtual void visit(const Dot &) const;
  virtual void visit(const Line &);
  virtual void visit(const Line &) const;
  virtual void visit(const Arrow &);
  virtual void visit(const Arrow & arrow) const;
  virtual void visit(const Polyline &);
  virtual void visit(const Polyline &) const;
  virtual void visit(const Ellipse &);
  virtual void visit(const Ellipse &) const;
  virtual void visit(const Text &);
  virtual void visit(const Text &) const;
  virtual void visit(const Bezier &);
  virtual void visit(const Bezier &) const;
  virtual ~ConstShapeVisitor();
};

/**
 * @brief A CompositeShapeTransform may be used to duplicate/transform a composite shape tree.
 */
struct CompositeShapeTransform {
  virtual Shape * map(const Shape &) = 0;
  virtual Shape * map(const Shape &) const = 0;
  virtual Shape * map(const Dot &);
  virtual Shape * map(const Dot &) const;
  virtual Shape * map(const Line &);
  virtual Shape * map(const Line &) const;
  virtual Shape * map(const Arrow &);
  virtual Shape * map(const Arrow &) const;
  virtual Shape * map(const Polyline &);
  virtual Shape * map(const Polyline &) const;
  virtual Shape * map(const Ellipse &);
  virtual Shape * map(const Ellipse &) const;
  virtual Shape * map(const Text &);
  virtual Shape * map(const Text &) const;
  virtual Shape * map(const Bezier &);
  virtual Shape * map(const Bezier &) const;
  virtual ~CompositeShapeTransform();
};

/**
 * @brief The BoundingBoxExtractor struct may be used to compute the
 *        bounding box of a composite shape tree.
 */
struct BoundingBoxExtractor : public ConstShapeVisitor {
  BoundingBoxExtractor(ShapeList &);
  void visit(const Shape &) override;
  void visit(const Shape &) const override;
  const ShapeList & shapeList() const;

private:
  ShapeList & _shapeList;
};

/**
 * @brief The BoundingBoxViewer struct may be used to display the bounding
 *        boxes of the shapes in a composite tree.
 */
struct BoundingBoxViewer : public ConstShapeVisitor {
  void visit(const Shape &) override;
  void visit(const Shape &) const override;
};

/**
 * @brief The ShapeCounter struct may be used to count shapes in
 *        the composite shapes tree.
 */
struct ShapeCounter : public ConstShapeVisitor {
  ShapeCounter();
  void clear();
  std::size_t value() const;
  void visit(const Shape &) override;
  void visit(const Shape &) const override;

private:
  std::size_t _count;
};

/**
 * @brief The pen-color extractor may be used to retrieve all pen colors.
 *
 */
struct PenColorExtractor : public ConstShapeVisitor {
  PenColorExtractor();
  void clear();
  void visit(const Shape &) override;
  void visit(const Shape &) const override;
  void visit(const Dot & d) override;
  void visit(const Line &) override;
  void visit(const Arrow &) override;
  void visit(const Polyline &) override;
  void visit(const Ellipse &) override;
  void visit(const Text &) override;
  void visit(const Bezier &) override;
  const std::set<Color> & colors() const;

private:
  std::set<Color> _colors;
};

/**
 * @brief The fill-color extractor may be used to retrieve all fill colors.
 */
struct FillColorExtractor : public ConstShapeVisitor {
  FillColorExtractor();
  void clear();
  void visit(const Shape &) override;
  void visit(const Shape &) const override;
  void visit(const Dot &) override;
  void visit(const Line &) override;
  void visit(const Arrow &) override;
  void visit(const Polyline &) override;
  void visit(const Ellipse &) override;
  void visit(const Text &) override;
  void visit(const Bezier &) override;
  const std::set<Color> & colors() const;

private:
  std::set<Color> _colors;
};

/**
 * @brief Leaf visitor may be used to apply a function on each leaf, in back-to-front order.
 */
struct ConstLeafVisitor : public ConstShapeVisitor {
  typedef std::function<void(const Shape &)> Function;
  ConstLeafVisitor(Function f);
  void visit(const Shape &) override;
  void visit(const Shape &) const override;
  void visit(const Dot &) override;
  void visit(const Line &) override;
  void visit(const Arrow &) override;
  void visit(const Polyline &) override;
  void visit(const Ellipse &) override;
  void visit(const Text &) override;
  void visit(const Bezier &) override;

private:
  Function _f;
};

/**
 * @brief Leaf visitor may be used to apply a function on each leaf, in back-to-front order.
 */
struct LeafVisitor : public ShapeVisitor {
  typedef std::function<void(Shape &)> Function;
  LeafVisitor(Function f);
  void visit(Shape &) override;
  void visit(Shape &) const override;
  void visit(Dot &) override;
  void visit(Line &) override;
  void visit(Arrow &) override;
  void visit(Polyline &) override;
  void visit(Ellipse &) override;
  void visit(Text &) override;
  void visit(Bezier &) override;

private:
  Function _f;
};

/**
 * @brief Leaf visitor may be used to apply a function on each Shape with style, in back-to-front order.
 */
struct ShapeWithStyleVisitor : public ShapeVisitor {
  typedef std::function<void(ShapeWithStyle &)> Function;
  ShapeWithStyleVisitor(Function f);
  ShapeWithStyleVisitor(Style);
  ShapeWithStyleVisitor(Color color, ColorSpecification spec);
  ShapeWithStyleVisitor(Color penColor, Color fillColor);
  ShapeWithStyleVisitor(double lineWidth);
  ShapeWithStyleVisitor(LineStyle lineStyle);
  ShapeWithStyleVisitor(LineCap lineCap);
  ShapeWithStyleVisitor(LineJoin lineJoin);
  void visit(Shape &) override;
  void visit(Shape &) const override;
  void visit(ShapeWithStyle &) override;
  void visit(ShapeWithStyle &) const override;
  void visit(Dot &) override;
  void visit(Dot &) const override;
  void visit(Line &) override;
  void visit(Line &) const override;
  void visit(Arrow &) override;
  void visit(Arrow &) const override;
  void visit(Polyline &) override;
  void visit(Polyline &) const override;
  void visit(Ellipse &) override;
  void visit(Ellipse &) const override;
  void visit(Text &) override;
  void visit(Text &) const override;
  void visit(Bezier &) override;
  void visit(Bezier &) const override;

private:
  Function _f;
};

} // namespace LibBoard

#endif /* BOARD_SHAPE_VISITOR_H */
