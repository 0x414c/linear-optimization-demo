#pragma once

#ifndef PLOTDATA2D_HXX
#define PLOTDATA2D_HXX


#include <list>
#include <utility>

#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using namespace NumericTypes;
  using namespace std;


  struct PlotData2D
  {
    PlotData2D() = default;

    PlotData2D(
      const pair<Real, Real>& extremePoint,
      Real extremeValue,
      const list<pair<Real, Real>>& feasibleRegionExtremePoints,
      const pair<Real, Real>& gradientDirection
    );
    PlotData2D(
      pair<Real, Real>&& extremePoint,
      Real extremeValue,
      list<pair<Real, Real>>&& feasibleRegionExtremePoints,
      pair<Real, Real>&& gradientDirection
    );


    pair<Real, Real> extremePoint;

    Real extremeValue;

    list<pair<Real, Real>> feasibleRegionExtremePoints;

    pair<Real, Real> gradientDirection;
  };
}


#endif // PLOTDATA2D_HXX
