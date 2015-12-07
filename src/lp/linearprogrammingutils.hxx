#pragma once

#ifndef LINEARPROGRAMMINGUTILS_HXX
#define LINEARPROGRAMMINGUTILS_HXX


#include <utility>

#include <QList>
#include <QPointF>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "linearprogramdata.hxx"
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

  void sortPointsClockwise(QList<QPointF>& points);
}


#include "linearprogrammingutils.txx"


#endif // LINEARPROGRAMMINGUTILS_HXX
