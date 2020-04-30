/* -*- mode: c++ -*- */
/**
 * @file   Polyline.h
 * @author Sebastien Fourey (GREYC)
 * @date   Aug 2007
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
#ifndef _BOARD_POLYLINE_H_
#define _BOARD_POLYLINE_H_

#include "board/Shape.h"

#if __cplusplus < 201100
#define override
#endif

namespace LibBoard
{

/**
 * The polyline structure.
 * @brief A polygonal line described by a series of 2D points.
 */
struct Polyline : public Shape {

  inline Polyline(const std::vector<Point> & points, bool closed, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(), double lineWidth = Shape::defaultLineWidth(),
                  const LineStyle lineStyle = Shape::defaultLineStyle(), const LineCap cap = Shape::defaultLineCap(), const LineJoin join = Shape::defaultLineJoin(), int depth = -1);

  inline Polyline(const Path & path, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(), double lineWidth = Shape::defaultLineWidth(),
                  const LineStyle lineStyle = Shape::defaultLineStyle(), const LineCap cap = Shape::defaultLineCap(), const LineJoin join = Shape::defaultLineJoin(), int depth = -1);

  inline Polyline(bool closed, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(), double lineWidth = Shape::defaultLineWidth(),
                  const LineStyle lineStyle = Shape::defaultLineStyle(), const LineCap cap = Shape::defaultLineCap(), const LineJoin join = Shape::defaultLineJoin(), int depth = -1);

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const override;

  /**
   * Add a point to the polyline.
   *
   * @param p
   *
   * @return
   */
  Polyline & operator<<(const Point & p);

  /**
   * Returns the n-th point of the polyline.
   *
   * @param i
   *
   * @return
   */
  Point & operator[](const std::size_t n) { return _path[n]; }

  /**
   * Returns the n-th point of the polyline.
   *
   * @param i
   *
   * @return
   */
  const Point & operator[](const std::size_t n) const { return _path[n]; }

  Polyline & rotate(double angle, const Point & center) override;

  /**
   *
   *
   * @param angle
   * @param center
   *
   * @return
   */
  Polyline rotated(double angle, const Point & center) const;

  Polyline & rotate(double angle) override;

  /**
   *
   *
   * @param angle
   *
   * @return
   */
  Polyline rotated(double angle) const;

  /**
   * Translate the polyline by a given offset.
   *
   * @param dx The x offset.
   * @param dy The y offset.
   *
   * @return A reference to the polyline itself.
   */
  Polyline & translate(double dx, double dy) override;

  /**
   * Returns a translated copy of the polyline.
   *
   * @param dx Shift of the first coordinate.
   * @param dy Shift of the second coordinate.
   *
   * @return A copy of the polyline, translated.
   */
  Polyline translated(double dx, double dy) const;

  /**
   * Scale the polyline, given two scaling factors.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy Scaling factor along the y axis.
   *
   * @return A reference to the polyline itself, once scaled.
   */
  Polyline & scale(double sx, double sy) override;

  /**
   * Scale the polyline, given a scaling factor.
   *
   * @param s Scaling factor.
   *
   * @return A reference to the polyline itself, once scaled.
   */
  Polyline & scale(double s) override;

  /**
   * Returns a scaled copy of the line.
   *
   * @param sx The scaling factor along the x axis.
   * @param sy The scaling factor along the y axis.
   *
   * @return A scaled copy of the line.
   */
  Polyline scaled(double sx, double sy) const;

  /**
   * Returns a scaled copy of the line.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the line.
   */
  Polyline scaled(double s) const;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll(double s) override;

  /**
   * Returns a resized copy of the polyline.
   *
   * @param w Width of the new polyline.
   * @param h Height of the new polyline.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   * @return A resized copy of the polyline.
   */
  Polyline resized(double w, double h, LineWidthFlag lineWidthFlag) const;

  /**
   * @brief Add a hole to the path. Warning: Hole share the polyline line style.
   * @param path Vertices of the polyline
   */
  void addHole(const Path & path);

  void flushPostscript(std::ostream & stream, const TransformEPS & transform) const override;

  void flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const override;

  void flushSVG(std::ostream & stream, const TransformSVG & transform) const override;

  void flushTikZ(std::ostream & stream, const TransformTikZ & transform) const override;

  Rect boundingBox(LineWidthFlag) const override;

  Polyline * clone() const override;

  inline std::size_t vertexCount() const;

  inline const Path & path() const;

  void setRectangleFlag();

private:
  static const std::string _name; /**< The generic name of the shape. */
  bool _isCreatedRectangle;
  void flushRectangleFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const;
  void flushRectangleSVG(std::ostream & stream, const TransformSVG & transform) const;

protected:
  Path _path;
  std::vector<Path> _holes;
};

Polyline rectangle(double left, double top, double width, double height, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(),
                   double lineWidth = Shape::defaultLineWidth(), const Shape::LineStyle lineStyle = Shape::defaultLineStyle(), const Shape::LineCap cap = Shape::defaultLineCap(),
                   const Shape::LineJoin join = Shape::defaultLineJoin(), int depth = -1);

Polyline rectangle(const Rect & rect, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(), double lineWidth = Shape::defaultLineWidth(),
                   const Shape::LineStyle lineStyle = Shape::defaultLineStyle(), const Shape::LineCap cap = Shape::defaultLineCap(), const Shape::LineJoin join = Shape::defaultLineJoin(),
                   int depth = -1);

