#pragma once

#ifndef SIMPLEXSOLVER_TXX
#define SIMPLEXSOLVER_TXX


#include "simplexsolver.hxx"

#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <utility>
#include <vector>

#include <QDebug>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "linearprogramdata.hxx"
#include "linearprogramsolution.hxx"
#include "simplextableau.hxx"
#include "solutiontype.hxx"
#include "../math/mathutils.hxx"
#include "../math/numericlimits.hxx"
#include "../misc/eigenextensions.hxx"
#include "../misc/utils.hxx"
#include "../config.hxx"


namespace LinearProgramming
{
  using boost::optional;
  using namespace Config::LinearProgramming;
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using MathUtils::isEqual;
  using MathUtils::isEqualToZero;
  using MathUtils::isGreaterThanOrEqualToZero;
  using MathUtils::isGreaterThanZero;
  using MathUtils::isLessThan;
  using MathUtils::isLessThanZero;
  using NumericTypes::Rational;
  using NumericTypes::Real;
  using std::list;
  using std::make_pair;
  using std::pair;
  using std::ref;
  using Utils::makeString;


  template<typename T>
  /**
   * @brief SimplexSolver<T>::SimplexSolver
   * Solves small-scaled linear programs
   * using the Two-Phase Simplex method.
   * For the reference see:
   *  `http://en.wikipedia.org/wiki/Simplex_algorithm',
   *  `http://en.wikipedia.org/wiki/Linear_programming',
   *  `http://mathworld.wolfram.com/SimplexMethod.html',
   *  `http://mathworld.wolfram.com/LinearProgramming.html'.
   *   Introduction to algorithms / Thomas H. Cormen ... [et al.]. -— 3rd ed.,
   *   Кормен, Томаc Х. и др. Алгоритмы: построение и анализ, 3-е изд.
   * @param linearProgramData
   * Source data containing constraints coefficients
   * matrix `A', right-hand-side column-vector `b' and
   * objective function coefficients row-vector `c'.
   */
  SimplexSolver<T>::SimplexSolver(
    const LinearProgramData<T>& linearProgramData
  ) :
    linearProgramData_(linearProgramData)
  { }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::SimplexSolver
   * Solves small-scaled linear programs
   * using the Two-Phase Simplex method.
   * For the reference see:
   *  `http://en.wikipedia.org/wiki/Simplex_algorithm',
   *  `http://en.wikipedia.org/wiki/Linear_programming',
   *  `http://mathworld.wolfram.com/SimplexMethod.html',
   *  `http://mathworld.wolfram.com/LinearProgramming.html',
   *  Introduction to algorithms / Thomas H. Cormen ... [et al.]. -— 3rd ed.,
   *  Кормен, Томаc Х. и др. Алгоритмы: построение и анализ, 3-е изд.
   * @param linearProgramData
   * Source data containing constraints coefficients
   * matrix `A', right-hand-side column-vector `b' and
   * objective function coefficients row-vector `c'.
   */
  SimplexSolver<T>::SimplexSolver(
    LinearProgramData<T>&& linearProgramData
  ) :
    linearProgramData_(std::move(linearProgramData))
  { }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::linearProgramData
   * Provides read-only access to source data.
   * @return
   */
  const LinearProgramData<T>&
  SimplexSolver<T>::linearProgramData() const
  {
    return linearProgramData_;
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::setLinearProgramData
   * Updates source data using `linearProgramData'.
   * @param linearProgramData
   */
  void SimplexSolver<T>::setLinearProgramData(
    const LinearProgramData<T>& linearProgramData
  )
  {
    linearProgramData_ = linearProgramData;
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::setLinearProgramData
   * Updates source data using `linearProgramData'.
   * @param linearProgramData
   */
  void SimplexSolver<T>::setLinearProgramData(
    LinearProgramData<T>&& linearProgramData
  )
  {
    linearProgramData_ = std::move(linearProgramData);
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::solve
   * Finds the extreme (minimum) value `F*' and
   * the extreme point `x*' of the given objective linear
   * function F(x) == (c, x) within the given constraints
   * defined by the following system of linear equations
   * in matrix form:
   *   αx = β,
   * where:
   *  `M' is the constraints count,
   *  `N' is the variables count,
   *  `α' is M × N matrix of constraints coefficients,
   *  `β' is the right-hand-side column-vector of length M
   * and non-negativity constraints
   *   (x[i] >= 0) for all `i' in [0; N)
   * using the two-phase Simplex method.
   * @return `LinearProgramSolution' instance
   * w/ the row-vector of length `N' for function extreme point `x*'
   * and the extreme (minimal) value `F*'.
   */
  pair<SolutionType, optional<LinearProgramSolution<T>>>
  SimplexSolver<T>::solve()
  {
    //Reset internal state
    reset();

    //For result
    optional<LinearProgramSolution<T>> ret;

    LOG(
      "c == \n{0},\nA ==\n{1},\nb ==\n{2}",
      linearProgramData_.objectiveFunctionCoeffs,
      linearProgramData_.constraintsCoeffs,
      linearProgramData_.constraintsRHS
    );

    //Make a new Phase-1 tableau
    SimplexTableau<T> phase1Tableau(
      SimplexTableau<T>::makePhaseOne(linearProgramData_)
    );

    LOG(
      "~X({0}) ==\n{1},\n~x == {2},\n x == {3}",
      iterCount_,
      phase1Tableau.entries(),
      makeString(phase1Tableau.basicVars()),
      makeString(phase1Tableau.freeVars())
    );

    //Solve Phase-1: do `iterate()' while the solution is not optimal
    const SolutionType phase1SolutionType(optimize(phase1Tableau));

    LOG(
      "~X({0}) ==\n{1},\n~x == {2},\n x == {3}",
      iterCount_,
      phase1Tableau.entries(),
      makeString(phase1Tableau.basicVars()),
      makeString(phase1Tableau.freeVars())
    );

    //TODO: !
    if (iterCount_ == 0)
    {
      return make_pair(SolutionType::Unknown, ret);
    }

    if (phase1SolutionType == SolutionType::Optimal)
    {
      //Recheck Phase-1 solution (~F* == 0) and try to solve the Phase-2
      const SolutionType phase1SolutionType(
        checkPhase1Solution(phase1Tableau)
      );

      if (phase1SolutionType == SolutionType::Optimal)
      {
        ++iterCount_;

        SimplexTableau<T> phase2Tableau(
          SimplexTableau<T>::makePhaseTwo(linearProgramData_, phase1Tableau)
        );

        LOG(
          "X({0}) ==\n{1},\n~x == {2},\n x == {3}",
          iterCount_,
          phase2Tableau.entries(),
          makeString(phase2Tableau.basicVars()),
          makeString(phase2Tableau.freeVars())
        );

        //Solve Phase-2: do `iterate()' while the solution is not optimal
        const SolutionType phase2SolutionType(optimize(phase2Tableau));

        LOG(
          "X({0}) ==\n{1},\n~x == {2},\n x == {3}",
          iterCount_,
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
            const LinearProgramSolution<T> linearProgramSolution(
              phase2Tableau.extremePoint(),
              phase2Tableau.extremeValue()
            );

            LOG(
              "x* == {0},\nF* == {1}",
              linearProgramSolution.extremePoint,
              linearProgramSolution.extremeValue
            );

            ret = linearProgramSolution;

            return make_pair(phase2SolutionType, ret);
          }
          else
          {
            return make_pair(phase2SolutionType, ret);
          }
        }
        else
        {
          return make_pair(phase2SolutionType, ret);
        }
      }
      else
      {
        return make_pair(phase1SolutionType, ret);
      }
    }
    else
    {
      return make_pair(phase1SolutionType, ret);
    }
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::reset
   * Resets the internal state of the object.
   */
  void
  SimplexSolver<T>::reset()
  {
    iterCount_ = 0;
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::optimize
   * Performs the Simplex algorithm steps on the given tableau.
   * @param tableau
   * @return
   */
  SolutionType
  SimplexSolver<T>::optimize(SimplexTableau<T>& tableau)
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

        case SolutionType::Infeasible:
          return solutionType;

        case SolutionType::Incomplete:
          break;

        case SolutionType::Unknown:
          return solutionType;

        default:
          break;
      }
    }
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::iterate
   * Tries to execute one iteration of the Simplex method
   * on the given tableau.
   * @param tableau
   * @return
   */
  SolutionType
  SimplexSolver<T>::iterate(SimplexTableau<T>& tableau)
  {
    if (iterCount_ >= MaxSimplexIterations) //If can iterate
    {
      return SolutionType::Unknown;
    }
    else
    {
      const pair<SolutionType, MaybeIndex2D> pivotIdx(computePivotIdx(tableau));

      if (pivotIdx.second) //If indices is present
      {
        ++iterCount_;

        transformTableau(tableau, (*pivotIdx.second).first, (*pivotIdx.second).second);

        return SolutionType::Incomplete;
      }
      else
      {
        return pivotIdx.first; //Return why the indices cannot be obtained
      }
    }
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::iterate
   * Tries to execute one iteration of the Simplex method
   * on the given tableau.
   * @param tableau
   * @return
   */
  SolutionType
  SimplexSolver<T>::iterate(
    SimplexTableau<T>& tableau, pair<DenseIndex, DenseIndex> pivotIdx
  )
  {
    if (iterCount_ >= MaxSimplexIterations) //If can iterate
    {
      return SolutionType::Unknown;
    }
    else
    {
      ++iterCount_;

      transformTableau(tableau, pivotIdx.first, pivotIdx.second);

      return SolutionType::Incomplete;
    }
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::computePivotIdx
   * Tries to compute pivot element position in the given tableau.
   * @param tableau
   * @return
   */
  pair<SolutionType, MaybeIndex2D>
  SimplexSolver<T>::computePivotIdx(
    const SimplexTableau<T>& tableau
  ) const
  {
    MaybeIndex2D idx;

    const optional<DenseIndex> pivotColIdx(computePivotColIdx(tableau));
    if (pivotColIdx)
    {
      //If (∃s: P[s] < 0), we can try to select pivot row `k'
      const optional<DenseIndex> pivotRowIdx(
        computePivotRowIdx(tableau, *pivotColIdx)
      );

      if (pivotRowIdx)
      {
        //If (∃s: P[s] < 0) ∧ (∃k: α[k, s] > 0),
        //the pivoting operation can be performed on the tableau
        idx = make_pair(*pivotRowIdx, *pivotColIdx);

        return make_pair(SolutionType::Incomplete, idx);
      }
      else
      {
        //If (∃s: P[s] < 0) ∧ (∀i: α[i, s] <= 0),
        //the objective function is unbounded over the feasible region

        return make_pair(SolutionType::Unbounded, idx);
      }
    }
    else
    {
      //If (∀j: P[j] >= 0), the current iteration is the last one
      //and the solution is optimal

      return make_pair(SolutionType::Optimal, idx);
    }
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::computePivotColIdx
   * For the reference see:
   *  `http://www.math.toronto.edu/mpugh/Teaching/APM236_04/bland',
   *  `http://people.orie.cornell.edu/dpw/orie6300/Lectures/lec13.pdf',
   *  `http://web.stanford.edu/class/msande310/blandrule.pdf'.
   * @param tableau
   * @return (optional)
   *  argmin{P[s]} for all `s' where (P[s] < 0) (Dantzig's original pivot
   * selection rule)
   *   or
   *  min{s} for all `s' where (P[s] < 0) (Bland's pivot selection rule that
   * prevents cycling).
   */
  MaybeIndex1D
  SimplexSolver<T>::computePivotColIdx(
    const SimplexTableau<T>& tableau
  ) const
  {
    MaybeIndex1D ret;

    DenseIndex minCoeffColIdx(0);
    bool haveNegativeCoeffs(false);


#ifdef LP_WITH_BLAND_RULE
    //Find min{s} for all `s' where (∃s: P[s] < 0)
    DenseIndex minVarIdx(
      tableau.freeVarsCount() + tableau.basicVarsCount()
    );

    for (DenseIndex colIdx(0); colIdx < tableau.cols() - 1; ++colIdx)
    {
      const DenseIndex currVarIdx(tableau.freeVars()[colIdx]);
      const T currCoeff(tableau(tableau.rows() - 1, colIdx));

      //Pick only negative `P[j]'
      if (isLessThanZero<T>(currCoeff))
      {
        haveNegativeCoeffs = true;

        //Update w/ the new found index
        if (currVarIdx < minVarIdx)
        {
          minVarIdx = currVarIdx;
          minCoeffColIdx = colIdx;
        }
      }
    }
#else
    //Find argmin{P[s]} for all `s' where (P[s] < 0)
    T minCoeff(0);

    //Find the most negative coeff `P[s]' at the bottom row
    for (DenseIndex colIdx(0); colIdx < tableau.cols() - 1; ++colIdx)
    {
      const T currCoeff(tableau(tableau.rows() - 1, colIdx));
      //Pick only negative `P[j]'
      if (isLessThanZero<T>(currCoeff))
      {
        haveNegativeCoeffs = true;
        //Update w/ new found index
        if (currCoeff < minCoeff)
        {
          minCoeff = currCoeff;
          minCoeffColIdx = colIdx;
        }
      }
    }
#endif // LP_WITH_BLAND_RULE


    if (haveNegativeCoeffs)
    {
      ret = minCoeffColIdx;
    }

    return ret;
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::computePivotRowIdx
   * Computes pivot row index `k' for the pivot column index `s'.
   * @param tableau
   * @param pivotColIdx
   * @return (optional)
   *  `k' for min{β[k] / α[k, s]} if (∃s: α[k, s] > 0)
   * or
   *  min{k} for min{β[k] / α[k, s]} if (∃s: α[k, s] > 0)
   */
  MaybeIndex1D
  SimplexSolver<T>::computePivotRowIdx(
    const SimplexTableau<T>& tableau, DenseIndex pivotColIdx
  ) const
  {
    MaybeIndex1D ret;


#ifdef LP_WITH_BLAND_RULE
    list<DenseIndex> minRatioRowsIndices;
#else
    DenseIndex minRatioRowIdx(0);
#endif // LP_WITH_BLAND_RULE


    T minRatio(NumericLimits::max<T>());
    bool havePositiveCoeffs(false);

    //Find the pivot row index `k' for the given pivot column index `s'
    for (DenseIndex rowIdx(0); rowIdx < tableau.rows() - 1; ++rowIdx)
    {
      //For the minimum-ratio-test pick
      //only non-negative elements (α[k, s] > 0)
      if (isGreaterThanZero<T>(tableau(rowIdx, pivotColIdx)))
      {
        havePositiveCoeffs = true;

        //Compute ratio β[k] / α[k, s]
        const T currRatio(
          tableau(rowIdx, tableau.cols() - 1) / //!
          tableau(rowIdx, pivotColIdx)
        );


#ifdef LP_WITH_BLAND_RULE
        if (isEqual<T>(currRatio, minRatio))
        {
          minRatioRowsIndices.push_back(rowIdx);
        }
        else
        {
          if (isLessThan<T>(currRatio, minRatio))
          {
            minRatioRowsIndices.clear();
            minRatioRowsIndices.push_back(rowIdx);
            minRatio = currRatio;
          }
        }
#else
        //Update w/ new found minimum
        if (isLessThan<T>(currRatio, minRatio))
        {
          minRatio = currRatio;
          minRatioRowIdx = rowIdx;
        }
#endif // LP_WITH_BLAND_RULE


      }
    }


#ifdef LP_WITH_BLAND_RULE
    if (havePositiveCoeffs)
    {
      if (minRatioRowsIndices.size() == 1)
      {
        ret = minRatioRowsIndices.front();
      }
      else
      {
        if (minRatioRowsIndices.size() > 1)
        {
          DenseIndex minRatioRowIdx(minRatioRowsIndices.front());
          DenseIndex minRatioVarIdx(tableau.basicVarsCount());

          for (DenseIndex rowIdx : minRatioRowsIndices)
          {
            const DenseIndex currVarIdx(tableau.basicVars()[rowIdx]);

            if (currVarIdx < minRatioVarIdx)
            {
              minRatioVarIdx = currVarIdx;
              minRatioRowIdx = rowIdx;
            }
          }

          ret = minRatioRowIdx;
        }
      }
    }
#else
    if (havePositiveCoeffs)
    {
      ret = minRatioRowIdx;
    }
#endif // LP_WITH_BLAND_RULE


    return ret;
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::pivotize
   * Performs transforming operations using chosen
   * pivot row `k' and column `s' of the given tableau.
   * `k' is the leaving variable index,
   * `s' is the entering variable index.
   * The pivot element is `α[k, s]'.
   * @param tableau
   * @param rowIdx
   * @param colIdx
   */
  void
  SimplexSolver<T>::transformTableau(
    SimplexTableau<T>& tableau, DenseIndex rowIdx, DenseIndex colIdx
  )
  {
    //Cache the value of the pivot element `α[k, s]'
    const T pivotElement(tableau(rowIdx, colIdx));

    LOG(
      "pivotElement == {2}, rowIdx == {0}, colIdx == {1}, swap ~x{3} <> x{4}",
      rowIdx, colIdx, pivotElement,
      tableau.basicVars()[rowIdx], tableau.freeVars()[colIdx]
    );

    //Swap the free and basic variable ~x[k] ↔ x[s]
    std::swap(tableau.basicVars()[rowIdx], tableau.freeVars()[colIdx]);

    //For the new row at the pivot position
    //(new pivot row) = (old pivot row) / (old pivot)
    for (DenseIndex j(0); j < tableau.cols(); ++j)
    {
      if (j != colIdx) //Skip pivot column
      {
        tableau(rowIdx, j) /= pivotElement; //!
      }
    }

    //For the new pivot element value
    //(new pivot) = 1 / (old pivot)
    tableau(rowIdx, colIdx) = T(1) / pivotElement; //!

    //For each row above and below the pivot row
    for (DenseIndex i(0); i < tableau.rows(); ++i)
    {
      if (i != rowIdx) //Skip pivot row
      {
        const T factor(tableau(i, colIdx));
        for (DenseIndex j(0); j < tableau.cols(); ++j)
        {
          if (j != colIdx) //Skip pivot column
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
      if (i != rowIdx) //Skip pivot row
      {
        tableau(i, colIdx) /= pivotElement * T(-1); //!
      }
    }

    LOG(
      "T({0}) ==\n{1},\n~x == {2},\n x == {3}",
      iterCount_, tableau.entries(),
      makeString(tableau.basicVars()),
      makeString(tableau.freeVars())
    );
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::checkPhase1Solution
   * Checks if the solution obtained at the Phase-1 is valid.
   * @param tableau
   * @return
   */
  SolutionType
  SimplexSolver<T>::checkPhase1Solution(
    const SimplexTableau<T>& tableau
  ) const
  {
    const T objFuncValue(tableau.extremeValue());
     //If (~F* == 0) -- The end of the Phase-1
    if (isEqualToZero<T>(objFuncValue))
    {
      return SolutionType::Optimal;
    }
    else
    {
       //If (~F* > 0) -- Inconsistent (infeasible) program
      if (isGreaterThanZero<T>(objFuncValue))
      {
        return SolutionType::Infeasible;
      }
      else
      {
         //If (~F* < 0) -- Oops... smth went wrong...
        if (isLessThanZero<T>(objFuncValue))
        {
          return SolutionType::Unknown;
        }
      }
    }
  }


  template<typename T>
  /**
   * @brief SimplexSolver<T>::checkPhase2Solution
   * Checks if the solution obtained at the Phase-2 is valid.
   * @param tableau
   * @return `true' if (β >= (0)), `false' otherwise.
   */
  SolutionType
  SimplexSolver<T>::checkPhase2Solution(
    const SimplexTableau<T>& tableau
  ) const
  {
    if (
      tableau.col(tableau.cols() - 1).
      head(tableau.rows() - 1).
      unaryExpr(ref(isGreaterThanOrEqualToZero<T>)).
      all()
    )
    {
      return SolutionType::Optimal;
    }
    else
    {
      return SolutionType::Infeasible;
    }
  }
}


#endif // SIMPLEXSOLVER_TXX
