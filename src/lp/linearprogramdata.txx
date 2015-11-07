#ifndef LINEARPROGRAMDATA_TXX
#define LINEARPROGRAMDATA_TXX

#include "linearprogramdata.hxx"

#include "eigen3/Eigen/Dense"

using namespace Eigen;

template<typename T>
LinearProgramData<T>::LinearProgramData() :
  objectiveFunctionCoefficients(Matrix<T, 1, Dynamic>::Zero(1, 1)),
  constraintsCoefficients(Matrix<T, Dynamic, Dynamic>::Zero(1, 1)),
  constraintsRHS(Matrix<T, Dynamic, 1>::Zero(1, 1))
{ }

//TODO: ~ Check for sizes consistency
template<typename T>
LinearProgramData<T>::LinearProgramData(const Matrix<T, 1, Dynamic>& objFuncCoeffs,
                                        const Matrix<T, Dynamic, Dynamic>& constraintsCoeffs,
                                        const Matrix<T, Dynamic, 1>& constraintsRHS) :
  objectiveFunctionCoefficients(objFuncCoeffs),
  constraintsCoefficients(constraintsCoeffs),
  constraintsRHS(constraintsRHS)
{ }

template<typename T>
DenseIndex LinearProgramData<T>::constraintsCount() const
{
  return constraintsCoefficients.rows();
}

template<typename T>
DenseIndex LinearProgramData<T>::variablesCount() const
{
  return constraintsCoefficients.cols();
}

#endif // LINEARPROGRAMDATA_TXX
