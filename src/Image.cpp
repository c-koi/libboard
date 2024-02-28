/* -*- mode: c++ -*- */
/**
 * @file   Image.cpp
 * @author Sebastien Fourey (GREYC)
 * @date   June 2014
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
#include <BoardConfig.h>
#include <board/Image.h>
#include <board/ShapeVisitor.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>

#if (BOARD_HAVE_MAGICKPLUSPLUS == 1)
#include <Magick++.h>
#endif

namespace LibBoard
{

const std::string LibBoard::Image::_name("Image");

Image::Image(const char * filename, double left, double top, double width, double height)
    : _rectangle(LibBoard::rectangle(left, top, width, height, Color::Black, Color::Null, 0.0, SolidStyle, ButtCap, MiterJoin)),
      _originalRectangle(LibBoard::rectangle(left, top, width, height, Color::Black, Color::Null, 0.0, SolidStyle, ButtCap, MiterJoin)), //
      _filename(filename)
{
  if (height == 0.0) {
#if (BOARD_HAVE_MAGICKPLUSPLUS == 1)
    try {
      Magick::Image image;
      image.read(_filename);
      height = width * (image.rows() / (double)image.columns());
      _rectangle = _originalRectangle = LibBoard::rectangle(left, top, width, height, Color::Black, Color::Null, 0.0, SolidStyle, ButtCap, MiterJoin);
    } catch (std::exception & e) {
      Tools::error << "Image::Image(): ";
      std::cerr << e.what() << std::endl;
    }
#else
    Tools::error << "Image::Image(): ImageMagick is required to retrieve the aspect ratio of bitmap images.\n";
#endif
  }
}

Image::Image(const char * filename, const Rect & rect)
    : _rectangle(LibBoard::rectangle(rect, Color::Black, Color::Null, 0.0, SolidStyle, ButtCap, MiterJoin)),
      _originalRectangle(LibBoard::rectangle(rect, Color::Black, Color::Null, 0.0, SolidStyle, ButtCap, MiterJoin)), _filename(filename)
{
}

const std::string & Image::name() const
{
  return _name;
}

Image * Image::clone() const
{
  return new Image(*this);
}

Shape & Image::rotate(double angle, const Point & center)
{
  _transformMatrixSVG = TransformMatrix::rotation(angle, center, TransformMatrix::SVG) * _transformMatrixSVG;
  _transformMatrixEPS = TransformMatrix::rotation(angle, center, TransformMatrix::Postscript) * _transformMatrixEPS;
  _rectangle.rotate(angle, center);
  return *this;
}

Shape & Image::rotate(double angle)
{
  Point c = _transformMatrixSVG * _originalRectangle.center();
  _transformMatrixSVG = TransformMatrix::rotation(angle, c, TransformMatrix::SVG) * _transformMatrixSVG;
  c = _transformMatrixEPS * _originalRectangle.center();
  _transformMatrixEPS = TransformMatrix::rotation(angle, c, TransformMatrix::Postscript) * _transformMatrixEPS;
  _rectangle.rotate(angle);
  return *this;
}

Image Image::rotated(double angle) const
{
  return static_cast<Image &>(Image(*this).rotate(angle));
}

Image Image::rotated(double angle, const Point & center) const
{
  return static_cast<Image &>(Image(*this).rotate(angle, center));
}

Shape & Image::translate(double dx, double dy)
{
  _rectangle.translate(dx, dy);
  _transformMatrixSVG += Point(dx, dy);
  _transformMatrixEPS += Point(dx, dy);
  return *this;
}

Image Image::translated(double dx, double dy) const
{
  return static_cast<Image &>(Image(*this).translate(dx, dy));
}

Shape & Image::scale(double sx, double sy)
{
  _rectangle.scale(sx, sy);

  Point currentCenter = _transformMatrixSVG * _originalRectangle.center();
  _transformMatrixSVG = TransformMatrix::scaling(sx, sy) * _transformMatrixSVG;
  Point newCenter = _transformMatrixSVG * _originalRectangle.center();
  _transformMatrixSVG = TransformMatrix::translation(currentCenter - newCenter) * _transformMatrixSVG;

  currentCenter = _transformMatrixEPS * _originalRectangle.center();
  _transformMatrixEPS = TransformMatrix::scaling(sx, sy) * _transformMatrixEPS;
  newCenter = _transformMatrixEPS * _originalRectangle.center();
  _transformMatrixEPS = TransformMatrix::translation(currentCenter - newCenter) * _transformMatrixEPS;
  return *this;
}

Shape & Image::scale(double s)
{
  scale(s, s);
  return *this;
}

Image Image::scaled(double sx, double sy)
{
  return static_cast<Image &>(Image(*this).scale(sx, sy));
}

Rect Image::boundingBox(LineWidthFlag) const
{
  return _rectangle.boundingBox(IgnoreLineWidth);
}

void Image::scaleAll(double s)
{
  _transformMatrixSVG *= TransformMatrix::scaling(s, s);
  _transformMatrixEPS *= TransformMatrix::scaling(s, s);
  _rectangle.scaleAll(s);
}

void Image::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
#if (BOARD_HAVE_MAGICKPLUSPLUS == 1)
  Magick::Image image;
  image.read(_filename);
  const char * tmpFilename = Tools::temporaryFilename(".eps");
  image.write(tmpFilename);
  Rect rect;
  Tools::getEPSBoundingBox(tmpFilename, rect);

  double originalWidth = (_originalRectangle[1] - _originalRectangle[0]).norm();
  double originalHeight = (_originalRectangle[2] - _originalRectangle[1]).norm();
  double scaleX = transform.scale(originalWidth) / rect.width;
  double scaleY = transform.scale(originalHeight) / rect.height;

  TransformMatrix originalMoveAndScale = TransformMatrix::scaling(scaleX, scaleY) + _originalRectangle[3];

  stream << "%\n";
  stream << "% Bitmap Image\n";
  stream << "%\n";
  stream << "%%BeginDocument: board_temporary.eps\n";
  stream << "gs\n";

  Point shift = transform.map(_rectangle[3]) - (_transformMatrixEPS * _originalRectangle[3]);
  ((_transformMatrixEPS + shift) * originalMoveAndScale).flushEPS(stream);

  stream << "\n";
  Tools::flushFile(tmpFilename, stream);
  std::remove(tmpFilename);
  stream << "%%EndDocument\n";
  stream << "gr\n";
#else
  (void)stream;
  (void)transform;
  Tools::error << "Image::flushPostscript(): requires ImageMagick's Magick++ lib. Aborted.\n";
#endif
}

void Image::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
  _rectangle.flushFIG(stream, transform, colormap);
  Rect bbox = _rectangle.boundingBox(UseLineWidth);
  Polyline rectangle = LibBoard::rectangle(bbox, Color::Null, Color::Null, 0.0);
  stream << "2 5 0 1 0 -1 " << transform.shapeDepth(this) << " -1 -1 0.000 0 0 -1 0 0 5\n";
  stream << "\t0 " << _filename << "\n";
  stream << "\t";
  for (std::size_t i = 0; i < rectangle.vertexCount(); ++i) {
    stream << " " << static_cast<int>(transform.mapX(rectangle[i].x)) << " " << static_cast<int>(transform.mapY(rectangle[i].y));
  }
  stream << " " << static_cast<int>(transform.mapX(rectangle[0].x)) << " " << static_cast<int>(transform.mapY(rectangle[0].y)) << "\n";
}

void Image::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  static unsigned int imageId = 0;
  stream << "<image x=\"" << _originalRectangle[0].x << "\"";
  stream << " y=\"" << _originalRectangle[0].y << "\" ";
  stream << " width=\"" << transform.scale(_originalRectangle[1].x - _originalRectangle[0].x) << "\"";
  stream << " height=\"" << transform.scale(_originalRectangle[0].y - _originalRectangle[3].y) << "\"";
  stream << " preserveAspectRatio=\"none\"";
  stream << " id=\"image" << imageId++ << "\"";
  if (Tools::stringEndsWith(_filename.c_str(), ".png", Tools::CaseInsensitive))
    stream << "\n     xlink:href=\"data:image/png;base64,";
  else if (Tools::stringEndsWith(_filename.c_str(), ".jpg", Tools::CaseInsensitive) || Tools::stringEndsWith(_filename.c_str(), ".jpeg", Tools::CaseInsensitive))
    stream << "\n     xlink:href=\"data:image/jpeg;base64,";
  else {
    Tools::error << "Only png and jpeg image files may be included. SVG file will be corrupted.\n";
  }
  std::ifstream in;
  in.open(_filename.c_str());
  Tools::base64encode(in, stream);
  stream << "\"\n  ";
  Point shift = transform.map(_rectangle[0]) - (_transformMatrixSVG * _originalRectangle[0]);
  (_transformMatrixSVG + shift).flushSVG(stream);
  stream << " />\n";
}

void Image::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  _rectangle.flushTikZ(stream, transform);
  Tools::error << "Image::flushTikZ(): not available.\n";
}

void Image::accept(ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Image::accept(const ShapeVisitor & visitor)
{
  visitor.visit(*this);
}

void Image::accept(ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

void Image::accept(const ConstShapeVisitor & visitor) const
{
  visitor.visit(*this);
}

Shape * Image::accept(CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

Shape * Image::accept(const CompositeShapeTransform & transform) const
{
  return transform.map(*this);
}

} // namespace LibBoard
