#pragma once

#ifndef GRAPHICALSOLVER2D_TXX
#define GRAPHICALSOLVER2D_TXX


#include "graphicalsolver2d.hxx"

#include <cstdint>

#include <algorithm>
#include <list>
#include <utility>
#include <vector>

#include <QDebug>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "linearfunction.hxx"
#include "linearprogrammingutils.hxx"
#include "linearprogramsolution.hxx"
#include "plotdatareal2d.hxx"
#include "solutiontype.hxx"
#include "../math/numericlimits.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"


namespace LinearProgramming
{
  using namespace boost;
  using namespace DataConvertors;
  using namespace Eigen;
  using namespace LinearProgrammingUtils;
  using namespace std;
  using namespace Utils;


  template<typename T>
  GraphicalSolver2D<T>::GraphicalSolver2D(
    const LinearProgramData<T>& linearProgramData
  ) :
    _linearProgramData(linearProgramData)
  { }


  template<typename T>
  GraphicalSolver2D<T>::GraphicalSolver2D(
    LinearProgramData<T>&& linearProgramData
  ) :
    _linearProgramData(std::move(linearProgramData))
  { }


  template<typename T>
  void
  GraphicalSolver2D<T>::setLinearProgramData(
    const LinearProgramData<T>& linearProgramData
  )
  {
    _linearProgramData = linearProgramData;
  }


  template<typename T>
  void
  GraphicalSolver2D<T>::setLinearProgramData(
    LinearProgramData<T>&& linearProgramData
  )
  {
    _linearProgramData = std::move(linearProgramData);
  }


