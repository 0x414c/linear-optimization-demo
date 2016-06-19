#pragma once

#ifndef NUMERICTYPES_HXX
#define NUMERICTYPES_HXX


#ifdef LP_WITH_MULTIPRECISION
#include <memory>
#endif // LP_WITH_MULTIPRECISION

#ifdef LP_WITH_MULTIPRECISION
#include "boost/multiprecision/cpp_bin_float.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#else // LP_WITH_MULTIPRECISION
#include "boost/rational.hpp"
#endif // LP_WITH_MULTIPRECISION


namespace NumericTypes
{
  enum struct Field : int
  {
    Real = 1, Rational = 2
  };


  //TODO: ~! Multiprecision mode.
#ifdef LP_WITH_MULTIPRECISION
  //NOTE: `boost_*' aliases is for specializations only (just for clarity).
  namespace mp = boost::multiprecision;

  using boost_real_t = mp::cpp_bin_float_quad;
  using real_t = boost_real_t;

  using boost_integer_t = mp::int128_t;
  using integer_t = boost_integer_t;

  using boost_rational_t = mp::cpp_rational;
  using rational_t = boost_rational_t;
#else // LP_WITH_MULTIPRECISION
  //NOTE: `builtin_*' and `boost_*' aliases is for specializations only (just for clarity).

  using builtin_real_t = double;
  using real_t = builtin_real_t;

  using builtin_integer_t = long long;
  using integer_t = builtin_integer_t;

  using boost_rational_t = boost::rational<integer_t>;
  using rational_t = boost_rational_t;
#endif // LP_WITH_MULTIPRECISION
}


#endif // NUMERICTYPES_HXX
