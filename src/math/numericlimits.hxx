﻿#pragma once

#ifndef NUMERICLIMITS_HXX
#define NUMERICLIMITS_HXX


#include "numerictypes.hxx"
#include "../misc/utils.hxx"


//TODO: ~? Convert to struct w/ static members
//TODO: ~? Remove this and use `std::numeric_limits' directly
//TODO: ~? Check stdlib support for `boost::rational'
namespace NumericLimits
{
  using NumericTypes::Real;


  template<typename T = Real>
  T
  max()
  {
    static_assert(
      Utils::AlwaysFalse<T>::value,
      "NumericLimits::max<T>:"
      " You can only use one of the specified specializations!"
    );

    return T(0);
  }


  template<typename T = Real>
  T
  min()
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
