#pragma once

#ifndef SIMPLEXSOLVER_HXX
#define SIMPLEXSOLVER_HXX


#include <cstdint>

#include <map>
#include <utility>
#include <vector>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "simplexsolvercontroller_fwd.hxx"
#include "isolver.hxx"
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
  using NumericTypes::rational_t;
  using NumericTypes::real_t;
  using std::pair;

  using MaybeIndex2D = optional<pair<DenseIndex, DenseIndex>>;
  using MaybeIndex1D = optional<DenseIndex>;


  template<typename T = real_t>
  class SimplexSolver :
    public ISolver<LinearProgramSolution<T>>
  {
    public:
      friend SimplexSolverController<T>;


      SimplexSolver() = default;

      explicit SimplexSolver(
        const LinearProgramData<T>& linearProgramData
      );
      explicit SimplexSolver(LinearProgramData<T>&& linearProgramData);

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

      void transformTableau(
        SimplexTableau<T>& tableau, DenseIndex rowIdx, DenseIndex colIdx
      );

      SolutionType checkPhase1Solution(const SimplexTableau<T>& tableau) const;

      SolutionType checkPhase2Solution(const SimplexTableau<T>& tableau) const;
  };
}


#include "simplexsolver.txx"


#endif // SIMPLEXSOLVER_HXX
