#pragma once

#ifndef NUMERICTYPES_HXX
#define NUMERICTYPES_HXX


#ifdef LP_WITH_MULTIPRECISION
#include "boost/multiprecision/cpp_bin_float.hpp"
//#include "boost/multiprecision/cpp_dec_float.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#else
#include "boost/rational.hpp"
#endif // LP_WITH_MULTIPRECISION


namespace NumericTypes
{
  enum struct Field : int { Real = 1, Rational = 2 };

  //TODO: ~
#ifdef LP_WITH_MULTIPRECISION
  //NOTE: `Boost*' aliases is for specializations only.
  namespace mpr = boost::multiprecision;

  using BoostReal =
    mpr::number<
      mpr::backends::cpp_bin_float<
        113,
        mpr::backends::digit_base_2,
        void,
        boost::int16_t,
        -16382,
        16383
    >, mpr::et_off
  >;
  using Real = BoostReal;

  using BoostIntegerBackend =
    mpr::cpp_int_backend<
      128u,
      128u,
      mpr::cpp_integer_type::signed_magnitude,
      mpr::cpp_int_check_type::unchecked,
      void
    >;
  using BoostInteger = mpr::number<BoostIntegerBackend>;
  using Integer = BoostInteger;

//  using BoostRational = boost::rational<BoostInteger>;
  using BoostRational =
    mpr::number<
      mpr::rational_adaptor<BoostIntegerBackend>
    >;
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
