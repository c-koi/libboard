/* -*- mode: c++ -*- */
/**
 * @file   Tools.cpp
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

#include "board/Tools.h"
#include <cctype>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <cstdio>

LibBoard::MessageStream LibBoard::notice( std::cerr, "Information: " );

LibBoard::MessageStream LibBoard::warning( std::cerr, "Warning: " );

LibBoard::MessageStream LibBoard::error( std::cerr, "Error: " );

namespace LibBoard {

bool
base64encode( std::istream & in, std::ostream & out, int linesize )
{
  static const char b64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  unsigned char input[3];
  char output[4];
  int nbBlocks = 0;

  *input = (unsigned char) 0;
  *output = (unsigned char) 0;
  while( in ) {
    int len = 0;
    for( int i = 0; i < 3; i++ ) {
      input[i] = (unsigned char) in.get();
      if( in ) {
        len++;
      } else {
        input[i] = (unsigned char) 0;
      }
    }
    if( len > 0 ) {
      output[0] = (unsigned char) b64[ (int)(input[0] >> 2) ];
      output[1] = (unsigned char) b64[ (int)(((input[0] & 0x03) << 4) | ((input[1] & 0xf0) >> 4)) ];
      output[2] = (unsigned char) (len > 1 ? b64[ (int)(((input[1] & 0x0f) << 2) | ((input[2] & 0xc0) >> 6)) ] : '=');
      output[3] = (unsigned char) (len > 2 ? b64[ (int)(input[2] & 0x3f) ] : '=');
      if ( ! out.write(output,4) ) return false;
      ++nbBlocks;
    }
    if( nbBlocks >= (linesize/4) || ! in ) {
      if( nbBlocks > 0 ) out << "\r\n";
      nbBlocks = 0;
    }
  }
  return true;
}

bool stringEndsWith(const char * str, const char * end)
{
  size_t nstr = strlen(str);
  size_t nend = strlen(end);
  if ( nstr < nend )
    return false;
  for ( size_t i = 1; i <= nend; ++i ) {
    if ( toupper(str[nstr-i]) != toupper(end[nend-i]) )
      return false;
  }
  return true;
}

void flushFile(const char *filename, std::ostream & out)
{
  std::ifstream file;
  char line[4096];
  file.open(filename);
  do {
    file.read(line,4096);
    if (file)
      out.write(line,4096);
    else
      out.write(line,file.gcount());
  } while (file);
  file.close();
}

Rect
getEPSBoundingBox(const char *filename)
{
  std::ifstream file;
  char line[4096];
  file.open(filename);
  bool done = false;
  double x1,y1,x2,y2;
  while ( file && !done ) {
    file.getline(line,4096);
    if ( std::strstr(line,"%%BoundingBox:") == line ) {
      if ( std::sscanf(line,"%%%%BoundingBox: %lf %lf %lf %lf",&x1,&y1,&x2,&y2) != 4 ) {
        error << "getEPSBoundingBox(): Cannot read bounding box information.\n";
        file.close();
        return Rect(0.0,0.0,0.0,0.0);
      }
      done = true;
    }
  }
  file.close();
  return Rect(x1,y2,x2-x1,y2-y1);
}

bool
canCreateFile(const char * filename)
{
  std::ofstream file;
  file.open(filename);
  bool ok = static_cast<bool>( file );
  if ( ok ) {
    file.close();
    std::remove(filename);
  }
  return ok;
}

const char *
temporaryFilename(const char * extension)
{
  static char buffer[1024];
  char name[] = "libboard_XXXXXX";
  mktemp(name);
#if ( _BOARD_WIN32_ == 1 )
  const char * separator = "\\";
  const int nbPaths = 5;
  const char * paths[] = {
    getenv("TMP"),
    getenv("TEMP"),
    "C:\\WINNT\\TEMP",
    "C:\\WINDOWS\\TEMP",
    "C:\\TEMP"
  };
#else
  const char * separator = "/";
  const int nbPaths = 3;
  const char * paths[] = {
    getenv("TMP"),
    "/tmp",
    "/var/tmp"
  };
#endif
  const char * path = 0;
  for ( int i=0; i < nbPaths && !path; ++i ) {
    if ( paths[i] ) {
      std::snprintf(buffer,1024,"%s%s%s",paths[i],separator,name);
      if ( canCreateFile(buffer) )
        path = paths[i];
    }
  }
  if ( path )
    std::strncat(buffer,extension,1024);
  else
    buffer[0] = '\0';
  return buffer;
}


}  // namespace LibBoard;
