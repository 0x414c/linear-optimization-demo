#ifndef NUMERICTYPES_TXX
#define NUMERICTYPES_TXX

#include <limits>

#include "numerictypes.hxx"

namespace NumericTypes
{
  namespace NumericLimits
  {
    template<>
    inline Real max()
    {
      return std::numeric_limits<Real>::max();
    }

    template<>
    inline Real min()
    {
      return std::numeric_limits<Real>::min();
    }

    template<>
    inline Rational max()
    {
      return Rational(std::numeric_limits<Integer>::max());
    }

    template<>
    inline Rational min()
    {
      return Rational(std::numeric_limits<Integer>::min());
    }
  }
}

#endif // NUMERICTYPES_TXX
