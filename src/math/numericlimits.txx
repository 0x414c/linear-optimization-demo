#pragma once

#ifndef NUMERICLIMITS_TXX
#define NUMERICLIMITS_TXX


#include <limits>

#include "numerictypes.hxx"


namespace NumericLimits
{
  template<>
  inline constexpr NumericTypes::Real
  max()
  {
    return std::numeric_limits<NumericTypes::Real>::max();
  }


  template<>
  inline constexpr NumericTypes::Real
  min()
  {
    return std::numeric_limits<NumericTypes::Real>::min();
  }


  template<>
  inline constexpr NumericTypes::Rational
  max()
  {
    return NumericTypes::Rational(
      std::numeric_limits<NumericTypes::Integer>::max()
    );
  }


  template<>
  inline constexpr NumericTypes::Rational
  min()
  {
    return NumericTypes::Rational(
      std::numeric_limits<NumericTypes::Integer>::min()
    );
  }


  template<>
  inline constexpr NumericTypes::Integer
  max()
  {
    return std::numeric_limits<NumericTypes::Integer>::max();
  }


  template<>
  inline constexpr NumericTypes::Integer
  min()
  {
    return std::numeric_limits<NumericTypes::Integer>::min();
  }
}


#endif // NUMERICLIMITS_TXX
