#ifndef LINEARPROGRAMDATA_HXX
#define LINEARPROGRAMDATA_HXX

#include <stdexcept>

#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"

namespace LinearProgramming
{
  using namespace Eigen;
  using namespace NumericTypes;
  using namespace Utils;

  template<typename T = Real>
  /**
   * @brief The LinearProgramData struct
   */
  struct LinearProgramData
  {
    LinearProgramData()/* = delete*/;
    LinearProgramData(const LinearProgramData<T>& other);
    LinearProgramData(LinearProgramData<T>&& other);
    LinearProgramData(
      const Matrix<T, 1, Dynamic>& objectiveFuncCoeffs,
      const Matrix<T, Dynamic, Dynamic>& constraintsCoeffs,
      const Matrix<T, Dynamic, 1>& constraintsRHS
    ) throw(invalid_argument);
    LinearProgramData(
      Matrix<T, 1, Dynamic>&& objectiveFuncCoeffs,
      Matrix<T, Dynamic, Dynamic>&& constraintsCoeffs,
      Matrix<T, Dynamic, 1>&& constraintsRHS
    ) throw(invalid_argument);

    const LinearProgramData<T>& operator =(const LinearProgramData<T>& other);
    const LinearProgramData<T>& operator =(LinearProgramData<T>&& other);

    DenseIndex constraintsCount() const;

    DenseIndex variablesCount() const;

//    static LinearProgramData<T> make(...);

    Matrix<T, 1, Dynamic> objectiveFuncCoeffs;
    Matrix<T, Dynamic, Dynamic> constraintsCoeffs;
    Matrix<T, Dynamic, 1> constraintsRHS;
  };
}

#include "linearprogramdata.txx"

#endif // LINEARPROGRAMDATA_HXX
