#ifndef LINEARPROGRAMUTILS_HXX
#define LINEARPROGRAMUTILS_HXX

#include <utility>

#include <QPointF>
#include <QVector>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Dense"

#include "linearprogramdata.hxx"
#include "../misc/utils.hxx"

using namespace boost;
using namespace Eigen;
using namespace std;
using namespace Utils;

namespace LinearProgramUtils
{
  template<typename T = Real>
  bool isPointInFeasibleRegion(const Matrix<T, Dynamic, 1>& point,
                               const LinearProgramData<T>& linearProgramData);

  template<typename T = Real>
  optional<Matrix<T, 2, 1>> findIntersection(const Matrix<T, 2, 2>& a,
                                   const Matrix<T, 2, 1>& b);

  template<typename T = Real>
  pair<Matrix<T, Dynamic, Dynamic>, Matrix<T, Dynamic, Dynamic>>
  reducedRowEchelonForm(const Matrix<T, Dynamic, Dynamic>& matrix);

  void sortPointsClockwise(QVector<QPointF>& points);
}

#include "linearprogramutils.txx"

#endif // LINEARPROGRAMUTILS_HXX
