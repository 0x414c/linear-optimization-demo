#include "plotdata2d.hxx"

#include <utility>

#include <QLineF>
#include <QList>
#include <QPointF>

#include "solutiontype.hxx"


namespace LinearProgramming
{
  PlotData2D::PlotData2D(
    const QPointF& extremePoint, qreal extremeValue,
    const QList<QPointF>& vertices, const QLineF& gradient
  ) :
    extremeVertex(extremePoint),
    extremeValue(extremeValue),
    vertices(vertices),
    gradient(gradient)
  { }


  PlotData2D::PlotData2D(
    const QPointF& extremePoint, qreal extremeValue,
    QList<QPointF>&& vertices, const QLineF& gradient
  ) :
    extremeVertex(extremePoint),
    extremeValue(extremeValue),
    vertices(std::move(vertices)),
    gradient(gradient)
  { }
}
