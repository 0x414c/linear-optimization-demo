#ifndef DANTZIGNUMERICSOLVER_TXX
#define DANTZIGNUMERICSOLVER_TXX

#include "dantzignumericsolver.hxx"

#include <functional>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include <QDebug>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Dense"

#include "linearprogramdata.hxx"
#include "linearprogramsolution.hxx"
#include "../misc/eigenextensions.hxx"
#include "../misc/utils.hxx"

using namespace boost;
using namespace Eigen;
using namespace std;
using namespace Utils;

template<typename T>
DantzigNumericSolver<T>::DantzigNumericSolver()
{ }

template<typename T>
/**
 * @brief DantzigNumericSolver<T>::DantzigNumericSolver
 * Solves small-scaled linear optimization problems
 * using the Two-Phase Simplex method.
 * @param linearProgramData
 */
DantzigNumericSolver<T>::DantzigNumericSolver(const LinearProgramData<T>& linearProgramData) :
  _linearProgramData(linearProgramData)
{ }

//TODO: new EqualityConstraint class: Coeffs, Restriction
//TODO: add Constant term to LinearFunction class
//TODO: new SimplexTableau struct
template<typename T>
optional<LinearProgramSolution<T>>
/**
 * @brief DantzigNumericSolver<T>::optimize
 * Finds the minimum value of the given objective linear function
 * on the given feasible region defined by system of linear equations
 *   Ax ⩵ b
 * using the two-phase Simplex method.
 * The simplex tableau is the matrix of the following form:
 * X(l) | x1 x2 .. | b
 * ----------------------
 * ~x1  |          | b1
 * ~x2  |     A    | b2
 *  ..  |          | ..
 * ----------------------
 *      | P1 P2 .. | -P0,
 * where:
 *   `l' is the current iteration index,
 *   `~x[i]' is the basic variables,
 *   `x[j]' is the free variables,
 *   `-P[0]' is the current value of the objective function,
 *   `P[i]' is the objective function coefficients,
 *   `A' is the coefficients matrix,
 *   `b' is the right-hand-side column-vector.
 */
DantzigNumericSolver<T>::solve()
{
  //For result
  optional<LinearProgramSolution<T>> ret;

  //Program params
  const DenseIndex constrsCount(_linearProgramData.constraintsCount());
  const DenseIndex varsCount(_linearProgramData.variablesCount());

  //Initialize variables indices storage
  _freeVars = vector<DenseIndex>(varsCount);
  for (DenseIndex i(0); i < varsCount; ++i)
  {
    _freeVars[i] = i;
  }
//  LOG("X :=\n{0}", _freeVars);
  _basicVars = vector<DenseIndex>(constrsCount);
  for (DenseIndex i(varsCount); i < varsCount + constrsCount; ++i)
  {
    _basicVars[i] = i;
  }
//  LOG("~X :=\n{0}", _basicVars);

  //Let's build an auxilliary program tableau `X~'
  Matrix<T, Dynamic, Dynamic> auxTableau(constrsCount + 1, varsCount + 1);
  auxTableau <<
    _linearProgramData.constraintsCoefficients,
    _linearProgramData.constraintsRHS,
    Matrix<T, 1, Dynamic>::Zero(1, auxTableau.cols());
  //Fill bottom row
  for (DenseIndex j(0); j < auxTableau.cols(); ++j)
  {
    auxTableau(auxTableau.rows() - 1, j) = -(auxTableau.
                                             col(j).
                                             head(constrsCount).
                                             sum());
  }
  LOG("c := \n{0}\nA :=\n{1}\nb :=\n{2}",
      _linearProgramData.objectiveFunctionCoefficients,
      _linearProgramData.constraintsCoefficients,
      _linearProgramData.constraintsRHS);
  LOG("X~({0}) :=\n{1}", _iterCount, auxTableau);

  //Solve Phase-1 do iterate() while not optimal
  const SolutionType phase1SolutionType(optimize(auxTableau));
  LOG("X~{0} :=\n{1}", _iterCount, auxTableau);
  if (phase1SolutionType == SolutionType::Optimal)
  {
    //Recheck Phase-1 solution (f* ⩵ 0)...
    const SolutionType auxSolutionType(checkPhase1Solution(auxTableau));
//    if (auxSolutionType == SolutionType::Optimal)
//    {
//      Matrix<T, Dynamic, Dynamic> tableau(constrsCount + 1, varsCount - constrsCount + 1);
//      tableau <<
//        Matrix<T, Dynamic, Dynamic>::Zero(constrsCount, varsCount - constrsCount),
//        auxTableau.col(auxTableau.cols() -1),
//        Matrix<T, 1, Dynamic>::Zero(1, constrsCount + 1);
//      //Fill the objective function row
//      for (DenseIndex j(0); j < tableau.cols() - 1; ++j)
//      {
//        tableau(tableau.rows() - 1, j) = -(_linearProgramData.objFuncCoeffs(j) *
//                                         tableau.
//                                         col(j).
//                                         head(tableau.rows() - 1).
//                                         sum());
//      }
//      //Set obj. function value
//      tableau(tableau.rows() - 1, tableau.cols() - 1) = tableau.
//                                                        col(tableau.cols() - 1).
//                                                        head(tableau.rows() - 1).
//                                                        sum();
//      //Fill the coeffs part
////      for (DenseIndex i(0); i < tableau.rows() - 1; ++i)
////      {
////        for (DenseIndex j(0); j < tableau.cols() - 1; ++j)
////        {
////          tableau(i, j) = auxTableau(i, j);
////        }
////      }
//      //Fill the RHS
//      for (DenseIndex i(0); i < tableau.rows() - 1; ++i)
//      {
//        tableau(i, tableau.cols() - 1) = auxTableau(i, auxTableau.cols() - 1);
//      }
//      LOG("X{0} :=\n{1}", _iterCount, tableau);

//      //TODO: Solve Phase-2: do iterate() while not optimal
//      const SolutionType phase2SolutionType(optimize(tableau));
//      LOG("X{0} :=\n{1}", _iterCount, tableau);
//      if (phase2SolutionType == SolutionType::Optimal)
//      {
//        //TODO: Recheck Phase-2 solution (x >= (0)) and return solution if it is valid
//        const SolutionType phase2SolutionType(checkPhase2Solution(tableau));
//        if (phase2SolutionType == SolutionType::Optimal)
//        {
//          LinearProgramSolution<T> linearProgramSolution;
//          LOG("X* == {0}, F* == {1}", linearProgramSolution.extremePoint,
//              linearProgramSolution.extremeValue);

//          ret = linearProgramSolution;
//        }
//      }
//    }
  }

  return ret;
}

