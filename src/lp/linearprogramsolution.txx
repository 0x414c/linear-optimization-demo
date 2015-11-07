#ifndef LINEARPROGRAMSOLUTION_TXX
#define LINEARPROGRAMSOLUTION_TXX

#include "linearprogramsolution.hxx"

#include "eigen3/Eigen/Dense"

using namespace Eigen;

template<typename T>
LinearProgramSolution<T>::LinearProgramSolution() :
  extremePoint(Matrix<T, 1, Dynamic>::Zero(1, 1))
{ }

template<typename T>
LinearProgramSolution<T>::LinearProgramSolution(SolutionType resultType,
                                                const Matrix<T, 1, Dynamic>& extremePoint,
                                                const T& extremeValue) :
  resultType(resultType),
  extremePoint(extremePoint),
  extremeValue(extremeValue)
{ }

#endif // LINEARPROGRAMSOLUTION_TXX
