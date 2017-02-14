#pragma once

#ifndef MATHUTILS_HXX
#define MATHUTILS_HXX


#include <utility>

#include "numericlimits.hxx"
#include "numerictypes.hxx"
#include "../misc/utils.hxx"
#include "../config.hxx"


namespace MathUtils
{
  using namespace Config::MathUtils;
  using NumericTypes::integer_t;
  using NumericTypes::rational_t;
  using NumericTypes::real_t;
  using std::pair;
  using Utils::AlwaysFalse;


  template<typename R = integer_t, typename T = real_t>
  R
  integerPart(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::integerPart<R, T>: "
      "You can only use one of the specified template specializations!"
    );

    return R(0);
  }


  template<typename T = real_t>
  bool
  isFinite(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isFinite<T>: "
      "You can only use one of the specified template specializations!"
    );

    return false;
  }


  template<typename R = integer_t, typename T = real_t>
  pair<R, R>
  rationalize(
    T x, T tolerance = Epsilon,
    R maxDenominator = NumericLimits::max<R>(),
    uint16_t maxIterations = MaxRationalizeIterations
  );


  template<typename T = real_t>
  T
  absoluteValue(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::absoluteValue<T>: "
      "You can only use one of the specified template specializations!"
    );

    return T(0);
  }


  //TODO: [0;1] Replace all the `is*' funcs w/ single comparison function.
  template<typename T = real_t>
  bool
  isEqual(T x, T y, T tolerance)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isEqual<T>: "
      "You can only use one of the specified template specializations!"
    );

    return false;
  }


  template<typename T = real_t>
  bool
  isEqual(T x, T y)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isEqual<T>: "
      "You can only use one of the specified template specializations!"
    );

    return false;
  }


  template<typename T = real_t>
  bool
  isLessThan(T x, T y)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isLessThan<T>: "
      "You can only use one of the specified template specializations!"
    );

    return false;
  }


  template<typename T = real_t>
  bool
  isGreaterThan(T x, T y)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isGreaterThan<T>: "
      "You can only use one of the specified template specializations!"
    );

    return false;
  }


  template<typename T = real_t>
  bool
  isEqualToZero(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isEqualToZero<T>: "
      "You can only use one of the specified template specializations!"
    );

    return false;
  }


  template<typename T = real_t>
  bool
  isGreaterThanZero(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isGreaterThanZero<T>: "
      "You can only use one of the specified template specializations!"
    );

    return false;
  }


  template<typename T = real_t>
  bool
  isLessThanZero(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isLessThanZero<T>: "
      "You can only use one of the specified template specializations!"
    );

    return false;
  }


  template<typename T = real_t>
  bool
  isGreaterThanOrEqualToZero(T x)
  {
    static_assert(
      AlwaysFalse<T>::value,
      "MathUtils::isGreaterThanOrEqualToZero<T>: "
      "You can only use one of the specified template specializations!"
    );

    return false;
  }
}


#include "mathutils.txx"


#endif // MATHUTILS_HXX
