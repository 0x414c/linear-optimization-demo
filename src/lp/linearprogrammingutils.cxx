#include "linearprogrammingutils.hxx"

#include <cmath>

#include <list>

#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"


namespace LinearProgrammingUtils
{
  using Eigen::Matrix;
  using NumericTypes::real_t;
  using std::atan2;
  using std::list;


  /**
   * @brief sortPointsByPolarAngle
   * Sorts given 2-D point in the clockwise order
   * using polar coordinates.
   * @param points Points container to sort.
   */
  void
  sortPointsByPolarAngle(list<Matrix<real_t, 2, 1>>& points)
  {
    //Centroid of a convex region will always
    //lie inside of that region
    Matrix<real_t, 2, 1> centroid(Matrix<real_t, 2, 1>::Zero());

    for (auto it(points.cbegin()); it != points.cend(); ++it)
    {
      centroid.x() += (*it).x();
      centroid.y() += (*it).y();
    }

    //Find a center point
    centroid.x() /= points.size();
    centroid.y() /= points.size();

    //Convert to polar coords on-the-fly and
    //use the polar angles (relative to centroid) to sort
    //all the vertices clockwise.
    points.sort(
      [&centroid](const Matrix<real_t, 2, 1>& lhs, const Matrix<real_t, 2, 1>& rhs)
      {
        return (
          atan2(lhs.y() - centroid.y(), lhs.x() - centroid.x()) <
          atan2(rhs.y() - centroid.y(), rhs.x() - centroid.x())
        );
      }
    );
  }


  /**
   * @brief perp
   * Returns vector perpendicular to `vec'.
   * @param point
   * @return
   */
  Matrix<real_t, 2, 1>
  perp(const Matrix<real_t, 2, 1>& vec)
  {
    Matrix<real_t, 2, 1> perp(2, 1);

    perp <<
      vec.y() * real_t(-1),
      vec.x();

    return perp;
  }


  /**
   * @brief norm
   * Returns vector `vec' as normalized.
   * @param vec
   * @return
   */
  Matrix<real_t, 2, 1>
  norm(const Matrix<real_t, 2, 1>& vec)
  {
    return vec.normalized();
  }


  /**
   * @brief lerp
   * Performs linear interpolation.
   *   @(x1)->(y1)----@(x0)->(y0)----@(x2)->(y2)
   * @param x1
   * @param y1
   * @param x2
   * @param y2
   * @param x0
   * @return y0
   */
  real_t
  lerp(real_t x1, real_t y1, real_t x2, real_t y2, real_t x0)
  {
    return (y1 + (y2 - y1) * ((x0 - x1) / (x2 - x1)));
  }


  /**
   * @brief blerp
   * Performs bilinear interpolation.
   *   @(x1; y2)->(z12)--------@(x2; y2)->(z22)
   *   |                       |
   *   |     @(x0; y0)->z00    |
   *   |                       |
   *   @(x1; y1)->(z11)--------@(x2; y1)->(z21)
   * @param x1
   * @param y1
   * @param z11
   * @param z12
   * @param x2
   * @param y2
   * @param z22
   * @param z21
   * @param x0
   * @param y0
   * @return z00
   */
  real_t
  blerp(
    real_t x1, real_t y1, real_t z11, real_t z12,
    real_t x2, real_t y2, real_t z22, real_t z21,
    real_t x0, real_t y0
  )
  {
    const real_t z01(lerp(x1, z11, x2, z21, x0));
    const real_t z02(lerp(x1, z12, x2, z22, x0));
    const real_t z00(lerp(y1, z01, y2, z02, y0));

    return z00;
  }
}
