#include "plotdata2d.hxx"

#include <QLineF>
#include <QPointF>
#include <QVector>

#include "linearprogramsolution.hxx"
#include "../misc/utils.hxx"

using namespace Utils;

PlotData2D::PlotData2D(ResultType resultType, const QPointF& extremeVertex,
                       real_t extremumValue, const QVector<QPointF>& vertices, const QLineF& gradient) :
  resultType(resultType), extremeVertex(extremeVertex),
  extremeValue(extremumValue), vertices(vertices), gradient(gradient) {}