  /**
   * @brief GraphicalSolver2D::solve
   * Naïve algorithm for solving two-dimensional linear programs.
   * @return
   */
  template<typename T>
  pair<SolutionType, optional<PlotDataReal2D>>
  GraphicalSolver2D<T>::solve()
  {
    //Input linear program params
    //`M' is the constraints count
    const DenseIndex M(_linearProgramData.constraintsCount());
    //`N' is the decision variables count
    const DenseIndex N(_linearProgramData.variablesCount());

    //For output
    optional<PlotDataReal2D> ret;

    //Construct an augmented matrix A|b
    Matrix<T, Dynamic, Dynamic> A_b(M, N + 1);
    A_b <<
      _linearProgramData.constraintsCoeffs,
      _linearProgramData.constraintsRHS;

    //Construct an reduced row echelon forms of `A' and `A|b'
    const pair<Matrix<T, Dynamic, Dynamic>, DenseIndex> rref_A(
      reducedRowEchelonForm<T>(_linearProgramData.constraintsCoeffs)
    );
    const pair<Matrix<T, Dynamic, Dynamic>, DenseIndex> rref_A_b(
      reducedRowEchelonForm<T>(A_b)
    );

    LOG("A' :=\n{}\n(A|b)' :=\n{}", rref_A.first, rref_A_b.first);
    LOG("rank(A) == {}, rank(A|b) == {}", rref_A.second, rref_A_b.second);

    //For the system {Ax == b} to be consistent
    //rank(A) should be equal to rank(A|b)
    if (rref_A.second != rref_A_b.second || rref_A_b.second == 0)
    {
      qWarning() << "GraphicalSolver2D<T>::solve: inconsistent system:"
                    " rank(A) != rank(A|b) || rank(A) == 0";

      return make_pair(SolutionType::Infeasible, ret);
    }

    //Constraints count `M^' in the new reduced system {A^x == b^}
    //is equal to rank(A|b)
    const DenseIndex M_(rref_A_b.second);

    //In the new system, the decision variables count `N^'
    //is equal to N - M^
    const DenseIndex N_(N - M_);
    if (N_ != 2)
    {
      qWarning() << "GraphicalSolver2D<T>::solve: could not solve this"
                    " linear program: `N^' != 2";

      return make_pair(SolutionType::Unknown, ret);
    }

    //`F^' in the terms of the new basis
    Matrix<T, 1, Dynamic> c_(1, N_);
    for (DenseIndex j(0); j < N_; ++j)
    {
      T sum(0);
      for (DenseIndex i(0); i < M_; ++i)
      {
        sum += _linearProgramData.objectiveFunctionCoeffs(i) *
               rref_A_b.first(i, j + M_) *
               T(-1);
      }
      sum += _linearProgramData.objectiveFunctionCoeffs(j + M_);
      c_(j) = sum;
    }

    T d_(0);
    for (DenseIndex i(0); i < M_; ++i)
    {
      d_ += _linearProgramData.objectiveFunctionCoeffs(i) *
            rref_A_b.first(i, rref_A_b.first.cols() - 1) *
            T(-1);
    }
    const LinearFunction<T, 2> F_(c_, d_ * T(-1));

    LOG("F^ := {} + {}", F_.coeffs(), F_.constTerm());

    //Construct a new system of linear equations {A^x == b^}
    //w/ non-negativity constraints {-x1 <= 0; -x2 <= 0} added.
    Matrix<T, Dynamic, Dynamic> A_(M_ + 2, N_);
    for (DenseIndex i(0); i < M_; ++i)
    {
      for (DenseIndex j(0); j < N_; ++j)
      {
        A_(i, j) = rref_A_b.first(i, j + M_);
      }
    }
    for (DenseIndex i(M_); i < M_ + 2; ++i)
    {
      for (DenseIndex j(0); j < N_; ++j)
      {
        if (j != i - M_)
        {
          A_(i, j) = T(0);
        }
        else
        {
          A_(i, j) = T(-1);
        }
      }
    }

    LOG("A^ :=\n{}", A_);

    Matrix<T, Dynamic, 1> b_(M_ + 2, 1);
    for (DenseIndex i(0); i < M_; ++i)
    {
      b_(i) = rref_A_b.first(i, N);
    }
    for (DenseIndex i(M_); i < M_ + 2; ++i)
    {
      b_(i) = T(0);
    }

    LOG("b^ :=\n{}", b_);

    //In two-dimensional case we have two decision variables.
    //So, we need to generate 2-combinations from all the M + 2 equations
    //and solve that 2x2 systems of linear equations to find intersection points,
    //that _can_ be a feasible region vertices.
    //There are Binomial[M + 2, 2] possible intersection points total.
    //NOTE: in general case we have N decision variables, and M equations,
    //so we need to check Binomial[M + N, N] intersection points for feasibility.

    vector<uint16_t> intersectionCounters(M_ + 2, 0);
    list<Matrix<T, 2, 1>> extremePoints;
    T extremeValue(NumericLimits::max<T>());
    list<Matrix<T, 2, 1>> feasibleRegionExtremePoints;

    //For each constraint equation
    for (DenseIndex r(0); r < (M_ + 2) - 1; ++r)
    {
      //For each other constraint equation
      for (DenseIndex s(r + 1); s < (M_ + 2); ++s)
      {
        //Take two rows and join them in new matrix...
        Matrix<T, 2, 2> C(2, 2);
        C <<
          A_.row(r),
          A_.row(s);
        //...and take also the corresponding RHS elements
        Matrix<T, 2, 1> d(2, 1);
        d <<
          b_(r),
          b_(s);

//        LOG("r == {}, s == {}\nC ==\n{},\nd ==\n{}", r, s, C, d);

        //Obtain basic solution `y': solve matrix equation {C * y == d}
        //& check solution correctness
        const optional<Matrix<T, 2, 1>> y(findIntersection<T>(C, d));
//        T err((eqs * sol - rhs).norm() / rhs.norm());
        if (y)
        {
          const Matrix<T, 2, 1> y_(*y);

          LOG("y ==\n{}", y_);

          const bool isValid((C * y_).isApprox(d));
          if (isValid)
          {
            //Check if Basic Solution `y' is also a Basic Feasible Solution:
            //(the point `y' should lie inside of the feasible region)
            //The found point is point of intersection of two lines
            //(feasible region edges) and it is the feasible region
            //corner point (vertex) <=>
            //it satisfies the following matrix equation `A^ * y^ <= b^'
            //NOTE: non-vertices (inner points) are also subj. to this eq.

            const bool isFeasible(isSolutionFeasible<T>(y_, A_, b_));

            LOG("isFeasible == {}", isFeasible);

            //If this is a feasible point and it is valid (non-degenerate)
            //solution, we can now try to compute objective function value
            //and update `extremeValue' to the new extreme value
            //(which should be less than current)
            if (isFeasible)
            {
              ++intersectionCounters[r];
              ++intersectionCounters[s];

              feasibleRegionExtremePoints.push_back(y_);

              const T newValue(F_(y_));

              if (isLessThan<T>(newValue, extremeValue))
              {
                LOG("<: newPoint == {}, newValue == {}", y_, newValue);

                extremeValue = newValue;
                extremePoints.clear();
                extremePoints.push_back(y_);
              }
              else
              {
                if (isEqual<T>(newValue, extremeValue))
                {
                  LOG("~: newPoint == {}, newValue == {}", y_, newValue);

                  extremePoints.push_back(y_);
                }
              }
            }
          }
        }
      }
    }

    LOG("intersectionCounters == {}", makeString(intersectionCounters));

    if (feasibleRegionExtremePoints.empty())
    {
      return make_pair(SolutionType::Infeasible, ret);
    }
    else
    {
      if (
        all_of(
          intersectionCounters.cbegin(),
          intersectionCounters.cend(),
          [](uint16_t i) { return (i != 1); }
        )
      )
      {
        PlotDataReal2D plotData(
          list<Matrix<Real, 2, 1>>(),
          0.,
          list<Matrix<Real, 2, 1>>(),
          Matrix<Real, 1, Dynamic>(1, 2),
          Matrix<Real, 2, 2>(2, 2),
          Matrix<Real, 2, 2>(2, 2)
        );

        plotData.extremePoints.resize(extremePoints.size());

        transform(
          extremePoints.cbegin(),
          extremePoints.cend(),
          plotData.extremePoints.begin(),
          [](const Matrix<T, 2, 1>& point)
          {
            Matrix<Real, 2, 1> realPoint(2, 1);

            realPoint <<
              numericCast<Real, T>(point.x()),
              numericCast<Real, T>(point.y());

            return realPoint;
          }
        );

        plotData.extremeValue = numericCast<Real, T>(extremeValue);

        plotData.feasibleRegionExtremePoints.resize(
          feasibleRegionExtremePoints.size()
        );

        transform(
          feasibleRegionExtremePoints.cbegin(),
          feasibleRegionExtremePoints.cend(),
          plotData.feasibleRegionExtremePoints.begin(),
          [](const Matrix<T, 2, 1>& point)
          {
            Matrix<Real, 2, 1> realPoint(2, 1);

            realPoint <<
              numericCast<Real, T>(point.x()),
              numericCast<Real, T>(point.y());

            return realPoint;
          }
        );

        //To plot a polygon we need to sort all its vertices in clockwise order.
        sortPointsByPolarAngle(plotData.feasibleRegionExtremePoints);

//        for (DenseIndex i(0); i < M; ++i)
//        {
//          plotData.gradientVector(i) =
//            rref_A_b.first(i, N) -
//              plotData.extremePoints.front().dot(rref_A_b.first.block(i, M_, 1, 2));
//        }

        for (DenseIndex i(0); i < 2; ++i)
        {
          plotData.gradientVector(i) = numericCast<Real, T>(F_.coeffAt(i));
        }

        const Matrix<T, 2, 2> boundingBox(
          computeBoundingBox(feasibleRegionExtremePoints)
        );

        plotData.feasibleRegionBoundingBox <<
          numericCast<Real, T>(boundingBox(0, 0)),
          numericCast<Real, T>(boundingBox(0, 1)),
          numericCast<Real, T>(boundingBox(1, 0)),
          numericCast<Real, T>(boundingBox(1, 1));

        plotData.feasibleRegionBoundingBoxHeights <<
          numericCast<Real, T>(F_(boundingBox.row(1).transpose())),
          numericCast<Real, T>(F_(boundingBox.col(1))),
          numericCast<Real, T>(F_(boundingBox.col(0))),
          numericCast<Real, T>(F_(boundingBox.row(0).transpose().reverse()));

        LOG(
          "\n{}\n{}",
          plotData.feasibleRegionBoundingBox,
          plotData.feasibleRegionBoundingBoxHeights
        );

//        LOG(
//          "Solution: V == {}, G == {}, x* == {}, F* == {}",
//          plotData2D.feasibleRegionExtremePoints,
//          plotData2D.gradientDirection,
//          plotData2D.extremePoints,
//          plotData2D.extremeValue
//        );

        ret = plotData;

        return make_pair(SolutionType::Optimal, ret);
      }
      else
      {
        return make_pair(SolutionType::Unbounded, ret);
      }
    }
  }
}


#endif // GRAPHICALSOLVER2D_TXX
