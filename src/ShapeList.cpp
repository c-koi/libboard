/* -*- mode: c++ -*- */
/**
 * @file   ShapeList.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
 *
 * @brief  Definition of the ShapeList class.
 *
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
#include <algorithm>
#include <board/Group.h>
#include <board/ShapeList.h>
#include <board/ShapeVisitor.h>
#include <board/Tools.h>
#include <typeinfo>
#include <utility>

#if defined(max)
#undef max
#endif

namespace LibBoard
{

//
// Definition of the ShapeList methods.
//

const std::string ShapeList::_name("ListOfShapes");

const std::string & ShapeList::name() const
{
  return _name;
}

ShapeList::ShapeList(const Shape & shape, unsigned int times, double dx, double dy, double scale)
{
  Shape * s = shape.clone();
  while (times--) {
    (*this) << (*s);
    if (scale != 1.0) {
      s->scale(scale);
    }
    s->translate(dx, dy);
  }
  delete s;
}

ShapeList::ShapeList(const Shape & shape, unsigned int times, double dx, double dy, double scaleX, double scaleY, double angle) //
{
  Shape * s = shape.clone();
  while (times--) {
    (*this) << (*s);
    if (scaleX != 1.0 || scaleY != 1.0) {
      s->scale(scaleX, scaleY);
    }
    if (dx != 0.0 || dy != 0.0) {
      s->translate(dx, dy);
    }
    if (angle != 0.0) {
      s->rotate(angle);
    }
  }
  delete s;
}

ShapeList::~ShapeList()
{
  deleteShapes();
}

ShapeList & ShapeList::clear()
{
  deleteShapes();
  _shapes.clear();
  return *this;
}

void ShapeList::deleteShapes()
{
  for (Shape * p : _shapes) {
    delete p;
  }
}

ShapeList::ShapeList(const ShapeList & other) : Shape(other)
{
  if (!other._shapes.size()) {
    return;
  }
  _shapes.resize(other._shapes.size(), nullptr);
  std::vector<Shape *>::iterator t = _shapes.begin();
  std::vector<Shape *>::const_iterator i = other._shapes.begin();
  std::vector<Shape *>::const_iterator end = other._shapes.end();
  while (i != end) {
    *t = (*i)->clone();
    ++i;
    ++t;
  }
}

ShapeList & ShapeList::operator=(const ShapeList & other)
{
  deleteShapes();
  if (!other._shapes.size()) {
    return *this;
  }
  _shapes.resize(other._shapes.size(), nullptr);
  std::vector<Shape *>::iterator t = _shapes.begin();
  std::vector<Shape *>::const_iterator i = other._shapes.begin();
  std::vector<Shape *>::const_iterator end = other._shapes.end();
  while (i != end) {
    *t = (*i)->clone();
    ++i;
    ++t;
  }
  return *this;
}

ShapeList::ShapeList(ShapeList && other) : Shape(other)
{
  _shapes = std::move(other._shapes);
}

ShapeList & ShapeList::operator=(ShapeList && other)
{
  deleteShapes();
  _shapes = std::move(other._shapes);
  return *this;
}

ShapeList & ShapeList::operator<<(const Shape & shape)
{
  if (typeid(shape) == typeid(ShapeList)) {
    // Insertion on top, respecting the same depth order.
    const ShapeList & sl = dynamic_cast<const ShapeList &>(shape);
    for (const Shape * shape : sl._shapes) {
      (*this) << *shape; // This will recursively flatten shape lists (not groups)
    }
  } else {
    _shapes.push_back(shape.clone());
  }
  return *this;
}

void ShapeList::addShape(const Shape & shape, double scaleFactor)
{
  if (typeid(shape) == typeid(ShapeList)) {
    // Insertion on top, respecting the same depth order.
    const ShapeList & sl = dynamic_cast<const ShapeList &>(shape);
    for (const Shape * shape : sl._shapes) {
      Shape * s = shape->clone();
      if (scaleFactor != 1.0) {
        s->scaleAll(scaleFactor);
      }
      _shapes.push_back(s);
    }
  } else {
    Shape * s = shape.clone();
    if (scaleFactor != 1.0) {
      s->scaleAll(scaleFactor);
    }
    _shapes.push_back(s);
  }
}

ShapeList & ShapeList::duplicateLast(std::size_t copies)
{
  if (!_shapes.size()) {
    Tools::warning << "dup() called with an empty list of shapes.\n";
    return *this;
  }
  while (copies--) {
    (*this) << (*_shapes.back());
  }
  return *this;
}

ShapeList & ShapeList::operator+=(const Shape & shape)
{
  if (typeid(shape) == typeid(ShapeList)) {
    const ShapeList & sl = dynamic_cast<const ShapeList &>(shape);
    std::vector<Shape *>::const_iterator i = sl._shapes.begin();
    std::vector<Shape *>::const_iterator end = sl._shapes.end();
    while (i != end) {
      Shape * s = (*i)->clone();
      _shapes.push_back(s);
      ++i;
    }
  } else {
    _shapes.push_back(shape.clone());
  }
  return *this;
}

void ShapeList::addRepeated(const Shape & shape, unsigned int times, double dx, double dy, double scaleX, double scaleY, double angle)
{
  Shape * s = shape.clone();
  while (times--) {
    (*this) << (*s);
    if (scaleX != 1.0 || scaleY != 1.0) {
      s->scale(scaleX, scaleY);
    }
    if (dx != 0.0 || dy != 0.0) {
      s->translate(dx, dy);
    }
    if (angle != 0.0) {
      s->rotate(angle);
    }
  }
  delete s;
}

ShapeList & ShapeList::append(const Shape & shape, Direction direction, Alignment alignment, double margin, LineWidthFlag lineWidthFlag)
{
  if (_shapes.empty()) {
    (*this) << shape;
    return *this;
  }
  Rect box = boundingBox(lineWidthFlag);
  Point c = box.center();
  Rect shapeBox = shape.boundingBox(lineWidthFlag);
  const double shapeHalfWidth = shapeBox.width / 2.0;
  const double shapeHalfHeight = shapeBox.height / 2.0;
  double x = c.x;
  double y = c.y;
  Shape * s = shape.clone();
  if (direction == Direction::Right || direction == Direction::Left) {
    x = (direction == Direction::Right) ? (box.right() + shapeHalfWidth + margin) : (box.left - (margin + shapeHalfWidth));
    switch (alignment) {
    case Alignment::Center:
      y = c.y;
      break;
    case Alignment::Top:
      y = box.top - shapeBox.height / 2.0;
      break;
    case Alignment::Bottom:
      y = (box.top - box.height) + shapeBox.height / 2.0;
      break;
    case Alignment::Left:
    case Alignment::Right:
      Tools::error << "ShapeList::append(): bad alignement\n";
      break;
    }
  } else {
    y = (direction == Direction::Top) ? (box.top + shapeHalfHeight + margin) : (box.bottom() - (shapeHalfHeight + margin));
    switch (alignment) {
    case Alignment::Center:
      x = c.x;
      break;
    case Alignment::Left:
      x = box.left + shapeBox.width / 2.0;
      break;
    case Alignment::Right:
      x = (box.left + box.width) - shapeBox.width / 2.0;
      break;
    case Alignment::Top:
    case Alignment::Bottom:
      Tools::error << "ShapeList::append(): bad alignement\n";
      break;
    }
  }
  s->moveCenter(x, y);
  (*this) << (*s);
  delete s;
  return *this;
}

ShapeList & ShapeList::rotate(double angle, const Point & center)
{
  std::vector<Shape *>::iterator i = _shapes.begin();
  std::vector<Shape *>::iterator end = _shapes.end();
  while (i != end) {
    (*i)->rotate(angle, center);
    ++i;
  }
  return *this;
}

ShapeList ShapeList::rotated(double angle, const Point & center)
{
  ShapeList r(*this);
  return static_cast<ShapeList &>(r.rotate(angle, center));
}

ShapeList & ShapeList::rotate(double angle)
{
  return ShapeList::rotate(angle, center());
}

ShapeList ShapeList::rotated(double angle)
{
  return static_cast<ShapeList &>(ShapeList(*this).rotate(angle, center()));
}

ShapeList & ShapeList::translate(double dx, double dy)
{
  std::vector<Shape *>::iterator i = _shapes.begin();
  std::vector<Shape *>::iterator end = _shapes.end();
  while (i != end) {
    (*i)->translate(dx, dy);
    ++i;
  }
  return *this;
}

ShapeList ShapeList::translated(double dx, double dy)
{
  return static_cast<ShapeList &>(ShapeList(*this).translate(dx, dy));
}

ShapeList & ShapeList::scale(double sx, double sy)
{
  Point c = center();
  Point delta;
  std::vector<Shape *>::iterator i = _shapes.begin();
  std::vector<Shape *>::iterator end = _shapes.end();
  while (i != end) {
    delta = (*i)->center() - c;
    delta.x *= sx;
    delta.y *= sy;
    (*i)->scale(sx, sy);
    delta = (c + delta) - (*i)->center();
    (*i++)->translate(delta.x, delta.y);
  }
  return *this;
}

ShapeList & ShapeList::scale(double s)
{
  return ShapeList::scale(s, s);
}

ShapeList ShapeList::scaled(double sx, double sy) const
{
  return static_cast<ShapeList &>(ShapeList(*this).scale(sx, sy));
}

ShapeList ShapeList::scaled(double s) const
{
  return static_cast<ShapeList &>(ShapeList(*this).scale(s, s));
}

void ShapeList::scaleAll(double s)
{
  std::vector<Shape *>::iterator i = _shapes.begin();
  std::vector<Shape *>::iterator end = _shapes.end();
  while (i != end) {
    (*i++)->scaleAll(s);
  }
}

void ShapeList::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  stream << "%%% Begin ShapeList\n";
  for (const Shape * shape : _shapes) {
    shape->flushPostscript(stream, transform);
  }
  stream << "%%% End ShapeList\n";
}

void ShapeList::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  for (const Shape * shape : _shapes) {
    shape->flushFIG(stream, transform, colormap);
  }
}

void ShapeList::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  // stream << "<g>\n";
  for (const Shape * shape : _shapes) {
    shape->flushSVG(stream, transform);
  }
  // stream << "</g>\n";
}

void ShapeList::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  stream << "\\begin{scope}\n";
  for (const Shape * shape : _shapes) {
    shape->flushTikZ(stream, transform);
  }
  stream << "\\end{scope}\n";
}

Rect ShapeList::boundingBox(LineWidthFlag flag) const
{
  Rect r;
  std::vector<Shape *>::const_iterator i = _shapes.begin();
  std::vector<Shape *>::const_iterator end = _shapes.end();
  if (i == end) {
    return r;
  }
  r = (*i)->boundingBox(flag);
  ++i;
  while (i != end) {
    r = r || (*i)->boundingBox(flag);
    ++i;
  }
  return r;
}

ShapeList * ShapeList::clone() const
{
  return new ShapeList(*this);
}

ShapeList & ShapeList::push_back(Shape * shape)
{
  _shapes.push_back(shape);
  return *this;
}

Shape & ShapeList::last(const std::size_t position)
{
  return last<Shape>(position);
}

Shape & ShapeList::top()
{
  return last<Shape>(0);
}

void ShapeList::accept(ShapeVisitor & visitor)
{
  std::vector<Shape *>::const_iterator i = _shapes.begin();
  std::vector<Shape *>::const_iterator end = _shapes.end();
  while (i != end) {
    (*i++)->accept(visitor);
  }
}

void ShapeList::accept(const ShapeVisitor & visitor)
{
  std::vector<Shape *>::const_iterator i = _shapes.begin();
  std::vector<Shape *>::const_iterator end = _shapes.end();
  while (i != end) {
    (*i++)->accept(visitor);
  }
}

void ShapeList::accept(ConstShapeVisitor & visitor) const
{
  std::vector<Shape *>::const_iterator i = _shapes.begin();
  std::vector<Shape *>::const_iterator end = _shapes.end();
  while (i != end) {
    const Shape & shape = **i;
    shape.accept(visitor);
    ++i;
  }
}

void ShapeList::accept(const ConstShapeVisitor & visitor) const
{
  std::vector<Shape *>::const_iterator i = _shapes.begin();
  std::vector<Shape *>::const_iterator end = _shapes.end();
  while (i != end) {
    const Shape & shape = **i;
    shape.accept(visitor);
    ++i;
  }
}

Shape * ShapeList::accept(CompositeShapeTransform & transform) const
{
  ShapeList * shapeList = new ShapeList;
  for (const Shape * shape : _shapes) {
    shapeList->push_back(shape->accept(transform));
  }
  return shapeList;
}

Shape * ShapeList::accept(const CompositeShapeTransform & transform) const
{
  ShapeList * shapeList = new ShapeList;
  for (const Shape * shape : _shapes) {
    shapeList->push_back(shape->accept(transform));
  }
  return shapeList;
}

std::size_t ShapeList::deepSize() const
{
  ShapeCounter counter;
  accept(counter);
  return counter.value();
}

void ShapeList::DepthFirstIterator::moveToFirstActuelShape()
{
  Shape * shape = nullptr;
  while (!shape) {
    if (_shapeListsStack.empty()) {
      return;
    }
    if (_iteratorsStack.top() == _shapeListsStack.top()->end()) {
      // Go up one level, move right
      _iteratorsStack.pop();
      _shapeListsStack.pop();
      if (!_iteratorsStack.empty()) {
        ++_iteratorsStack.top();
      }
    } else {
      ShapeList * list = dynamic_cast<ShapeList *>(_iteratorsStack.top().pointer());
      if (list != nullptr) {
        // Visit the ShapeList
        _iteratorsStack.push(list->begin());
        _shapeListsStack.push(list);
      } else {
        // Found an actual Shape which is not a ShapeList
        shape = dynamic_cast<Shape *>(_iteratorsStack.top().pointer());
      }
    }
  }
}

void ShapeList::DepthFirstIterator::moveToNextActualShape()
{
  if (_shapeListsStack.empty()) {
    return;
  }
  ++_iteratorsStack.top();
  moveToFirstActuelShape();
}

void ShapeList::BreadthFirstIterator::moveToFirstActuelShape()
{
  Shape * shape = nullptr;
  while (!shape) {
    if (_shapeListsQueue.empty()) {
      return;
    }
    if (_iteratorsQueue.front() == _shapeListsQueue.front()->end()) {
      // Go up one level, move right
      _iteratorsQueue.pop();
      _shapeListsQueue.pop();
    } else {
      ShapeList * list = dynamic_cast<ShapeList *>(_iteratorsQueue.front().pointer());
      if (list != nullptr) {
        // Enqueue this list of shapes
        _iteratorsQueue.push(list->begin());
        _shapeListsQueue.push(list);
        // Shift right at current level
        ++_iteratorsQueue.front();
      } else {
        // Found an actual Shape which is not a ShapeList
        shape = dynamic_cast<Shape *>(_iteratorsQueue.front().pointer());
      }
    }
  }
}

void ShapeList::BreadthFirstIterator::moveToNextActualShape()
{
  if (_shapeListsQueue.empty()) {
    return;
  }
  ++_iteratorsQueue.front();
  moveToFirstActuelShape();
}

} // namespace LibBoard
