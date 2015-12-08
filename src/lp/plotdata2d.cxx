#include "plotdata2d.hxx"

#include <list>
#include <utility>

#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using namespace NumericTypes;
  using namespace std;


  PlotData2D::PlotData2D(
    const pair<Real, Real>& extremePoint,
    Real extremeValue,
    const list<pair<Real, Real>>& extremePoints,
    const pair<Real, Real>& gradientDirection
  ) :
    extremePoint(extremePoint),
    extremeValue(extremeValue),
    feasibleRegionExtremePoints(extremePoints),
    gradientDirection(gradientDirection)
  { }


  PlotData2D::PlotData2D(
    pair<Real, Real>&& extremeVertex,
    Real extremeValue,
    list<pair<Real, Real>>&& extremePoints,
    pair<Real, Real>&& gradientDirection
  ) :
    extremePoint(extremePoint),
    extremeValue(extremeValue),
    feasibleRegionExtremePoints(std::move(extremePoints)),
    gradientDirection(gradientDirection)
  { }
}
