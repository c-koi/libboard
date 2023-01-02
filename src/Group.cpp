/* -*- mode: c++ -*- */
/**
 * @file   Group.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
 *
 * @brief  Definition of the Group class.
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
#include <board/Group.h>
#include <algorithm>
#include <typeinfo>
#include <utility>
#include <BoardConfig.h>
#include <board/ShapeVisitor.h>
#include <board/Tools.h>

#if defined(max)
#undef max
#endif

namespace LibBoard
{

//
// Definition of the Group methods.
//

const std::string Group::_name("GroupOfShapes");
std::size_t Group::_clippingCount = 0;

Group::~Group() {}

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

void Group::setClippingRectangle(double x, double y, double width, double height)
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
  _clippingPath.close();
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
  if (_clippingPath.size() > 2) {
    return ShapeList::boundingBox(lineWidthFlag) && _clippingPath.boundingBox();
  } else {
    return ShapeList::boundingBox(lineWidthFlag);
  }
}

Group * Group::accept(CompositeShapeTransform & transform) const
{
  Group * group = new Group;
  std::vector<Shape *>::const_iterator i = _shapes.begin();
  std::vector<Shape *>::const_iterator end = _shapes.end();
  while (i != end) {
    const Shape & shape = **i;
    group->push_back(shape.accept(transform));
    ++i;
  }
  return group;
}

Group * Group::accept(const CompositeShapeTransform & transform) const
{
  Group * group = new Group;
  std::vector<Shape *>::const_iterator i = _shapes.begin();
  std::vector<Shape *>::const_iterator end = _shapes.end();
  while (i != end) {
    const Shape & shape = **i;
    group->push_back(shape.accept(transform));
    ++i;
  }
  return group;
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

} // namespace LibBoard
