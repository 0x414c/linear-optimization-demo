#ifndef DANTZIGNUMERICSOLVER_HXX
#define DANTZIGNUMERICSOLVER_HXX

#include <cstdint>

#include <map>
#include <utility>
#include <vector>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Dense"

#include "inumericsolver.hxx"
#include "linearprogramdata.hxx"
#include "linearprogramsolution.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"

using namespace boost;
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
    explicit DantzigNumericSolver(const LinearProgramData<T>& linearProgramData);

    virtual optional<LinearProgramSolution<T>> solve() override;

    void setLinearProgramData(const LinearProgramData<T>& linearProgramData);

  private:
    LinearProgramData<T> _linearProgramData = LinearProgramData<T>();

    uint16_t _iterCount = uint16_t(0);
    const uint16_t _maxIterations = uint16_t(100); //Increase this if you want to
                                                   //solve larger-scaled programs

    vector<DenseIndex> _freeVars;
    vector<DenseIndex> _basicVars;

    SolutionType optimize(Matrix<T, Dynamic, Dynamic>& tableau);
    SolutionType iterate(Matrix<T, Dynamic, Dynamic>& tableau);

    void pivotize(Matrix<T, Dynamic, Dynamic>& tableau,
                  DenseIndex rowIdx, DenseIndex colIdx);

    SolutionType checkPhase1Solution(const Matrix<T, Dynamic, Dynamic>& tableau) const;
    SolutionType checkPhase2Solution(const Matrix<T, Dynamic, Dynamic>& tableau) const;

    bool isPivotColValid(const Matrix<T, Dynamic, Dynamic>& tableau,
                         DenseIndex colIdx) const;

    optional<DenseIndex> computePivotColIdx(const Matrix<T, Dynamic, Dynamic>& tableau) const;
    optional<DenseIndex> computePivotRowIdx(const Matrix<T, Dynamic, Dynamic>& tableau,
                                            DenseIndex pivotColIdx,
                                            bool forceToLeaveBasis = false) const;
};

#include "dantzignumericsolver.txx"

#endif // DANTZIGNUMERICSOLVER_HXX
