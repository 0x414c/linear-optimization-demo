#pragma once

#ifndef LINEARPROGRAMSOLUTION_HXX
#define LINEARPROGRAMSOLUTION_HXX


#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using namespace Eigen;
  using namespace NumericTypes;


  template<typename T = Real>
  /**
   * @brief The LinearProgramSolution struct
   */
  struct LinearProgramSolution
  {
    LinearProgramSolution() = default;

    LinearProgramSolution(const LinearProgramSolution<T>& other);
    LinearProgramSolution(LinearProgramSolution<T>&& other);

    LinearProgramSolution(
      const Matrix<T, 1, Dynamic>& extremePoint,
      const T& extremeValue
    );
    LinearProgramSolution(
      Matrix<T, 1, Dynamic>&& extremePoint,
      const T& extremeValue
    );

    const LinearProgramSolution<T>& operator =(
      const LinearProgramSolution<T>& other
    );
    const LinearProgramSolution<T>& operator =(
      LinearProgramSolution<T>&& other
    );


    Matrix<T, 1, Dynamic> extremePoint;

    T extremeValue;
  };
}


#include "linearprogramsolution.txx"


#endif // LINEARPROGRAMSOLUTION_HXX
