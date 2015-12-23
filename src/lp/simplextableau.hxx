#pragma once

#ifndef SIMPLEXTABLEAU_HXX
#define SIMPLEXTABLEAU_HXX


#include <cstdlib>

#include <vector>

#include "eigen3/Eigen/Core"

#include "dantzignumericsolver_fwd.hxx"
#include "dantzignumericsolvercontroller_fwd.hxx"
#include "linearprogramdata.hxx"
#include "optimizationgoaltype.hxx"
#include "solutionphase.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using Eigen::Block;
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using NumericTypes::Real;
  using std::invalid_argument;
  using std::vector;


  template<typename T = Real>
  /**
   * @brief The SimplexTableau class
   */
  class SimplexTableau
  {
    public:
      friend DantzigNumericSolver<T>;

      friend DantzigNumericSolverController<T>;


      [[deprecated(
        "Only for internal usage. Use factory methods `makePhase...' instead!"
      )]]
      SimplexTableau();

      SimplexTableau(const SimplexTableau<T>& simplexTableau);
      SimplexTableau(SimplexTableau<T>&& simplexTableau);

      T& operator ()(DenseIndex rowIdx, DenseIndex colIdx);
      const T& operator ()(DenseIndex rowIdx, DenseIndex colIdx) const;

      Block<Matrix<T, Dynamic, Dynamic>, Dynamic, Dynamic> entries();
      const Block<const Matrix<T, Dynamic, Dynamic>, Dynamic, Dynamic>
      entries() const;

      typename
      Matrix<T, Dynamic, Dynamic>::RowXpr row(DenseIndex rowIdx);
      typename
      Matrix<T, Dynamic, Dynamic>::ConstRowXpr row(DenseIndex rowIdx) const;

      typename
      Matrix<T, Dynamic, Dynamic>::ColXpr col(DenseIndex colIdx);
      typename
      Matrix<T, Dynamic, Dynamic>::ConstColXpr col(DenseIndex colIdx) const;

      DenseIndex rows() const;

      DenseIndex cols() const;

      SolutionPhase phase() const;

      vector<DenseIndex>& basicVars();
      const vector<DenseIndex>& basicVars() const;

      vector<DenseIndex>& freeVars();
      const vector<DenseIndex>& freeVars() const;

      size_t basicVarsCount() const;

      size_t freeVarsCount() const;

      Matrix<T, Dynamic, 1> extremePoint() const;

      T extremeValue(
        OptimizationGoalType goalType = OptimizationGoalType::Minimize
      ) const;


      static SimplexTableau<T> makePhaseOne(
        const LinearProgramData<T>& linearProgramData
      );

      static SimplexTableau<T> makePhaseTwo(
        const LinearProgramData<T>& linearProgramData,
        const SimplexTableau<T>& phase1Tableau
      ) throw(invalid_argument);


    private:
      /**
       * @brief phase_
       * Marks the phase of the Simplex algorithm
       * for which the tableau was created.
       */
      SolutionPhase phase_;

      /**
       * @brief basicVars_
       * Maps row indices to the basic variables indices.
       */
      vector<DenseIndex> basicVars_;

      /**
       * @brief freeVars_
       * Maps columns indices to the free variables indices.
       */
      vector<DenseIndex> freeVars_;

      /**
       * @brief entries_
       * Stores all the entries of the tableau.
       */
      Matrix<T, Dynamic, Dynamic> entries_;
  };
}


#include "simplextableau.txx"


#endif // SIMPLEXTABLEAU_HXX
