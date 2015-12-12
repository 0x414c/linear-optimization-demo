#pragma once

#ifndef PLOTDATAREAL2D_HXX
#define PLOTDATAREAL2D_HXX


#include <list>
#include <utility>

#include "pointreal2d.hxx"
#include "pointvaluereal2d.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using namespace NumericTypes;
  using namespace std;


  struct PlotDataReal2D
  {
    PlotDataReal2D() = default;

    PlotDataReal2D(
      const list<PointReal2D>& extremePoints,
      Real extremeValue,
      const list<PointReal2D>& feasibleRegionExtremePoints,
      const PointReal2D& gradientVectorDirection,
      const PointReal2D& boundingBox
    );
    PlotDataReal2D(
      list<PointReal2D>&& extremePoints,
      Real extremeValue,
      list<PointReal2D>&& feasibleRegionExtremePoints,
      PointReal2D&& gradientVectorDirection,
      PointReal2D&& boundingBox
    );


    list<PointReal2D> extremePoints;

    Real extremeValue;

    list<PointReal2D> feasibleRegionExtremePoints;

    PointReal2D gradientVectorDirection;

    PointReal2D boundingBox;

    Real z11;
    Real z12;
    Real z22;
    Real z21;
  };
}


#endif // PLOTDATAREAL2D_HXX
