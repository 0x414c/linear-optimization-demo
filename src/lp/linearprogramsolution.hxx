#pragma once

#ifndef LINEARPROGRAMSOLUTION_HXX
#define LINEARPROGRAMSOLUTION_HXX


#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using NumericTypes::real_t;


  template<typename T = real_t>
  /**
   * @brief The LinearProgramSolution struct
   */
  struct LinearProgramSolution
  {
    LinearProgramSolution() = default;

    LinearProgramSolution(const LinearProgramSolution<T>& other);
    LinearProgramSolution(LinearProgramSolution<T>&& other);

    LinearProgramSolution(
      const Matrix<T, Dynamic, 1>& extremePoint,
      const T& extremeValue
    );
    LinearProgramSolution(
      Matrix<T, Dynamic, 1>&& extremePoint,
      const T& extremeValue
    );

    const LinearProgramSolution<T>& operator =(
      const LinearProgramSolution<T>& other
    );
    const LinearProgramSolution<T>& operator =(
      LinearProgramSolution<T>&& other
    );


    Matrix<T, Dynamic, 1> extremePoint;

    T extremeValue;
  };
}


#include "linearprogramsolution.txx"


#endif // LINEARPROGRAMSOLUTION_HXX
