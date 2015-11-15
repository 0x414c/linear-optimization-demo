#ifndef LINEARPROGRAMSOLUTION_TXX
#define LINEARPROGRAMSOLUTION_TXX

#include <utility>

#include "linearprogramsolution.hxx"

#include "eigen3/Eigen/Core"

namespace LinearProgramming
{
  using namespace Eigen;

  template<typename T>
  LinearProgramSolution<T>::LinearProgramSolution() :
    extremePoint(Matrix<T, 1, Dynamic>::Zero(1, 1))
  { }

  template<typename T>
  LinearProgramSolution<T>::LinearProgramSolution(
    const LinearProgramSolution<T>& other
  ) :
    solutionType(other.solutionType),
    extremePoint(other.extremePoint),
    extremeValue(other.extremeValue)
  { }

  template<typename T>
  LinearProgramSolution<T>::LinearProgramSolution(
    LinearProgramSolution<T>&& other
  ) :
    solutionType(other.solutionType),
    extremePoint(std::move(other.extremePoint)),
    extremeValue(other.extremeValue)
  { }

  template<typename T>
  LinearProgramSolution<T>::LinearProgramSolution(
    SolutionType solutionType,
    const Matrix<T, 1, Dynamic>& extremePoint,
    const T& extremeValue
  ) :
    solutionType(solutionType),
    extremePoint(extremePoint),
    extremeValue(extremeValue)
  { }

  template<typename T>
  LinearProgramSolution<T>::LinearProgramSolution(
    SolutionType solutionType,
    Matrix<T, 1, Dynamic>&& extremePoint,
    const T& extremeValue
  ) :
    solutionType(solutionType),
    extremePoint(std::move(extremePoint)),
    extremeValue(extremeValue)
  { }

  template<typename T>
  const LinearProgramSolution<T>&
  LinearProgramSolution<T>::operator =(
    const LinearProgramSolution<T>& other
  )
  {
    if (this != &other)
    {
      solutionType = other.solutionType;
      extremePoint = other.extremePoint;
      extremeValue = other.extremeValue;
    }

    return *this;
  }

  template<typename T>
  const LinearProgramSolution<T>&
  LinearProgramSolution<T>::operator =(
    LinearProgramSolution<T>&& other
  )
  {
    if (this != &other)
    {
      solutionType = other.solutionType;
      extremePoint = std::move(other.extremePoint);
      extremeValue = other.extremeValue;
    }

    return *this;
  }
}

#endif // LINEARPROGRAMSOLUTION_TXX
