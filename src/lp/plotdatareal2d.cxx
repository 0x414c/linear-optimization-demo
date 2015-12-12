#include "plotdatareal2d.hxx"

#include <list>
#include <utility>

#include "pointreal2d.hxx"
#include "pointvaluereal2d.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using namespace NumericTypes;
  using namespace std;


  PlotDataReal2D::PlotDataReal2D(
    const list<PointReal2D>& extremePoints,
    Real extremeValue,
    const list<PointReal2D>& feasibleRegionExtremePoints,
    const PointReal2D& gradientDirection,
    const PointReal2D& boundingBox
  ) :
    extremePoints(extremePoints),
    extremeValue(extremeValue),
    feasibleRegionExtremePoints(feasibleRegionExtremePoints),
    gradientVectorDirection(gradientDirection),
    boundingBox(boundingBox)
  { }


  PlotDataReal2D::PlotDataReal2D(
    list<PointReal2D>&& extremePoints,
    Real extremeValue,
    list<PointReal2D>&& feasibleRegionExtremePoints,
    PointReal2D&& gradientDirection,
    PointReal2D&& boundingBox
  ) :
    extremePoints(std::move(extremePoints)),
    extremeValue(extremeValue),
    feasibleRegionExtremePoints(std::move(feasibleRegionExtremePoints)),
    gradientVectorDirection(gradientDirection),
    boundingBox(boundingBox)
  { }
}
