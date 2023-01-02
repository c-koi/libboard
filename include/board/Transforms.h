/* -*- mode: c++ -*- */
/**
 * @file   Transforms.h
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
#ifndef BOARD_TRANSFORMS_H
#define BOARD_TRANSFORMS_H

#include <board/Point.h>
#include <board/Rect.h>
#include <board/TransformMatrix.h>
#include <cmath>
#include <limits>
#include <map>
#include <vector>
namespace LibBoard
{

struct Shape;
struct ShapeList;

/**
 * The base class for transforms.
 * @brief
 */
struct Transform {
  inline Transform();
  virtual ~Transform();
  virtual double mapX(double x) const;
  virtual double mapY(double y) const = 0;
  virtual Point map(const Point &) const;
  virtual void apply(double & x, double & y) const;
  virtual double scale(double x) const;
  virtual Point scale(const Point &) const;
  virtual double rounded(double x) const;
  virtual void setBoundingBox(const Rect & rect, const double pageWidth, const double pageHeight, const double margin) = 0;
  static inline double round(const double & x);

protected:
  double _scale;
  double _deltaX;
  double _deltaY;
  double _height;
};

/**
 * The TransformEPS structure.
 * @brief Structure representing a scaling and translation
 * suitable for an EPS output.
 */
struct TransformEPS : public Transform {
  double mapWidth(double w) const;
  double mapY(double y) const;
  void setBoundingBox(const Rect & rect, const double pageWidth, const double pageHeight, const double margin);
  double scaleBackMM(double);
  Rect pageBoundingBox() const;

private:
  Rect _pageBoundingBox;
};

/**
 * The TransformFIG structure.
 * @brief Structure representing a scaling and translation
 * suitable for an XFig output.
 */
struct TransformFIG : public Transform {
  inline TransformFIG();
  double rounded(double x) const;
  double mapY(double y) const;
  int mapWidth(double width) const;
  void setBoundingBox(const Rect & rect, const double pageWidth, const double pageHeight, const double margin);
  unsigned int shapeDepth(const Shape *) const;
  unsigned int mapDepth(unsigned int depth) const;
  void setDepthMap(const std::map<const Shape *, unsigned int> *, unsigned int min);

private:
  unsigned int _maxDepth;
  unsigned int _minDepth;
  double _postscriptScale;
  const std::map<const Shape *, unsigned int> * _depthMap;
};

/**
 * The TransformSVG structure.
 * @brief Structure representing a scaling and translation
 * suitable for an SVG output.
 */
struct TransformSVG : public Transform {
  double rounded(double x) const;
  double mapY(double y) const;
  double mapWidth(double width) const;
  void setBoundingBox(const Rect & rect, const double pageWidth, const double pageHeight, const double margin);
  double scaleBackMM(double);
  TransformMatrix matrix() const;
  Point translation() const;
  double deltaX() const;
  double deltaY() const;
};

/**
 * The TransformTikZ structure.
 * @brief Structure representing a scaling and translation
 * suitable for an TikZ output.
 */
struct TransformTikZ : public TransformSVG {
  ~TransformTikZ() override;
};

// Inline methods and functions

#if defined(max)
#undef max
#define HAS_MSVC_MAX true
#endif

Transform::Transform() : _scale(1.0), _deltaX(0.0), _deltaY(0.0), _height(0.0) {}

TransformFIG::TransformFIG() : _maxDepth(std::numeric_limits<int>::max()), _minDepth(0), _postscriptScale(1.0)
{
  _depthMap = nullptr;
}

double Transform::round(const double & x)
{
  return std::floor(x + 0.5);
}

#if defined(HAS_MSVC_MAX)
#define max(A, B) ((A) > (B) ? (A) : (B))
#endif

} // namespace LibBoard

#endif /* BOARD_TRANSFORMS_H */
