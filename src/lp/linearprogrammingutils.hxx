#pragma once

#ifndef LINEARPROGRAMMINGUTILS_HXX
#define LINEARPROGRAMMINGUTILS_HXX


#include <list>
#include <utility>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"


namespace LinearProgrammingUtils
{
  using boost::optional;
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using NumericTypes::real_t;
  using std::list;
  using Utils::AlwaysFalse;


  template<typename T = real_t>
  bool isSolutionFeasible(
    const Matrix<T, Dynamic, 1>& x,
    const Matrix<T, Dynamic, Dynamic>& A,
    const Matrix<T, Dynamic, 1>& b
  );


  //TODO: ~ Move all the following functions to the `MathUtils' namespace.
  template<typename TCoeff = real_t, DenseIndex TDim = 2>
  struct findIntersectionImpl_
  {
    static optional<Matrix<TCoeff, TDim, 1>>
    findIntersection(
      const Matrix<TCoeff, TDim, TDim>& A, const Matrix<TCoeff, TDim, 1>& b
    )
    {
      static_assert(
        AlwaysFalse<TCoeff>::value,
        "LinearProgrammingUtils::findIntersection<TCoeff, TDim>: "
        "You can only use one of the specified template specializations!"
      );
    }
  };


  template<typename TCoeff = real_t, DenseIndex TDim = 2>
  optional<Matrix<TCoeff, TDim, 1>> findIntersection(
    const Matrix<TCoeff, TDim, TDim>& A, const Matrix<TCoeff, TDim, 1>& b
  )
  {
    return findIntersectionImpl_<TCoeff, TDim>::findIntersection(A, b);
  }


  template<typename T>
  struct RREF
  {
    RREF(const Matrix<T, Dynamic, Dynamic>& rref, DenseIndex rank) :
      rref(rref), rank(rank)
    { }

    RREF(Matrix<T, Dynamic, Dynamic>&& rref, DenseIndex rank) :
      rref(std::move(rref)), rank(rank)
    { }


    Matrix<T, Dynamic, Dynamic> rref;
    DenseIndex rank;
  };


  template<typename T = real_t>
  RREF<T> reducedRowEchelonForm(const Matrix<T, Dynamic, Dynamic>& A);

  void sortPointsByPolarAngle(list<Matrix<real_t, 2, 1>>& points);

  Matrix<real_t, 2, 1> perp(const Matrix<real_t, 2, 1>& vec);

  Matrix<real_t, 2, 1> norm(const Matrix<real_t, 2, 1>& vec);

  real_t lerp(real_t x1, real_t y1, real_t x2, real_t y2, real_t x0);

  real_t blerp(
    real_t x1, real_t y1, real_t z11, real_t z12,
    real_t x2, real_t y2, real_t z22, real_t z21,
    real_t x0, real_t y0
  );

  template<typename T = real_t>
  Matrix<T, 2, 2> computeBoundingBox(const list<Matrix<T, 2, 1>>& points);
}


#include "linearprogrammingutils.txx"


#endif // LINEARPROGRAMMINGUTILS_HXX
