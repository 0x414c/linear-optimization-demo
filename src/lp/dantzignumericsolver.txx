#ifndef DANTZIGNUMERICSOLVER_TXX
#define DANTZIGNUMERICSOLVER_TXX

#include "dantzignumericsolver.hxx"

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include <QDebug>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "linearprogramdata.hxx"
#include "linearprogramsolution.hxx"
#include "simplextableau.hxx"
#include "../math/mathutils.hxx"
#include "../math/numericlimits.hxx"
#include "../misc/eigenextensions.hxx"
#include "../misc/utils.hxx"

namespace LinearProgramming
{
  using namespace boost;
  using namespace Eigen;
  using namespace MathUtils;
  using namespace std;
  using namespace Utils;

  template<typename T>
  DantzigNumericSolver<T>::DantzigNumericSolver()
  { }

  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::DantzigNumericSolver
   * Solves small-scaled linear programs
   * using the Two-Phase Simplex method.
   * For the reference see:
   * `http://en.wikipedia.org/wiki/Simplex_algorithm',
   * `http://en.wikipedia.org/wiki/Linear_programming',
   * `http://mathworld.wolfram.com/SimplexMethod.html',
   * `http://mathworld.wolfram.com/LinearProgramming.html'.
   * @param linearProgramData
   * Source data containing
   * constraints coefficients matrix `α',
   * right-hand-side column-vector `β' and
   * objective function coefficients row-vector `c'.
   */
  DantzigNumericSolver<T>::DantzigNumericSolver(
    const LinearProgramData<T>& linearProgramData
  ) :
    _linearProgramData(linearProgramData)
  { }

  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::DantzigNumericSolver
   * Solves small-scaled linear programs
   * using the Two-Phase Simplex method.
   * For the reference see:
   * `http://en.wikipedia.org/wiki/Simplex_algorithm',
   * `http://en.wikipedia.org/wiki/Linear_programming',
   * `http://mathworld.wolfram.com/SimplexMethod.html',
   * `http://mathworld.wolfram.com/LinearProgramming.html'.
   * @param linearProgramData
   * Source data containing
   * constraints coefficients matrix `α',
   * right-hand-side column-vector `β' and
   * objective function coefficients row-vector `c'.
   */
  DantzigNumericSolver<T>::DantzigNumericSolver(
    LinearProgramData<T>&& linearProgramData
  ) :
    _linearProgramData(std::move(linearProgramData))
  { }

  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::setLinearProgramData
   * Updates source data using `linearProgramData'.
   * @param linearProgramData
   */
  void DantzigNumericSolver<T>::setLinearProgramData(
    const LinearProgramData<T>& linearProgramData
  )
  {
    _linearProgramData = linearProgramData;
  }

  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::setLinearProgramData
   * Updates source data using `linearProgramData'.
   * @param linearProgramData
   */
  void DantzigNumericSolver<T>::setLinearProgramData(
    LinearProgramData<T>&& linearProgramData
  )
  {
    _linearProgramData = std::move(linearProgramData);
  }

