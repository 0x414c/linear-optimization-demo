#pragma once

#ifndef LINEARPROGRAMSOLUTION_TXX
#define LINEARPROGRAMSOLUTION_TXX


#include "linearprogramsolution.hxx"

#include <utility>

#include "eigen3/Eigen/Core"

#include "solutiontype.hxx"


namespace LinearProgramming
{
  using Eigen::Dynamic;
  using Eigen::Matrix;


  template<typename T>
  LinearProgramSolution<T>::LinearProgramSolution(
    const LinearProgramSolution<T>& other
  ) :
    extremePoint(other.extremePoint),
    extremeValue(other.extremeValue)
  { }


  template<typename T>
  LinearProgramSolution<T>::LinearProgramSolution(
    LinearProgramSolution<T>&& other
  ) :
    extremePoint(std::move(other.extremePoint)),
    extremeValue(other.extremeValue)
  { }


  template<typename T>
  LinearProgramSolution<T>::LinearProgramSolution(
    const Matrix<T, Dynamic, 1>& extremePoint,
    const T& extremeValue
  ) :
    extremePoint(extremePoint),
    extremeValue(extremeValue)
  { }


  template<typename T>
  LinearProgramSolution<T>::LinearProgramSolution(
    Matrix<T, Dynamic, 1>&& extremePoint,
    const T& extremeValue
  ) :
    extremePoint(std::move(extremePoint)),
    extremeValue(extremeValue)
  { }


  template<typename T>
  const LinearProgramSolution<T>&
  LinearProgramSolution<T>::operator =(const LinearProgramSolution<T>& other)
  {
    if (this != &other)
    {
      extremePoint = other.extremePoint;
      extremeValue = other.extremeValue;
    }

    return *this;
  }


  template<typename T>
  const LinearProgramSolution<T>&
  LinearProgramSolution<T>::operator =(LinearProgramSolution<T>&& other)
  {
    if (this != &other)
    {
      extremePoint = move(other.extremePoint);
      extremeValue = other.extremeValue;
    }

    return *this;
  }
}


#endif // LINEARPROGRAMSOLUTION_TXX
