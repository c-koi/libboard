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
#ifndef BOARD_TOOLS_H
#define BOARD_TOOLS_H

#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>

#if defined(_MSC_VER)
#define secured_sprintf sprintf_s
#else
#define secured_sprintf snprintf
#endif // defined( _MSC_VER )

namespace LibBoard
{

struct Rect;

namespace Tools
{

enum CaseSensitivity
{
  CaseSensitive,
  CaseInsensitive
};

/**
 * A "prefixable" message stream
 */
class MessageStream {
public:
  inline MessageStream(std::ostream & out, const char * prefix);
  template <typename T> inline MessageStream operator<<(const T & v);
  inline MessageStream operator<<(std::ostream & (*fun)(std::ostream &));

private:
  std::ostream & _out;
  const char * _prefix;
};

extern MessageStream error;
extern MessageStream warning;
extern MessageStream notice;

MessageStream::MessageStream(std::ostream & out, const char * prefix) : _out(out), _prefix(prefix) {}

template <typename T> //
MessageStream MessageStream::operator<<(const T & v)
{
  if (_prefix) {
    _out << _prefix << v;
  } else {
    _out << v;
  }
  return MessageStream(_out, nullptr);
}

MessageStream MessageStream::operator<<(std::ostream & (*fun)(std::ostream &))
{
  if (_prefix) {
    _out << _prefix << fun;
  } else {
    _out << fun;
  }
  return MessageStream(_out, nullptr);
}

inline void secured_strncpy(char * dst, const char * src, size_t count);

inline void secured_ctime(char * str, const time_t * t, size_t count);

bool base64encode(std::istream & in, std::ostream &, int linesize = 80);

bool stringEndsWith(const char * str, const char * end, CaseSensitivity sensitivity = CaseSensitive);

void flushFile(const char * filename, std::ostream & out);

void getEPSBoundingBox(const char * filename, Rect & rect);

bool canCreateFile(const char * filename);

bool canReadFile(const char * filename);

const char * temporaryFilename(const char * extension);

void initBoardRand(unsigned long seed);

unsigned int boardRand();

double boardRandDouble();

double boardRandDouble(double min, double max);

inline bool almostEqual(const double & a, const double & b);

inline double mix(const double & a, const double & b, const double & time)
{
  return a * (1 - time) + (b * time);
}

template <typename T> inline void unused(const T &, ...) {}

// Inline methods and functions

void secured_strncpy(char * dst, const char * src, size_t count)
{
#if defined(_MSC_VER)
  strncpy_s(dst, count, src, _TRUNCATE);
#else
  strncpy(dst, src, count - 1);
#endif // defined( _MSC_VER )
}

void secured_ctime(char * str, const time_t * t, size_t count)
{
#if defined(_MSC_VER)
  ctime_s(str, count, t);
#else
  strncpy(str, ctime(t), count - 1);
#endif // defined( _MSC_VER )
}

inline bool almostEqual(const double & a, const double & b)
{
  const double nothing = 1e-10;
  return a > b ? ((a - b) < nothing) : (a < b) ? ((b - a) < nothing) : true;
}

template <typename T> void clamp(T & value, const T & min, const T & max)
{
  if (value < min) {
    value = min;
    return;
  }
  if (value > max) {
    value = max;
    return;
  }
}

template <typename T> inline T square(const T & t)
{
  return t * t;
}

bool solveQuadratic(double a, double b, double c, double & x1, double & x2);

inline double rad2deg(double angle)
{
  return (angle / M_PI) * 180;
}

inline double deg2rad(double angle)
{
  return (angle / 180) * M_PI;
}

} // namespace Tools

} // namespace LibBoard

#endif /* BOARD_TOOLS_H */
