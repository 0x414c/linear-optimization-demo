#pragma once

#ifndef INUMERICSOLVER_HXX
#define INUMERICSOLVER_HXX


#include <utility>

#include "boost/optional.hpp"

#include "linearprogramsolution.hxx"
#include "solutiontype.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using namespace boost;
  using namespace NumericTypes;
  using namespace std;


  template<typename T>
  class INumericSolver
  {
    public:
      virtual ~INumericSolver() = 0;

      virtual pair<SolutionType, optional<T>> solve() = 0;
  };
}


#include "inumericsolver.txx"


#endif // INUMERICSOLVER_HXX
