#pragma once

#ifndef OPTIMIZATIONGOALTYPE
#define OPTIMIZATIONGOALTYPE


namespace LinearProgramming
{
  /**
   * @brief The OptimizationGoalType enum
   * TODO: [0;0] Use it in GUI.
   */
  enum struct OptimizationGoalType : int
  {
    Minimize = -1,
    Maximize = 1
  };
}


#endif // OPTIMIZATIONGOALTYPE
