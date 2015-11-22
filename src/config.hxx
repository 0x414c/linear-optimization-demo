#pragma once

#ifndef CONFIG_HXX
#define CONFIG_HXX


#include <cstdint>

#include "math/numerictypes.hxx"


namespace Config
{
  namespace GUI
  {
    constexpr int ProgramModelsCount = 3;
    constexpr int SimplexModelsCount = 3;

    constexpr int DefaultConstraints = 3;
    constexpr int DefaultVariables = 3;
    constexpr int MinConstraints = 1;
    constexpr int MinVariables = 1;
    constexpr int MaxConstraints = 16;
    constexpr int MaxVariables = 16;
  }


  namespace LinearProgramming
  {
    //NOTE: Increase this if you want to solve larger-scaled programs.
    //NOTE: The maximal number of iterations is 2M + N.
    constexpr uint16_t MaxSimplexIterations = 48;
  }


  namespace MathUtils
  {
    constexpr NumericTypes::Real Epsilon = 1E-8;
    constexpr uint16_t MaxRationalizeIterations = 22;
  }


  namespace TableModelUtils
  {
    constexpr int Seed = 1337;
    constexpr double DistMin = -10.;
    constexpr double DistMax = 10.;
  }
}


#endif // CONFIG_HXX
