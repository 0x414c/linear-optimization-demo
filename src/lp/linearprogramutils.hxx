#ifndef LINEARPROGRAMUTILS_HXX
#define LINEARPROGRAMUTILS_HXX

#include <algorithm>
#include <cmath>

#include <QPointF>
#include <QVector>

#include "eigen3/Eigen/Dense"

#include "linearprogramdata.hxx"
#include "../misc/utils.hxx"

using namespace Utils;
using namespace Eigen;

namespace LinearProgramUtils {
  template<typename T = real_t>
  /**
   * @brief isPointInFeasibleRegion
   * Checks if given point (as column-vector) lies
   * inside of the feasible region
   * which interior is defined by following
   * matrix equation:
   *   A * x <= b
   * and if point is also satisfies mandatory
   * non-negativity constraint:
   *   x[n] >= 0 for any n in [1; N]
   * @param point
   * @param linearProgramData
   * @return
   */
  inline bool isPointInFeasibleRegion(const Matrix<T, Dynamic, 1>& point, const LinearProgramData<T>& linearProgramData)
  {
    return
      (point.array() >= 0).all()
      &&
      (
        (linearProgramData.constraintsCoeffs * point).array()
        <=
        linearProgramData.constraintsRHS.array()
      ).all();
  }

  /**
   * @brief sortClockwise
   * @param points
   */
  inline void sortClockwise(QVector<QPointF>& points)
  {
    //find a center point
    QPointF centroid(0, 0);
    for (int i = 0; i < points.length(); ++i)
    {
      centroid += points[i];
    }
    centroid /= points.length();

    //convert to polar coords on-the-fly
    //and use the polar angles (relative to centroid) to sort
    //all the vertices clockwise
    std::sort(
      points.begin(),
      points.end(),
      [&centroid](const QPointF& lhs, const QPointF& rhs)
      {
        return
          atan2(lhs.y() - centroid.y(), lhs.x() - centroid.x())
          <
          atan2(rhs.y() - centroid.y(), rhs.x() - centroid.x());
      }
    );
  }
}

#endif // LINEARPROGRAMUTILS_HXX
