#pragma once

#ifndef LINEARPROGRAMMINGUTILS_HXX
#define LINEARPROGRAMMINGUTILS_HXX


#include <list>
#include <utility>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "linearprogramdata.hxx"
#include "plotdatareal2d.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgrammingUtils
{
  using namespace boost;
  using namespace Eigen;
  using namespace LinearProgramming;
  using namespace NumericTypes;
  using namespace std;


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

  template<typename T = Real>
  pair<Matrix<T, Dynamic, Dynamic>, DenseIndex>
  reducedRowEchelonForm(const Matrix<T, Dynamic, Dynamic>& A);

  void sortPointsClockwise(list<PointReal2D>& points);

  PointReal2D perp(const PointReal2D& point);

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
