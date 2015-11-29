﻿#pragma once

#ifndef LINEARFUNCTION_HXX
#define LINEARFUNCTION_HXX


#include <utility>

#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"
#include "../misc/eigenextensions.hxx"


namespace LinearProgramming
{
  using namespace Eigen;
  using namespace NumericTypes;


  /**
   * @brief The `LinearFunction' templated class
   * represents linear functions on a field w/ elements of type `T':
   *   F(x1, x2, ..., xN) ==
   *   c1 * x1 + c2 * X2 + ... + cN*xN + d ==
   *   (c, x) + d,
   * where:
   *   x := (x1, ..., xN),
   *   c := (c1, ..., cN),
   *   d is the constant term,
   * and operation (*, *) denotes the dot product.
   * This class is using `Eigen' library for internal
   * representation of function coefficents vector `C'.
   * The grad(F) defines normal vector
   *   X := <X1, ..., XN>
   * to the hyperplane that is defined by equation
   *   F(X1, ..., XN) == 0.
   * This vector defines direction in which
   * function value increases.
   * We can use this property to solve LP.
   * For reference see:
   * `https://en.wikipedia.org/wiki/Field_(mathematics)',
   * `https://en.wikipedia.org/wiki/Linear_form',
   * `https://en.wikipedia.org/wiki/Hyperplane'.
   */
  template<typename T = Real, DenseIndex N = Dynamic>
  class LinearFunction
  {
    public:
      LinearFunction() = delete;


      explicit LinearFunction(
        const Matrix<T, 1, N>& coeffs, T constTerm = T(0)
      ) :
        _coeffs(coeffs), _constTerm(constTerm)
      { }


      explicit LinearFunction(Matrix<T, 1, N>&& coeffs, T constTerm = T(0)) :
        _coeffs(std::move(coeffs)), _constTerm(constTerm)
      { }


      DenseIndex
      dim() const
      {
        return _coeffs.cols();
      }


      const Matrix<T, 1, N>&
      coeffs() const
      {
        return _coeffs;
      }


//      typename Matrix<T, N, 1>::CoeffReturnType
      const T&
      coeffAt(DenseIndex coeffIdx) const
      {
        return _coeffs(coeffIdx);
      }

      T&
      coeffAt(DenseIndex coeffIdx)
      {
        return _coeffs(coeffIdx);
      }


      const T&
      constTerm() const
      {
        return _constTerm;
      }


      T&
      constTerm()
      {
        return _constTerm;
      }


      T
      operator ()(const Matrix<T, N, 1>& args) const
      {
        return valueAt(args);
      }


      T
      valueAt(const Matrix<T, N, 1>& args) const
      {
        return (_coeffs.dot(args) + _constTerm);
      }


    private:
      Matrix<T, 1, N> _coeffs;

      T _constTerm = T(0);
  };
}


#endif // LINEARFUNCTION_HXX
