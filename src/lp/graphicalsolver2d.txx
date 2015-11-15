#ifndef GRAPHICALSOLVER2D_TXX
#define GRAPHICALSOLVER2D_TXX

#include "graphicalsolver2d.hxx"

#include <iostream>
#include <limits>
#include <utility>

#include <QDebug>
#include <QLineF>
#include <QPointF>
#include <QVector>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "linearfunction.hxx"
#include "linearprogramutils.hxx"
#include "linearprogramsolution.hxx"
#include "plotdata2d.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"

namespace LinearProgramming
{
  using namespace boost;
  using namespace DataConvertors;
  using namespace Eigen;
  using namespace LinearProgramUtils;
  using namespace std;
  using namespace Utils;

  template<typename T>
  GraphicalSolver2D<T>::GraphicalSolver2D()
  { }

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
  void GraphicalSolver2D<T>::setLinearProgramData(
    const LinearProgramData<T>& linearProgramData
  )
  {
    _linearProgramData = linearProgramData;
  }

  template<typename T>
  void GraphicalSolver2D<T>::setLinearProgramData(
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
  optional<PlotData2D> GraphicalSolver2D<T>::solve() const
  {
    //Input data params
    const DenseIndex constrsCount(
      _linearProgramData.constraintsCoeffs.rows())
    ;
    const DenseIndex varsCount(
      _linearProgramData.constraintsCoeffs.cols()
    );

    //For further computations
    LinearFunction<T, Dynamic> objFunc(
      _linearProgramData.objectiveFuncCoeffs
    );

    //For output
    PlotData2D plotData2D(
      SolutionType::Unknown,
      QPointF(
        numeric_limits<qreal>::lowest(),
        numeric_limits<qreal>::lowest()
      ),
      numeric_limits<qreal>::max(),
      QVector<QPointF>(0),
      QLineF(
        QPointF(0, 0),
        QPointF(
          numericCast<qreal, T>(objFunc.coeffAt(0)),
          numericCast<qreal, T>(objFunc.coeffAt(1))
        )
      )
    );
    optional<PlotData2D> ret;

    //Find column-basis
    //Its column count (== free variables count) should be equal to 2 for 2D graphical method.
    Matrix<T, Dynamic, Dynamic> augm(constrsCount, varsCount + 1);
    augm <<
      _linearProgramData.constraintsCoeffs,
      _linearProgramData.constraintsRHS;

    //For the system `Ax == b' to be consistent rank(A) should be equal to rank(A⊔b)
    const int rankA(2), rankAb(2);
    qDebug() << "The rank of A is " << rankA << "\nThe rank of A⊔b is " << rankAb;
    if (rankA != rankAb || rankA == 0)
    {
      qDebug() << "GraphicalSolver2D<T>::solve: inconsistent system";
      plotData2D.resultType = SolutionType::Inconsistent;

      return ret;
    }

    const int nullityA(2);
    if (nullityA != 2)
    {
      qDebug() << "GraphicalSolver2D<T>::solve: cannot solve this system by using 2D solver";
      plotData2D.resultType = SolutionType::Unknown;

      return ret;
    }

    //Construct a new matrices w/ non-negativity constraints {-X <= 0; -Y <= 0}.
    Matrix<T, Dynamic, Dynamic> coeffs(constrsCount + 2, varsCount);
    coeffs <<
      _linearProgramData.constraintsCoeffs,
      -Matrix<T, 2, 2>::Identity(2, 2);

    Matrix<T, Dynamic, 1> RHS(constrsCount + 2, 1);
    RHS <<
      _linearProgramData.constraintsRHS,
      Matrix<T, 2, 1>::Zero(2, 1);

    //In two-dimensional case we have two decision variables.
    //So, we need to generate 2-combinations from all the M + 2 equations
    //and solve that 2x2 systems of linear equations to find intersection points,
    //that _can_ be a feasible region vertices.
    //There are Binomial[M + 2, 2] possible intersection points total.
    //NB: in general case we have N decision variables, and M equations,
    //so we need to check Binomial[M + N, N] intersection points for feasibility.

    //For each constraint
    for (DenseIndex r(0); r < (constrsCount + 2) - 1; ++r)
    {
      //For each other constraint
      for (DenseIndex s(r + 1); s < (constrsCount + 2); ++s)
      {
        //Take two rows and join them in new matrix...
        Matrix<T, 2, Dynamic> eqs(2, varsCount);
        eqs <<
          coeffs.row(r),
          coeffs.row(s);
        //...and take also the corresponding RHS elements
        Matrix<T, 2, 1> rhs(2, 1);
        rhs <<
          RHS(r),
          RHS(s);

        //Obtain Basic Solution:
        //Try to solve matrix equation `eqs * x == rhs'
        //& check solution correctness
        const optional<Matrix<T, 2, 1>> sol(findIntersection<T>(eqs, rhs));
  //      T err((eqs * sol - rhs).norm() / rhs.norm());
        if (sol)
        {
          const bool isValid((eqs * (*sol)).isApprox(rhs));
          if (isValid)
          {
            //Check if Basic Solution is also a Basic Feasible Solution:
            //(the point given by the solution is lying inside of the given feasible region)
            //The found point is point of intersection of two lines (feasible region edges)
            //and it is the feasible region corner point (vertice) iff
            //it satisfies the following matrix equation `A * x <= b'
            //NB: non-vertices (inner points) are also subj. to this eq.
            const bool isFeasible(isPointInFeasibleRegion<T>(*sol, _linearProgramData));

            //If this is a feasible point (feasible region corner (extreme) point -- vertex)
            //and it is valid (non-degenerate) solution,
            //we can try to compute objective function value
            //and update `extremeValue' to the new extreme value (which should be less than current)
            if (isFeasible)
            {
              const T x((*sol)(0)), y((*sol)(1)), f(objFunc(*sol));

              QPointF newVertice(
                numericCast<qreal, T>(x),
                numericCast<qreal, T>(y)
              );
              plotData2D.vertices.append(newVertice);
              qreal newValue(numericCast<qreal, T>(f));

              qDebug() << "GraphicalSolver2D<T>::solve: value for" << newVertice <<
                          "is" << newValue << "~=" << f;

              if (newValue < plotData2D.extremeValue)
              {
                qDebug() << "GraphicalSolver2D<T>::solve: new minimum found for" << newVertice << "is" <<
                            newValue << "~= (" << x << ";" << y << ")";

                plotData2D.extremeValue = newValue;
                plotData2D.extremeVertex = newVertice;
              }
            }
          }
        }
      }
    }

    plotData2D.resultType = SolutionType::Optimal;

    //To plot a polygon we should sort all the found vertices in clockwise order.
    sortPointsClockwise(plotData2D.vertices);

    qDebug() << "GraphicalSolver2D::solve: solution: \nV:=" <<
      plotData2D.vertices << "\nG:=" << plotData2D.gradient <<
      "\nX*:=" << plotData2D.extremeVertex << "\nF*:=" << plotData2D.extremeValue;

    ret = plotData2D;

    return ret;
  }
}

#endif // GRAPHICALSOLVER2D_TXX
