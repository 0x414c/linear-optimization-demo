#ifndef LINEARPROGRAMDATA_TXX
#define LINEARPROGRAMDATA_TXX

#include "linearprogramdata.hxx"

#include "eigen3/Eigen/Dense"

using namespace Eigen;

template<typename T>
LinearProgramData<T>::LinearProgramData() {}

template<typename T>
LinearProgramData<T>::LinearProgramData(const Matrix<T, 1, Dynamic>& objFuncCoeffs,
                                        const Matrix<T, Dynamic, Dynamic>& constraintsCoeffs,
                                        const Matrix<T, Dynamic, 1>& constraintsRHS) :
  objFuncCoeffs(objFuncCoeffs),
  constraintsCoeffs(constraintsCoeffs),
  constraintsRHS(constraintsRHS) {}

#endif // LINEARPROGRAMDATA_TXX
