#ifndef LINEARPROGRAMDATA_HXX
#define LINEARPROGRAMDATA_HXX

#include "eigen3/Eigen/Dense"

#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"

using namespace Eigen;
using namespace NumericTypes;
using namespace Utils;

template<typename T = Real>
struct LinearProgramData
{
  LinearProgramData()/* = delete*/;
  LinearProgramData(const Matrix<T, 1, Dynamic>& objectiveFunctionCoefficients,
                    const Matrix<T, Dynamic, Dynamic>& constraintsCoefficients,
                    const Matrix<T, Dynamic, 1>& constraintsRHS);

  DenseIndex constraintsCount() const;
  DenseIndex variablesCount() const;

  Matrix<T, 1, Dynamic> objectiveFunctionCoefficients/* = Matrix<T, 1, Dynamic>::Zero(1, 1)*/;
  Matrix<T, Dynamic, Dynamic> constraintsCoefficients/* = Matrix<T, Dynamic, Dynamic>::Zero(1, 1)*/;
  Matrix<T, Dynamic, 1> constraintsRHS/* = Matrix<T, Dynamic, 1>::Zero(1, 1)*/;
};

#include "linearprogramdata.txx"

#endif // LINEARPROGRAMDATA_HXX
