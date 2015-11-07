#ifndef INUMERICSOLVER_HXX
#define INUMERICSOLVER_HXX

#include "boost/optional.hpp"

#include "linearprogramsolution.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"

using namespace boost;
using namespace NumericTypes;
using namespace Utils;

template<typename T = Real>
class INumericSolver
{
  public:
    virtual ~INumericSolver() = 0;

    virtual optional<LinearProgramSolution<T>> solve() = 0;
};

#include "inumericsolver.txx"

#endif // INUMERICSOLVER_HXX
