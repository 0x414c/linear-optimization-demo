#ifndef PLOTDATA2D_TXX
#define PLOTDATA2D_TXX


#include "plotdata2d.hxx"

#include <list>
#include <utility>
#include <vector>

#include "eigen3/Eigen/Core"

#include "linearprogramsolution.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using Eigen::DenseIndex;
  using Eigen::Matrix;
  using NumericTypes::real_t;
  using std::list;
  using std::vector;


  template<typename TCoeff/*, DenseIndex TDim*/>
  PlotData2D<TCoeff>::PlotData2D(
    const LinearProgramSolution<TCoeff>& linearProgramSolution,
    const list<Matrix<real_t, 2, 1>>& extremePoints,
    real_t extremeValue,
    const list<Matrix<real_t, 2, 1>>& feasibleRegionExtremePoints,
    const Matrix<real_t, 2, 2>& feasibleRegionBoundingBox,
    const Matrix<real_t, 2, 2>& feasibleRegionBoundingBoxHeights,
    const Matrix<real_t, 1, 2>& gradientVector,
    const vector<DenseIndex>& decisionVariables
  ) :
    linearProgramSolution(linearProgramSolution),
    extremePoints(extremePoints),
    extremeValue(extremeValue),
    feasibleRegionExtremePoints(feasibleRegionExtremePoints),
    feasibleRegionBoundingBox(feasibleRegionBoundingBox),
    feasibleRegionBoundingBoxHeights(feasibleRegionBoundingBoxHeights),
    gradientVector(gradientVector),
    decisionVariables(decisionVariables)
  { }


  template<typename TCoeff/*, DenseIndex TDim*/>
  PlotData2D<TCoeff>::PlotData2D(
    LinearProgramSolution<TCoeff>&& linearProgramSolution,
    list<Matrix<real_t, 2, 1>>&& extremePoints,
    real_t extremeValue,
    list<Matrix<real_t, 2, 1>>&& feasibleRegionExtremePoints,
    Matrix<real_t, 2, 2>&& feasibleRegionBoundingBox,
    Matrix<real_t, 2, 2>&& feasibleRegionBoundingBoxHeights,
    Matrix<real_t, 1, 2>&& gradientVector,
    vector<DenseIndex>&& decisionVariables
  ) :
    linearProgramSolution(std::move(linearProgramSolution)),
    extremePoints(std::move(extremePoints)),
    extremeValue(extremeValue),
    feasibleRegionExtremePoints(std::move(feasibleRegionExtremePoints)),
    feasibleRegionBoundingBox(std::move(feasibleRegionBoundingBox)),
    feasibleRegionBoundingBoxHeights(
      std::move(feasibleRegionBoundingBoxHeights)
    ),
    gradientVector(std::move(gradientVector)),
    decisionVariables(std::move(decisionVariables))
  { }
}


#endif // PLOTDATA2D_TXX
