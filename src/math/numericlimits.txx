#pragma once

#ifndef NUMERICLIMITS_TXX
#define NUMERICLIMITS_TXX


#include "numericlimits.hxx"

#include <limits>

#include "numerictypes.hxx"


namespace NumericLimits
{
  using NumericTypes::integer_t;
  using NumericTypes::rational_t;
  using NumericTypes::real_t;
  using std::numeric_limits;


  template<>
  inline /*constexpr*/ real_t
  max()
  {
    return numeric_limits<real_t>::max();
  }


  template<>
  inline /*constexpr*/ real_t
  min()
  {
    return numeric_limits<real_t>::min();
  }


  template<>
  inline /*constexpr*/ integer_t
  max()
  {
    return numeric_limits<integer_t>::max();
  }


  template<>
  inline /*constexpr*/ integer_t
  min()
  {
    return numeric_limits<integer_t>::min();
  }


  template<>
  inline /*constexpr*/ rational_t
  max()
  {
    return rational_t(max<integer_t>());
  }


  template<>
  inline /*constexpr*/ rational_t
  min()
  {
    return rational_t(min<integer_t>());
  }
}


#endif // NUMERICLIMITS_TXX