Polyline triangle(const Point & p1, const Point & p2, const Point & p3, Color penColor = Shape::defaultPenColor(), Color fillColor = Shape::defaultFillColor(),
                  double lineWidth = Shape::defaultLineWidth(), const Shape::LineStyle lineStyle = Shape::defaultLineStyle(), const Shape::LineCap cap = Shape::defaultLineCap(),
                  const Shape::LineJoin join = Shape::defaultLineJoin(), int depth = -1);

Polyline triangle(const double x1, const double y1, const double x2, const double y2, const double x3, const double y3, Color penColor = Shape::defaultPenColor(),
                  Color fillColor = Shape::defaultFillColor(), double lineWidth = Shape::defaultLineWidth(), const Shape::LineStyle lineStyle = Shape::defaultLineStyle(),
                  const Shape::LineCap cap = Shape::defaultLineCap(), const Shape::LineJoin join = Shape::defaultLineJoin(), int depth = -1);

/**
 * The GouraudTriangle structure.
 * @brief A triangle with shaded filling according to colors given for each vertex.
 */
struct GouraudTriangle : public Polyline {

  GouraudTriangle(const Point & p0, const Color & color0, const Point & p1, const Color & color1, const Point & p2, const Color & color2, int subdivisions, int depth = -1);

  GouraudTriangle(const Point & p0, float brightness0, const Point & p1, float brightness1, const Point & p2, float brightness2, const Color & fillColor, int subdivisions, int depth = -1);

  /**
   * Returns the generic name of the shape (e.g., Circle, Rectangle, etc.)
   *
   * @return
   */
  const std::string & name() const override;

  GouraudTriangle & rotate(double angle, const Point & center) override;

  GouraudTriangle rotated(double angle, const Point & center) const;

  GouraudTriangle & rotate(double angle) override;

  GouraudTriangle rotated(double angle) const;

  /**
   * Returns a translated copy of the triangle.
   *
   * @param dx The shift along the x axis.
   * @param dy The shift along the y axis.
   *
   * @return A translated copy of the triangle.
   */
  GouraudTriangle translated(double dx, double dy) const;

  /**
   * Returns a scaled copy of the triangle.
   *
   * @param sx Scaling factor along the x axis.
   * @param sy scaling factor along the y axis.
   *
   * @return A scaled copy of the arrow.
   */
  GouraudTriangle scaled(double sx, double sy) const;

  /**
   * Returns a scaled copy of the triangle.
   *
   * @param s The scaling factor.
   *
   * @return A scaled copy of the tiangle.
   */
  GouraudTriangle scaled(double s) const;

  /**
   * Scales all the values (positions, dimensions, etc.) associated
   * with the shape.
   *
   * @param s The scaling factor.
   */
  void scaleAll(double s) override;

  /**
   * Returns a resized copy of the triangle.
   *
   * @param w Width of the new triangle.
   * @param h Height of the new triangle.
   * @param lineWidthFlag Should the line width be considered when computing bounding boxes.
   * @return A resized copy of the triangle.
   */
  GouraudTriangle resized(double w, double h, LineWidthFlag lineWidthFlag) const;

  /**
   * Sends the triangle to a Postscript document.
   *
   * @param stream
   * @param transform
   */
  void flushPostscript(std::ostream & stream, const TransformEPS & transform) const override;

  /**
   * Sends the Triangle to a FIG file format stream.
   * <p><b>Warning!</b> Because shading would generally require
   * more colors in the colormap than allowed by the FIG file format,
   * rendering a Gouraud triangle in an XFig file is the same as rendering
   * a simple triangle whose filling color is the average of the vertex colors.
   *
   * @param stream
   * @param transform
   * @param Color
   * @param colormap
   */
  void flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const override;

  void flushSVG(std::ostream & stream, const TransformSVG & transform) const override;

  void flushTikZ(std::ostream & stream, const TransformTikZ & transform) const override;

  GouraudTriangle * clone() const override;

private:
  static const std::string _name; /**< The generic name of the shape. */

protected:
  Color _color0;
  Color _color1;
  Color _color2;
  int _subdivisions;
};

} // namespace LibBoard

/*
 * Inline methods
 */
namespace LibBoard
{

Polyline::Polyline(const std::vector<Point> & points, bool closed, Color penColor, Color fillColor, double lineWidth, const LineStyle lineStyle, const LineCap cap, const LineJoin join, int depth)
    : Shape(penColor, fillColor, lineWidth, lineStyle, cap, join, depth), _path(points, closed)
{
  _isCreatedRectangle = false;
}

Polyline::Polyline(const Path & path, Color penColor, Color fillColor, double lineWidth, const LineStyle lineStyle, const LineCap cap, const LineJoin join, int depth)
    : Shape(penColor, fillColor, lineWidth, lineStyle, cap, join, depth), _path(path)
{
  _isCreatedRectangle = false;
}

Polyline::Polyline(bool closed, Color penColor, Color fillColor, double lineWidth, const LineStyle lineStyle, const LineCap cap, const LineJoin join, int depth)
    : Shape(penColor, fillColor, lineWidth, lineStyle, cap, join, depth), _path(closed)
{
  _isCreatedRectangle = false;
}

std::size_t Polyline::vertexCount() const
{
  return _path.size();
}

const Path & Polyline::path() const
{
  return _path;
}
} // namespace LibBoard

#if __cplusplus < 201100
#undef override
#endif

#endif /* _BOARD_POLYLINE_H_ */
