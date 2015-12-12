﻿#include "linearprogrammingutils.hxx"


#include <cmath>

#include <list>
#include <utility>

#include "plotdatareal2d.hxx"
#include "pointreal2d.hxx"
#include "pointvaluereal2d.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgrammingUtils
{
  using namespace NumericTypes;
  using namespace std;


  /**
   * @brief sortPointsClockwise
   * Sorts given 2-D point in the clockwise order
   * using polar coordinates.
   * @param points Points container to sort.
   */
  void
  sortPointsClockwise(list<PointReal2D>& points)
  {
    //Centroid of a convex region will always
    //lie inside of that region
    PointReal2D centroid(0., 0.);

    for (auto it(points.cbegin()); it != points.cend(); ++it)
    {
      centroid.x += (*it).x;
      centroid.y += (*it).y;
    }

     //Find a center point
    centroid.x /= points.size();
    centroid.y /= points.size();

    //Convert to polar coords on-the-fly and
    //use the polar angles (relative to centroid) to sort
    //all the vertices clockwise.
    points.sort(
      [&centroid] (const PointReal2D& lhs, const PointReal2D& rhs)
      {
        return (
          atan2(lhs.y - centroid.y, lhs.x - centroid.x) <
          atan2(rhs.y - centroid.y, rhs.x - centroid.x)
        );
      }
    );
  }


  /**
   * @brief perp
   * @param point
   * @return
   */
  PointReal2D
  perp(const PointReal2D& point)
  {
    return PointReal2D(point.y * Real(-1), point.x);
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
  Real
  lerp(Real x1, Real y1, Real x2, Real y2, Real x0)
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
  Real
  blerp(
    Real x1, Real y1, Real z11, Real z12,
    Real x2, Real y2, Real z22, Real z21,
    Real x0, Real y0
  )
  {
    const Real z01(lerp(x1, z11, x2, z21, x0));
    const Real z02(lerp(x1, z12, x2, z22, x0));
    const Real z00(lerp(y1, z01, y2, z02, y0));

    return z00;
  }
}
