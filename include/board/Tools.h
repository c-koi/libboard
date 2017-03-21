/* -*- mode: c++ -*- */
/**
 * @file   Tools.h
 * @author Sebastien Fourey (GREYC)
 * @date   Nov 2008
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
#ifndef _BOARD_TOOLS_H_
#define _BOARD_TOOLS_H_

#include <iostream>
#include <ctime>
#include <cstring>
#include "Rect.h"

#if defined( _MSC_VER )
#define secured_sprintf sprintf_s
#else
#define secured_sprintf snprintf
#endif // defined( _MSC_VER )

namespace LibBoard {

namespace Tools {

enum CaseSensitivity { CaseSensitive, CaseInsensitive };

/**
  * A "prefixable" message stream
  */
class MessageStream {
public:

  inline MessageStream( std::ostream & out, const char * prefix  );

  template<typename T>
  inline MessageStream operator<<( const T & v );

  inline MessageStream operator<<( std::ostream & (*fun)(std::ostream &) );

private:
  std::ostream & _out;
  const char * _prefix;
};

extern MessageStream error;
extern MessageStream warning;
extern MessageStream notice;

MessageStream::MessageStream( std::ostream & out, const char * prefix )
  : _out( out ),
    _prefix( prefix )
{
}

template<typename T>
MessageStream MessageStream::operator<<( const T & v )
{
  if ( _prefix ) {
    _out << _prefix << v;
  } else {
    _out << v;
  }
  return MessageStream( _out, 0 );
}

MessageStream MessageStream::operator<<( std::ostream & (*fun)(std::ostream &) )
{
  if ( _prefix ) {
    _out << _prefix << fun;
  } else {
    _out << fun;
  }
  return MessageStream( _out, 0 );
}


inline void secured_strncpy( char * dst, const char * src, size_t count );

inline void secured_ctime( char * str, const time_t * t, size_t count );

bool base64encode(std::istream & in, std::ostream & , int linesize = 80);

bool stringEndsWith( const char * str, const char * end, CaseSensitivity sensitivity = CaseSensitive );

void flushFile( const char * filename, std::ostream & out );

Rect getEPSBoundingBox( const char * filename );

bool canCreateFile( const char * filename );

bool canReadFile( const char * filename );

const char * temporaryFilename( const char * extension );

unsigned int boardRand();

template<typename T> inline void unused(const T&, ...) {}

}  // namespace Tools

}  // namespace LibBoard

#include "Tools.ih"

#endif /* _SHAPE_H_ */
