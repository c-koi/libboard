/* -*- mode: c++ -*- */
/**
 * @file   ShapeList.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
 *
 * @brief  Definition of the ShapeList and Group classes.
 *
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
#include "board/ShapeList.h"
#include <algorithm>
#include <typeinfo>
#include <utility>
#include "BoardConfig.h"
#include "board/ShapeVisitor.h"
#include "board/Tools.h"

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
    : Shape(Color::Null, Color::Null, 1.0, SolidStyle, ButtCap, MiterJoin, -1), _nextDepth(std::numeric_limits<int>::max() - 1)
{
  Shape * s = shape.clone();
  while (times--) {
    (*this) << (*s);
    if (scale != 1.0)
      s->scale(scale);
    s->translate(dx, dy);
  }
  delete s;
}

ShapeList::ShapeList(const Shape & shape, unsigned int times, double dx, double dy, double scaleX, double scaleY, double angle)
    : Shape(Color::Null, Color::Null, 1.0, SolidStyle, ButtCap, MiterJoin, -1), _nextDepth(std::numeric_limits<int>::max() - 1)
{
  Shape * s = shape.clone();
  while (times--) {
    (*this) << (*s);
    if (scaleX != 1.0 || scaleY != 1.0)
      s->scale(scaleX, scaleY);
    if (dx != 0.0 || dy != 0.0)
      s->translate(dx, dy);
    if (angle != 0.0)
      s->rotate(angle);
  }
  delete s;
}

ShapeList::~ShapeList()
{
  free();
}

ShapeList & ShapeList::clear()
{
  free();
  _shapes.clear();
  _nextDepth = std::numeric_limits<int>::max() - 1;
  return *this;
}

void ShapeList::free()
{
  std::vector<Shape *>::const_iterator i = _shapes.begin();
  std::vector<Shape *>::const_iterator end = _shapes.end();
  while (i != end) {
    delete *i;
    ++i;
  }
}

ShapeList::ShapeList(const ShapeList & other) : Shape(other)
{
  _nextDepth = other._nextDepth;
  if (!other._shapes.size()) {
    return;
  }
  _shapes.resize(other._shapes.size(), 0);
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
  free();
  if (!other._shapes.size()) {
    return *this;
  }
  _shapes.resize(other._shapes.size(), 0);
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

#if __cplusplus > 201100

ShapeList::ShapeList(ShapeList && other) : Shape(other)
{
  _nextDepth = other._nextDepth;
  _shapes = std::move(other._shapes);
}

ShapeList & ShapeList::operator=(ShapeList && other)
{
  free();
  _nextDepth = other._nextDepth;
  _shapes = std::move(other._shapes);
  return *this;
}

#endif // __cplusplus > 201100

ShapeList & ShapeList::operator<<(const Shape & shape)
{
  if (typeid(shape) == typeid(ShapeList)) {
    // Insertion on top, respecting the same depth order.
    const ShapeList & sl = dynamic_cast<const ShapeList &>(shape);
    std::vector<Shape *> shapes = sl._shapes;
    stable_sort(shapes.begin(), shapes.end(), shapeGreaterDepth);
    std::vector<Shape *>::iterator i = shapes.begin();
    std::vector<Shape *>::iterator end = shapes.end();
    while (i != end) {
      Shape * s = (*i)->clone();
      s->depth(_nextDepth--);
      _shapes.push_back(s);
      ++i;
    }
  } else {
    Shape * s = shape.clone();
    if (s->depth() == -1)
      s->depth(_nextDepth--);
    _shapes.push_back(s);
    if (typeid(shape) == typeid(Group)) {
      _nextDepth = dynamic_cast<const Group &>(shape).minDepth() - 1;
    }
  }
  return *this;
}

void ShapeList::addShape(const Shape & shape, double scaleFactor)
{
  if (typeid(shape) == typeid(ShapeList)) {
    // Insertion on top, respecting the same depth order.
    const ShapeList & sl = dynamic_cast<const ShapeList &>(shape);
    std::vector<Shape *> shapes = sl._shapes;
    stable_sort(shapes.begin(), shapes.end(), shapeGreaterDepth);
    std::vector<Shape *>::iterator i = shapes.begin();
    std::vector<Shape *>::iterator end = shapes.end();
    while (i != end) {
      Shape * s = (*i)->clone();
      s->depth(_nextDepth--);
      if (scaleFactor != 1.0) {
        s->scaleAll(scaleFactor);
      }
      _shapes.push_back(s);
      ++i;
    }
  } else {
    Shape * s = shape.clone();
    if (s->depth() == -1)
      s->depth(_nextDepth--);
    if (scaleFactor != 1.0) {
      s->scaleAll(scaleFactor);
    }
    _shapes.push_back(s);
    if (typeid(shape) == typeid(Group)) {
      _nextDepth = dynamic_cast<const Group &>(shape).minDepth() - 1;
    }
  }
}

ShapeList & ShapeList::dup(std::size_t copies)
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

Group & ShapeList::addTiling(const Shape & shape, Point topLeftCorner, std::size_t columns, std::size_t rows, double spacing, LineWidthFlag lineWidthFlag)
{
  Group group;
  if (columns && rows) {
    Shape * s = shape.clone();
    Rect box = shape.boundingBox(lineWidthFlag);
    s->translate(topLeftCorner.x - box.left, topLeftCorner.y - box.top);
    for (std::size_t r = 0; r < rows; ++r) {
      group << (*s);
      for (std::size_t c = 1; c < columns; ++c) {
        group << group.last();
        group.last().translate(box.width + spacing, 0);
      }
      s->translate(0, -(box.height + spacing));
    }
    delete s;
  }
  (*this) << group;
  return last<Group>();
}

void ShapeList::repeat(const Shape & shape, unsigned int times, double dx, double dy, double scaleX, double scaleY, double angle)
{
  Shape * s = shape.clone();
  while (times--) {
    (*this) << (*s);
    if (scaleX != 1.0 || scaleY != 1.0)
      s->scale(scaleX, scaleY);
    if (dx != 0.0 || dy != 0.0)
      s->translate(dx, dy);
    if (angle != 0.0)
      s->rotate(angle);
  }
  delete s;
}

ShapeList & ShapeList::append(const Shape & shape, ShapeList::Direction direction, ShapeList::Alignment alignment, double margin, LineWidthFlag lineWidthFlag)
{
  if (_shapes.size() == 0) {
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
  if (direction == Right || direction == Left) {
    x = (direction == Right) ? (box.right() + shapeHalfWidth + margin) : (box.left - (margin + shapeHalfWidth));
    switch (alignment) {
    case AlignCenter:
      y = c.y;
      break;
    case AlignTop:
      y = box.top - shapeBox.height / 2.0;
      break;
    case AlignBottom:
      y = (box.top - box.height) + shapeBox.height / 2.0;
      break;
    case AlignLeft:
    case AlignRight:
      Tools::error << "ShapeList::append(): bad alignement\n";
      break;
    }
  } else {
    y = (direction == Top) ? (box.top + shapeHalfHeight + margin) : (box.bottom() - (shapeHalfHeight + margin));
    switch (alignment) {
    case AlignCenter:
      x = c.x;
      break;
    case AlignLeft:
      x = box.left + shapeBox.width / 2.0;
      break;
    case AlignRight:
      x = (box.left + box.width) - shapeBox.width / 2.0;
      break;
    case AlignTop:
    case AlignBottom:
      Tools::error << "ShapeList::append(): bad alignement\n";
      break;
    }
  }
  s->moveCenter(x, y);
  (*this) << (*s);
  delete s;
  return *this;
}

ShapeList & ShapeList::insert(const Shape &, int /* depth */)
{
  Tools::warning << "ShapeList::insert() not implemented yet.\n";
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

ShapeList ShapeList::scaled(double sx, double sy)
{
  return static_cast<ShapeList &>(ShapeList(*this).scale(sx, sy));
}

ShapeList ShapeList::scaled(double s)
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
  std::vector<Shape *> shapes = _shapes;
  stable_sort(shapes.begin(), shapes.end(), shapeGreaterDepth);
  std::vector<Shape *>::const_iterator i = shapes.begin();
  std::vector<Shape *>::const_iterator end = shapes.end();
  stream << "%%% Begin ShapeList\n";
  while (i != end) {
    (*i++)->flushPostscript(stream, transform);
  }
  stream << "%%% End ShapeList\n";
}

