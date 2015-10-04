#ifndef DANTZIGNUMERICSOLVER_HXX
#define DANTZIGNUMERICSOLVER_HXX

#include "inumericsolver.hxx"
#include "linearprogramdata.hxx"
#include "utils.hxx"

using namespace Utils;

template<typename T = real_t>
class DantzigNumericSolver : public INumericSolver
{
  public:
    DantzigNumericSolver();

    virtual void solve() override;

  private:
    LinearProgramData<T> _lpData;
};

#endif // DANTZIGNUMERICSOLVER_HXX
