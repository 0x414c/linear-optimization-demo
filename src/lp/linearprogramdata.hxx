#ifndef LINEARPROGRAMDATA_HXX
#define LINEARPROGRAMDATA_HXX

#include "eigen3/Eigen/Dense"

#include "../misc/utils.hxx"

using namespace Utils;
using namespace Eigen;

template<typename T = real_t>
struct LinearProgramData
{
  LinearProgramData();
  LinearProgramData(const Matrix<T, 1, Dynamic>& objFuncCoeffs,
                    const Matrix<T, Dynamic, Dynamic>& constraintsCoeffs,
                    const Matrix<T, Dynamic, 1>& constraintsRHS);

  Matrix<T, 1, Dynamic> objFuncCoeffs;
  Matrix<T, Dynamic, Dynamic> constraintsCoeffs;
  Matrix<T, Dynamic, 1> constraintsRHS;
};

#include "linearprogramdata.txx"

#endif // LINEARPROGRAMDATA_HXX
