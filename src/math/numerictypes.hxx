#pragma once

#ifndef NUMERICTYPES_HXX
#define NUMERICTYPES_HXX


#include "boost/rational.hpp"


//TODO: ~ Use `boost::multiprecision' library
//NOTE: `http://www.boost.org/doc/libs/1_59_0/libs/
//multiprecision/doc/html/boost_multiprecision/intro.html'
namespace NumericTypes
{
  enum struct Field : int { Real = 1, Rational = 2 };

  using Real = double;

  using Integer = signed long long int;

  using Rational = boost::rational<Integer>;
}


#endif // NUMERICTYPES_HXX