  //TODO: new EqualityConstraint class: Coeffs, Restriction
  //TODO: add Constant term to LinearFunction class
  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::solve
   * Finds the extreme (minimum) value w* and
   * the extreme point x* of the given objective linear
   * function w(x) = (c, x) within the given constraints
   * defined by the following system of linear equations
   *   αx = β,
   * where:
   *   M is the constraints count,
   *   N is the variables count,
   *   α is M × N matrix of constraints coefficients,
   *   β is the right-hand-side column-vector of length M
   * and non-negativity constraints
   *   x[i] >= 0 for all i in [0; N)
   * using the two-phase Simplex method.
   * @return `LinearProgramSolution' instance
   * w/ row-vector for extreme point `x*'
   * and the extreme value `w*'.
   */
  optional<LinearProgramSolution<T>>
  DantzigNumericSolver<T>::solve()
  {
    //For result
    optional<LinearProgramSolution<T>> ret;

    //Reset internal state
    _iterCount = 0;

    LOG(
      "c := \n{0}\nA :=\n{1}\nb :=\n{2}",
      _linearProgramData.objectiveFuncCoeffs,
      _linearProgramData.constraintsCoeffs,
      _linearProgramData.constraintsRHS
    );

    //Make new Phase-1 tableau
    SimplexTableau<T> phase1Tableau(
      SimplexTableau<T>::makePhase1(_linearProgramData)
    );
    LOG(
      "~X({0}) :=\n{1}\n~X := {2}\nX := {3}",
      _iterCount,
      phase1Tableau.entries(),
      makeString(phase1Tableau.basicVars()),
      makeString(phase1Tableau.freeVars())
    );

    //Solve Phase-1: do `iterate()' while the solution is not optimal
    const SolutionType phase1SolutionType(optimize(phase1Tableau));
    LOG(
      "~X({0}) :=\n{1}\n~X := {2}\nX := {3}",
      _iterCount,
      phase1Tableau.entries(),
      makeString(phase1Tableau.basicVars()),
      makeString(phase1Tableau.freeVars())
    );
    if (phase1SolutionType == SolutionType::Optimal)
    {
      //Recheck Phase-1 solution (~w* == 0) and try to solve Phase-2
      const SolutionType phase1SolutionType(
        checkPhase1Solution(phase1Tableau)
      );
      if (phase1SolutionType == SolutionType::Optimal)
      {
        SimplexTableau<T> phase2Tableau(
          SimplexTableau<T>::makePhase2(_linearProgramData, phase1Tableau)
        );
        LOG(
          "X({0}) :=\n{1}\n~X := {2}\nX := {3}",
          _iterCount,
          phase2Tableau.entries(),
          makeString(phase2Tableau.basicVars()),
          makeString(phase2Tableau.freeVars())
        );

        //Solve Phase-2: do `iterate()' while the solution is not optimal
        const SolutionType phase2SolutionType(optimize(phase2Tableau));
        LOG(
          "X({0}) :=\n{1}\n~X := {2}\nX := {3}",
          _iterCount,
          phase2Tableau.entries(),
          makeString(phase2Tableau.basicVars()),
          makeString(phase2Tableau.freeVars())
        );
        if (phase2SolutionType == SolutionType::Optimal)
        {
          //Recheck Phase-2 solution (x* >= (0))
          const SolutionType phase2SolutionType(
            checkPhase2Solution(phase2Tableau)
          );
          if (phase2SolutionType == SolutionType::Optimal)
          {
            LinearProgramSolution<T> linearProgramSolution(
              SolutionType::Optimal,
              phase2Tableau.extremePoint(),
              phase2Tableau.extremeValue()
            );
            LOG(
              "x* == {0},\nw* == {1}",
              linearProgramSolution.extremePoint,
              linearProgramSolution.extremeValue
            );

            ret = linearProgramSolution;
          }
        }
      }
    }

