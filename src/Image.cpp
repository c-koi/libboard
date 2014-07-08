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
#include "board/Image.h"
#include <sstream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include "BoardConfig.h"

#if ( _BOARD_HAVE_MAGICKPLUSPLUS_ == 1)
#define MAGICKCORE_QUANTUM_DEPTH 16
#define MAGICKCORE_HDRI_ENABLE 0
#include <Magick++.h>
#endif

namespace LibBoard {

const std::string LibBoard::Image::_name("Image");

Image::Image(const char * filename,
             double left, double top, double width, double height, int depth)
  : Shape(Color::None, Color::None, 0.0, SolidStyle, ButtCap, MiterJoin, depth),
    _rectangle(left, top, width, height, Color::Black, Color::None, 0.1, SolidStyle, ButtCap, MiterJoin, depth ),
    _originalRectangle(left, top, width, height, Color::Black, Color::None, 0.1, SolidStyle, ButtCap, MiterJoin, depth ),
    _filename( filename )
{
  if ( height == 0.0 ) {
#if ( _BOARD_HAVE_MAGICKPLUSPLUS_ == 1)
    try {
      Magick::Image image;
      image.read(_filename);
      Magick::Geometry geometry = image.geometry();
      height = width * (geometry.height() / (double) geometry.width());
      _rectangle = _originalRectangle = Rectangle(left, top, width, height, Color::Black, Color::None, 0.1, SolidStyle, ButtCap, MiterJoin, depth);
    } catch (...) {
      error << "Image::Image(): Could not read image.\n";
    }
#else
    error << "Image::Image(): ImageMagick is required to retrieve bitmap image aspect ratio.\n";
#endif
  }
}

Image::Image(const char * filename,
             const Rect & rect, int depth)
  : Shape(Color::None, Color::None, 0.0, SolidStyle, ButtCap, MiterJoin, depth),
    _rectangle(rect, Color::Black, Color::None, 0.1, SolidStyle, ButtCap, MiterJoin, depth ),
    _originalRectangle(rect, Color::Black, Color::None, 0.1, SolidStyle, ButtCap, MiterJoin, depth ),
    _filename( filename )
{
}

const std::string &
Image::name() const
{
  return _name;
}

Image *
Image::clone() const
{
  return new Image(*this);
}

Point
Image::center() const
{
  return _rectangle.center();
}

Shape &
Image::rotate(double angle, const Point &center)
{
  _transformMatrixSVG = TransformMatrix::rotation(angle,center,TransformMatrix::SVG) * _transformMatrixSVG;
  _transformMatrixEPS = TransformMatrix::rotation(angle,center,TransformMatrix::Postscript) * _transformMatrixEPS;
  _rectangle.rotate(angle,center);
  return *this;
}

Shape &
Image::rotate(double angle)
{
  Point c = _transformMatrixSVG * _originalRectangle.center();
  _transformMatrixSVG = TransformMatrix::rotation(angle,c,TransformMatrix::SVG) * _transformMatrixSVG;
  c = _transformMatrixEPS * _originalRectangle.center();
  _transformMatrixEPS = TransformMatrix::rotation(angle,c,TransformMatrix::Postscript) * _transformMatrixEPS;
  _rectangle.rotate(angle);
  return *this;
}

Image Image::rotated(double angle) const
{
  return static_cast<Image &>( Image(*this).rotate( angle ) );
}

Image Image::rotated(double angle, const Point & center ) const
{
  return static_cast<Image &>( Image(*this).rotate( angle, center ) );
}

Shape &
Image::translate(double dx, double dy)
{
  _rectangle.translate(dx,dy);
  _transformMatrixSVG += Point(dx,dy);
  _transformMatrixEPS += Point(dx,dy);
  return *this;
}

Image
Image::translated(double dx, double dy) const
{
  return static_cast<Image &>( Image(*this).translate( dx, dy ) );
}

Shape &
Image::scale(double sx, double sy)
{
  _rectangle.scale(sx,sy);
  Point currentCenter = _transformMatrixSVG * _originalRectangle.center();
  _transformMatrixSVG = TransformMatrix::scaling(sx,sy) * _transformMatrixSVG;
  Point newCenter = _transformMatrixSVG * _originalRectangle.center();
  _transformMatrixSVG = TransformMatrix::translation(currentCenter - newCenter) * _transformMatrixSVG;
  
  currentCenter = _transformMatrixEPS * _originalRectangle.center();
  _transformMatrixEPS = TransformMatrix::scaling(sx,sy) * _transformMatrixEPS;
  newCenter = _transformMatrixEPS * _originalRectangle.center();
  _transformMatrixEPS = TransformMatrix::translation(currentCenter - newCenter) * _transformMatrixEPS;
  return *this;
}

Shape &
Image::scale(double s)
{
  scale(s,s);
  return *this;
}

Image Image::scaled(double sx, double sy)
{
  return static_cast<Image &>( Image(*this).scale( sx, sy ) );
}

Rect
Image::boundingBox() const
{
  return _rectangle.boundingBox();
}

void
Image::scaleAll(double s)
{
  _transformMatrixSVG *= TransformMatrix::scaling(s,s);
  _transformMatrixEPS *= TransformMatrix::scaling(s,s);
  _rectangle.scaleAll(s);
}

void
Image::flushPostscript(std::ostream & stream, const TransformEPS & transform) const
{
  //  _rectangle.flushPostscript( stream, transform );
#if ( _BOARD_HAVE_MAGICKPLUSPLUS_ == 1 )
  Magick::Image image;
  image.read(_filename);
  const char * tmpFilename = temporaryFilename(".eps");
  image.write(tmpFilename);

  Rect rect = getEPSBoundingBox(tmpFilename);
  double scaleX = (_originalRectangle.width() / rect.width);
  double scaleY = (_originalRectangle.height() / rect.height);
  TransformMatrix scaling = TransformMatrix::scaling(scaleX,scaleY);
  Point scaledCenter = scaling * rect.center();
  Point shift = _originalRectangle.center() - scaledCenter;
  scaling += shift;
  TransformMatrix fullTransform = _transformMatrixEPS * scaling;

  stream << "%\n";
  stream << "% Bitmap Image\n";
  stream << "%\n";
  stream <<  "%%BeginDocument: board_temporary.eps\n";
  stream << "gs\n";

  Point tmShift = transform.map(_rectangle.topLeft()) - (_transformMatrixEPS*_originalRectangle.topLeft());
  (fullTransform+tmShift).flushEPS(stream);

  stream << "\n";

  flushFile(tmpFilename,stream);
  std::remove(tmpFilename);
  stream <<  "%%EndDocument\n";
  stream << "gr\n";
#else
  error << "Image::flushPostscript(): requires ImageMagick's Magick++ lib. Aborted.\n";
  return;
#endif
}

void
Image::flushFIG(std::ostream & stream, const TransformFIG & transform, std::map<Color, int> & colormap) const
{
   _rectangle.flushFIG( stream, transform, colormap );
   Rect bbox = _rectangle.boundingBox();
   Rectangle rectangle(bbox,Color::None,Color::None,0.0);

   stream << "2 5 0 1 0 -1 " << transform.mapDepth(_depth) << " -1 -1 0.000 0 0 -1 0 0 5\n";
   stream << "\t0 " << _filename << "\n";
   stream << "\t";
   for (unsigned int i = 0; i < rectangle.vertexCount(); ++i ) {
      stream << " " << static_cast<int>( transform.mapX( rectangle[i].x ) )
             << " " << static_cast<int>( transform.mapY( rectangle[i].y ) );
   }
   stream << " " << static_cast<int>( transform.mapX( rectangle[0].x ) )
         << " " << static_cast<int>( transform.mapY( rectangle[0].y ) )
         << "\n";

}

void
Image::flushSVG(std::ostream & stream, const TransformSVG & transform) const
{
  static unsigned int imageId = 0;
  _rectangle.flushSVG( stream, transform );

  stream << "<image x=\"" << _originalRectangle[0].x << "\"";
  stream << " y=\"" << _originalRectangle[0].y << "\" ";
  stream << " width=\"" << (_originalRectangle[1].x - _originalRectangle[0].x) << "\"";
  stream << " height=\"" << ( _originalRectangle[0].y - _originalRectangle[3].y) << "\"";
  stream << " id=\"image" << imageId++ << "\"";
  if ( stringEndsWith(_filename.c_str(),".png") )
    stream << "\n     xlink:href=\"data:image/png;base64,";
  else if ( stringEndsWith(_filename.c_str(),".jpg") || stringEndsWith(_filename.c_str(),".jpeg") )
    stream << "\n     xlink:href=\"data:image/jpeg;base64,";
  else {
    error << "Only png and jpeg image files may be included. SVG file will be corrupted.\n";
  }
  std::ifstream in;
  in.open(_filename.c_str());
  base64encode(in,stream);
  stream << "\"\n  ";

  // _transformMatrix.flushSVG(stream);
  //(_transformMatrix*transform.matrix()).flushSVG(stream);
  Point shift = transform.map(_rectangle.topLeft()) - (_transformMatrixSVG*_originalRectangle.topLeft());
  (_transformMatrixSVG+shift).flushSVG(stream);
  stream << " />\n";
}

void
Image::flushTikZ(std::ostream & stream, const TransformTikZ & transform) const
{
  _rectangle.flushTikZ( stream, transform );
}

} // namespace LibBoard