void ShapeList::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  std::vector<Shape *> shapes = _shapes;
  stable_sort(shapes.begin(), shapes.end(), shapeGreaterDepth);
  std::vector<Shape *>::const_iterator i = shapes.begin();
  std::vector<Shape *>::const_iterator end = shapes.end();
  while (i != end) {
    (*i)->flushFIG(stream, transform, colormap);
    ++i;
  }
}

void ShapeList::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  std::vector<Shape *> shapes = _shapes;
  stable_sort(shapes.begin(), shapes.end(), shapeGreaterDepth);
  std::vector<Shape *>::const_iterator i = shapes.begin();
  std::vector<Shape *>::const_iterator end = shapes.end();
  // stream << "<g>\n";
  while (i != end) {
    (*i)->flushSVG(stream, transform);
    ++i;
  }
  // stream << "</g>\n";
}

void ShapeList::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  std::vector<Shape *> shapes = _shapes;
  stable_sort(shapes.begin(), shapes.end(), shapeGreaterDepth);
  std::vector<Shape *>::const_iterator i = shapes.begin();
  std::vector<Shape *>::const_iterator end = shapes.end();
  stream << "\\begin{scope}\n";
  while (i != end) {
    (*i)->flushTikZ(stream, transform);
    ++i;
  }
  stream << "\\end{scope}\n";
}