    return ret;
  }

  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::optimize
   * Performs the Simplex algorithm steps on the given tableau.
   * @param tableau
   * @return
   */
  SolutionType
  DantzigNumericSolver<T>::optimize(SimplexTableau<T>& tableau)
  {
    //Iterate while the solution is incomplete, stop if
    //the program is unsolvable, return value decribing why it is
    while (true)
    {
      const SolutionType solutionType(iterate(tableau));

      switch (solutionType)
      {
        case SolutionType::Optimal:
          return solutionType;
        case SolutionType::Unbounded:
          return solutionType;
        case SolutionType::Incomplete:
          break;
        case SolutionType::Inconsistent:
          return solutionType;
        case SolutionType::Unknown:
          return solutionType;
        default:
          break;
      }
    }
  }

  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::iterate
   * Executes one iteration of the Simplex method on the given tableau.
   * @param tableau
   * @return
   */
  SolutionType
  DantzigNumericSolver<T>::iterate(SimplexTableau<T>& tableau)
  {
    if (++_iterCount > _maxIterations)
    {
      return SolutionType::Unknown;
    }
    else
    {
      const optional<DenseIndex> pivotColIdx(computePivotColIdx(tableau));
      if (pivotColIdx)
      {
        //If (∃s: P[s] < 0)
        const optional<DenseIndex> pivotRowIdx(
          computePivotRowIdx(tableau, *pivotColIdx)
        );

        if (pivotRowIdx)
        {
          //If (∃s: P[s] < 0) ∧ (∃i: α[i, s] > 0),
          //the pivoting operation can be performed on the tableau
          pivotize(tableau, *pivotRowIdx, *pivotColIdx);

          return SolutionType::Incomplete;
        }
        else
        {
          //If (∃s: P[s] < 0) ∧ (∀i: α[i, s] <= 0),
          //the objective function is unbounded on the feasible region

          return SolutionType::Unbounded;
        }
      }
      else
      {
        //If (∀j: P[j] >= 0), the current iteration is the last one
        //and the solution is optimal

        return SolutionType::Optimal;
      }
    }
  }

  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::pivotize
   * Performs pivoting operations for the selected
   * row and column of the given tableau.
   * The pivot element is α[k, s].
   * @param tableau
   * @param rowIdx
   * @param colIdx
   */
  void
  DantzigNumericSolver<T>::pivotize(
    SimplexTableau<T>& tableau, DenseIndex rowIdx, DenseIndex colIdx
  )
  {
    LOG(
      "swap ~x{0} <> x{1}",
      tableau.basicVars()[rowIdx],
      tableau.freeVars()[colIdx]
    );
    //Swap the free and basic variable ~x[k] ↔ x[s]
    std::swap(
      tableau.basicVars()[rowIdx],
      tableau.freeVars()[colIdx]
    );

    //Cache the value of the pivot element α[k, s]
    const T pivotElement(tableau(rowIdx, colIdx));
    LOG(
      "rowIdx := {0}, colIdx := {1}, pivot := {2}",
      rowIdx, colIdx, pivotElement
    );

    //For the new row at the pivot position
    //(new pivot row) = (old pivot row) / (old pivot)
    for (DenseIndex j(0); j < tableau.cols(); ++j)
    {
      if (j != colIdx)
      {
        tableau(rowIdx, j) /= pivotElement;
      }
    }

    //(new pivot) = 1 / (old pivot)
    tableau(rowIdx, colIdx) = T(1) / pivotElement;

    //For each row above and below the pivot row
    for (DenseIndex i(0); i < tableau.rows(); ++i)
    {
      if (i != rowIdx) //skip pivot row
      {
        const T factor(tableau(i, colIdx));
        for (DenseIndex j(0); j < tableau.cols(); ++j)
        {
          if (j != colIdx) //skip pivot column
          {
            //(new row) = (old row) - (factor) * (pivot row)
            tableau(i, j) -= factor * tableau(rowIdx, j);
          }
        }
      }
    }

    //For the new column at the pivot position
    //(new pivot column) = (old pivot column) / ((-1) * (old pivot))
    for (DenseIndex i(0); i < tableau.rows(); ++i)
    {
      if (i != rowIdx)
      {
        tableau(i, colIdx) /= pivotElement * T(-1);
      }
    }

    LOG(
      "T({0}) :=\n{1}\n~X := {2}\nX := {3}",
      _iterCount, tableau.entries(),
      makeString(tableau.basicVars()),
      makeString(tableau.freeVars())
    );
  }

  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::checkPhase1Solution
   * Checks if the solution obtained at the Phase-1 is valid.
   * @param tableau
   * @return
   */
  SolutionType
  DantzigNumericSolver<T>::checkPhase1Solution(
    const SimplexTableau<T>& tableau
  ) const
  {
    const T objFuncValue(tableau.extremeValue());
     //If (~w* == 0) -- The end of the Phase-1
    if (isEqualToZero<T>(objFuncValue))
    {
      return SolutionType::Optimal;
    }
    else
    {
       //If (~w* > 0) -- Inconsistent (infeasible) program
      if (isGreaterThanZero<T>(objFuncValue))
      {
        return SolutionType::Inconsistent;
      }
      else
      {
         //If (~w* < 0) -- Oops... smth went wrong...
        if (isLessThanZero<T>(objFuncValue))
        {
          return SolutionType::Unknown;
        }
        else
        {
          return SolutionType::Unknown;
        }
      }
    }
  }

  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::checkPhase2Solution
   * Checks if the solution obtained at the Phase-2 is valid.
   * @param tableau
   * @return `true' if (β >= (0)), `false' otherwise.
   */
  SolutionType
  DantzigNumericSolver<T>::checkPhase2Solution(
    const SimplexTableau<T>& tableau
  ) const
  {
    if (
      tableau.col(tableau.cols() - 1).
      head(tableau.rows() - 1).
      unaryExpr(std::ref(isGreaterOrEqualToZero<T>)).
      all()
    )
    {
      return SolutionType::Optimal;
    }
    else
    {
      return SolutionType::Inconsistent;
    }
  }

  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::isPivotColValid
   * @param tableau
   * @param colIdx
   * @return true if (∃i: α[i, s] > 0), false otherwise.
   */
  bool
  DantzigNumericSolver<T>::isPivotColValid(
    const SimplexTableau<T>& tableau, DenseIndex colIdx
  ) const
  {
    return (
      (
        tableau.
        col(colIdx).
        head(tableau.rows() - 1).
        unaryExpr(ref(isGreaterThanZero<T>))
      ).
      any()
    );
  }

  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::computePivotColIdx
   * @param tableau
   * @return min{s} for all possible s if (∃s: P[s] < 0),
   * empty optional otherwise.
   */
  optional<DenseIndex>
  DantzigNumericSolver<T>::computePivotColIdx(
    const SimplexTableau<T>& tableau
  ) const
  {
    optional<DenseIndex> ret;

    T min(0);
    DenseIndex minIdx(0);
    bool haveNegativeCoeffs(false);

    //Find the most negative coeff at the bottom row
    for (DenseIndex colIdx(0); colIdx < tableau.cols() - 1; ++colIdx)
    {
      if (tableau(tableau.rows() - 1, colIdx) < min)
      {
        min = tableau(tableau.rows() - 1, colIdx);
        minIdx = colIdx;
        haveNegativeCoeffs = true;
      }
    }

    if (haveNegativeCoeffs)
    {
      ret = minIdx;
    }

    return ret;
  }

  template<typename T>
  /**
   * @brief DantzigNumericSolver<T>::computePivotRowIdx
   * Computes pivot row index k for the pivot column index s.
   * @param tableau
   * @param pivotColIdx
   * @return k for min{β[k] / α[k, s]} if (∃i: α[k, s] > 0),
   * empty boost::optional otherwise.
   */
  optional<DenseIndex>
  DantzigNumericSolver<T>::computePivotRowIdx(
    const SimplexTableau<T>& tableau, DenseIndex pivotColIdx
  ) const
  {
    optional<DenseIndex> ret;

    T minRatio(NumericLimits::max<T>());
    DenseIndex minRatioIdx(0);
    bool isPivotColValid(false);

    //Find the pivot row index `k' for the given pivot column index `s'
    for (DenseIndex rowIdx(0); rowIdx < tableau.rows() - 1; ++rowIdx)
    {
      //For the minimum-ratio-test
      //Pick only non-negative elements (α[k, s] > 0)
      if (isGreaterThanZero<T>(tableau(rowIdx, pivotColIdx)))
      {
        //Compute ratio β[k] / α[k, s]
        const T currRatio(
          tableau(rowIdx, tableau.cols() - 1) /
          tableau(rowIdx, pivotColIdx)
        );
        //Update w/ new found minimum
        if (currRatio < minRatio)
        {
          minRatio = currRatio;
          minRatioIdx = rowIdx;
        }
        isPivotColValid = true;
      }
    }

    if (isPivotColValid)
    {
      //TODO: ! Bland rule

      ret = minRatioIdx;
    }

    return ret;
  }
}

#endif // DANTZIGNUMERICSOLVER_TXX
