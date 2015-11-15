#ifndef NUMERICLIMITS_HXX
#define NUMERICLIMITS_HXX

#include "numerictypes.hxx"
#include "../misc/utils.hxx"

namespace NumericLimits
{
  template<typename T = NumericTypes::Real>
  T max()
  {
    static_assert(
      Utils::AlwaysFalse<T>::value,
      "NumericLimits::max<T>:"
      " You can only use one of the specified specializations!"
    );

    return T(0);
  }

  template<typename T = NumericTypes::Real>
  T min()
  {
    static_assert(
      Utils::AlwaysFalse<T>::value,
      "NumericLimits::min<T>:"
      " You can only use one of the specified specializations!"
    );

    return T(0);
  }
}

#include "numericlimits.txx"

#endif // NUMERICLIMITS_HXX
