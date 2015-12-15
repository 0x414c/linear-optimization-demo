#include "plotdatareal2d.hxx"

#include <list>
#include <utility>

#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using Eigen::Matrix;
  using Eigen::Dynamic;
  using NumericTypes::Real;
  using std::list;
  using std::move;


  PlotDataReal2D::PlotDataReal2D(
    const list<Matrix<Real, 2, 1>>& extremePoints,
    Real extremeValue,
    const list<Matrix<Real, 2, 1>>& feasibleRegionExtremePoints,
    const Matrix<Real, 1, Dynamic>& gradientVector,
    const Matrix<Real, 2, 2>& feasibleRegionBoundingBox,
    const Matrix<Real, 2, 2>& feasibleRegionBoundingBoxHeights
  ) :
    extremePoints(extremePoints),
    extremeValue(extremeValue),
    feasibleRegionExtremePoints(feasibleRegionExtremePoints),
    gradientVector(gradientVector),
    feasibleRegionBoundingBox(feasibleRegionBoundingBox),
    feasibleRegionBoundingBoxHeights(feasibleRegionBoundingBoxHeights)
  { }


  PlotDataReal2D::PlotDataReal2D(
    list<Matrix<Real, 2, 1>>&& extremePoints,
    Real extremeValue,
    list<Matrix<Real, 2, 1>>&& feasibleRegionExtremePoints,
    Matrix<Real, 1, Dynamic>&& gradientVector,
    Matrix<Real, 2, 2>&& feasibleRegionBoundingBox,
    Matrix<Real, 2, 2>&& feasibleRegionBoundingBoxHeights
  ) :
    extremePoints(move(extremePoints)),
    extremeValue(extremeValue),
    feasibleRegionExtremePoints(move(feasibleRegionExtremePoints)),
    gradientVector(move(gradientVector)),
    feasibleRegionBoundingBox(move(feasibleRegionBoundingBox)),
    feasibleRegionBoundingBoxHeights(move(feasibleRegionBoundingBoxHeights))
  { }
}
