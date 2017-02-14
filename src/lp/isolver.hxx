#pragma once

#ifndef ISOLVER_HXX
#define ISOLVER_HXX


#include <utility>

#include "boost/optional.hpp"

#include "linearprogramsolution.hxx"
#include "solutiontype.hxx"


namespace LinearProgramming
{
  using boost::optional;
  using std::pair;


  template<typename T>
  class ISolver
  {
    public:
      virtual ~ISolver() = 0;

      virtual pair<SolutionType, optional<T>> solve() = 0;
  };
}


#include "isolver.txx"


#endif // ISOLVER_HXX
