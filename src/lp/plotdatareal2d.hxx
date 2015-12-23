#pragma once

#ifndef PLOTDATAREAL2D_HXX
#define PLOTDATAREAL2D_HXX


#include <list>
#include <utility>
#include <vector>

#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using NumericTypes::Real;
  using std::list;
  using std::vector;


//  template<typename TCoeff = Real, DenseIndex TDim = 2>
  struct PlotDataReal2D
  {
    PlotDataReal2D() = default;

    PlotDataReal2D(
      const list<Matrix<Real, 2, 1>>& extremePoints,
      Real extremeValue,
      const list<Matrix<Real, 2, 1>>& feasibleRegionExtremePoints,
      const Matrix<Real, 1, Dynamic>& gradientVector,
      const Matrix<Real, 2, 2>& feasibleRegionBoundingBox,
      const Matrix<Real, 2, 2>& feasibleRegionBoundingBoxHeights,
      const vector<DenseIndex>& variablesNames
    );
    PlotDataReal2D(
      list<Matrix<Real, 2, 1>>&& extremePoints,
      Real extremeValue,
      list<Matrix<Real, 2, 1>>&& feasibleRegionExtremePoints,
      Matrix<Real, 1, Dynamic>&& gradientVector,
      Matrix<Real, 2, 2>&& feasibleRegionBoundingBox,
      Matrix<Real, 2, 2>&& feasibleRegionBoundingBoxHeights,
      vector<DenseIndex>&& variablesNames
    );


    list<Matrix<Real, 2, 1>> extremePoints;

    Real extremeValue;

    list<Matrix<Real, 2, 1>> feasibleRegionExtremePoints;

    Matrix<Real, 1, Dynamic> gradientVector;

    Matrix<Real, 2, 2> feasibleRegionBoundingBox;

    Matrix<Real, 2, 2> feasibleRegionBoundingBoxHeights;

    vector<DenseIndex> variablesNames;
  };
}


#endif // PLOTDATAREAL2D_HXX
