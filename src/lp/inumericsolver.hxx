#pragma once

#ifndef INUMERICSOLVER_HXX
#define INUMERICSOLVER_HXX


#include <utility>

#include "boost/optional.hpp"

#include "linearprogramsolution.hxx"
#include "solutiontype.hxx"


namespace LinearProgramming
{
  using boost::optional;
  using std::pair;


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
