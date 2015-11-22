#pragma once

#ifndef DANTZIGNUMERICSOLVER_HXX
#define DANTZIGNUMERICSOLVER_HXX


#include <cstdint>

#include <map>
#include <utility>
#include <vector>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "dantzignumericsolvercontroller_fwd.hxx"
#include "inumericsolver.hxx"
#include "linearprogramdata.hxx"
#include "solutiontype.hxx"
#include "simplextableau_fwd.hxx"
#include "../math/numerictypes.hxx"
#include "../config.hxx"


namespace LinearProgramming
{
  using namespace boost;
  using namespace Config::LinearProgramming;
  using namespace Eigen;
  using namespace NumericTypes;
  using namespace std;

  using Maybe2DIndex = optional<pair<DenseIndex, DenseIndex>>;
  using Maybe1DIndex = optional<DenseIndex>;


  template<typename T = Real>
  class DantzigNumericSolver :
    public virtual INumericSolver<T>
  {
    public:
      friend DantzigNumericSolverController<T>;


      DantzigNumericSolver()/* = delete*/;

      explicit DantzigNumericSolver(
        const LinearProgramData<T>& linearProgramData
      );
      explicit DantzigNumericSolver(LinearProgramData<T>&& linearProgramData);

      const LinearProgramData<T>& linearProgramData() const;

      void setLinearProgramData(const LinearProgramData<T>& linearProgramData);
      void setLinearProgramData(LinearProgramData<T>&& linearProgramData);

      virtual optional<LinearProgramSolution<T>> solve() override;


    private:
      LinearProgramData<T> _linearProgramData;

      uint16_t _iterCount;


      void reset();

      SolutionType optimize(SimplexTableau<T>& tableau);

      SolutionType iterate(SimplexTableau<T>& tableau);

      pair<SolutionType, Maybe2DIndex>
      computePivotIdx(const SimplexTableau<T>& tableau) const;

      Maybe1DIndex computePivotColIdx(
        const SimplexTableau<T>& tableau
      ) const;

      Maybe1DIndex computePivotRowIdx(
        const SimplexTableau<T>& tableau, DenseIndex pivotColIdx
      ) const;

      void pivotize(
        SimplexTableau<T>& tableau, DenseIndex rowIdx, DenseIndex colIdx
      );

      bool isPivotColValid(
        const SimplexTableau<T>& tableau, DenseIndex colIdx
      ) const;

      SolutionType checkPhase1Solution(const SimplexTableau<T>& tableau) const;

      SolutionType checkPhase2Solution(const SimplexTableau<T>& tableau) const;
  };
}


#include "dantzignumericsolver.txx"


#endif // DANTZIGNUMERICSOLVER_HXX
