/* -*- mode: c++ -*- */
/**
 * @file   ShapeList.h
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
 *
 * @brief  ShapeList class
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
#ifndef BOARD_SHAPELIST_H
#define BOARD_SHAPELIST_H

#include <board/Exception.h>
#include <board/Shape.h>
#include <board/Tools.h>
#include <queue>
#include <stack>
#include <typeinfo>
#include <vector>

namespace LibBoard
{

struct Group;

/**
 * The ShapeList structure.
 * @brief A group of shapes
 */
struct ShapeList : public Shape {

  typedef std::vector<Shape *>::size_type size_type;

  inline ShapeList();

  ShapeList(const ShapeList & other);

  ShapeList & operator=(const ShapeList & other);

  ShapeList(ShapeList && other);

  ShapeList & operator=(ShapeList && other);

  /**
   * Create a ShapeList by repeating a shape (translation & scaling)
   * @param shape The shape to be repeated.
   * @param times The number of repetitions.
   * @param dx The x shift between two repetitions.
   * @param dy The y shift between two repetitions.
   * @param scale The scaling factor between two repetitions.
   */
  ShapeList(const Shape & shape, unsigned int times, double dx, double dy, double scale);

  /**
   * Create a ShapeList by repeating a shape (translation, scaling & rotation)
   * @param shape The shape to be repeated.
   * @param times The number of repetitions.
   * @param dx The x shift between two repetitions.
   * @param dy The y shift between two repetitions.
   * @param scaleX The x scaling factor between two repetitions.
   * @param scaleY The y scaling factor between two repetitions.
   * @param angle The rotation angle between two repetitions.
   */
  ShapeList(const Shape & shape, unsigned int times, double dx, double dy, double scaleX, double scaleY, double angle);

  /**
   * Construct a ShapeList from a vector of shapes
   * @param shapes A vector of shapes
   */
  template <typename S> ShapeList(const std::vector<S> & shapes);

  ~ShapeList() override;

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const override;

  /**
   * Remove all shapes from the list.
   * @return The ShapeList itself
   */
  ShapeList & clear();

  ShapeList & rotate(double angle, const Point & center) override;

  /**
   * Get a rotated copy of the shape list around a specified rotation center.
   * @param angle Rotation angle in radian
   * @param center Center of rotation
   * @return Rotated copy of the ShapeList object.
   */
  ShapeList rotated(double angle, const Point & center);

  /**
   * Rotate the shape list around its bounding box's center.
   * @param angle Rotation angle in radian
   * @return The ShapeList itself.
   */
  ShapeList & rotate(double angle) override;

  /**
   * Get a rotated copy of the shape list around its bounding box's center.
   * @param angle Rotation angle in radian
   * @return Rotated copy of the ShapeList object.
   */
  ShapeList rotated(double angle);

  /**
   * Translate the shape list.
   * @param dx x shift value
   * @param dy y shift value
   * @return The ShapeList itself.
   */
  ShapeList & translate(double dx, double dy) override;

  /**
   * Get a translated copy of the shape list.
   * @param dx x shift value
   * @param dy y shift value
   * @return Translated copy of the ShapeList object.
   */
  ShapeList translated(double dx, double dy);

  /**
   * Scale the shape list in X and Y directions.
   * @param sx x scaling factor
   * @param sy y scaling factor
   * @return The ShapeList itself.
   */
  ShapeList & scale(double sx, double sy) override;

  /**
   * Scale the shape list.
   * @param s The scaling factor
   * @return The ShapeList itself.
   */
  ShapeList & scale(double s) override;

  /**
   * Get a scaled copy of the shape list.
   * @param sx x scaling factor
   * @param sy y scaling factor
   * @return Scaled copy of the ShapeList object.
   */
  ShapeList scaled(double sx, double sy) const;

  /**
   * Get a scaled copy of the shape list.
   * @param s scaling factor
   * @return Scaled copy of the ShapeList object.
   */
  ShapeList scaled(double s) const;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll(double s) override;

  void flushPostscript(std::ostream & stream, const TransformEPS & transform) const override;

  void flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const override;

  void flushSVG(std::ostream & stream, const TransformSVG & transform) const override;

  void flushTikZ(std::ostream & stream, const TransformTikZ & transform) const override;

  Rect boundingBox(LineWidthFlag) const override;

