#pragma once

#ifndef EIGENEXTENSIONS_HXX
#define EIGENEXTENSIONS_HXX


/**
 * Include this header to all files that use
 * operations on `Eigen::Matrix<NumericTypes::Rational>' data type.
 * Contents of this header is not intended for end-user.
 */


#include "eigen3/Eigen/Core"

#include "boostextensions.hxx"
#include "../math/numerictypes.hxx"


namespace Eigen
{
  using NumericTypes::Integer;
  using NumericTypes::Rational;


  template<>
  struct NumTraits<Rational> :
    NumTraits<Integer>
  {
    typedef Rational Real;
    typedef Rational NonInteger;
    typedef Rational Nested;

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
