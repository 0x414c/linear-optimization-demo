#pragma once

#ifndef SIMPLEXTABLEAU_TXX
#define SIMPLEXTABLEAU_TXX


#include "simplextableau.hxx"

#include <cstddef>

#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "eigen3/Eigen/Core"

#include "linearprogramdata.hxx"
#include "optimizationgoaltype.hxx"
#include "solutionphase.hxx"
#include "../math/mathutils.hxx"


namespace LinearProgramming
{
  using Eigen::Block;
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using MathUtils::isLessThanZero;
  using std::invalid_argument;


  template<typename T>
  /**
   * @brief SimplexTableau<T>::SimplexTableau
   * Default ctor.
   * NOTE: It leaves class members uninitialized.
   */
  SimplexTableau<T>::SimplexTableau()
  { }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::SimplexTableau
   * Copy-ctor.
   * @param simplexTableau
   */
  SimplexTableau<T>::SimplexTableau(const SimplexTableau<T>& simplexTableau) :
    phase_(simplexTableau.phase_),
    basicVars_(simplexTableau.basicVars_),
    freeVars_(simplexTableau.freeVars_),
    entries_(simplexTableau.entries_)
  { }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::SimplexTableau
   * Move-ctor.
   * @param simplexTableau
   */
  SimplexTableau<T>::SimplexTableau(SimplexTableau<T>&& simplexTableau) :
    phase_(simplexTableau.phase_),
    basicVars_(std::move(simplexTableau.basicVars_)),
    freeVars_(std::move(simplexTableau.freeVars_)),
    entries_(std::move(simplexTableau.entries_))
  { }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::operator ()
   * Provides read-write access to the particular tableau entry.
   * @param rowIdx
   * @param colIdx
   * @return
   */
  T&
  SimplexTableau<T>::operator ()(DenseIndex rowIdx, DenseIndex colIdx)
  {
    return entries_(rowIdx, colIdx);
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::operator ()
   * Provides read-only access to the particular tableau entry.
   * @param rowIdx
   * @param colIdx
   * @return
   */
  const T&
  SimplexTableau<T>::operator ()(DenseIndex rowIdx, DenseIndex colIdx) const
  {
    return entries_(rowIdx, colIdx);
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::entries
   * Provides read-write access to the tableau entries matrix.
   * TODO: ~? Use `ColsBlockXpr' or `RowsBlockXpr'
   * @return
   */
  Block<Matrix<T, Dynamic, Dynamic>, Dynamic, Dynamic>
  SimplexTableau<T>::entries()
  {
    return entries_.block(0, 0, entries_.rows(), entries_.cols());
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::entries
   * Provides read-only access to the tableau entries matrix.
   * @return
   */
  const Block<const Matrix<T, Dynamic, Dynamic>, Dynamic, Dynamic>
  SimplexTableau<T>::entries() const
  {
    return entries_.block(0, 0, entries_.rows(), entries_.cols());
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::row
   * Provides read-write access to the particular tableau row.
   * NOTE: `RowXpr' is the special case of a `Block' expression,
   * which can give optimization opportunities.
   * @param rowIdx
   * @return
   */
  typename Matrix<T, Dynamic, Dynamic>::RowXpr
  SimplexTableau<T>::row(DenseIndex rowIdx)
  {
    return entries_.row(rowIdx);
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::row
   * Provides read-only access to the particular tableau row.
   * @param rowIdx
   * @return
   */
  typename Matrix<T, Dynamic, Dynamic>::ConstRowXpr
  SimplexTableau<T>::row(DenseIndex rowIdx) const
  {
    return entries_.row(rowIdx);
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::col
   * Provides read-write access to the particular tableau column.
   * @param colIdx
   * @return
   */
  typename Matrix<T, Dynamic, Dynamic>::ColXpr
  SimplexTableau<T>::col(DenseIndex colIdx)
  {
    return entries_.col(colIdx);
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::col
   * Provides read-only access to the particular tableau column.
   * @param colIdx
   * @return
   */
  typename Matrix<T, Dynamic, Dynamic>::ConstColXpr
  SimplexTableau<T>::col(DenseIndex colIdx) const
  {
    return entries_.col(colIdx);
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::rowsCount
   * Returns tableau rows count.
   * @return
   */
  DenseIndex
  SimplexTableau<T>::rows() const
  {
    return entries_.rows();
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::colsCount
   * Returns tableau columns count.
   * @return
   */
  DenseIndex
  SimplexTableau<T>::cols() const
  {
    return entries_.cols();
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::phase
   * Returns the phase of the Simplex algorithm
   * for which the tableau was created.
   * @return
   */
  SolutionPhase
  SimplexTableau<T>::phase() const
  {
    return phase_;
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::basicVariables
   * Provides read-write access to the basic variables indices `~x[j]'.
   * @return
   */
  vector<DenseIndex>&
  SimplexTableau<T>::basicVars()
  {
    return basicVars_;
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::basicVariables
   * Provides read-only access to the basic variables indices `~x[j]'.
   * @return
   */
  const vector<DenseIndex>&
  SimplexTableau<T>::basicVars() const
  {
    return basicVars_;
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::freeVariables
   * Provides read-write access to the free variables indices `x[i]'.
   * @return
   */
  vector<DenseIndex>&
  SimplexTableau<T>::freeVars()
  {
    return freeVars_;
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::freeVariables
   * Provides read-only access to the free variables indices `x[j]'.
   * @return
   */
  const vector<DenseIndex>&
  SimplexTableau<T>::freeVars() const
  {
    return freeVars_;
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::basicVarsCount
   * Returns basic variables `~x[i]' count `M'.
   * @return
   */
  size_t
  SimplexTableau<T>::basicVarsCount() const
  {
    return basicVars_.size();
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::freeVarsCount
   * Returns free variables `x[j]' count `N'.
   * @return
   */
  size_t
  SimplexTableau<T>::freeVarsCount() const
  {
    return freeVars_.size();
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::extremePoint
   * Constructs solution row-vector `x*' w/ M + N columns.
   * Basic variables values will be computed from the tableau
   * and free variables will be set to 0.
   * @return the current solution `x*' as row-vector.
   */
  Matrix<T, Dynamic, 1>
  SimplexTableau<T>::extremePoint() const
  {
    Matrix<T, Dynamic, 1> x_(basicVars_.size() + freeVars_.size(), 1);

    for (DenseIndex i(0); i < entries_.rows() - 1; ++i)
    {
      const DenseIndex varIdx(basicVars_[i]);
      x_(varIdx) = entries_(i, entries_.cols() - 1);
    }

    for (DenseIndex j(0); j < entries_.cols() - 1; j++)
    {
      const DenseIndex varIdx(freeVars_[j]);
      x_(varIdx) = T(0);
    }

    return x_;
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::extremeValue
   * @return the current value `F*' of the objective function `F'.
   */
  T
  SimplexTableau<T>::extremeValue(OptimizationGoalType goalType) const
  {
    switch (goalType) {
      case OptimizationGoalType::Minimize:
        return (entries_(entries_.rows() - 1, entries_.cols() - 1) * T(-1));

      case OptimizationGoalType::Maximize:
        return entries_(entries_.rows() - 1, entries_.cols() - 1);

      default:
        return T(0);
    }
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::SimplexTableau
   * This factory method constructs the Phase-1 simplex tableau
   * f/ the auxilliary program.
   *
   * The Phase-1 Simplex tableau is the (M+1) × (N+1) matrix
   * of the following form:
   *   |~X(t) |  x1   x2   x..   xN | β   |
   *   |----------------------------------|
   *   | ~x1  | α11                 | β1  |
   *   | ~x2  |      α22            | β2  |
   *   | ~x.. |            α..      | β.. |
   *   | ~xM  |                 αMN | βM  |
   *   |----------------------------------|
   *   | P    |  P1   P2   P..   PN | -P0 |,
   * where:
   *   `t' is the current iteration index,
   *   `~x[i]' is the basic (and it is the artificial) variable,
   *   `x[j]' is the free (and it is the decision) variable,
   *   `-P[0]' is the current value of the objective function (aka `F*'),
   *   `P[i]' is the `i'-th objective function coefficient (aka `c[i]'),
   *   `α' is the `M' × `N' constraints coefficients matrix,
   *   `α[i, j]' is the particular coefficient of `α',
   *   `β' is the `M' × 1 right-hand-side column-vector,
   *   `β[j]' is the particular coefficient of `β',
   *   `M' is the constraints count
   *       (and it is the basic artificial variables count),
   *   `N' is the variables count
   *       (and it is the free decision variables count),
   *    and (M <= N).
   * @param linearProgramData
   */
  SimplexTableau<T>
  SimplexTableau<T>::makePhaseOne(const LinearProgramData<T> &linearProgramData)
  {
    //Count of artificial basic vars `~x' (== M) for the Phase-1 tableau
    const DenseIndex basicVarsCount(linearProgramData.constraintsCount());
    //Count of decision free vars `x' (== N) for the Phase-1 tableau
    const DenseIndex freeVarsCount(linearProgramData.variablesCount());

    //Make a new tableau for the Phase-1
    SimplexTableau<T> phase1Tableau;

    //Set phase to `One'
    phase1Tableau.phase_ = SolutionPhase::One;

    //Initialize vectors w/ variables indices
    phase1Tableau.basicVars_ = vector<DenseIndex>(basicVarsCount);
    //For the basic artificial vars `~x[i]' where `i' in [N; N + M)
    for (DenseIndex i(freeVarsCount); i < freeVarsCount + basicVarsCount; ++i)
    {
      phase1Tableau.basicVars_[i - freeVarsCount] = i;
    }

    phase1Tableau.freeVars_ = vector<DenseIndex>(freeVarsCount);
    //For the free decision vars `x[i]' where i in [0; N)
    for (DenseIndex i(0); i < freeVarsCount; ++i)
    {
      phase1Tableau.freeVars_[i] = i;
    }

    //Let's build an auxilliary program tableau `~X(0)'
    //Initialize some of its entries (constraints coeffs matrix `α'
    //and right-hand-side column-vector `β' ) directly
    //from `linearProgramData'. `P' is left unintialized.
    phase1Tableau.entries_ = Matrix<T, Dynamic, Dynamic>(
      basicVarsCount + 1, freeVarsCount + 1
    );
    phase1Tableau.entries_.block(0, 0, basicVarsCount, freeVarsCount) =
    linearProgramData.constraintsCoeffs;
    phase1Tableau.entries_.block(0, freeVarsCount, basicVarsCount, 1) =
    linearProgramData.constraintsRHS;

    //Further, we need to multiply some equations by (-1) to make (β >= (0))
    for (DenseIndex i(0); i < phase1Tableau.entries_.rows(); ++i)
    {
      if (
        isLessThanZero<T>(
          phase1Tableau.entries_(i, phase1Tableau.entries_.cols() - 1)
        )
      )
      {
        phase1Tableau.entries_.row(i) *= T(-1);
      }
    }

    //Now, set values for the bottom row containing
    //objective function coeffs `P[j]' and func. value `P[0]'
    for (DenseIndex j(0); j < phase1Tableau.entries_.cols(); ++j)
    {
      //Each `P[j]' is the negative sum of the column `α.col[j]' above it
      phase1Tableau.entries_(phase1Tableau.entries_.rows() - 1, j) =
        phase1Tableau.entries_.col(j).head(basicVarsCount).sum() * T(-1);
    }

    return phase1Tableau;
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::makePhase2
   * Constructs the Phase-2 tableau from the given optimal Phase-1 tableau
   * by dropping all the columns corresponding to the basic artificial
   * variables that was forced out of the basis at the Phase-1 of
   * the Simplex algorithm.
   * TODO: ! Add support for degenerate cases.
   * @param linearProgramData
   * @param phase1Tableau
   * @return
   */
  SimplexTableau<T>
  SimplexTableau<T>::makePhaseTwo(
    const LinearProgramData<T>& linearProgramData,
    const SimplexTableau<T>& phase1Tableau
  ) throw(invalid_argument)
  {
    if (phase1Tableau.phase_ != SolutionPhase::One)
    {
      throw invalid_argument("Tableau is not a Phase-1 tableau");
    }

    //Count of free decision vars `x' (~N) in the Phase-1 tableau
    const DenseIndex phase1FreeVarsCount(phase1Tableau.freeVarsCount());
    //Count of basic vars `~x' (M) in the Phase-2 tableau
    //is equal to the one of Phase-1 tableau (~M)
    const DenseIndex phase2BasicVarsCount(phase1Tableau.basicVarsCount());
    //Count of free vars `x' (N) in the Phase-2 tableau
    //is equal to ~M - ~N
    const DenseIndex phase2FreeVarsCount(
      phase1FreeVarsCount - phase2BasicVarsCount
    );

    //Make a new Phase-2 tableau
    SimplexTableau<T> phase2Tableau;

    //Set phase to `Two'
    phase2Tableau.phase_ = SolutionPhase::Two;

    //Initialize vectors w/ variables indices
    //Copy all the basic variables `~x' from the Phase-1 tableau
    phase2Tableau.basicVars_ = vector<DenseIndex>(phase2BasicVarsCount);
    std::copy(
      phase1Tableau.basicVars_.begin(),
      phase1Tableau.basicVars_.end(),
      phase2Tableau.basicVars_.begin()
    );

    //Copy all the free variables `x[i]' except those vars
    //that was forced out of the basis at the Phase-1 (artificial)
    phase2Tableau.freeVars_ = vector<DenseIndex>(phase2FreeVarsCount);
    size_t varsCount(0);
    std::copy_if(
      phase1Tableau.freeVars_.begin(),
      phase1Tableau.freeVars_.end(),
      phase2Tableau.freeVars_.begin(),
      [phase1FreeVarsCount, phase2FreeVarsCount, &varsCount]
      (DenseIndex varIdx)
      {
        return (
          varIdx < phase1FreeVarsCount &&
          ++varsCount <= size_t(phase2FreeVarsCount)
        );
      }
    );

    //Fill the coeffs matrix `α' using the Phase-1 tableau
    phase2Tableau.entries_ = Matrix<T, Dynamic, Dynamic>(
      phase2BasicVarsCount + 1, phase2FreeVarsCount + 1
    );

    //Copy only necessary coeffs `α' from the Phase-1 tableau
    //From the columns corresponding to the free variables
    DenseIndex colIdx(0);
    for (DenseIndex j(0); j < phase1Tableau.entries_.cols() - 1; ++j)
    {
      if (colIdx < phase2FreeVarsCount)
      {
        //If it is the column corresponding to the free variable
        if (phase1Tableau.freeVars_[j] < phase1FreeVarsCount)
        {
          //Copy this column w/o the last element to the Phase-2 tableau
          for (DenseIndex i(0); i < phase2Tableau.entries_.rows() - 1; ++i)
          {
            phase2Tableau.entries_(i, colIdx) =
              phase1Tableau.entries_(i, j);
          }
          ++colIdx;
        }
      }
      else
      {
        break;
      }
    }

    //Fill the RHS vector `β' (the last column)
    //Copy its entries from the Phase-1 tableau w/o the last element
    for (DenseIndex i(0); i < phase1Tableau.entries_.rows() - 1; ++i)
    {
      phase2Tableau.entries_(i, phase2Tableau.entries_.cols() - 1) =
        phase1Tableau.entries_(i, phase1Tableau.entries_.cols() - 1);
    }

    //Fill the objective function coeffs `P[j]' (row at the bottom)
    for (DenseIndex j(0); j < phase2Tableau.entries_.cols() - 1; ++j)
    {
      T sum(0); //Dot product of `c' and `-α.col[j]'
      //(free variable x[j] in `α.col[j]' is set to -1,
      //other free vars are set to 0)
      for (DenseIndex i(0); i < phase2Tableau.entries_.rows() - 1; ++i)
      {
        const DenseIndex varIdx(phase2Tableau.basicVars_[i]);
        sum += linearProgramData.objectiveFunctionCoeffs(varIdx) *
               phase2Tableau.entries_(i, j) *
               T(-1);
      }

      const DenseIndex varIdx(phase2Tableau.freeVars_[j]);
      sum += linearProgramData.objectiveFunctionCoeffs(varIdx);

      phase2Tableau.entries_(phase2Tableau.entries_.rows() - 1, j) = sum;
    }

    //Set objective function value P[0] (the rightmost bottom element)
    T sum(0); //Dot product of `c' and `-β'
    //(all free vars in `β' are set to 0)
    for (DenseIndex i(0); i < phase2Tableau.entries_.rows() - 1; ++i)
    {
      const DenseIndex varIdx(phase2Tableau.basicVars_[i]);
      sum += linearProgramData.objectiveFunctionCoeffs(varIdx) *
             phase2Tableau.entries_(i, phase2Tableau.entries_.cols() - 1) *
             T(-1);
    }

    phase2Tableau.entries_(
      phase2Tableau.entries_.rows() - 1,
      phase2Tableau.entries_.cols() - 1
    ) = sum;

    return phase2Tableau;
  }
}


#endif // SIMPLEXTABLEAU_TXX