Rect ShapeList::boundingBox(LineWidthFlag flag) const
{
  Rect r;
  std::vector<Shape *>::const_iterator i = _shapes.begin();
  std::vector<Shape *>::const_iterator end = _shapes.end();
  if (i == end)
    return r;
  r = (*i)->boundingBox(flag);
  ++i;
  while (i != end) {
    r = r || (*i)->boundingBox(flag);
    ++i;
  }
  return r;
}

int ShapeList::minDepth() const
{
  int res = std::numeric_limits<int>::max();
  int d;
  ShapeList * sl;
  std::vector<Shape *>::const_iterator i = _shapes.begin();
  std::vector<Shape *>::const_iterator end = _shapes.end();
  while (i != end) {
    sl = dynamic_cast<ShapeList *>(*i);
    if (sl) {
      d = sl->minDepth();
    } else {
      d = (*i)->depth();
    }
    if (d < res)
      res = d;
    ++i;
  }
  return res;
}

int ShapeList::maxDepth() const
{
  int res = std::numeric_limits<int>::min();
  int d;
  ShapeList * sl;
  std::vector<Shape *>::const_iterator i = _shapes.begin();
  std::vector<Shape *>::const_iterator end = _shapes.end();
  while (i != end) {
    sl = dynamic_cast<ShapeList *>(*i);
    if (sl) {
      d = sl->maxDepth();
    } else {
      d = (*i)->depth();
    }
    if (d > res)
      res = d;
    ++i;
  }
  return res;
}

void ShapeList::shiftDepth(int shift)
{
  std::vector<Shape *>::const_iterator i = _shapes.begin();
  std::vector<Shape *>::const_iterator end = _shapes.end();
  while (i != end) {
    (*i++)->shiftDepth(shift);
  }
}

