#include "plotdata2d.hxx"

#include <QLineF>
#include <QPointF>
#include <QVector>

#include "linearprogramsolution.hxx"
#include "../misc/utils.hxx"

using namespace Utils;

PlotData2D::PlotData2D()
{ }

PlotData2D::PlotData2D(SolutionType resultType,
                       const QPointF& extremePoint,
                       qreal extremeValue,
                       const QVector<QPointF>& vertices,
                       const QLineF& gradient) :
  resultType(resultType),
  extremeVertex(extremePoint),
  extremeValue(extremeValue),
  vertices(vertices),
  gradient(gradient)
{ }
