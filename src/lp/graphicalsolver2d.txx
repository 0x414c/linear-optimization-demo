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
#include "plotdata2d.hxx"
#include "solutiontype.hxx"
#include "../math/numericlimits.hxx"
#include "../math/mathutils.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"


namespace LinearProgramming
{
  using boost::optional;
  using DataConvertors::numericCast;
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using LinearProgrammingUtils::reducedRowEchelonForm;
  using LinearProgrammingUtils::RREF;
  using LinearProgrammingUtils::findIntersection;
  using LinearProgrammingUtils::isSolutionFeasible;
  using MathUtils::isEqual;
  using MathUtils::isLessThan;
  using LinearProgrammingUtils::sortPointsByPolarAngle;
  using LinearProgrammingUtils::computeBoundingBox;
  using NumericTypes::real_t;
  using std::all_of;
  using std::transform;
  using std::make_pair;
  using std::pair;
  using std::list;
  using std::vector;
  using Utils::makeString;


  template<typename TCoeff/*, DenseIndex TDim = 2*/>
  GraphicalSolver2D<TCoeff>::GraphicalSolver2D(
    const LinearProgramData<TCoeff>& linearProgramData
  ) :
    linearProgramData_(linearProgramData)
  { }


  template<typename TCoeff/*, DenseIndex TDim = 2*/>
  GraphicalSolver2D<TCoeff>::GraphicalSolver2D(
    LinearProgramData<TCoeff>&& linearProgramData
  ) :
    linearProgramData_(std::move(linearProgramData))
  { }


  template<typename TCoeff/*, DenseIndex TDim = 2*/>
  void
  GraphicalSolver2D<TCoeff>::setLinearProgramData(
    const LinearProgramData<TCoeff>& linearProgramData
  )
  {
    linearProgramData_ = linearProgramData;
  }


  template<typename TCoeff/*, DenseIndex TDim = 2*/>
  void
  GraphicalSolver2D<TCoeff>::setLinearProgramData(
    LinearProgramData<TCoeff>&& linearProgramData
  )
  {
    linearProgramData_ = move(linearProgramData);
  }


