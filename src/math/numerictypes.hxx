#pragma once

#ifndef NUMERICTYPES_HXX
#define NUMERICTYPES_HXX


#ifdef LP_WITH_MULTIPRECISION
#include <memory>

#include "boost/multiprecision/cpp_bin_float.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#else
#include "boost/rational.hpp"
#endif // LP_WITH_MULTIPRECISION


namespace NumericTypes
{
  enum struct Field : int { Real = 1, Rational = 2 };

  //TODO: ~! Multiprecision mode
#ifdef LP_WITH_MULTIPRECISION
  //NOTE: `Boost*' aliases is for specializations only.
  namespace mprec = boost::multiprecision;

//  using BoostReal = mprec::number<
//    mprec::backends::cpp_bin_float<
//      113,
//      mprec::backends::digit_base_2,
//      void,
//      boost::int16_t,
//      -16382,
//      16383
//    >,
//    mprec::et_off
//  >;
//  using Real = BoostReal;

//  using BoostIntegerBackend = mprec::cpp_int_backend<
//    128u,
//    128u,
//    mprec::cpp_integer_type::signed_magnitude,
//    mprec::cpp_int_check_type::unchecked,
//    void
//  >;
//  using BoostInteger = mprec::number<BoostIntegerBackend, mprec::et_off>;
//  using Integer = BoostInteger;

//  using BoostRational = mprec::number<
//    mprec::rational_adaptor<BoostIntegerBackend>,
//    mprec::et_off
//  >;
//  using Rational = BoostRational;

  using BoostReal = mprec::cpp_bin_float_quad;
  using Real = BoostReal;

  using BoostInteger = mprec::cpp_int;
  using Integer = BoostInteger;

  using BoostRational = mprec::cpp_rational;
  using Rational = BoostRational;
#else
  //NOTE: `Builtin*' and `Boost*' aliases is for specializations only.

  using BuiltinReal = double;
  using Real = BuiltinReal;

  using BuiltinInteger = signed long long int;
  using Integer = BuiltinInteger;

  using BoostRational = boost::rational<Integer>;
  using Rational = BoostRational;
#endif // LP_WITH_MULTIPRECISION
}


#endif // NUMERICTYPES_HXX
