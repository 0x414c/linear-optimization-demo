#ifndef PLOTDATA2D_HXX
#define PLOTDATA2D_HXX

#include <QLineF>
#include <QPointF>
#include <QVector>

#include "linearprogramsolution.hxx"
#include "../misc/utils.hxx"

using namespace Utils;

struct PlotData2D
{
  PlotData2D() = delete;
  PlotData2D(ResultType resultType, const QPointF& extremeVertex,
             qreal extremeValue, const QVector<QPointF>& vertices,
             const QLineF& gradient);

  ResultType resultType;
  QPointF extremeVertex;
  qreal extremeValue;
  QVector<QPointF> vertices;
  QLineF gradient;
};

#endif // PLOTDATA2D_HXX
