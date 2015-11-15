#ifndef DANTZIGNUMERICSOLVER_HXX
#define DANTZIGNUMERICSOLVER_HXX

#include <cstdint>

#include <map>
#include <utility>
#include <vector>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "inumericsolver.hxx"
#include "linearprogramdata.hxx"
#include "linearprogramsolution.hxx"
#include "simplextableau.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"
#include "../config.hxx"

namespace LinearProgramming
{
  using namespace boost;
  using namespace Config::LinearProgramming;
  using namespace Eigen;
  using namespace NumericTypes;
  using namespace std;
  using namespace Utils;

  template<typename T = Real>
  class DantzigNumericSolver :
    virtual public INumericSolver<T>
  {
    public:
      DantzigNumericSolver()/* = delete*/;
      explicit DantzigNumericSolver(
        const LinearProgramData<T>& linearProgramData
      );
      explicit DantzigNumericSolver(LinearProgramData<T>&& linearProgramData);

      void setLinearProgramData(const LinearProgramData<T>& linearProgramData);
      void setLinearProgramData(LinearProgramData<T>&& linearProgramData);

      virtual optional<LinearProgramSolution<T>> solve() override;

    private:
      LinearProgramData<T> _linearProgramData;

      uint16_t _iterCount;

      const uint16_t _maxIterations = MaxSimplexIterations;

      SolutionType optimize(SimplexTableau<T>& tableau);

      SolutionType iterate(SimplexTableau<T>& tableau);

      void pivotize(
        SimplexTableau<T>& tableau, DenseIndex rowIdx, DenseIndex colIdx
      );

      SolutionType checkPhase1Solution(const SimplexTableau<T>& tableau) const;

      SolutionType checkPhase2Solution(const SimplexTableau<T>& tableau) const;

      bool isPivotColValid(
        const SimplexTableau<T>& tableau, DenseIndex colIdx
      ) const;

      optional<DenseIndex> computePivotColIdx(
        const SimplexTableau<T>& tableau
      ) const;

      optional<DenseIndex> computePivotRowIdx(
        const SimplexTableau<T>& tableau, DenseIndex pivotColIdx
      ) const;
  };
}

#include "dantzignumericsolver.txx"

#endif // DANTZIGNUMERICSOLVER_HXX
