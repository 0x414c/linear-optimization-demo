#include "linearprogrammingutils.hxx"


#include <cmath>

#include <list>
#include <utility>

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
  sortPointsClockwise(list<pair<Real, Real>>& points)
  {
    //Centroid of a convex region will always
    //lie inside of that region
    pair<Real, Real> centroid(0, 0);
    for (auto it(points.cbegin()); it != points.cend(); ++it)
    {
      centroid.first += (*it).first;
      centroid.second += (*it).second;
    }

     //Find a center point
    centroid.first /= points.size();
    centroid.second /= points.size();

    //Convert to polar coords on-the-fly and
    //use the polar angles (relative to centroid) to sort
    //all the vertices clockwise.
    points.sort(
      [&centroid] (const pair<Real, Real>& lhs, const pair<Real, Real>& rhs)
      {
        return (
          atan2(lhs.second - centroid.second, lhs.first - centroid.first) <
          atan2(rhs.second - centroid.second, rhs.first - centroid.first)
        );
      }
    );
  }


  /**
   * @brief perp
   * @param point
   * @return
   */
  pair<Real, Real>
  perp(pair<Real, Real> point)
  {
    return make_pair(point.second * Real(-1), point.first);
  }
}
