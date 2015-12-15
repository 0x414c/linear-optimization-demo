#pragma once

#ifndef PLOTDATAREAL2D_HXX
#define PLOTDATAREAL2D_HXX


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


  struct PlotDataReal2D
  {
    PlotDataReal2D() = default;

    PlotDataReal2D(
      const list<Matrix<Real, 2, 1>>& extremePoints,
      Real extremeValue,
      const list<Matrix<Real, 2, 1>>& feasibleRegionExtremePoints,
      const Matrix<Real, 1, Dynamic>& gradientVector,
      const Matrix<Real, 2, 2>& feasibleRegionBoundingBox,
      const Matrix<Real, 2, 2>& feasibleRegionBoundingBoxHeights
    );
    PlotDataReal2D(
      list<Matrix<Real, 2, 1>>&& extremePoints,
      Real extremeValue,
      list<Matrix<Real, 2, 1>>&& feasibleRegionExtremePoints,
      Matrix<Real, 1, Dynamic>&& gradientVector,
      Matrix<Real, 2, 2>&& feasibleRegionBoundingBox,
      Matrix<Real, 2, 2>&& feasibleRegionBoundingBoxHeights
    );


    list<Matrix<Real, 2, 1>> extremePoints;

    Real extremeValue;

    list<Matrix<Real, 2, 1>> feasibleRegionExtremePoints;

    Matrix<Real, 1, Dynamic> gradientVector;

    Matrix<Real, 2, 2> feasibleRegionBoundingBox;

    Matrix<Real, 2, 2> feasibleRegionBoundingBoxHeights;
  };
}


#endif // PLOTDATAREAL2D_HXX
