#pragma once

#ifndef SIMPLEXTABLEAU_HXX
#define SIMPLEXTABLEAU_HXX


#include <cstdlib>

#include <vector>

#include <QDebug>

#include "eigen3/Eigen/Core"

#include "dantzignumericsolver_fwd.hxx"
#include "dantzignumericsolvercontroller_fwd.hxx"
#include "linearprogramdata.hxx"
#include "optimizationgoaltype.hxx"
#include "solutionphase.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using namespace Eigen;
  using namespace NumericTypes;
  using namespace std;


  template<typename T = Real>
  /**
   * @brief The SimplexTableau class
   */
  class SimplexTableau
  {
    public:
      friend DantzigNumericSolver<T>;

      friend DantzigNumericSolverController<T>;


      [[deprecated("Only for internal usage."
                   " Use factory methods `makePhase...' instead!")]]
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

      Matrix<T, 1, Dynamic> extremePoint() const;

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
       * @brief _phase
       * Marks the phase of the Simplex algorithm
       * for which the tableau was created.
       */
      SolutionPhase _phase;

      /**
       * @brief _basicVars
       * Maps row indices to the basic variables indices.
       */
      vector<DenseIndex> _basicVars;

      /**
       * @brief _freeVars
       * Maps columns indices to the free variables indices.
       */
      vector<DenseIndex> _freeVars;

      /**
       * @brief _entries
       * Stores all the entries of the tableau.
       */
      Matrix<T, Dynamic, Dynamic> _entries;
  };
}


#include "simplextableau.txx"


#endif // SIMPLEXTABLEAU_HXX
