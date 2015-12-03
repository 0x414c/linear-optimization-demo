#pragma once

#ifndef GRAPHICALSOLVER2D_TXX
#define GRAPHICALSOLVER2D_TXX


#include "graphicalsolver2d.hxx"

#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include <QDebug>
#include <QLineF>
#include <QPointF>
#include <QVector>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "linearfunction.hxx"
#include "linearprogrammingutils.hxx"
#include "linearprogramsolution.hxx"
#include "plotdata2d.hxx"
#include "solutiontype.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"


namespace LinearProgramming
{
  using namespace boost;
  using namespace DataConvertors;
  using namespace Eigen;
  using namespace LinearProgrammingUtils;
  using namespace std;


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
  pair<SolutionType, optional<PlotData2D>>
  GraphicalSolver2D<T>::solve()
  {
    //Input linear program params
    //`M' is the constraints count
    const DenseIndex M(_linearProgramData.constraintsCount());
    //`N' is the decision variables count
    const DenseIndex N(_linearProgramData.variablesCount());

    //For output
    optional<PlotData2D> ret;

    PlotData2D plotData2D(
      QPointF(
        numeric_limits<qreal>::min(),
        numeric_limits<qreal>::min()
      ),
      numeric_limits<qreal>::max(),
      QVector<QPointF>(0),
      QLineF(
        QPointF(0, 0),
        QPointF(0, 0)
      )
    );

    //Make augmented matrix A|b
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
    if (rref_A.second != rref_A_b.second || rref_A.second == 0)
    {
      qWarning() << "GraphicalSolver2D<T>::solve: inconsistent system:"
                    " rank(A) != rank(A|b) || rank(A) == 0";

      return make_pair(SolutionType::Inconsistent, ret);
    }

    //Constraints count `M^' in the new reduced system {A^x == b^}
    //is equal to rank(A|b)
    const DenseIndex M_(rref_A_b.second);

    //In the new reduced basis, the decision variables count `N^'
    //is equal to (N - rank(A|b))
    const DenseIndex N_(N - M_);
    if (N_ != 2)
    {
      qWarning() << "GraphicalSolver2D<T>::solve: could not solve this"
                    " linear program: `N^' != 2";

      return make_pair(SolutionType::Unknown, ret);
    }

    //`F' in the terms of the reduced basis
    Matrix<T, 1, Dynamic> c_(1, N_);

    for (DenseIndex j(0); j < N_; ++j)
    {
      T sum(0);

      for (DenseIndex i(0); i < M_; ++i)
      {
//        LOG(
//          "sum += c[{}] * A'[{}, {}] <=> {} * {} <=> {}",
//          i, i, j + M_,
//          _linearProgramData.objectiveFunctionCoeffs(i),
//          rref_A_b.first(i, j + M_) * T(-1),
//          _linearProgramData.objectiveFunctionCoeffs(i) *
//          rref_A_b.first(i, j + M_) *
//          T(-1)
//        );

        sum += _linearProgramData.objectiveFunctionCoeffs(i) *
               rref_A_b.first(i, j + M_) *
               T(-1);

//        LOG("sum == {}", sum);
      }

//      LOG(
//        "sum += c[{}] <=> {}",
//        j + M_,
//        _linearProgramData.objectiveFunctionCoeffs(j + M_)
//      );

      sum += _linearProgramData.objectiveFunctionCoeffs(j + M_);

//      LOG("sum == {}", sum);

      c_(j) = sum;
    }

//    LOG("c^ := {}", c_);

    T d_(0);
    for (DenseIndex i(0); i < M_; ++i)
    {
      d_ += _linearProgramData.objectiveFunctionCoeffs(i) *
            rref_A_b.first(i, rref_A_b.first.cols() - 1) *
            T(-1);
    }

    LOG("c_ := {}\nd_ := {}", c_, d_);

    const LinearFunction<T, 2> F_(c_, d_ * T(-1));

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

    LOG("{}", A_);

    Matrix<T, Dynamic, 1> b_(M_ + 2, 1);

    for (DenseIndex i(0); i < M_; ++i)
    {
      b_(i) = rref_A_b.first(i, N);
    }

    for (DenseIndex i(M_); i < M_ + 2; ++i)
    {
      b_(i) = T(0);
    }

    LOG("{}", b_);

    //In two-dimensional case we have two decision variables.
    //So, we need to generate 2-combinations from all the M + 2 equations
    //and solve that 2x2 systems of linear equations to find intersection points,
    //that _can_ be a feasible region vertices.
    //There are Binomial[M + 2, 2] possible intersection points total.
    //NOTE: in general case we have N decision variables, and M equations,
    //so we need to check Binomial[M + N, N] intersection points for feasibility.

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

//        LOG("r := {}, s := {}\nC :=\n{},\nd :=\n{}", r, s, C, d);

        //Obtain basic solution `y':
        //Try to solve matrix equation {C * y == d}
        //& check solution correctness
        const optional<Matrix<T, 2, 1>> y(findIntersection<T>(C, d));

//        T err((eqs * sol - rhs).norm() / rhs.norm());

        if (y)
        {
//          LOG("y ==\n{}", (*y));

          const bool isValid((C * (*y)).isApprox(d));
          if (isValid)
          {
            //Check if Basic Solution `y' is also a Basic Feasible Solution:
            //(the point `y' should lie inside of the feasible region)
            //The found point is point of intersection of two lines
            //(feasible region edges) and it is the feasible region
            //corner point (vertex) <=>
            //it satisfies the following matrix equation `A^ * y <= b^'
            //NOTE: non-vertices (inner points) are also subj. to this eq.

            const bool isFeasible(isSolutionFeasible<T>((*y), A_, b_));

//            LOG("isFeasible := {}", isFeasible);

            //If this is a feasible point and it is valid (non-degenerate)
            //solution, we can now try to compute objective function value
            //and update `extremeValue' to the new extreme value
            //(which should be less than current)
            if (isFeasible)
            {
              const T y_1((*y)(0)), y_2((*y)(1)), F__(F_(*y));

              QPointF newVertice(
                numericCast<qreal, T>(y_1),
                numericCast<qreal, T>(y_2)
              );
              plotData2D.vertices.append(newVertice);

              qreal newValue(numericCast<qreal, T>(F__));

              qInfo() << "GraphicalSolver2D<T>::solve: value for" <<
                         newVertice << "is" << newValue << "~=" << F__;

              if (newValue < plotData2D.extremeValue)
              {
                qInfo() << "GraphicalSolver2D<T>::solve:"
                           " new minimum found for" << newVertice << "is" <<
                           newValue << "~=(" << y_1 << "," << y_2 << ")";

                plotData2D.extremeValue = newValue;
                plotData2D.extremeVertex = newVertice;
              }
            }
          }
        }
      }
    }

    //To plot a polygon we should sort all the found vertices in clockwise order.
    sortPointsClockwise(plotData2D.vertices);

    plotData2D.gradient.setP2(
      QPointF(
        numericCast<qreal, T>(F_.coeffAt(0)),
        numericCast<qreal, T>(F_.coeffAt(1))
      )
    );

    qDebug() << "Solution: \nV:=" <<
                plotData2D.vertices << "\nG:=" << plotData2D.gradient <<
                "\nX*:=" << plotData2D.extremeVertex <<
                "\nF*:=" << plotData2D.extremeValue;

    ret = plotData2D;

    return make_pair(SolutionType::Optimal, ret);
  }
}


#endif // GRAPHICALSOLVER2D_TXX
