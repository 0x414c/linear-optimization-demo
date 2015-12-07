#include "linearprogrammingutils.hxx"


#include <cmath>

#include <algorithm>

#include <QList>
#include <QPointF>


namespace LinearProgrammingUtils
{
  /**
   * @brief sortPointsClockwise
   * Sorts given 2-D point in the clockwise order
   * using polar coordinates.
   * @param points Points vector to sort.
   */
  void
  sortPointsClockwise(QList<QPointF>& points)
  {
    //Centroid of a convex region will always
    //lie inside of that region
    QPointF centroid(0., 0.);
    for (int i(0); i < points.count(); ++i)
    {
      centroid += points.at(i);
    }
    centroid /= points.count(); //Find a center point

    //Convert to polar coords on-the-fly and
    //use the polar angles (relative to centroid) to sort
    //all the vertices clockwise.
    std::sort(
      points.begin(),
      points.end(),
      [&centroid](const QPointF& lhs, const QPointF& rhs)
      {
        return (
          atan2(lhs.y() - centroid.y(), lhs.x() - centroid.x()) <
          atan2(rhs.y() - centroid.y(), rhs.x() - centroid.x())
        );
      }
    );
  }
}
