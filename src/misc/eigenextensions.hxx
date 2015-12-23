#pragma once

#ifndef EIGENEXTENSIONS_HXX
#define EIGENEXTENSIONS_HXX


/**
 * Include this header to all files that use
 * operations on `Eigen::Matrix<NumericTypes::Rational>' data type.
 * Contents of this header is not intended for end-user.
 */


#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"


namespace Eigen
{
#ifdef LP_WITH_MULTIPRECISION
  template<>
  struct NumTraits<NumericTypes::BoostRational> :
    GenericNumTraits<int>
  {
    typedef NumericTypes::BoostRational Real;
    typedef NumericTypes::BoostRational NonInteger;
    typedef NumericTypes::BoostRational Nested;


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
#else
  template<>
  struct NumTraits<NumericTypes::BoostRational> :
    NumTraits<NumericTypes::BuiltinInteger>
  {
    typedef NumericTypes::BoostRational Real;
    typedef NumericTypes::BoostRational NonInteger;
    typedef NumericTypes::BoostRational Nested;


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
#endif // LP_WITH_MULTIPRECISION
}


#endif // EIGENEXTENSIONS_HXX