ShapeList * ShapeList::clone() const
{
  return new ShapeList(*this);
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

//
// Definition of the Group methods.
//

const std::string Group::_name("GroupOfShapes");

const std::string & Group::name() const
{
  return _name;
}

Group & Group::rotate(double angle, const Point & center)
{
  ShapeList::rotate(angle, center);
  _clippingPath.rotate(angle, center);
  return (*this);
}

Group & Group::rotate(double angle)
{
  ShapeList::rotate(angle);
  _clippingPath.rotate(angle, center());
  return (*this);
}

Group & Group::translate(double dx, double dy)
{
  ShapeList::translate(dx, dy);
  _clippingPath.translate(dx, dy);
  return (*this);
}

Group & Group::scale(double sx, double sy)
{
  Point delta = _clippingPath.center() - center();
  delta.x *= sx;
  delta.y *= sy;
  _clippingPath.scale(sx, sy);
  ShapeList::scale(sx, sy);
  delta = (center() + delta) - _clippingPath.center();
  _clippingPath.translate(delta.x, delta.y);
  return (*this);
}

Group & Group::scale(double s)
{
  Point delta = _clippingPath.center() - center();
  delta *= s;
  _clippingPath.scale(s);
  ShapeList::scale(s);
  delta = (center() + delta) - _clippingPath.center();
  _clippingPath.translate(delta.x, delta.y);
  return (*this);
}

Group Group::rotated(double angle, const Point & center)
{
  return static_cast<const Group &>(Group(*this).rotate(angle, center));
}

Group Group::rotated(double angle)
{
  return static_cast<const Group &>(Group(*this).rotate(angle));
}

Group Group::translated(double dx, double dy)
{
  return static_cast<const Group &>(Group(*this).translate(dx, dy));
}

Group Group::scaled(double sx, double sy)
{
  return static_cast<const Group &>(Group(*this).scale(sx, sy));
}

Group Group::scaled(double s)
{
  return static_cast<const Group &>(Group(*this).scale(s));
}

void Group::setClippingRectangle(float x, float y, float width, float height)
{
  _clippingPath.clear();
  _clippingPath << Point(x, y);
  _clippingPath << Point(x + width, y);
  _clippingPath << Point(x + width, y - height);
  _clippingPath << Point(x, y - height);
}

void Group::setClippingPath(const std::vector<Point> & points)
{
  _clippingPath.clear();
  std::vector<Point>::const_iterator it = points.begin();
  std::vector<Point>::const_iterator end = points.end();
  while (it != end) {
    _clippingPath << *it;
    ++it;
  }
}

void Group::setClippingPath(const Path & path)
{
  _clippingPath = path;
  _clippingPath.setClosed(true);
  if (_clippingPath.size() > 1) {
    if (_clippingPath[0] == _clippingPath[_clippingPath.size() - 1])
      _clippingPath.pop_back();
  }
}

void Group::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  if (_clippingPath.size() > 2) {
    stream << "%%% Begin Clipped Group " << _clippingCount << "\n";
    stream << " gsave n ";
    _clippingPath.flushPostscript(stream, transform);
    stream << " 0 slw clip " << std::endl;
    ShapeList::flushPostscript(stream, transform);
    stream << " grestore\n";
    stream << "%%% End Clipped Group " << _clippingCount << "\n";
    ++_clippingCount;
  } else {
    stream << "%%% Begin Group\n";
    ShapeList::flushPostscript(stream, transform);
    stream << "%%% End Group\n";
  }
}

void Group::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  Rect bbox = boundingBox(UseLineWidth);
  stream << "# Begin group\n";
  stream << "6 " << transform.mapX(bbox.left) << " " << transform.mapY(bbox.top) << " " << transform.mapX(bbox.left + bbox.width) << " " << transform.mapY(bbox.top - bbox.height) << "\n";
  ShapeList::flushFIG(stream, transform, colormap);
  stream << "-6\n";
  stream << "# End Group\n";
}

void Group::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  if (_clippingPath.size() > 2) {
    stream << "<g clip-rule=\"nonzero\">\n"
           << " <clipPath id=\"LocalClipPath" << _clippingCount << "\">\n"
           << "  <path clip-rule=\"evenodd\"  d=\"";
    _clippingPath.flushSVGCommands(stream, transform);
    stream << "\" />\n";
    stream << " </clipPath>\n";
    stream << "<g clip-path=\"url(#LocalClipPath" << _clippingCount << ")\">\n";
    ++_clippingCount;
    ShapeList::flushSVG(stream, transform);
    stream << "</g>\n";
    stream << "</g>\n";
  } else {
    stream << "<g>\n";
    ShapeList::flushSVG(stream, transform);
    stream << "</g>\n";
  }
}

void Group::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  // FIXME: implement clipping
  stream << "\\begin{scope}\n";
  ShapeList::flushTikZ(stream, transform);
  stream << "\\end{scope}\n";
}

Rect Group::boundingBox(LineWidthFlag lineWidthFlag) const
{
  if (_clippingPath.size() > 2)
    return ShapeList::boundingBox(lineWidthFlag) && _clippingPath.boundingBox();
  else
    return ShapeList::boundingBox(lineWidthFlag);
}

Group * Group::clone() const
{
  return new Group(*this);
}

Group & Group::operator=(const Group & other)
{
  ShapeList::operator=(other);
  return *this;
}

std::size_t Group::_clippingCount = 0;

} // namespace LibBoard
