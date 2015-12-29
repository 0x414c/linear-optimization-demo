#pragma once

#ifndef MATHUTILS_HXX
#define MATHUTILS_HXX


#include <type_traits>
#include <utility>

#include "numericlimits.hxx"
#include "numerictypes.hxx"
#include "../misc/utils.hxx"
#include "../config.hxx"


namespace MathUtils
{
  using namespace Config::MathUtils;
  using NumericTypes::Integer;
  using NumericTypes::Rational;
  using NumericTypes::Real;
  using std::enable_if;
  using std::is_integral;
  using std::pair;
  using Utils::AlwaysFalse;


  template<
    typename R, typename T = Real/*, typename = enable_if<is_integral<R>::value>*/
  >
  pair<R, R>
  rationalize(
    T x, T tolerance = Epsilon,
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
  isEqual(T x, T y)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isEqual<T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }


  template<typename T = Real>
  bool
  isLessThan(T x, T y)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isLessThan<T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }


  template<typename T = Real>
  bool
  isGreaterThan(T x, T y)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isGreaterThan<T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }


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
  isGreaterThanOrEqualToZero(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isGreaterThanOrEqualToZero<T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }
}


#include "mathutils.txx"


#endif // MATHUTILS_HXX
