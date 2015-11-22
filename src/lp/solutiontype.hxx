#pragma once

#ifndef SOLUTIONTYPE_HXX
#define SOLUTIONTYPE_HXX


namespace LinearProgramming
{
  /**
   * @brief The SolutionType enum
   */
  enum struct SolutionType : int
  {
    Optimal = 0,
    Unbounded = 1,
    Inconsistent = 2,
    Incomplete = 4,
    Unknown = 8
  };
}


#endif // SOLUTIONTYPE_HXX
