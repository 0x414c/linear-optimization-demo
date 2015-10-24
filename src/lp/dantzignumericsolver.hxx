#ifndef DANTZIGNUMERICSOLVER_HXX
#define DANTZIGNUMERICSOLVER_HXX

#include "inumericsolver.hxx"
#include "linearprogramdata.hxx"
#include "linearprogramutils.hxx"
#include "../misc/utils.hxx"

using namespace Utils;

template<typename T = real_t>
class DantzigNumericSolver : public INumericSolver
{
  public:
    DantzigNumericSolver()/* = delete*/;
    explicit DantzigNumericSolver(const LinearProgramData<T>& linearProgramData);

    virtual void solve() override;

  private:
    LinearProgramData<T> _linearProgramData;
};

#include "dantzignumericsolver.txx"

#endif // DANTZIGNUMERICSOLVER_HXX
