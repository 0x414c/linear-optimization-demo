#pragma once

#ifndef CONFIG_HXX
#define CONFIG_HXX

#include <cstdint>

#include <QString>

#include "math/numerictypes.hxx"


namespace Config
{
  namespace AppGlobal
  {
    const QString AppName = "Linear Programming";
    const QString OrgName = "0x414c!";
    const QString AppVersion = "0.0.1";
    const QString AppDomain = "www.0x414c.tk";
    const QString AppDisplayName = AppName;
  }


  namespace GUI
  {
    constexpr int ProgramModelsCount = 3;
    constexpr int SimplexModelsCount = 3;

    constexpr int MinConstraints = 1;
    constexpr int MaxConstraints = 16;
    constexpr int DefaultConstraints = 3;

    constexpr int MinVariables = 1;
    constexpr int MaxVariables = 16;
    constexpr int DefaultVariables = 3;
  }


  namespace LinearProgramming
  {
    //NOTE: Increase this if you want to solve larger-scaled programs.
    //NOTE: The average number of iterations is 2M + N.
    //NOTE: The maximal number of iterations is Binomial[N + M, N]
    constexpr uint32_t MaxSimplexIterations = 601080390;
  }


  namespace MathUtils
  {
    constexpr NumericTypes::Real Epsilon = 1E-8;
    constexpr uint16_t MaxRationalizeIterations = 22;
    constexpr NumericTypes::Integer DefaultRationalizeDemoninator = 10;
  }


  namespace TableModelUtils
  {
    constexpr int Seed = 1337;
    constexpr double DistMin = -10.;
    constexpr double DistMax = 10.;
  }
}


#endif // CONFIG_HXX
