﻿#pragma once

#ifndef MATHUTILS_HXX
#define MATHUTILS_HXX


#include <utility>

#include "numericlimits.hxx"
#include "numerictypes.hxx"
#include "../config.hxx"
#include "../misc/utils.hxx"


namespace MathUtils
{
  using namespace Config::MathUtils;
  using namespace NumericTypes;
  using namespace std;
  using namespace Utils;


  template<typename R = Integer>
  pair<R, R>
  rationalize(
    Real x, Real tolerance = Epsilon,
    uint16_t maxIterations = MaxRationalizeIterations,
    R maxDenominator = NumericLimits::max<R>()
  );


  template<typename T = Real>
  T
  absoluteValue(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::absoluteValue<T>:"
      " You can only use one of the specified specializations!"
    );

    return T(0);
  }


  //TODO: ~? Replace all the `is*' funcs w/ single comparison func
  template<typename T = Real>
  bool
  isEqualToZero(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isEqualToZero<T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }


  template<typename T = Real>
  bool
  isGreaterThanZero(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isGreaterThanZero<T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }


  template<typename T = Real>
  bool
  isLessThanZero(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isLessThanZero<T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }


  template<typename T = Real>
  bool
  isGreaterOrEqualToZero(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isGreaterOrEqualToZero<T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }
}


#include "mathutils.txx"


#endif // MATHUTILS_HXX