  ShapeList * clone() const override;

  /**
   * Add a shape to the list, taking ownership.
   *
   * @param shape The shape to be inserted.
   * @return The shapelist itself.
   */
  ShapeList & push_back(Shape * shape);

  /**
   * Adds a shape to the shape list. If the shape has no given depth
   * or is a compound shape (ShapeList) then it is placed on top of
   * the shapes stack. Otherwise, the shape depth is left unchanged.
   *
   * @param shape
   *
   * @return
   */
  ShapeList & operator<<(const Shape & shape);

  /**
   * Add of shapes from a vector to the shape list.
   */
  template <typename S> //
  ShapeList & operator<<(const std::vector<S> & shapes);

  /**
   * Adds a shape to the list of shape, always preserving the shape's depth.
   *
   * @param shape
   *
   * @return
   */
  ShapeList & operator+=(const Shape & shape);

  /**
   * Append a shape beside the shapelist.
   *
   * @param shape A shape.
   * @param direction The direction where the shape should be appended.
   * @param alignment The alignement with the current shapelist.
   * @param margin A margin between the shapelist and the shape.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   * @return The shapelist itself, after the shape has been appended.
   */
  ShapeList & append(const Shape & shape, Direction direction = Direction::Right, Alignment alignment = Alignment::Center, double margin = 0.0, LineWidthFlag lineWidthFlag = UseLineWidth);

  /**
   * Add a repeated shape (with translation, scaling & rotation)
   * @param shape The shape to be repeated.
   * @param times The number of repetitions.
   * @param dx The x shift between two repetitions.
   * @param dy The y shift between two repetitions.
   * @param scaleX The x scaling factor between two repetitions.
   * @param scaleY The y scaling factor between two repetitions.
   * @param angle The rotation angle between two repetitions.
   */
  void addRepeated(const Shape & shape, unsigned int times, double dx, double dy, double scaleX = 1.0, double scaleY = 1.0, double angle = 0.0);

  /**
   * Duplicates the last inserted shape.
   * @param copies The number of copies.
   */
  ShapeList & duplicateLast(std::size_t copies = 1);

  /**
   * Return the last inserted shape with its actual type.
   *
   * @param position The position. 0 is the last inserted shape, 1 is the one before, etc.
   * @return A reference to the addressed shape.
   */
  template <typename T> T & last(const std::size_t position = 0);

  /**
   * Return a reference to the last inserted Shape.
   *
   * @param position The position. 0 is the last inserted shape, 1 is the one before, etc.
   * @return A reference to the addressed shape.
   */
  Shape & last(const std::size_t position = 0);

  /**
   * Find the n-th most recently inserted shape with type T.
   *
   * @return A reference to the n-th most recently inserted shape with type T.
   */
  template <typename T> T & topLevelFindLast(std::size_t position = 0);

  /**
   * Convenience function that simply calls last(0).
   * @return A reference to the last inserted shape
   */
  Shape & top();

  /**
   * @brief Accepts a visitor object.
   * @param visitor A visitor object.
   */
  void accept(ShapeVisitor & visitor) override;

  /**
   * @brief Accepts a visitor object.
   *
   * @param visitor A visitor object.
   */
  void accept(const ShapeVisitor & visitor) override;

  /**
   * @brief Accepts a const-shape visitor object.
   *
   * @param visitor A const-shape visitor object.
   */
  void accept(ConstShapeVisitor & visitor) const override;

  /**
   * @brief Accepts a const-shape visitor object.
   *
   * @param visitor A const-shape visitor object.
   */
  void accept(const ConstShapeVisitor & visitor) const override;

  /**
   * @brief Accept a composite shape transform.
   *
   * @param transform A composite shape transform object.
   */
  Shape * accept(CompositeShapeTransform & transform) const override;

  /**
   * @brief Accept a constant composite shape transform.
   *
   * @param transform A constant composite shape transform object..
   */
  Shape * accept(const CompositeShapeTransform & transform) const override;

  /**
   * @brief The TopLevelIterator struct
   *
   * Allows the traversal of a ShapeList using an STL-like syntax.
   */
  struct TopLevelIterator {
    inline TopLevelIterator(std::vector<Shape *>::iterator it);
    inline Shape & operator*();
    inline Shape * operator->();
    inline Shape * pointer();
    inline TopLevelIterator & operator++();
    inline TopLevelIterator operator++(int);
    inline bool operator==(const TopLevelIterator & other);
    inline bool operator!=(const TopLevelIterator & other);