template<typename T>
/**
 * @brief DantzigNumericSolver<T>::setLinearProgramData
 * Updates source data.
 * @param linearProgramData
 */
void DantzigNumericSolver<T>::setLinearProgramData(const LinearProgramData<T>& linearProgramData)
{
  _linearProgramData = linearProgramData;
}

template<typename T>
/**
 * @brief DantzigNumericSolver<T>::optimize
 * Starts the Simplex algorithm on the given tableau.
 * @param tableau
 * @return
 */
SolutionType DantzigNumericSolver<T>::optimize(Matrix<T, Dynamic, Dynamic>& tableau)
{
  while (true) //Iterate while the solution is incomplete
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
SolutionType DantzigNumericSolver<T>::iterate(Matrix<T, Dynamic, Dynamic>& tableau)
{
  if (++_iterCount > _maxIterations)
  {
    return SolutionType::Unknown;
  }
  else
  {
    const optional<DenseIndex> pivotColIdx(computePivotColIdx(tableau));
    if (pivotColIdx) //If (∃s: P[s] < 0)
    {
      const optional<DenseIndex> pivotRowIdx(computePivotRowIdx(tableau, *pivotColIdx));
      if (pivotRowIdx) //If (∃s: P[s] < 0) ∧ (∃i: A[i, s] > 0),
                       //the pivoting operation can be performed on the tableau
      {
        pivotize(tableau, *pivotRowIdx, *pivotColIdx);

        return SolutionType::Incomplete;
      }
      else //If (∃s: P[s] < 0) ∧ (∀i: A[i, s] <= 0),
           //the objective function is unbounded on the feasible region
      {
        return SolutionType::Unbounded;
      }
    }
    else //If (∀j: P[j] >= 0), the current iteration is the last one
         //and the solution is optimal
    {
      return SolutionType::Optimal;
    }
  }
}

template<typename T>
/**
 * @brief DantzigNumericSolver<T>::pivotize
 * Performs pivoting operations for the selected
 * row and column on the given tableau.
 * @param tableau
 * @param rowIdx
 * @param colIdx
 */
void DantzigNumericSolver<T>::pivotize(Matrix<T, Dynamic, Dynamic>& tableau,
                                       DenseIndex rowIdx, DenseIndex colIdx)
{
  const T pivotElement(tableau(rowIdx, colIdx)); //Store the pivot element
  LOG("rowIdx := {0}, colIdx := {1}, pivot := {2}", rowIdx, colIdx, pivotElement);

  tableau.row(rowIdx) /= pivotElement; //For the new row at the pivot position
  tableau(rowIdx, colIdx) = T(1) / pivotElement;

  for (DenseIndex i(0); i < rowIdx; ++i) //For each row above and below the pivot row
  {
    const T factor(tableau(i, colIdx));
    tableau.row(i) -= factor * tableau.row(rowIdx); //(row) = (row) - (factor) * (row)
  }
  for (DenseIndex i(rowIdx + 1); i < tableau.rows(); ++i)
  {
    const T factor(tableau(i, colIdx));
    tableau.row(i) -= factor * tableau.row(rowIdx);
  }

  tableau.col(colIdx) /= (-pivotElement); //For the column at the pivot position
  tableau(rowIdx, colIdx) = T(1) / pivotElement; //Restore the pivot element value

  LOG("X({0}) :=\n{1}", _iterCount, tableau);
}

template<typename T>
/**
 * @brief DantzigNumericSolver<T>::checkPhase1Solution
 * Checks if solution of Phase-1 is valid.
 * @param tableau
 * @return
 */
SolutionType DantzigNumericSolver<T>::checkPhase1Solution(
  const Matrix<T, Dynamic, Dynamic>& tableau) const
{
  const T objFuncValue(tableau(tableau.rows() - 1, tableau.cols() - 1));
  if (isEqualToZero<T>(objFuncValue)) //If (f~)* ⩵ 0 -- The end of the Phase-1
  {
    return SolutionType::Optimal;
  }
  else
  {
    if (isGreaterThanZero<T>(objFuncValue)) //If (f~)* > 0 -- Inconsistent
    {
      return SolutionType::Inconsistent;
    }
    else
    {
      if (isLessThanZero<T>(objFuncValue)) //If (f~)* < 0 -- Oops... smth went wrong...
      {
        return SolutionType::Unknown;
      }
    }
  }
}

template<typename T>
/**
 * @brief DantzigNumericSolver<T>::checkPhase2Solution
 * Check if solution of Phase-2 is valid.
 * @param tableau
 * @return `true' if b >= (0), `false' otherwise.
 */
SolutionType DantzigNumericSolver<T>::checkPhase2Solution(
  const Matrix<T, Dynamic, Dynamic>& tableau) const
{
  if (
    (tableau.col(tableau.cols() - 1).
    head(tableau.rows() - 1).
    unaryExpr(ref(isGreaterOrEqualToZero<T>))
    ).all()
  )
  {
    return SolutionType::Inconsistent;
  }
  else
  {
    return SolutionType::Optimal;
  }
}

template<typename T>
/**
 * @brief DantzigNumericSolver<T>::computePivotColIdx
 * @param tableau
 * @return
 */
optional<DenseIndex> DantzigNumericSolver<T>::computePivotColIdx(
    const Matrix<T, Dynamic, Dynamic>& tableau) const
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
 * @brief DantzigNumericSolver<T>::isPivotColValid
 * @param tableau
 * @param colIdx
 * @return
 */
bool DantzigNumericSolver<T>::isPivotColValid(const Matrix<T, Dynamic, Dynamic>& tableau,
                                              DenseIndex colIdx) const
{
  return (
    tableau.
    col(colIdx).
    head(tableau.rows() - 1).
    unaryExpr(ref(isGreaterThanZero<T>))
  ).any();
}

template<typename T>
/**
 * @brief DantzigNumericSolver<T>::computePivotRowIdx
 * Computes pivot row index for the given pivot column index.
 * @param tableau
 * @param pivotColIdx
 * @param forceToLeaveBasis
 * @return
 */
optional<DenseIndex> DantzigNumericSolver<T>::computePivotRowIdx(
  const Matrix<T, Dynamic, Dynamic>& tableau,
  DenseIndex pivotColIdx,
  bool forceToLeaveBasis) const
{
  optional<DenseIndex> ret;
  T minRatio(NumericLimits::max<T>());
  DenseIndex minRatioIdx(0);
  bool isPivotColValid(false);

  //Find the pivot row for the given column
  for (DenseIndex rowIdx(0); rowIdx < tableau.rows() - 1; ++rowIdx)
  {
    //Pick only non-negative elements
    if (isGreaterThanZero<T>(tableau(rowIdx, pivotColIdx)))
    {
      //Compute ratio b(i) / A(i, p)
      const T currRatio((tableau(rowIdx, tableau.cols() - 1) / tableau(rowIdx, pivotColIdx)));
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
    if (forceToLeaveBasis)
    {
      //TODO: !
    }

    ret = minRatioIdx;
  }

  return ret;
}

#endif // DANTZIGNUMERICSOLVER_TXX