  /**
   * @brief GraphicalSolver2D::solve
   * Naïve algorithm for solving two-dimensional linear programs.
   * TODO: Handle three-dimensional case (use `TDim' parameter)
   * @return
   */
  template<typename TCoeff/*, DenseIndex TDim = 2*/>
  pair<SolutionType, optional<PlotData2D<TCoeff>>>
  GraphicalSolver2D<TCoeff>::solve()
  {
    //Input linear program params
    //`M' is the constraints count
    const DenseIndex M(linearProgramData_.constraintsCount());
    //`N' is the decision variables count
    const DenseIndex N(linearProgramData_.variablesCount());

    //For output
    optional<PlotData2D<TCoeff>> ret;

/**************************************************************************/

    //Construct an augmented matrix A|b
    Matrix<TCoeff, Dynamic, Dynamic> A_b(M, N + 1);
    A_b <<
      linearProgramData_.constraintsCoeffs,
      linearProgramData_.constraintsRHS;

    //Construct reduced row echelon forms of `A' and `A|b'
    const RREF<TCoeff> rref_A(
      reducedRowEchelonForm<TCoeff>(linearProgramData_.constraintsCoeffs)
    );

    const RREF<TCoeff> rref_A_b(reducedRowEchelonForm<TCoeff>(A_b));

    LOG("A^ ==\n{}\n(A|b)^ ==\n{}", rref_A.rref, rref_A_b.rref);
    LOG("rank(A^) == {}, rank((A|b)^) == {}", rref_A.rank, rref_A_b.rank);

    //Perform preliminary checks
    //For the system {Ax == b} to be consistent
    //rank(A^) should be equal to rank((A|b)^)
    if (rref_A.rank != rref_A_b.rank)
    {
      qWarning() << "GraphicalSolver2D<T>::solve: inconsistent system:"
                    " rank(A^) != rank((A|b)^)";

      return make_pair(SolutionType::Infeasible, ret);
    }

    //If (rank((A|b)^) == 0), all values of `x' are solutions
    if(rref_A_b.rank == 0)
    {
      qWarning() << "GraphicalSolver2D<T>::solve: empty system:"
                    " rank((A|b)^) == 0";

      return make_pair(SolutionType::Unbounded, ret);
    }

/**************************************************************************/

    //Constraints count `M^' in the new reduced system {A^x == b^}
    //is equal to rank((A|b)^)
    const DenseIndex M_(rref_A_b.rank);

    //In the new system, the decision variables count `N^'
    //is equal to (N - M^)
    const DenseIndex N_(N - M_);
    if (N_ != 2)
    {
      qWarning() << "GraphicalSolver2D<T>::solve: could not solve this"
                    " linear program: `N^' != 2";

      return make_pair(SolutionType::Unknown, ret);
    }

/**************************************************************************/

    //Function `F^' in the terms of the new basis
    //Fill vector `c^'...
    Matrix<TCoeff, 1, Dynamic> c_(1, N_);
    for (DenseIndex j(0); j < N_; ++j)
    {
      TCoeff sum(0);
      for (DenseIndex i(0); i < M_; ++i)
      {
        sum += linearProgramData_.objectiveFunctionCoeffs(i) *
               rref_A_b.rref(i, j + M_) *
               TCoeff(-1);
      }
      sum += linearProgramData_.objectiveFunctionCoeffs(j + M_);
      c_(j) = sum;
    }

    //...and compute constant term `d^'
    TCoeff d_(0);
    for (DenseIndex i(0); i < M_; ++i)
    {
      d_ += linearProgramData_.objectiveFunctionCoeffs(i) *
            rref_A_b.rref(i, rref_A_b.rref.cols() - 1);
    }

    const LinearFunction<TCoeff, 2> F_(c_, d_);

    LOG("F^ == ({}) + ({})", F_.coeffs(), F_.constTerm());

/**************************************************************************/

    //Construct a new system of linear equations {A^x == b^}
    //w/ non-negativity constraints {-x1 <= 0; -x2 <= 0} added.
    //Drop the basic minor and append new constraints
    Matrix<TCoeff, Dynamic, Dynamic> A_(M_ + 2, N_);
    for (DenseIndex i(0); i < M_; ++i)
    {
      for (DenseIndex j(0); j < N_; ++j)
      {
        A_(i, j) = rref_A_b.rref(i, j + M_);
      }
    }

    for (DenseIndex i(M_); i < M_ + 2; ++i)
    {
      for (DenseIndex j(0); j < N_; ++j)
      {
        if (j != i - M_)
        {
          A_(i, j) = TCoeff(0);
        }
        else
        {
          A_(i, j) = TCoeff(-1);
        }
      }
    }

    LOG("A^ ==\n{}", A_);

    Matrix<TCoeff, Dynamic, 1> b_(M_ + 2, 1);
    for (DenseIndex i(0); i < M_; ++i)
    {
      b_(i) = rref_A_b.rref(i, N);
    }
    for (DenseIndex i(M_); i < M_ + 2; ++i)
    {
      b_(i) = TCoeff(0);
    }

    LOG("b^ ==\n{}", b_);

/**************************************************************************/

    //In two-dimensional case we have two decision variables.
    //So, we need to generate 2-combinations from all the M + 2 equations
    //and solve that 2x2 systems of linear equations to find intersection points,
    //that _can_ be a feasible region vertices.
    //There are Binomial[M + 2, 2] possible intersection points total.
    //NOTE: in general case we have N decision variables, and M equations,
    //so we need to check Binomial[M + N, N] intersection points for feasibility.

    list<Matrix<TCoeff, 2, 1>> extremePoints;
    TCoeff extremeValue(NumericLimits::max<TCoeff>());
    list<Matrix<TCoeff, 2, 1>> feasibleRegionExtremePoints;

    vector<uint16_t> intersectionCounters(M_ + 2, 0);

    //For each constraint equation
    //TODO: ~! Use `std::next_permutation'
    for (DenseIndex r(0); r < (M_ + 2) - 1; ++r)
    {
      //For each other constraint equation
      for (DenseIndex s(r + 1); s < (M_ + 2); ++s)
      {
        //Take two rows and join them in new matrix...
        Matrix<TCoeff, 2, 2> C(2, 2);
        C <<
          A_.row(r),
          A_.row(s);
        //...and take also the corresponding RHS elements
        Matrix<TCoeff, 2, 1> d(2, 1);
        d <<
          b_(r),
          b_(s);

//        LOG("r == {}, s == {}\nC ==\n{},\nd ==\n{}", r, s, C, d);

        //Obtain basic solution `y': solve matrix equation {Cy == d}
        //& check solution correctness
        const optional<Matrix<TCoeff, 2, 1>> y(
//          findIntersection<TCoeff/*, TDim*/>(C, d) //TODO: ~!
          findIntersection<TCoeff, 2>(C, d)
        );

        if (y)
        {
          const Matrix<TCoeff, 2, 1> y_(*y);

//          LOG("y ==\n{}", y_);

//          TCoeff err((eqs * sol - rhs).norm() / rhs.norm());
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

            const bool isFeasible(isSolutionFeasible<TCoeff>(y_, A_, b_));

//            LOG("isFeasible == {}", isFeasible);

            //If this is a feasible point and it is valid (non-degenerate)
            //solution, we can now try to compute objective function value
            //and update `extremeValue' to the new extreme value
            //(which should be less than current)
            if (isFeasible)
            {
              ++intersectionCounters[r];
              ++intersectionCounters[s];

              feasibleRegionExtremePoints.push_back(y_);

              const TCoeff newValue(F_(y_));

              if (isLessThan<TCoeff>(newValue, extremeValue))
              {
//                LOG("<: newPoint == {}, newValue == {}", y_, newValue);

                extremeValue = newValue;
                extremePoints.clear();
                extremePoints.push_back(y_);
              }
              else
              {
                if (isEqual<TCoeff>(newValue, extremeValue))
                {
//                  LOG("~: newPoint == {}, newValue == {}", y_, newValue);

                  extremePoints.push_back(y_);
                }
              }
            }
          }
        }
      }
    }

