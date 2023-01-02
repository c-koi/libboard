/* -*- mode: c++ -*- */
/**
 * @file   BoardFontText.cpp
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
#include <board/BoardFontText.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <board/Bezier.h>
#include <board/Debug.h>
#include <board/Polyline.h>
#include <board/ShapeList.h>
#include <board/ShapeVisitor.h>
#include <board/Tools.h>

using std::string;
using std::vector;

// FIXME : handle locale with sscanf

namespace
{
using LibBoard::Bezier;
using LibBoard::Path;
using LibBoard::Point;
using LibBoard::Polyline;
using LibBoard::ShapeList;
extern const char * font_paths[];
extern const double font_baseline;
extern const double font_topline;

const double font_scale = std::abs(font_topline - font_baseline);

const unsigned int font_glyph_count = 95;
// const unsigned int font_glyph_count = 1 + (1 + '~') - '!';
std::unique_ptr<LibBoard::ShapeList> font_glyphs[font_glyph_count];
std::unique_ptr<LibBoard::ShapeList> unknown_glyph;
double font_glyph_vertical_shift[font_glyph_count];
double font_glyph_width[font_glyph_count];

vector<string> split(const string & str, const char * delimiters);

inline bool glyphValid(int c)
{
  return (c >= '!' && c <= 255);
}

inline double glyph_vshift(int c)
{
  return glyphValid(c) ? font_glyph_vertical_shift[c - '!'] : 0.0;
}

inline double glyph_width(int c)
{
  return glyphValid(c) ? font_glyph_width[c - '!'] : 0.0;
}

void parseSVGPath(const std::string & text, ShapeList & paths);

const LibBoard::ShapeList & fontGlyph(int c)
{
  if (!glyphValid(c)) {
    return fontGlyph(255);
  }
  // TSHOW(static_cast<char>(c));
  int index = c - '!';
  if (font_glyphs[index]) {
    return *(font_glyphs[index]);
  }
  font_glyphs[index] = std::unique_ptr<LibBoard::ShapeList>(new LibBoard::ShapeList);
  LibBoard::ShapeList & glyph = *(font_glyphs[index]);
  std::string text = font_paths[index];
  if (text.empty()) {
    text = font_paths[font_glyph_count - 1];
  }
  std::vector<string> paths = split(text, ";");
  for (const string & path : paths) {
    parseSVGPath(path, glyph);
  }
  LibBoard::Rect box = glyph.boundingBox(LibBoard::UseLineWidth);
  font_glyph_vertical_shift[index] = (box.bottom() - font_baseline) / font_scale;
  font_glyph_width[index] = box.width / font_scale;
  glyph.scale(1.0 / font_scale);
  glyph.accept(LibBoard::ShapeWithStyleVisitor(0.2));
  box = glyph.boundingBox(LibBoard::IgnoreLineWidth);
  glyph.translate(-box.left, box.height - box.top);
  return glyph;
}

vector<string> split(const string & str, const char * delimiters)
{
  vector<string> result;
  vector<char> buffer(str.c_str(), str.c_str() + str.length());
  buffer.emplace_back('\0');
  result.emplace_back(strtok(buffer.data(), delimiters));
  char * pc;
  while ((pc = strtok(nullptr, delimiters))) {
    result.emplace_back(pc);
  }
  return result;
}

LibBoard::Point token2point(const std::string & token, char command)
{
  double x, y;
  if (command == 'h' || command == 'H') {
    if (sscanf(token.c_str(), "%lf", &x) != 1) {
      throw LibBoard::Exception("token2point(): cannot parse token");
    }
    return LibBoard::Point(x, 0.0);
  }
  if (command == 'v' || command == 'V') {
    if (sscanf(token.c_str(), "%lf", &y) != 1) {
      throw LibBoard::Exception("token2point(): cannot parse token");
    }
    return LibBoard::Point(0.0, -y);
  }
  if (std::sscanf(token.c_str(), "%lf,%lf", &x, &y) == 2) {
    return LibBoard::Point(x, -y);
  } else {
    throw LibBoard::Exception("token2point(): cannot parse token");
  }
}

bool isCommand(const std::string & token)
{
  if (token.size() != 1) {
    return false;
  }
  const char & letter = token.front();
  switch (letter) {
  case 'l':
  case 'L':
  case 'm':
  case 'M':
  case 'c':
  case 'C':
  case 'h':
  case 'H':
  case 'v':
  case 'V':
  case 'z':
  case 'Z':
  case 's':
  case 'S':
    return true;
  }
  return false;
}

// void splitAndParseSVGPaths(const std::string & text, ShapeList & shapes)
//{
//  if (text.length() == 0) {
//    return;
//  }
//  std::vector<string> paths = split(text, ";");
//  for (const string & path : paths) {
//    parseSVGPath(path, shapes);
//  }
//}

bool pathHasCurve(const string & str)
{
  for (const char & c : str) {
    if (c == 'c' || c == 'C') {
      return true;
    }
  }
  return false;
}

void flush(const std::vector<Point> & points, const std::vector<Point> & controls, ShapeList & paths)
{
  //  using ::operator<<;
  //  LSHOW(points);
  //  LSHOW(controls);
  if (points.size() <= 1) {
    return;
  }
  if (controls.empty()) {
    paths << Polyline(points, Path::Open);
  } else {
    paths << Bezier(points, controls);
  }
}

void parseSVGPath(const std::string & text, ShapeList & paths)
{
  // TSHOW(text);
  std::vector<string> tokens = split(text, " ");
  std::vector<Point> points;
  std::vector<Point> controls;

  bool absolute = false;
  char mode = 'l';
  const bool hasCurves = pathHasCurve(text);

  Point currentPoint;

  //  for (const auto & token : tokens) {
  //    TSHOW(token);
  //  }

  //  using ::operator<<;
  // LSHOW(text);
  auto token = tokens.begin();
  while (token != tokens.end()) {
    if (isCommand(*token)) {
      // SHOW(*token);
      const char command = (*token)[0];
      const char upperCommand = static_cast<char>(toupper(command));
      const char uppperMode = static_cast<char>(toupper(mode));
      absolute = isupper(command);
      // MSHOW("Command", command);
      if (upperCommand == 'M') {
        flush(points, controls, paths);
        points.clear();
        controls.clear();
        const bool firstIsAbsolute = (token == tokens.begin());
        ++token;
        if (command == 'm' && firstIsAbsolute) {
          currentPoint = token2point(*token++, command);
          points.emplace_back(currentPoint);
        }
        while (token != tokens.end() && !isCommand(*token)) {
          currentPoint = absolute ? token2point(*token++, command) : (currentPoint + token2point(*token++, command));
          points.emplace_back(currentPoint);
        }
        currentPoint = points.back();
        if (hasCurves) {
          // Straight segment as a curve
          for (unsigned int i = 0; i < points.size() - 1; ++i) {
            controls.emplace_back(points[i]);
            controls.emplace_back(points[i + 1]);
          }
        }
        mode = command;
        continue;
      } else if (upperCommand == 'L' || upperCommand == 'H' || upperCommand == 'V') {
        if (uppperMode == 'M' || uppperMode == 'L' || uppperMode == 'H' || uppperMode == 'V') {
          ++token;
          std::vector<Point>::size_type start = points.size() - 1;
          assert(points.size());
          while (token != tokens.end() && !isCommand(*token)) {
            currentPoint = absolute ? token2point(*token++, command) : (currentPoint + token2point(*token++, command));
            points.emplace_back(currentPoint);
          }
          if (hasCurves) {
            // Straight segment as a curve
            for (; start < points.size() - 1; ++start) {
              controls.emplace_back(points[start]);
              controls.emplace_back(points[start + 1]);
            }
          }
        } else { // Was previously a curve
          std::vector<Point>::size_type start = points.size() - 1;
          assert(points.size());
          ++token;
          while (token != tokens.end() && !isCommand(*token)) {
            currentPoint = absolute ? token2point(*token++, command) : (currentPoint + token2point(*token++, command));
            points.emplace_back(currentPoint);
          }
          for (; start < points.size() - 1; ++start) {
            // Straight segment as a curve
            controls.emplace_back(points[start]);
            controls.emplace_back(points[start + 1]);
          }
        }
        mode = command;
        continue;
      } else if (upperCommand == 'C') {
        ++token;
        while (token != tokens.end() && !isCommand(*token)) {
          const Point x1 = absolute ? token2point(*token++, command) : (currentPoint + token2point(*token++, command));
          const Point x2 = absolute ? token2point(*token++, command) : (currentPoint + token2point(*token++, command));
          currentPoint = absolute ? token2point(*token++, command) : (currentPoint + token2point(*token++, command));
          controls.emplace_back(x1);
          controls.emplace_back(x2);
          points.emplace_back(currentPoint);
        }
        mode = command;
      }
    } else {
      TSHOW("Parse Error");
    }
  }
  flush(points, controls, paths);
}

} // namespace

namespace LibBoard
{

Group boardFontText(Point baselineStart, const std::string & text, double size, Color penColor, double lineWidth)
{
  Group group;
  double x = baselineStart.x;
  double y = baselineStart.y;
  for (const char c : text) {
    LSHOW(c);
    if (c == ' ') {
      x += 0.7 * size;
      continue;
    }
    const ShapeList & g = fontGlyph(c);
    ShapeList list = g.scaled(size);
    Rect box = list.boundingBox(IgnoreLineWidth);
    list.translate(x - box.left, y + glyph_vshift(c) * size - box.bottom());
    group << list;
    x += 1.1 * glyph_width(c) * size + lineWidth;
  }
  Style style;
  style.penColor = penColor;
  if (lineWidth != 0.0) {
    style.lineWidth = lineWidth;
  } else {
    style.lineWidth = 0.025 * size;
  }
  style.fillColor = Color::Null;
  style.lineCap = RoundCap;
  style.lineJoin = RoundJoin;
  group.accept(ShapeWithStyleVisitor(style));
  return group;
}

} // namespace LibBoard

namespace
{
#include "./inc/BoardFont.cpp"
} // namespace
