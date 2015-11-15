#ifndef LINEARPROGRAMDATA_TXX
#define LINEARPROGRAMDATA_TXX

#include <stdexcept>
#include <utility>

#include "linearprogramdata.hxx"

#include "eigen3/Eigen/Core"

namespace LinearProgramming
{
  using namespace Eigen;

  template<typename T>
  LinearProgramData<T>::LinearProgramData() :
    objectiveFuncCoeffs(Matrix<T, 1, Dynamic>::Zero(1, 1)),
    constraintsCoeffs(Matrix<T, Dynamic, Dynamic>::Zero(1, 1)),
    constraintsRHS(Matrix<T, Dynamic, 1>::Zero(1, 1))
  { }

  template<typename T>
  LinearProgramData<T>::LinearProgramData(const LinearProgramData<T>& other) :
    objectiveFuncCoeffs(other.objectiveFuncCoeffs),
    constraintsCoeffs(other.constraintsCoeffs),
    constraintsRHS(other.constraintsRHS)
  { }

  template<typename T>
  LinearProgramData<T>::LinearProgramData(LinearProgramData<T>&& other) :
    objectiveFuncCoeffs(
      std::move(other.objectiveFuncCoeffs)
    ),
    constraintsCoeffs(std::move(other.constraintsCoeffs)),
    constraintsRHS(std::move(other.constraintsRHS))
  { }

  template<typename T>
  LinearProgramData<T>::LinearProgramData(
      const Matrix<T, 1, Dynamic>& objFuncCoeffs,
      const Matrix<T, Dynamic, Dynamic>& constraintsCoeffs,
      const Matrix<T, Dynamic, 1>& constraintsRHS
  ) throw(invalid_argument) :
    objectiveFuncCoeffs(objFuncCoeffs),
    constraintsCoeffs(constraintsCoeffs),
    constraintsRHS(constraintsRHS)
  {
    if (
      objFuncCoeffs.cols() != constraintsCoeffs.cols() ||
      constraintsRHS.rows() != constraintsCoeffs.rows()
    )
    {
      throw invalid_argument("Inconsistent sizes.");
    }
  }

  template<typename T>
  LinearProgramData<T>::LinearProgramData(
    Matrix<T, 1, Dynamic>&& objFuncCoeffs,
    Matrix<T, Dynamic, Dynamic>&& constraintsCoeffs,
    Matrix<T, Dynamic, 1>&& constraintsRHS
  ) throw(invalid_argument) :
    objectiveFuncCoeffs(std::move(objFuncCoeffs)),
    constraintsCoeffs(std::move(constraintsCoeffs)),
    constraintsRHS(std::move(constraintsRHS))
  {
    if (
      objFuncCoeffs.cols() != constraintsCoeffs.cols() ||
      constraintsRHS.rows() != constraintsCoeffs.rows()
    )
    {
      throw invalid_argument("Inconsistent sizes.");
    }
  }

  template<typename T>
  const LinearProgramData<T>&
  LinearProgramData<T>::operator =(
    const LinearProgramData<T>& other
  )
  {
    if (this != &other)
    {
      objectiveFuncCoeffs = other.objectiveFuncCoeffs;
      constraintsCoeffs = other.constraintsCoeffs;
      constraintsRHS = other.constraintsRHS;
    }

    return *this;
  }

  template<typename T>
  const LinearProgramData<T>&
  LinearProgramData<T>::operator =(
    LinearProgramData<T>&& other
  )
  {
    if (this != &other)
    {
      objectiveFuncCoeffs =
        std::move(other.objectiveFuncCoeffs);
      constraintsCoeffs = std::move(other.constraintsCoeffs);
      constraintsRHS = std::move(other.constraintsRHS);
    }

    return *this;
  }

  template<typename T>
  DenseIndex
  LinearProgramData<T>::constraintsCount() const
  {
    return constraintsCoeffs.rows();
  }

  template<typename T>
  DenseIndex
  LinearProgramData<T>::variablesCount() const
  {
    return constraintsCoeffs.cols();
  }
}

#endif // LINEARPROGRAMDATA_TXX
