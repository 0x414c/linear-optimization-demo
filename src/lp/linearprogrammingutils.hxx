#pragma once

#ifndef LINEARPROGRAMMINGUTILS_HXX
#define LINEARPROGRAMMINGUTILS_HXX


#include <list>
#include <utility>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"


namespace LinearProgrammingUtils
{
  using boost::optional;
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using NumericTypes::Real;
  using std::list;


  template<typename T = Real>
  bool isSolutionFeasible(
    const Matrix<T, Dynamic, 1>& x,
    const Matrix<T, Dynamic, Dynamic>& A,
    const Matrix<T, Dynamic, 1>& b
  );

  //TODO: ~ Move all the following functions to the `MathUtils' namespace
  template<typename T = Real>
  optional<Matrix<T, 2, 1>> findIntersection(
    const Matrix<T, 2, 2>& A, const Matrix<T, 2, 1>& b
  );

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

  template<typename T = Real>
  RREF<T> reducedRowEchelonForm(const Matrix<T, Dynamic, Dynamic>& A);

  void sortPointsByPolarAngle(list<Matrix<Real, 2, 1>>& points);

  Matrix<Real, 2, 1> perp(const Matrix<Real, 2, 1>& vec);

  Matrix<Real, 2, 1> norm(const Matrix<Real, 2, 1>& vec);

  Real lerp(Real x1, Real y1, Real x2, Real y2, Real x0);

  Real blerp(
    Real x1, Real y1, Real z11, Real z12,
    Real x2, Real y2, Real z22, Real z21,
    Real x0, Real y0
  );

  template<typename T = Real>
  Matrix<T, 2, 2> computeBoundingBox(const list<Matrix<T, 2, 1>>& points);
}


#include "linearprogrammingutils.txx"


#endif // LINEARPROGRAMMINGUTILS_HXX
