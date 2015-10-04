#ifndef LINEARPROGRAMDATA_HXX
#define LINEARPROGRAMDATA_HXX

#include "utils.hxx"

#include "lib/eigen3/Eigen/Dense"

using namespace Utils;
using namespace Eigen;

template<typename T = real_t>
struct LinearProgramData
{
  LinearProgramData();

  Matrix<T, 1, Dynamic> objFuncCoeffs;
  Matrix<T, Dynamic, 1> constraintsRHS;
  Matrix<T, Dynamic, Dynamic> constraintsCoeffs;
};

#endif // LINEARPROGRAMDATA_HXX
