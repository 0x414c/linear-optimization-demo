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
  PlotData2D()/* = delete*/;
  PlotData2D(SolutionType resultType, const QPointF& extremeVertex,
             qreal extremeValue, const QVector<QPointF>& vertices,
             const QLineF& gradient);

  SolutionType resultType = SolutionType::Unknown;
  QPointF extremeVertex = QPointF(0., 0.);
  qreal extremeValue = 0.;
  QVector<QPointF> vertices = QVector<QPointF>(0);
  QLineF gradient = QLineF(0., 0., 0., 0.);
};

#endif // PLOTDATA2D_HXX
