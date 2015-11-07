﻿#ifndef MATHUTILS_HXX
#define MATHUTILS_HXX

#include <limits>
#include <utility>

#include "numerictypes.hxx"
#include "../misc/utils.hxx"

using namespace NumericTypes;
using namespace std;
using namespace Utils;

namespace MathUtils
{
  const Real Epsilon = 1E-16;

  template<typename R = Integer>
  pair<R, R>
  rationalize(Real x,
              Real tolerance = Epsilon,
              uint16_t maxIterations = 22,
              R maxDenominator = numeric_limits<R>::max());

  template<typename T = Real>
  T absoluteValue(T x)
  {
    static_assert(
      False<T>::value,
      "MathUtils::absoluteValue<T>:"
      " You can only use one of the specified specializations!"
    );

    return T(0);
  }

  template<typename T = Real>
  bool isEqualToZero(T x)
  {
    static_assert(
      False<T>::value,
      "MathUtils::isEqualToZero<T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }

  template<typename T = Real>
  bool isGreaterThanZero(T x)
  {
    static_assert(
      False<T>::value,
      "MathUtils::isGreaterThanZero<T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }

  template<typename T = Real>
  bool isLessThanZero(T x)
  {
    static_assert(
      False<T>::value,
      "MathUtils::isLessThanZero<T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }

  template<typename T = Real>
  bool isGreaterOrEqualToZero(T x)
  {
    static_assert(
      False<T>::value,
      "MathUtils::isGreaterOrEqualToZero<T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }
}

#include "mathutils.txx"

#endif // MATHUTILS_HXX