  private:
    std::vector<Shape *>::iterator _it;
  };

  /**
   * @brief The TopLevelConstIterator struct
   *
   * Allows the traversal of a const ShapeList using an STL-like syntax.
   */
  struct TopLevelConstIterator {
    inline TopLevelConstIterator(std::vector<Shape *>::const_iterator it);
    inline const Shape & operator*();
    inline const Shape * operator->();
    inline const Shape * pointer();
    inline TopLevelConstIterator & operator++();
    inline TopLevelConstIterator operator++(int);
    inline bool operator==(const TopLevelConstIterator & other);
    inline bool operator!=(const TopLevelConstIterator & other);

  private:
    std::vector<Shape *>::const_iterator _it;
  };

  /**
   * @brief The DepthFirstIterator struct allows to traverse the shape tree
   * using a depth-first strategy.
   */
  struct DepthFirstIterator {
    inline DepthFirstIterator();
    inline DepthFirstIterator(ShapeList * list);
    inline Shape & operator*();
    inline Shape * operator->();
    inline Shape * pointer();
    inline bool operator==(const DepthFirstIterator & other);
    inline bool operator!=(const DepthFirstIterator & other);
    inline DepthFirstIterator & operator++();
    inline DepthFirstIterator operator++(int);

  private:
    void moveToFirstActuelShape();
    void moveToNextActualShape();
    std::stack<ShapeList *> _shapeListsStack;
    std::stack<TopLevelIterator> _iteratorsStack;
  };

  /**
   * @brief The BreadthFirstIterator struct allows to traverse the shape tree
   * using a breadth-first strategy.
   */
  struct BreadthFirstIterator {
    inline BreadthFirstIterator();
    inline BreadthFirstIterator(ShapeList * list);
    inline Shape & operator*();
    inline Shape * operator->();
    inline Shape * pointer();
    inline bool operator==(const BreadthFirstIterator & other);
    inline bool operator!=(const BreadthFirstIterator & other);
    inline BreadthFirstIterator & operator++();
    inline BreadthFirstIterator operator++(int);

  private:
    void moveToFirstActuelShape();
    void moveToNextActualShape();
    std::queue<ShapeList *> _shapeListsQueue;
    std::queue<TopLevelIterator> _iteratorsQueue;
  };

  /**
   * @brief begin
   * @return An iterator referencing the first shape of the ShapeList.
   */
  inline TopLevelIterator begin();

  /**
   * @brief end
   * @return An iterator pointing at the end of the ShapeList.
   */
  inline TopLevelIterator end();

  /**
   * @brief begin
   * @return A const iterator referencing the first shape of the ShapeList.
   */
  inline TopLevelConstIterator begin() const;

  /**
   * @brief cbegin
   * @return A const iterator referencing the first shape of the ShapeList.
   */
  inline TopLevelConstIterator cbegin() const;

  /**
   * @brief end
   * @return A const iterator pointing at the end of the ShapeList.
   */
  inline TopLevelConstIterator end() const;

  /**
   * @brief cend
   * @return A const iterator pointing at the end of the ShapeList.
   */
  inline TopLevelConstIterator cend() const;

  /**
   * @brief depthFirstBegin
   * @return A depth-first iterator at the begining of the ShapeList shapes.
   */
  inline DepthFirstIterator depthFirstBegin();

  /**
   * @brief depthFirstEnd
   * @return A depth-first iterator at the end of the ShapeList shapes.
   */
  inline DepthFirstIterator depthFirstEnd();

  /**
   * @brief breadthFirstBegin
   * @return A breadth-first iterator at the begining of the ShapeList shapes.
   */
  inline BreadthFirstIterator breadthFirstBegin();

  /**
   * @brief breadthFirstEnd
   * @return A breadth-first iterator at the end of the ShapeList shapes.
   */
  inline BreadthFirstIterator breadthFirstEnd();

  /**
   * @brief Recursively counts the number of shapes in the list.
   * @return The total number of shapes in the
   */
  std::size_t deepSize() const;

