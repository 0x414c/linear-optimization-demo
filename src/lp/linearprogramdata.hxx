#pragma once

#ifndef LINEARPROGRAMDATA_HXX
#define LINEARPROGRAMDATA_HXX


#include <stdexcept>

#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using std::invalid_argument;
  using NumericTypes::real_t;


  template<typename T = real_t>
  /**
   * @brief The LinearProgramData struct
   */
  struct LinearProgramData
  {
    LinearProgramData() = default;

    LinearProgramData(const LinearProgramData<T>& other);
    LinearProgramData(LinearProgramData<T>&& other);

    LinearProgramData(
      const Matrix<T, 1, Dynamic>& objectiveFunctionCoeffs,
      const Matrix<T, Dynamic, Dynamic>& constraintsCoeffs,
      const Matrix<T, Dynamic, 1>& constraintsRHS
    ) throw(invalid_argument);
    LinearProgramData(
      Matrix<T, 1, Dynamic>&& objectiveFunctionCoeffs,
      Matrix<T, Dynamic, Dynamic>&& constraintsCoeffs,
      Matrix<T, Dynamic, 1>&& constraintsRHS
    ) throw(invalid_argument);

    const LinearProgramData<T>& operator =(const LinearProgramData<T>& other);
    const LinearProgramData<T>& operator =(LinearProgramData<T>&& other);

    DenseIndex constraintsCount() const;

    DenseIndex variablesCount() const;

//    static LinearProgramData<T> make(???); //TODO: ~?


    Matrix<T, 1, Dynamic> objectiveFunctionCoeffs;

    Matrix<T, Dynamic, Dynamic> constraintsCoeffs;

    Matrix<T, Dynamic, 1> constraintsRHS;
  };
}


#include "linearprogramdata.txx"


#endif // LINEARPROGRAMDATA_HXX
