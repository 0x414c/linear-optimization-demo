#ifndef SIMPLEXTABLEAU_TXX
#define SIMPLEXTABLEAU_TXX

#include <cstdlib>

#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include <QDebug>
#include <QDebugStateSaver>

#include "eigen3/Eigen/Core"

#include "linearprogramdata.hxx"
#include "simplextableau.hxx"
#include "../math/mathutils.hxx"

namespace LinearProgramming
{
  using namespace Eigen;
  using namespace MathUtils;
  using namespace std;

  template<typename T>
  /**
   * @brief SimplexTableau<T>::SimplexTableau
   * Default ctor.
   * NOTE: It leaves struct members uninitialized.
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
   * @return
   */
  Block<Matrix<T, Dynamic, Dynamic>>
//  Matrix<T, Dynamic, Dynamic>&
  SimplexTableau<T>::entries()
  {
    return _entries.block(0, 0, _entries.rows(), _entries.cols());
//    return _entries;
  }

  template<typename T>
  /**
   * @brief SimplexTableau<T>::entries
   * Provides read-only access to the tableau entries matrix.
   * @return
   */
  Block<const Matrix<T, Dynamic, Dynamic>>
//  const Matrix<T, Dynamic, Dynamic>&
  SimplexTableau<T>::entries() const
  {
    return _entries.block(0, 0, _entries.rows(), _entries.cols());
//    return _entries;
  }

  template<typename T>
  /**
   * @brief SimplexTableau<T>::row
   * Provides read-write access to the particular tableau row.
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
   * Returns the current phase of the Simplex algorithm
   * for which this tableau is created.
   * @return
   */
  Phase
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
   * Provides read-only access to the free variables indices `x[i]'.
   * @return
   */
  const vector<DenseIndex>&
  SimplexTableau<T>::freeVars() const
  {
    return _freeVars;
  }

  template<typename T>
  /**
   * @brief SimplexTableau<T>::basicVariablesCount
   * Returns basic variables count `M'.
   * @return
   */
  size_t
  SimplexTableau<T>::basicVarsCount() const
  {
    return _basicVars.size();
  }

  template<typename T>
  /**
   * @brief SimplexTableau<T>::freeVariablesCount
   * Returns free variables count `N'.
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
  Matrix<T, 1, Dynamic>
  SimplexTableau<T>::extremePoint() const
  {
    Matrix<T, 1, Dynamic> x(1, basicVarsCount() + freeVarsCount());

    for (DenseIndex i(0); i < _entries.rows() - 1; ++i)
    {
      const DenseIndex varIdx(_basicVars[i]);
      x(varIdx) = _entries(i, _entries.cols() - 1);
    }

    for (DenseIndex j(0); j < _entries.cols() - 1; j++)
    {
      const DenseIndex varIdx(_freeVars[j]);
      x(varIdx) = T(0);
    }

    return x;
  }

  template<typename T>
  /**
   * @brief SimplexTableau<T>::extremeValue
   * @return the current value `w*' of the objective function `w'.
   */
  T
  SimplexTableau<T>::extremeValue() const
  {
    return _entries(_entries.rows() - 1, _entries.cols() - 1);
  }

