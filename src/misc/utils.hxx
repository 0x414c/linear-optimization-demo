#ifndef UTILS_HXX
#define UTILS_HXX

#include <cmath>

#include <QPointF>

#include "boost/rational.hpp"

namespace Utils
{
  template<typename T1, typename... TN>
  struct False
  {
    enum : bool { value = false };
  };

  enum struct DialogResult { Fail, Nothing, Success };

  enum struct Field : int { Real = 1, Rational = 2 };

  using real_t = double_t;

  using integer_t = signed long long;

  using rational_t = boost::rational<integer_t>; //TODO: ~ Move these usings to typedefs.hxx
}

#endif // UTILS_HXX
