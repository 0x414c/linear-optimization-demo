#pragma once

#ifndef NUMERICLIMITS_TXX
#define NUMERICLIMITS_TXX


#include "numericlimits.hxx"

#include <limits>

#include "numerictypes.hxx"


namespace NumericLimits
{
  using NumericTypes::Integer;
  using NumericTypes::Rational;
  using NumericTypes::Real;
  using std::numeric_limits;


  template<>
  inline constexpr Real
  max()
  {
    return numeric_limits<Real>::max();
  }


  template<>
  inline constexpr Real
  min()
  {
    return numeric_limits<Real>::min();
  }


  template<>
  inline constexpr Rational
  max()
  {
    return Rational(numeric_limits<Integer>::max());
  }


  template<>
  inline constexpr Rational
  min()
  {
    return Rational(numeric_limits<Integer>::min());
  }


  template<>
  inline constexpr Integer
  max()
  {
    return numeric_limits<Integer>::max();
  }


  template<>
  inline constexpr Integer
  min()
  {
    return numeric_limits<Integer>::min();
  }
}


#endif // NUMERICLIMITS_TXX
