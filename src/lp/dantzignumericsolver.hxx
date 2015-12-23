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
#include "simplextableau_fwd.hxx"
#include "solutiontype.hxx"
#include "../math/numerictypes.hxx"
#include "../config.hxx"


namespace LinearProgramming
{
  using boost::optional;
  using namespace Config::LinearProgramming;
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using NumericTypes::Rational;
  using NumericTypes::Real;
  using std::pair;

  using MaybeIndex2D = optional<pair<DenseIndex, DenseIndex>>;
  using MaybeIndex1D = optional<DenseIndex>;


  template<typename T = Real>
  class DantzigNumericSolver :
    public INumericSolver<LinearProgramSolution<T>>
  {
    public:
      friend DantzigNumericSolverController<T>;


      DantzigNumericSolver() = default;

      explicit DantzigNumericSolver(
        const LinearProgramData<T>& linearProgramData
      );
      explicit DantzigNumericSolver(LinearProgramData<T>&& linearProgramData);

      const LinearProgramData<T>& linearProgramData() const;

      void setLinearProgramData(const LinearProgramData<T>& linearProgramData);
      void setLinearProgramData(LinearProgramData<T>&& linearProgramData);

      virtual pair<SolutionType, optional<LinearProgramSolution<T>>>
      solve() override;


    private:
      LinearProgramData<T> linearProgramData_;

      uint16_t iterCount_;


      void reset();

      SolutionType optimize(SimplexTableau<T>& tableau);

      SolutionType iterate(SimplexTableau<T>& tableau);

      SolutionType iterate(
        SimplexTableau<T>& tableau, pair<DenseIndex, DenseIndex> pivotIdx
      );

      pair<SolutionType, MaybeIndex2D>
      computePivotIdx(const SimplexTableau<T>& tableau) const;

      MaybeIndex1D computePivotColIdx(
        const SimplexTableau<T>& tableau
      ) const;

      MaybeIndex1D computePivotRowIdx(
        const SimplexTableau<T>& tableau, DenseIndex pivotColIdx
      ) const;

      void pivotize(
        SimplexTableau<T>& tableau, DenseIndex rowIdx, DenseIndex colIdx
      );

      SolutionType checkPhase1Solution(const SimplexTableau<T>& tableau) const;

      SolutionType checkPhase2Solution(const SimplexTableau<T>& tableau) const;
  };
}


#include "dantzignumericsolver.txx"


#endif // DANTZIGNUMERICSOLVER_HXX
