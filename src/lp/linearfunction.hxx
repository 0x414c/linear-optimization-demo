#pragma once

#ifndef LINEARFUNCTION_HXX
#define LINEARFUNCTION_HXX


#include <utility>

#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"
#include "../misc/eigenextensions.hxx"


namespace LinearProgramming
{
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using NumericTypes::Real;


  /**
   * @brief The `LinearFunction' templated class
   * represents linear functions on a field w/ elements of type `T':
   *   F(x1, x2, ..., xN) ==
   *   c1 * x1 + c2 * x2 + ... + cN*xN + d ==
   *   (c, x) + d,
   * where:
   *   `x' is the column-vector (x1, ..., xN),
   *   `c' is the row-vector (c1, ..., cN),
   *   `d' is the constant term,
   * and operation (*, *) denotes the dot product.
   * This class is using `Eigen' library for internal
   * representation of function coefficents vector `C'.
   * The grad(F) defines normal vector
   *   n == (x1, ..., xN)
   * to the hyperplane that is defined by equation
   *   F(x1, ..., xN) == 0.
   * This vector defines direction in which
   * function value increases.
   * We can use this property to solve linear programs.
   * For reference see:
   *  `https://en.wikipedia.org/wiki/Field_(mathematics)',
   *  `https://en.wikipedia.org/wiki/Linear_form',
   *  `https://en.wikipedia.org/wiki/Hyperplane'.
   */
  template<typename TCoeff = Real, DenseIndex TDim = Dynamic>
  class LinearFunction
  {
    public:
      LinearFunction() = delete;


      explicit LinearFunction(
        const Matrix<TCoeff, 1, TDim>& coeffs, TCoeff constTerm = TCoeff(0)
      ) :
        coeffs_(coeffs), constTerm_(constTerm)
      { }


      explicit LinearFunction(
        Matrix<TCoeff, 1, TDim>&& coeffs, TCoeff constTerm = TCoeff(0)
      ) :
        coeffs_(std::move(coeffs)), constTerm_(constTerm)
      { }


      DenseIndex
      dim() const
      {
        return coeffs_.cols();
      }


      const Matrix<TCoeff, 1, TDim>&
      coeffs() const
      {
        return coeffs_;
      }


//      typename Matrix<T, TDim, 1>::CoeffReturnType
      const TCoeff&
      coeffAt(DenseIndex coeffIdx) const
      {
        return coeffs_(coeffIdx);
      }


      TCoeff&
      coeffAt(DenseIndex coeffIdx)
      {
        return coeffs_(coeffIdx);
      }


      const TCoeff&
      constTerm() const
      {
        return constTerm_;
      }


      TCoeff&
      constTerm()
      {
        return constTerm_;
      }


      TCoeff
      operator ()(const Matrix<TCoeff, TDim, 1>& args) const
      {
        return valueAt(args);
      }


      TCoeff
      valueAt(const Matrix<TCoeff, TDim, 1>& args) const
      {
        return (coeffs_.dot(args) + constTerm_);
      }


    private:
      /**
       * @brief coeffs_
       * Row-vector `c'.
       */
      Matrix<TCoeff, 1, TDim> coeffs_;

      /**
       * @brief constTerm_
       * Constant term `d'.
       */
      TCoeff constTerm_;
  };
}


#endif // LINEARFUNCTION_HXX
