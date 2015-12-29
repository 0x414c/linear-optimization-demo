#pragma once

#ifndef PLOTDATA2D_HXX
#define PLOTDATA2D_HXX


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
  using NumericTypes::Real;
  using std::list;
  using std::vector;


  template<typename TCoeff = Real/*, DenseIndex TDim = 2*/>
  struct PlotData2D
  {
    PlotData2D() = default;

    PlotData2D(
      const LinearProgramSolution<TCoeff>& linearProgramSolution,
      const list<Matrix<Real, 2, 1>>& extremePoints,
      Real extremeValue,
      const list<Matrix<Real, 2, 1>>& feasibleRegionExtremePoints,
      const Matrix<Real, 2, 2>& feasibleRegionBoundingBox,
      const Matrix<Real, 2, 2>& feasibleRegionBoundingBoxHeights,
      const Matrix<Real, 1, 2>& gradientVector,
      const vector<DenseIndex>& decisionVariables
    );
    PlotData2D(
      LinearProgramSolution<TCoeff>&& linearProgramSolution,
      list<Matrix<Real, 2, 1>>&& extremePoints,
      Real extremeValue,
      list<Matrix<Real, 2, 1>>&& feasibleRegionExtremePoints,
      Matrix<Real, 2, 2>&& feasibleRegionBoundingBox,
      Matrix<Real, 2, 2>&& feasibleRegionBoundingBoxHeights,
      Matrix<Real, 1, 2>&& gradientVector,
      vector<DenseIndex>&& decisionVariables
    );


    LinearProgramSolution<TCoeff> linearProgramSolution;

    list<Matrix<Real, 2, 1>> extremePoints;

    Real extremeValue;

    list<Matrix<Real, 2, 1>> feasibleRegionExtremePoints;

    Matrix<Real, 2, 2> feasibleRegionBoundingBox;

    Matrix<Real, 2, 2> feasibleRegionBoundingBoxHeights;

    Matrix<Real, 1, 2> gradientVector;

    vector<DenseIndex> decisionVariables;
  };
}


#include "plotdata2d.txx"


#endif // PLOTDATA2D_HXX
