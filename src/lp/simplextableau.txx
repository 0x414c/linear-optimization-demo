#pragma once

#ifndef SIMPLEXTABLEAU_TXX
#define SIMPLEXTABLEAU_TXX


#include "simplextableau.hxx"

#include <cstdlib>

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
    _phase(simplexTableau._phase),
    _basicVars(simplexTableau._basicVars),
    _freeVars(simplexTableau._freeVars),
    _entries(simplexTableau._entries)
  { }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::SimplexTableau
   * Move-ctor.
   * @param simplexTableau
   */
  SimplexTableau<T>::SimplexTableau(SimplexTableau<T>&& simplexTableau) :
    _phase(simplexTableau._phase),
    _basicVars(std::move(simplexTableau._basicVars)),
    _freeVars(std::move(simplexTableau._freeVars)),
    _entries(std::move(simplexTableau._entries))
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
    return _entries(rowIdx, colIdx);
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
    return _entries(rowIdx, colIdx);
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::entries
   * Provides read-write access to the tableau entries matrix.
   * TODO: ~? Use `ColsBlockXpr' or `RowsBlockXpr'.
   * @return
   */
  Block<Matrix<T, Dynamic, Dynamic>, Dynamic, Dynamic>
  SimplexTableau<T>::entries()
  {
    return _entries.block(0, 0, _entries.rows(), _entries.cols());
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
    return _entries.block(0, 0, _entries.rows(), _entries.cols());
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
    return _entries.row(rowIdx);
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
    return _entries.row(rowIdx);
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
    return _entries.col(colIdx);
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
    return _entries.col(colIdx);
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
    return _entries.rows();
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
    return _entries.cols();
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
    return _phase;
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
    return _basicVars;
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
    return _basicVars;
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
    return _freeVars;
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
    return _freeVars;
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
    return _basicVars.size();
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
    return _freeVars.size();
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
    Matrix<T, Dynamic, 1> x_(_basicVars.size() + _freeVars.size(), 1);

    for (DenseIndex i(0); i < _entries.rows() - 1; ++i)
    {
      const DenseIndex varIdx(_basicVars[i]);
      x_(varIdx) = _entries(i, _entries.cols() - 1);
    }

    for (DenseIndex j(0); j < _entries.cols() - 1; j++)
    {
      const DenseIndex varIdx(_freeVars[j]);
      x_(varIdx) = T(0);
    }

    return x_;
  }


  template<typename T>
  /**
   * @brief SimplexTableau<T>::extremeValue
   * @return the current value `w*' of the objective function `w'.
   */
  T
  SimplexTableau<T>::extremeValue(OptimizationGoalType goalType) const
  {
    switch (goalType) {
      case OptimizationGoalType::Minimize:
        return _entries(_entries.rows() - 1, _entries.cols() - 1) * T(-1);

      case OptimizationGoalType::Maximize:
        return _entries(_entries.rows() - 1, _entries.cols() - 1);

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
   *   `-P[0]' is the current value of the objective function (aka `w*'),
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
    phase1Tableau._phase = SolutionPhase::One;

    //Initialize vectors w/ variables indices
    phase1Tableau._basicVars = vector<DenseIndex>(basicVarsCount);
    //For the basic artificial vars `~x[i]' where `i' in [N; N + M)
    for (DenseIndex i(freeVarsCount); i < freeVarsCount + basicVarsCount; ++i)
    {
      phase1Tableau._basicVars[i - freeVarsCount] = i;
    }

    phase1Tableau._freeVars = vector<DenseIndex>(freeVarsCount);
    //For the free decision vars `x[i]' where i in [0; N)
    for (DenseIndex i(0); i < freeVarsCount; ++i)
    {
      phase1Tableau._freeVars[i] = i;
    }

    //Let's build an auxilliary program tableau `~X(0)'
    //Initialize some of its entries (constraints coeffs matrix `α'
    //and right-hand-side column-vector `β' ) directly
    //from `linearProgramData'. `P' is left unintialized.
    phase1Tableau._entries = Matrix<T, Dynamic, Dynamic>(
      basicVarsCount + 1, freeVarsCount + 1
    );
    phase1Tableau._entries.block(0, 0, basicVarsCount, freeVarsCount) =
    linearProgramData.constraintsCoeffs;
    phase1Tableau._entries.block(0, freeVarsCount, basicVarsCount, 1) =
    linearProgramData.constraintsRHS;

    //Further, we need to multiply some equations by (-1) to make (β >= (0))
    for (DenseIndex i(0); i < phase1Tableau._entries.rows(); ++i)
    {
      if (
        isLessThanZero<T>(
          phase1Tableau._entries(i, phase1Tableau._entries.cols() - 1)
        )
      )
      {
        phase1Tableau._entries.row(i) *= T(-1);
      }
    }

    //Now, set values for the bottom row containing
    //objective function coeffs `P[j]' and func. value `P[0]'
    for (DenseIndex j(0); j < phase1Tableau._entries.cols(); ++j)
    {
      //Each `P[j]' is the negative sum of the column `α.col[j]' above it
      phase1Tableau._entries(phase1Tableau._entries.rows() - 1, j) =
      phase1Tableau._entries.col(j).head(basicVarsCount).sum() * T(-1);
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
    if (phase1Tableau._phase != SolutionPhase::One)
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
    phase2Tableau._phase = SolutionPhase::Two;

    //Initialize vectors w/ variables indices
    //Copy all the basic variables `~x' from the Phase-1 tableau
    phase2Tableau._basicVars = vector<DenseIndex>(phase2BasicVarsCount);
    std::copy(
      phase1Tableau._basicVars.begin(),
      phase1Tableau._basicVars.end(),
      phase2Tableau._basicVars.begin()
    );

    //Copy all the free variables `x[i]' except those vars
    //that was forced out of the basis at the Phase-1 (artificial)
    phase2Tableau._freeVars = vector<DenseIndex>(phase2FreeVarsCount);
    size_t varsCount(0);
    std::copy_if(
      phase1Tableau._freeVars.begin(),
      phase1Tableau._freeVars.end(),
      phase2Tableau._freeVars.begin(),
      [phase1FreeVarsCount, phase2FreeVarsCount, &varsCount]
      (DenseIndex varIdx)
      {
        return (
          varIdx < phase1FreeVarsCount &&
          ++varsCount <= phase2FreeVarsCount
        );
      }
    );

//    phase2Tableau._freeVars = vector<DenseIndex>(0);
//    phase2Tableau._freeVars.reserve(
//      phase1FreeVarsCount - phase2BasicVarsCount
//    );
//    for (DenseIndex j(0); j < phase1Tableau._entries.cols() - 1; ++j)
//    {
//      if (
//        isEqualToZero<T>(
//          phase1Tableau._entries(phase1Tableau._entries.rows() - 1, j)
//        )
//      )
//      {
//        phase2Tableau._freeVars.emplace_back(phase1Tableau._freeVars[j]);
//      }
//    }
//    phase2FreeVarsCount = phase2Tableau._freeVars.size();

    //Fill the coeffs matrix `α' using the Phase-1 tableau
    phase2Tableau._entries = Matrix<T, Dynamic, Dynamic>(
      phase2BasicVarsCount + 1, phase2FreeVarsCount + 1
    );

    //Copy only necessary coeffs `α' from the Phase-1 tableau
    //From the columns corresponding to the free variables
    DenseIndex colIdx(0);
    for (DenseIndex j(0); j < phase1Tableau._entries.cols() - 1; ++j)
    {
      if (colIdx < phase2FreeVarsCount)
      {
        //If it is the column corresponding to the free variable
        if (phase1Tableau._freeVars[j] < phase1FreeVarsCount)
        {
          //Copy this column w/o the last element to the Phase-2 tableau
          for (DenseIndex i(0); i < phase2Tableau._entries.rows() - 1; ++i)
          {
            phase2Tableau._entries(i, colIdx) =
            phase1Tableau._entries(i, j);
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
    for (DenseIndex i(0); i < phase1Tableau._entries.rows() - 1; ++i)
    {
      phase2Tableau._entries(i, phase2Tableau._entries.cols() - 1) =
      phase1Tableau._entries(i, phase1Tableau._entries.cols() - 1);
    }

    //Fill the objective function coeffs `P[j]' (row at the bottom)
    for (DenseIndex j(0); j < phase2Tableau._entries.cols() - 1; ++j)
    {
      T sum(0); //Dot product of `c' and `-α.col[j]'
      //(free variable x[j] in `α.col[j]' is set to -1,
      //other free vars are set to 0)
      for (DenseIndex i(0); i < phase2Tableau._entries.rows() - 1; ++i)
      {
        const DenseIndex varIdx(phase2Tableau._basicVars[i]);
        sum += linearProgramData.objectiveFunctionCoeffs(varIdx) *
               phase2Tableau._entries(i, j) *
               T(-1);
      }

      const DenseIndex varIdx(phase2Tableau._freeVars[j]);
      sum += linearProgramData.objectiveFunctionCoeffs(varIdx);

      phase2Tableau._entries(phase2Tableau._entries.rows() - 1, j) = sum;
    }

    //Set objective function value P[0] (the rightmost bottom element)
    T sum(0); //Dot product of `c' and `-β'
    //(all free vars in `β' are set to 0)
    for (DenseIndex i(0); i < phase2Tableau._entries.rows() - 1; ++i)
    {
      const DenseIndex varIdx(phase2Tableau._basicVars[i]);
      sum += linearProgramData.objectiveFunctionCoeffs(varIdx) *
             phase2Tableau._entries(i, phase2Tableau._entries.cols() - 1) *
             T(-1);
    }

    phase2Tableau._entries(
      phase2Tableau._entries.rows() - 1,
      phase2Tableau._entries.cols() - 1
    ) = sum;

    return phase2Tableau;
  }
}


#endif // SIMPLEXTABLEAU_TXX