  /**
   * @brief The number of shapes in the list (at top level).
   * @return The number of shapes (at top level).
   */
  inline std::size_t size() const;

private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
  void addShape(const Shape & shape, double scaleFactor);

  std::vector<Shape *> _shapes; /**< The vector of shapes (back to front). */

  /**
   * Free the memory used by the shapes in the shape vector.
   */
  void deleteShapes();
};

// Inline methods and functions

#if defined(max)
#undef max
#define HAS_MSVC_MAX true
#endif

ShapeList::ShapeList() {}

template <typename T> T & ShapeList::last(const std::size_t position)
{
  if (position < _shapes.size()) {
    std::vector<Shape *>::reverse_iterator it = _shapes.rbegin() + static_cast<std::vector<Shape *>::difference_type>(position);
    Shape * pshape = *it;
    T * result = dynamic_cast<T *>(pshape);
    if (!result) {
      std::cerr << "Error: ShapeList::last<> called with invalid result type\n";
      exit(-1);
    }
    return dynamic_cast<T &>(*result);
  } else {
    Tools::error << "Trying to access an element that does not exist (" << position << "/" << _shapes.size() << ").\n";
    throw -1;
  }
}

template <typename T> T & ShapeList::topLevelFindLast(std::size_t position)
{
  std::vector<Shape *>::reverse_iterator it = _shapes.rbegin();
  while (it != _shapes.rend()) {
    T * shape = dynamic_cast<T *>(*it);
    if (shape) {
      if (!position) {
        return *shape;
      } else {
        --position;
      }
    }
    ++it;
  }
  throw Exception("topLevelFindLast<T>(): no such shape type found (" + std::string(typeid(T).name()) + ")");
}

ShapeList::TopLevelIterator::TopLevelIterator(std::vector<Shape *>::iterator it) : _it(it) {}

Shape & ShapeList::TopLevelIterator::operator*()
{
  return **_it;
}

Shape * ShapeList::TopLevelIterator::operator->()
{
  return *_it;
}

Shape * ShapeList::TopLevelIterator::pointer()
{
  return *_it;
}

ShapeList::TopLevelIterator & ShapeList::TopLevelIterator::operator++()
{
  ++_it;
  return *this;
}

ShapeList::TopLevelIterator ShapeList::TopLevelIterator::operator++(int)
{
  TopLevelIterator previous(*this);
  ++_it;
  return previous;
}

bool ShapeList::TopLevelIterator::operator==(const TopLevelIterator & other)
{
  return _it == other._it;
}

bool ShapeList::TopLevelIterator::operator!=(const TopLevelIterator & other)
{
  return _it != other._it;
}

ShapeList::TopLevelConstIterator::TopLevelConstIterator(std::vector<Shape *>::const_iterator it) : _it(it) {}

const Shape & ShapeList::TopLevelConstIterator::operator*()
{
  return **_it;
}

const Shape * ShapeList::TopLevelConstIterator::operator->()
{
  return *_it;
}

const Shape * ShapeList::TopLevelConstIterator::pointer()
{
  return *_it;
}

ShapeList::TopLevelConstIterator & ShapeList::TopLevelConstIterator::operator++()
{
  ++_it;
  return *this;
}

ShapeList::TopLevelConstIterator ShapeList::TopLevelConstIterator::operator++(int)
{
  TopLevelConstIterator previous(*this);
  ++_it;
  return previous;
}

bool ShapeList::TopLevelConstIterator::operator==(const TopLevelConstIterator & other)
{
  return _it == other._it;
}

bool ShapeList::TopLevelConstIterator::operator!=(const TopLevelConstIterator & other)
{
  return _it != other._it;
}

ShapeList::TopLevelIterator ShapeList::begin()
{
  return TopLevelIterator(_shapes.begin());
}

ShapeList::TopLevelIterator ShapeList::end()
{
  return TopLevelIterator(_shapes.end());
}

ShapeList::TopLevelConstIterator ShapeList::begin() const
{
  return TopLevelConstIterator(_shapes.begin());
}

ShapeList::TopLevelConstIterator ShapeList::cbegin() const
{
  return TopLevelConstIterator(_shapes.begin());
}

ShapeList::TopLevelConstIterator ShapeList::end() const
{
  return TopLevelConstIterator(_shapes.end());
}

