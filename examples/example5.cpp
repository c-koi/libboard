/**
 * @file   example5.cpp
 * @author Nikos Efthimiou (University of Hull)
 *
 * @brief  Draw a vector image.
 *
 */
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "Board.h"
using namespace LibBoard;

int main( int, char *[] )
{
  Board board;

  std::vector<Color> matrix(100);
  const int num_cols = 10;
  const int num_rows = 10;

  double x_size = 40;
  double y_size = 10;

  for(unsigned int i = 0; i < 100 ; i++)
  {
    Color tmpColor(i);
    matrix[i] = tmpColor;
   }

  board.drawArray(matrix, num_cols, num_rows, x_size, y_size);

  Board::disableLineWidthScaling();
  board.saveEPS( "example5.eps" );
}

