#pragma once

#ifndef PLOTDATA2D_HXX
#define PLOTDATA2D_HXX


#include <QLineF>
#include <QList>
#include <QPointF>

#include "solutiontype.hxx"


namespace LinearProgramming
{
  struct PlotData2D
  {
    PlotData2D() = default;

    PlotData2D(
      const QPointF& extremeVertex, qreal extremeValue,
      const QList<QPointF>& vertices, const QLineF& gradient
    );
    PlotData2D(
      const QPointF& extremeVertex, qreal extremeValue,
      QList<QPointF>&& vertices, const QLineF& gradient
    );


    QPointF extremeVertex;

    qreal extremeValue;

    QList<QPointF> vertices;

    QLineF gradient;
  };
}


#endif // PLOTDATA2D_HXX