  template<typename T>
  /**
   * @brief SimplexTableau<T>::SimplexTableau
   * This factory method constructs the Phase-1 simplex tableau
   * f/ the auxilliary program.
   *
   * The Phase-1 simplex tableau is the matrix
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
   *   `-P[0]' is the current value of the objective function (aka w*),
   *   `P[i]' is the i-th objective function coefficient (aka c[i]),
   *   `α' is the constraints coefficients matrix,
   *   `α[i, j]' is the particular coefficient of α,
   *   `β' is the right-hand-side column-vector,
   *   `β[j]' is the particular coefficient of β,
   *   `M' is the constraints count
   *       (and it is the basic artificial variables count),
   *   `N' is the variables count (and is the free decision variables count),
   *    and M <= N.
   * @param linearProgramData
   */
  SimplexTableau<T>
  SimplexTableau<T>::makePhase1(const LinearProgramData<T> &linearProgramData)
  {
    //Count of ~x (M)
    const DenseIndex basicVarsCount(linearProgramData.constraintsCount());
    //Count of x (N)
    const DenseIndex freeVarsCount(linearProgramData.variablesCount());

    //Make a new tableau
    SimplexTableau<T> phase1Tableau;

    //Set phase to 1
    phase1Tableau._phase = Phase::One;

    //Initialize vectors w/ variables indices
    phase1Tableau._basicVars = vector<DenseIndex>(basicVarsCount);
    //For i in [N; N + M)
    for (DenseIndex i(freeVarsCount); i < freeVarsCount + basicVarsCount; ++i)
    {
      phase1Tableau._basicVars[i - freeVarsCount] = i;
    }

    phase1Tableau._freeVars = vector<DenseIndex>(freeVarsCount);
    //For i in [0; N)
    for (DenseIndex i(0); i < freeVarsCount; ++i)
    {
      phase1Tableau._freeVars[i] = i;
    }

    //Let's build an auxilliary program tableau ~X
    phase1Tableau._entries = Matrix<T, Dynamic, Dynamic>(
      basicVarsCount + 1, freeVarsCount + 1
    );
    phase1Tableau._entries <<
      linearProgramData.constraintsCoeffs,
      linearProgramData.constraintsRHS,
      Matrix<T, 1, Dynamic>::Zero(1, phase1Tableau._entries.cols());

    //We need to multiply some equations by (-1) to make (β>=(0))
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

    //Fill the bottom row w/ objective function coeffs and value
    for (DenseIndex j(0); j < phase1Tableau._entries.cols(); ++j)
    {
      //Each P[j] is the negative sum of the column α.col[j] above it
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
  SimplexTableau<T>::makePhase2(
    const LinearProgramData<T>& linearProgramData,
    const SimplexTableau<T>& phase1Tableau
  ) throw(invalid_argument)
  {
    if (phase1Tableau._phase != Phase::One)
    {
      throw invalid_argument("Tableau is not in Phase-1");
    }

    //Count of x (N)
    const DenseIndex phase1FreeVarsCount(phase1Tableau.freeVarsCount());
    //Count of ~x (M) in Tableau-1 is equal to count of ~x in Tableau-2
    const DenseIndex phase2BasicVarsCount(phase1Tableau.basicVarsCount());
    //Exclude M artificial variables (N - M)
    const DenseIndex phase2FreeVarsCount(
      phase1FreeVarsCount - phase2BasicVarsCount
    );

    //Make a new tableau
    SimplexTableau<T> phase2Tableau;

    //Set phase to 2
    phase2Tableau._phase = Phase::Two;

    //Initialize vectors w/ variables indices
    //Copy all basic variables from Tableau-1
    phase2Tableau._basicVars = vector<DenseIndex>(phase2BasicVarsCount);
    std::copy(
      phase1Tableau._basicVars.begin(),
      phase1Tableau._basicVars.end(),
      phase2Tableau._basicVars.begin()
    );
    LOG("{}", makeString(phase2Tableau._basicVars));

    //Copy only those x[i] where i from range [0, ~N)
    //and skip the others (~x[i] -- artificial)
    phase2Tableau._freeVars = vector<DenseIndex>(phase2FreeVarsCount);
    size_t sentry(0);
    std::copy_if(
      phase1Tableau._freeVars.begin(),
      phase1Tableau._freeVars.end(),
      phase2Tableau._freeVars.begin(),
      [phase1FreeVarsCount, phase2FreeVarsCount, &sentry]
      (DenseIndex idx)
      {
        return (
          idx < phase1FreeVarsCount &&
          ++sentry <= phase2FreeVarsCount
        );
      }
    );
    LOG("{}", makeString(phase2Tableau._freeVars));

    //Fill the coeffs matrix α: copy only cols corresponding to
    //free vars x[j] where j from range [0, ~N)
    phase2Tableau._entries = Matrix<T, Dynamic, Dynamic>(
      phase2BasicVarsCount + 1, phase2FreeVarsCount + 1
    );
    DenseIndex decisionColIdx(0);
    for (DenseIndex j(0); j < phase1Tableau._entries.cols() - 1; ++j)
    {
      //If it is the column corresponding to the decision variable
      if (phase1Tableau._freeVars[j] < phase1FreeVarsCount)
      {
        //Copy this column w/o the last element to the Phase-2 tableau
        for (DenseIndex i(0); i < phase2Tableau._entries.rows() - 1; ++i)
        {
          phase2Tableau._entries(i, decisionColIdx) =
          phase1Tableau._entries(i, j);
        }
        if (++decisionColIdx >= phase2FreeVarsCount)
        {
          break;
        }
      }
    }
    LOG("\n{}", phase2Tableau._entries);

    //Fill the RHS vector β (the last column),
    //copy it from the Phase-1 tableau the last element
    for (DenseIndex i(0); i < phase1Tableau._entries.rows() - 1; ++i)
    {
      phase2Tableau._entries(i, phase2Tableau._entries.cols() - 1) =
      phase1Tableau._entries(i, phase1Tableau._entries.cols() - 1);
    }

    //Fill the objective function coeffs P[j] (row at the bottom)
    for (DenseIndex j(0); j < phase2Tableau._entries.cols() - 1; ++j)
    {
      T sum(0); //Dot product of c and -α.col[j]
                //(we assume that free vars in α.col[j] are equal to -1)
      for (DenseIndex i(0); i < phase2Tableau._entries.rows() - 1; ++i)
      {
        const DenseIndex varIdx(phase2Tableau._basicVars[i]);
        sum += linearProgramData.objectiveFuncCoeffs(varIdx) *
               phase2Tableau._entries(i, j) * T(-1);
      }
      for (DenseIndex i(0); i < phase2Tableau._entries.cols() - 1; ++i)
      {
        DenseIndex varIdx(phase2Tableau._freeVars[i]);
        sum += linearProgramData.objectiveFuncCoeffs(varIdx);
      }
      phase2Tableau._entries(phase2Tableau._entries.rows() - 1, j) = sum;
    }
    LOG("\n{}", phase2Tableau._entries);

    //Set objective function value P[0] (the rightmost bottom element)
    T sum(0); //Dot product of c and β
              //(we assume that free vars in β are eq. to 0)
    for (DenseIndex i(0); i < phase2Tableau._entries.rows() - 1; ++i)
    {
      const DenseIndex varIdx(phase2Tableau._basicVars[i]);
      sum += linearProgramData.objectiveFuncCoeffs(varIdx) *
             phase2Tableau._entries(i, phase2Tableau._entries.cols() - 1);
    }
    phase2Tableau._entries(
      phase2Tableau._entries.rows() - 1,
      phase2Tableau._entries.cols() - 1
    ) = sum * T(-1);
    LOG("{}", phase2Tableau._entries);

    return phase2Tableau;
  }
}

#endif // SIMPLEXTABLEAU_TXX
