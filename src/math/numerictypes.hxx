#ifndef NUMERICTYPES_HXX
#define NUMERICTYPES_HXX

#include "../misc/utils.hxx"

#include "boost/rational.hpp"

namespace NumericTypes
{
  enum struct Field : int { Real = 1, Rational = 2 };

  using Real = double;

  using Integer = signed long long int;

  using Rational = boost::rational<Integer>;

  namespace NumericLimits
  {
    template<typename T = Real>
    T max()
    {
      static_assert(
        Utils::False<T>::value,
        "NumericTypes::NumericLimits::max<T>:"
        " You can only use one of the specified specializations!"
      );

      return T(0);
    }

    template<typename T = Real>
    T min()
    {
      static_assert(
        Utils::False<T>::value,
        "NumericTypes::NumericLimits::min<T>:"
        " You can only use one of the specified specializations!"
      );

      return T(0);
    }
  }
}

#include "numerictypes.txx"

#endif // NUMERICTYPES_HXX

