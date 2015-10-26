#ifndef LINEARPROGRAMUTILS_HXX
#define LINEARPROGRAMUTILS_HXX

#include <QPointF>
#include <QVector>

#include "eigen3/Eigen/Dense"

#include "linearprogramdata.hxx"
#include "../misc/utils.hxx"

using namespace Eigen;
using namespace Utils;

namespace LinearProgramUtils
{
  template<typename T = real_t>
  /**
   * @brief isPointInFeasibleRegion
   * Checks if given point (as column-vector) lies
   * inside of the feasible region
   * which interior is defined by following
   * matrix equation:
   *   Ax <= b
   * and if point is also satisfies mandatory
   * non-negativity constraint:
   *   x[n] >= 0 for any n in [1; N]
   * @param point
   * @param linearProgramData
   * @return
   */
  inline bool isPointInFeasibleRegion(const Matrix<T, Dynamic, 1>& point,
                                      const LinearProgramData<T>& linearProgramData)
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
  void sortClockwise(QVector<QPointF>& points);
}

#endif // LINEARPROGRAMUTILS_HXX
