#ifndef NUMERICLIMITS_TXX
#define NUMERICLIMITS_TXX

#include <limits>

#include "numerictypes.hxx"

namespace NumericLimits
{
  template<>
  inline NumericTypes::Real
  max()
  {
    return std::numeric_limits<NumericTypes::Real>::max();
  }

  template<>
  inline NumericTypes::Real
  min()
  {
    return std::numeric_limits<NumericTypes::Real>::min();
  }

  template<>
  inline NumericTypes::Rational
  max()
  {
    return NumericTypes::Rational(
      std::numeric_limits<NumericTypes::Integer>::max()
    );
  }

  template<>
  inline NumericTypes::Rational
  min()
  {
    return NumericTypes::Rational(
      std::numeric_limits<NumericTypes::Integer>::min()
    );
  }
}

#endif // NUMERICLIMITS_TXX
