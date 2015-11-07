#ifndef EIGENEXTENSIONS_HXX
#define EIGENEXTENSIONS_HXX

#include <cmath>

#include "eigen3/Eigen/Core"

#include "boostextensions.hxx"
#include "../math/numerictypes.hxx"

/**
 * Include this header to all files that use
 * operations on `Matrix<Rational>' type.
 * Contents of this header is not intended for end-user.
 */

namespace Eigen
{
  template<>
  struct NumTraits<NumericTypes::Rational> :
    NumTraits<NumericTypes::Integer>
  {
    typedef NumericTypes::Rational Real;
    typedef NumericTypes::Rational NonInteger;
    typedef NumericTypes::Rational Nested;
    enum
    {
      IsComplex = 0,
      IsInteger = 1,
      IsSigned = 1,
      RequireInitialization = 1,
      ReadCost = 2,
      AddCost = 10,
      MulCost = 8
    };
  };
}

#endif // EIGENEXTENSIONS_HXX
