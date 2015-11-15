#ifndef LINEARPROGRAMUTILS_HXX
#define LINEARPROGRAMUTILS_HXX

#include <utility>

#include <QPointF>
#include <QVector>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "linearprogramdata.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"

namespace LinearProgramUtils
{
  using namespace boost;
  using namespace Eigen;
  using namespace LinearProgramming;
  using namespace NumericTypes;
  using namespace std;
  using namespace Utils;

  template<typename T = Real>
  bool isPointInFeasibleRegion(
    const Matrix<T, Dynamic, 1>& point,
    const LinearProgramData<T>& linearProgramData
  );

  //TODO: ~ Move all the following functions to the `MathUtils' namespace
  template<typename T = Real>
  optional<Matrix<T, 2, 1>> findIntersection(
    const Matrix<T, 2, 2>& A, const Matrix<T, 2, 1>& b
  );

  template<typename T = Real>
  pair<Matrix<T, Dynamic, Dynamic>, DenseIndex>
  reducedRowEchelonForm(const Matrix<T, Dynamic, Dynamic>& matrix);

  void sortPointsClockwise(QVector<QPointF>& points);
}

#include "linearprogramutils.txx"

#endif // LINEARPROGRAMUTILS_HXX
