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
#include <board/Debug.h>
#include <board/Rect.h>
#include <board/Tools.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>

LibBoard::Tools::MessageStream LibBoard::Tools::notice(std::cerr, "Information: ");

LibBoard::Tools::MessageStream LibBoard::Tools::warning(std::cerr, "Warning: ");

LibBoard::Tools::MessageStream LibBoard::Tools::error(std::cerr, "Error: ");

namespace
{
unsigned long boardRandNext = static_cast<unsigned long>(time(nullptr));
}

namespace LibBoard
{

namespace Tools
{

bool base64encode(std::istream & in, std::ostream & out, int linesize)
{
  static const unsigned char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  unsigned char input[3];
  unsigned char output[4];
  int nbBlocks = 0;

  *input = 0;
  *output = 0;
  while (in) {
    int len = 0;
    for (int i = 0; i < 3; i++) {
      input[i] = static_cast<unsigned char>(in.get());
      if (in) {
        len++;
      } else {
        input[i] = 0;
      }
    }
    if (len > 0) {
      output[0] = b64[(int)(input[0] >> 2)];
      output[1] = b64[(int)(((input[0] & 0x03) << 4) | ((input[1] & 0xf0) >> 4))];
      output[2] = (len > 1 ? b64[(int)(((input[1] & 0x0f) << 2) | ((input[2] & 0xc0) >> 6))] : '=');
      output[3] = (len > 2 ? b64[(int)(input[2] & 0x3f)] : '=');
      if (!out.write(reinterpret_cast<char *>(output), 4))
        return false;
      ++nbBlocks;
    }
    if (nbBlocks >= (linesize / 4) || !in) {
      if (nbBlocks > 0)
        out << "\r\n";
      nbBlocks = 0;
    }
  }
  return true;
}

bool stringEndsWith(const char * str, const char * end, CaseSensitivity sensitivity)
{
  size_t nstr = strlen(str);
  size_t nend = strlen(end);
  if (nstr < nend)
    return false;
  if (sensitivity == CaseSensitive) {
    for (size_t i = 1; i <= nend; ++i) {
      if (str[nstr - i] != end[nend - i])
        return false;
    }
  } else {
    for (size_t i = 1; i <= nend; ++i) {
      if (toupper(str[nstr - i]) != toupper(end[nend - i]))
        return false;
    }
  }
  return true;
}

void flushFile(const char * filename, std::ostream & out)
{
  std::ifstream file;
  char line[4096];
  file.open(filename);
  do {
    file.read(line, 4096);
    if (file)
      out.write(line, 4096);
    else
      out.write(line, file.gcount());
  } while (file);
  file.close();
}

void getEPSBoundingBox(const char * filename, Rect & rect)
{
  std::ifstream file;
  char line[4096];
  file.open(filename);
  bool done = false;
  double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
  while (file && !done) {
    file.getline(line, 4096);
    if (std::strstr(line, "%%BoundingBox:") == line) {
      if (std::sscanf(line, "%%%%BoundingBox: %lf %lf %lf %lf", &x1, &y1, &x2, &y2) != 4) {
        error << "getEPSBoundingBox(): Cannot read bounding box information.\n";
        file.close();
        rect.clear();
        return;
      }
      done = true;
    }
  }
  file.close();
  rect = Rect(x1, y2, x2 - x1, y2 - y1);
  return;
}

bool canCreateFile(const char * filename)
{
  std::ofstream file;
  file.open(filename);
  bool ok = static_cast<bool>(file);
  if (ok) {
    file.close();
    std::remove(filename);
  }
  return ok;
}

bool canReadFile(const char * filename)
{
  std::ifstream file;
  file.open(filename);
  bool ok = static_cast<bool>(file);
  if (file.is_open()) {
    file.close();
  }
  return ok;
}

const char * temporaryFilename(const char * extension)
{
#if (BOARD_WIN32 == 1)
  const char * separator = "\\";
  const int nbPaths = 5;
  const char * paths[] = {getenv("TMP"), getenv("TEMP"), "C:\\WINNT\\TEMP", "C:\\WINDOWS\\TEMP", "C:\\TEMP"};
#else
  const char * separator = "/";
  const int nbPaths = 3;
  const char * paths[] = {getenv("TMP"), "/tmp", "/var/tmp"};
#endif
  static char buffer[1024];
  const char * path = nullptr;
  for (int i = 0; i < nbPaths && !path; ++i) {
    if (paths[i]) {
      do {
        const char prefix[] = "libboard";
        char suffix[] = "XXXXXXXXXXXXXXXXXXX";
        for (char * pc = suffix; *pc; ++pc) {
          switch (boardRand() % 3) {
          case 0:
            *pc = '0' + boardRand() % 10;
            break;
          case 1:
            *pc = 'a' + boardRand() % 26;
            break;
          case 2:
            *pc = 'A' + boardRand() % 26;
            break;
          default:
            break;
          }
        }
        std::snprintf(buffer, 1024, "%s%s%s_%s%s", paths[i], separator, prefix, suffix, extension);
      } while (canReadFile(buffer));
      if (canCreateFile(buffer)) {
        path = paths[i];
      }
    }
  }
  if (!path) {
    buffer[0] = '\0';
  }
  return buffer;
}

void initBoardRand(unsigned long seed)
{
  boardRandNext = seed;
}

unsigned int boardRand()
{
  boardRandNext = boardRandNext * 1103515245 + 12345;
  const unsigned long modulo = static_cast<unsigned long>(RAND_MAX) + 1;
  return static_cast<unsigned int>((boardRandNext / 65536) % modulo);
}

double boardRandDouble()
{
  return boardRand() / static_cast<double>(RAND_MAX);
}

double boardRandDouble(double min, double max)
{
  const double magnitude = max - min;
  return min + ((boardRand() / static_cast<double>(RAND_MAX)) * magnitude);
}

bool solveQuadratic(double a, double b, double c, double & x1, double & x2)
{
  if (almostEqual(a, 0.0)) {
    if (almostEqual(b, 0.0)) {
      return false;
    }
    x1 = x2 = -c / b;
    return true;
  }
  const double delta = b * b - 4 * a * c;
  if (delta < 0.0) {
    return false;
  }
  if (almostEqual(delta, 0.0)) {
    x1 = x2 = -b / (2 * a);
    return true;
  }
  const double twiceA = 2 * a;
  x1 = (-b - std::sqrt(delta)) / twiceA;
  x2 = (-b + std::sqrt(delta)) / twiceA;
  return true;
}

} // namespace Tools

} // namespace LibBoard
