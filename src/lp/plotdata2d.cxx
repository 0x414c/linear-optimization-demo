#include "plotdata2d.hxx"

#include <utility>

#include <QLineF>
#include <QPointF>
#include <QVector>

#include "linearprogramsolution.hxx"
#include "../misc/utils.hxx"

using namespace Utils;

namespace LinearProgramming
{
  PlotData2D::PlotData2D()
  { }

  PlotData2D::PlotData2D(
    SolutionType resultType, const QPointF& extremePoint,
    qreal extremeValue, const QVector<QPointF>& vertices,
    const QLineF& gradient
  ) :
    resultType(resultType),
    extremeVertex(extremePoint),
    extremeValue(extremeValue),
    vertices(vertices),
    gradient(gradient)
  { }

  PlotData2D::PlotData2D(
    SolutionType resultType, const QPointF& extremePoint,
    qreal extremeValue, QVector<QPointF>&& vertices,
    const QLineF& gradient
  ) :
    resultType(resultType),
    extremeVertex(extremePoint),
    extremeValue(extremeValue),
    vertices(std::move(vertices)),
    gradient(gradient)
  { }
}