    LOG("intersectionCounters == {}", makeString(intersectionCounters));

/**************************************************************************/

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
        PlotData2D<TCoeff> plotData(
          LinearProgramSolution<TCoeff>(),
          list<Matrix<real_t, 2, 1>>(),
          real_t(0),
          list<Matrix<real_t, 2, 1>>(),
          Matrix<real_t, 2, 2>(2, 2),
          Matrix<real_t, 2, 2>(2, 2),
          Matrix<real_t, 1, 2>(1, 2),
          vector<DenseIndex>(N_)
        );

        plotData.extremePoints.resize(extremePoints.size());

        transform(
          extremePoints.cbegin(),
          extremePoints.cend(),
          plotData.extremePoints.begin(),
          [](const Matrix<TCoeff, 2, 1>& point)
          {
            Matrix<real_t, 2, 1> realPoint(2, 1);

            realPoint <<
              numericCast<real_t, TCoeff>(point.x()),
              numericCast<real_t, TCoeff>(point.y());

            return realPoint;
          }
        );

        plotData.extremeValue = numericCast<real_t, TCoeff>(extremeValue);

        plotData.feasibleRegionExtremePoints.resize(
          feasibleRegionExtremePoints.size()
        );

        transform(
          feasibleRegionExtremePoints.cbegin(),
          feasibleRegionExtremePoints.cend(),
          plotData.feasibleRegionExtremePoints.begin(),
          [](const Matrix<TCoeff, 2, 1>& point)
          {
            Matrix<real_t, 2, 1> realPoint(2, 1);

            realPoint <<
              numericCast<real_t, TCoeff>(point.x()),
              numericCast<real_t, TCoeff>(point.y());

            return realPoint;
          }
        );

        //To plot a polygon we need to sort all its vertices in clockwise order.
        sortPointsByPolarAngle(plotData.feasibleRegionExtremePoints);

        const Matrix<TCoeff, 2, 2> boundingBox(
          computeBoundingBox(feasibleRegionExtremePoints)
        );

        plotData.feasibleRegionBoundingBox <<
          numericCast<real_t, TCoeff>(boundingBox(0, 0)),
          numericCast<real_t, TCoeff>(boundingBox(0, 1)),
          numericCast<real_t, TCoeff>(boundingBox(1, 0)),
          numericCast<real_t, TCoeff>(boundingBox(1, 1));

        plotData.feasibleRegionBoundingBoxHeights <<
          numericCast<real_t, TCoeff>(F_(boundingBox.row(1).transpose())),
          numericCast<real_t, TCoeff>(F_(boundingBox.col(1))),
          numericCast<real_t, TCoeff>(F_(boundingBox.col(0))),
          numericCast<real_t, TCoeff>(
            F_(boundingBox.row(0).transpose().reverse())
          );

/**************************************************************************/

        for (DenseIndex i(0); i < 2; ++i)
        {
          plotData.gradientVector(i) = numericCast<real_t, TCoeff>(F_.coeffAt(i));
        }

        for (DenseIndex j(0); j < N_; ++j)
        {
          plotData.decisionVariables[j] = j + M_ + 1;
        }

/**************************************************************************/

        plotData.linearProgramSolution.extremePoint =
          Matrix<TCoeff, Dynamic, 1>(N, 1);

        const Matrix<TCoeff, 2, 1> extremePoint2D(extremePoints.front());

        for (DenseIndex i(0); i < M_; ++i)
        {
          TCoeff sum(0);

          for (DenseIndex j(0); j < N_; ++j)
          {
            sum += A_(i, j) * extremePoint2D(j);
          }

          sum = b_(i) - sum;

          plotData.linearProgramSolution.extremePoint(i) = sum;
        }

        for (DenseIndex j(M_); j < N; ++j)
        {
          plotData.linearProgramSolution.extremePoint(j) =
            extremePoint2D(j - M_);
        }

        plotData.linearProgramSolution.extremeValue = extremeValue;

        LOG(
          "Solution: x* ==\n{},\nF* == {}, U ==\n{},\n"
          "grad(F) == {}, [U] ==\n{},\n[U]* ==\n{},\nx == {},\n"
          "x* ==\n{},\nF* == {}",
          plotData.extremePoints,
          plotData.extremeValue,
          plotData.feasibleRegionExtremePoints,
          plotData.gradientVector,
          plotData.feasibleRegionBoundingBox,
          plotData.feasibleRegionBoundingBoxHeights,
          plotData.decisionVariables,
          plotData.linearProgramSolution.extremePoint,
          plotData.linearProgramSolution.extremeValue
        );

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
