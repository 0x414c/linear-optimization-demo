#ifndef UTILS
#define UTILS

#include <cmath>

#include "lib/boost/rational.hpp"

namespace Utils
{
  template<typename T>
  struct always_false
  {
      enum { value = false };
  };

  enum struct DialogResult { Fail, Nothing, Success };

  enum struct ComputationMode { Real, Rational };

  typedef double_t real_t;

  typedef long long llong_t;

  typedef llong_t int_t;

  typedef boost::rational<int_t> rat_t;
}

#endif // UTILS