ShapeList::TopLevelConstIterator ShapeList::cend() const
{
  return TopLevelConstIterator(_shapes.end());
}

ShapeList::DepthFirstIterator ShapeList::depthFirstBegin()
{
  return DepthFirstIterator(this);
}

ShapeList::DepthFirstIterator ShapeList::depthFirstEnd()
{
  return DepthFirstIterator();
}

ShapeList::BreadthFirstIterator ShapeList::breadthFirstBegin()
{
  return BreadthFirstIterator(this);
}

ShapeList::BreadthFirstIterator ShapeList::breadthFirstEnd()
{
  return BreadthFirstIterator();
}

std::size_t ShapeList::size() const
{
  return _shapes.size();
}

ShapeList::DepthFirstIterator::DepthFirstIterator() {}

ShapeList::DepthFirstIterator::DepthFirstIterator(ShapeList * list)
{
  _shapeListsStack.push(list);
  _iteratorsStack.push(list->begin());
  moveToFirstActuelShape();
}

Shape & ShapeList::DepthFirstIterator::operator*()
{
  return *(_iteratorsStack.top());
}

Shape * ShapeList::DepthFirstIterator::operator->()
{
  return pointer();
}

Shape * ShapeList::DepthFirstIterator::pointer()
{
  return _iteratorsStack.top().pointer();
}

bool ShapeList::DepthFirstIterator::operator==(const DepthFirstIterator & other)
{
  return (_shapeListsStack.empty() && other._shapeListsStack.empty()) ||
         ((!_shapeListsStack.empty() && !other._shapeListsStack.empty()) && (_shapeListsStack.top() == other._shapeListsStack.top()) && (_iteratorsStack.top() == other._iteratorsStack.top()));
}

bool ShapeList::DepthFirstIterator::operator!=(const DepthFirstIterator & other)
{
  return !(*this == other);
}

ShapeList::DepthFirstIterator & ShapeList::DepthFirstIterator::operator++()
{
  moveToNextActualShape();
  return *this;
}

ShapeList::DepthFirstIterator ShapeList::DepthFirstIterator::operator++(int)
{
  DepthFirstIterator currentValue(*this);
  moveToNextActualShape();
  return currentValue;
}

ShapeList::BreadthFirstIterator::BreadthFirstIterator() {}

ShapeList::BreadthFirstIterator::BreadthFirstIterator(ShapeList * list)
{
  _shapeListsQueue.push(list);
  _iteratorsQueue.push(list->begin());
  moveToFirstActuelShape();
}

Shape & ShapeList::BreadthFirstIterator::operator*()
{
  return *(_iteratorsQueue.front());
}

Shape * ShapeList::BreadthFirstIterator::operator->()
{
  return pointer();
}

Shape * ShapeList::BreadthFirstIterator::pointer()
{
  return _iteratorsQueue.front().pointer();
}

bool ShapeList::BreadthFirstIterator::operator==(const BreadthFirstIterator & other)
{
  return (_shapeListsQueue.empty() && other._shapeListsQueue.empty()) ||
         ((!_shapeListsQueue.empty() && !other._shapeListsQueue.empty()) && (_shapeListsQueue.front() == other._shapeListsQueue.front()) && (_iteratorsQueue.front() == other._iteratorsQueue.front()));
}

bool ShapeList::BreadthFirstIterator::operator!=(const BreadthFirstIterator & other)
{
  return !(*this == other);
}

ShapeList::BreadthFirstIterator & ShapeList::BreadthFirstIterator::operator++()
{
  moveToNextActualShape();
  return *this;
}

ShapeList::BreadthFirstIterator ShapeList::BreadthFirstIterator::operator++(int)
{
  BreadthFirstIterator currentValue(*this);
  moveToNextActualShape();
  return currentValue;
}

template <typename S>                               //
ShapeList::ShapeList(const std::vector<S> & shapes) //
    : ShapeList()
{
  (*this) << shapes;
}

template <typename S>                                            //
ShapeList & ShapeList::operator<<(const std::vector<S> & shapes) //
{
  for (const S & shape : shapes) {
    (*this) << shape;
  }
  return *this;
}

#if defined(HAS_MSVC_MAX)
#define max(A, B) ((A) > (B) ? (A) : (B))
#endif

} // namespace LibBoard

#endif /* BOARD_SHAPELIST_H */
