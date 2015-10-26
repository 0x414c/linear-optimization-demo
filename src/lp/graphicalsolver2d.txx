#ifndef GRAPHICALSOLVER2D_TXX
#define GRAPHICALSOLVER2D_TXX

#include "graphicalsolver2d.hxx"

#include <algorithm>
#include <iostream>
#include <limits>

#include <QDebug>
#include <QLineF>
#include <QPointF>
#include <QVector>

#include "boost/optional.hpp"

#include "linearfunction.hxx"
#include "linearprogramutils.hxx"
#include "linearprogramsolution.hxx"
#include "plotdata2d.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"

using namespace boost;
using namespace DataConvertors;
using namespace LinearProgramUtils;
using namespace std;
using namespace Utils;

template<typename T>
GraphicalSolver2D<T>::GraphicalSolver2D(const LinearProgramData<T>& linearProgramData) :
  _linearProgramData(linearProgramData) {}

/**
 * @brief GraphicalSolver2D::solve
 * Naïve algorithm for solving two-dimensional linear optimization problems
 * @return
 */
template<typename T>
optional<PlotData2D> GraphicalSolver2D<T>::solve()
{
  //Input data params
  const int eqsCount = _linearProgramData.constraintsCoeffs.rows();
  const int coeffsCount = _linearProgramData.constraintsCoeffs.cols();

  //For further computations
  LinearFunction<T> objFunc(_linearProgramData.objFuncCoeffs);

  //For output
  PlotData2D plotData2D(
    ResultType::Unknown,
    QPointF(numeric_limits<qreal>::lowest(), numeric_limits<qreal>::lowest()),
    numeric_limits<qreal>::lowest(),
    QVector<QPointF>(0),
    QLineF(
      QPointF(0, 0),
      QPointF(numericCast<qreal>(objFunc.coeffAt(0)), numericCast<qreal>(objFunc.coeffAt(1)))
    )
  );
  optional<PlotData2D> ret;

  //Find column-basis
  //Its column count (== free variables count) should be equal to 2 for 2D graphical method
  Matrix<T, Dynamic, Dynamic> augm(eqsCount, coeffsCount + 1);
  augm <<
    _linearProgramData.constraintsCoeffs,
    _linearProgramData.constraintsRHS;

  //For the system `Ax == b' to be consistent rank(A) should be equal to rank(A⊔b)
  FullPivLU<Matrix<T, Dynamic, Dynamic>> augmLU(augm);
  int augmRank = augmLU.rank();
  FullPivLU<Matrix<T, Dynamic, Dynamic>> coeffLU(_linearProgramData.constraintsCoeffs);
  int rank = coeffLU.rank();
  qDebug() << "The rank of A is " << rank << endl << "The rank of A|b is " << augmRank;
  if (rank != augmRank || rank == 0)
  {
    qDebug() << "GraphicalSolver2D::solve: inconsistent system";
    plotData2D.resultType = ResultType::Inconsistent;

    return ret;
  }
  else
  {
    Matrix<T, Dynamic, Dynamic> ker = coeffLU.kernel();
    Matrix<T, Dynamic, Dynamic> im = coeffLU.image(_linearProgramData.constraintsCoeffs);
    cerr << "Here is a matrix whose columns form a basis of the null-space (ker) of A:\n" << ker << endl;
    cerr << "Here is a matrix whose columns form a basis of the column-space (im) of A:\n" << im << endl;

  //  Matrix<real_t, Dynamic, Dynamic> ka = ker * _linearProgramData.constraintsCoeffs;
  //  cerr << ka << endl;
  //  Matrix<real_t, Dynamic, Dynamic> ik = ker * im;
  //  cerr << ik << endl;
  //  Matrix<real_t, Dynamic, Dynamic> ck = objFunc.coeffs() * ker;
  //  cerr << ck << endl;

    if (im.cols() != 2)
    {
      qDebug() << "GraphicalSolver2D::solve: cannot solve this system by using 2D solver";
      plotData2D.resultType = ResultType::Unknown;

      return ret;
    }

    //construct a new matrices w/ axes equations {-X == 0; -Y == 0}
    //non-negativity constraints now will look as {-X <= 0; -Y <= 0}
    Matrix<T, Dynamic, Dynamic> coeffs(eqsCount + 2, coeffsCount);
    coeffs <<
      _linearProgramData.constraintsCoeffs,
      Matrix<T, 2, 2>::Identity(2, 2) * -1;

    Matrix<T, Dynamic, 1> RHS(eqsCount + 2, 1);
    RHS <<
      _linearProgramData.constraintsRHS,
      Matrix<T, 2, 1>::Zero(2, 1);


    //for two-dimensional case we have two decision variables
    //so, we need to generate 2-combinations from all the M + 2 equations
    //and solve that 2x2 systems of linear equations to find intersection points
    //that _can_ be a feasible region vertices.
    //There are Binomial[M + 2, 2] possible intersection points total.
    //NB: in general case we have N decision variables, and M equations,
    //so we need to check Binomial[M + N, N] intersection points for feasibility.

    //for each row
    for (int i = 0; i < (eqsCount + 2) - 1; ++i)
    {
      //for each other row
      for (int j = i + 1; j < (eqsCount + 2); ++j)
      {
        //take two equations and join them in matrix as rows
        Matrix<T, 2, Dynamic> eqs(2, coeffsCount);
        eqs <<
          coeffs.row(i),
          coeffs.row(j);
        //take corresponding RHS elements
        Matrix<T, 2, 1> rhs(2, 1);
        rhs <<
          RHS(i),
          RHS(j);

        //obtain Basic Solution
        //solve matrix equation `eqs * x == rhs'
        //& check solution correctness
        Matrix<T, Dynamic, 1> sol = eqs.fullPivLu().solve(rhs);
        bool isValid = (eqs * sol).isApprox(rhs);
        T err = (eqs * sol - rhs).norm() / rhs.norm();

        //check if Basic Solution is also a Basic Feasible Solution
        //if point given by the solution is lying inside of the given feasible region
        //the found point is point of intersection of two lines (feasible region edges)
        //and it is the feasible region corner point (vertice) iff
        //it satisfies the following matrix equation `A * x <= b'
        //NB: non-vertices (inner points) are also subj. to this eq.
        bool isFeasible = isPointInFeasibleRegion<T>(sol, _linearProgramData);

        //if this is feasible point (feasible region corner (extreme) point -- vertex)
        //and it is valid (non-degenerate) solution,
        //we can try to compute objective function value
        //and update extremeValue to a new extreme value (which should be less than current)
        if (isFeasible && isValid/* && fabs(err) <= numeric_limits<real_t>::epsilon()*/)
        {
          QPointF vert(numericCast<qreal, T>(sol(0)), numericCast<qreal, T>(sol(1)));
          plotData2D.vertices.append(vert);
          qreal currValue = numericCast<qreal, T>(objFunc.valueAt(sol));
          qDebug() << "GraphicalSolver2D::solve: value for" << plotData2D.vertices.last() << "is" << currValue;
          if (currValue < plotData2D.extremeValue)
          {
            qDebug() << "GraphicalSolver2D::solve: new minimum found for" << plotData2D.vertices.last() << "is" << currValue;
            plotData2D.extremeValue = currValue;
            plotData2D.extremeVertex = plotData2D.vertices.last();
          }
        }
      }
    }

    //now we should sort all the found vertices in clockwise order to plot a polygon later
    LinearProgramUtils::sortClockwise(plotData2D.vertices);

    qDebug() << "GraphicalSolver2D::solve: solution: \nV:=" << plotData2D.vertices << "\nG:=" << plotData2D.gradient << "\nX*:=" << plotData2D.extremeVertex << "\nF(X*):=" << plotData2D.extremeValue;

    plotData2D.resultType = ResultType::Optimal;
    ret = plotData2D;

    return ret;
  }
}

#endif // GRAPHICALSOLVER2D_TXX
