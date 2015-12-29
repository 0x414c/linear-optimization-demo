#pragma once

#ifndef EIGENEXTENSIONS_HXX
#define EIGENEXTENSIONS_HXX


/**
 * Include this header to all files that use
 * operations on `Eigen::Matrix<NumericTypes::Rational>' data type.
 * Contents of this header is not intended for end-user.
 * NOTE: For the reference see:
 *   `http://eigen.tuxfamily.org/dox/structEigen_1_1NumTraits.html'
 */


#ifdef LP_WITH_MULTIPRECISION
#include <limits>
#endif // LP_WITH_MULTIPRECISION

#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"


namespace Eigen
{
#ifdef LP_WITH_MULTIPRECISION
  template<>
  struct NumTraits<NumericTypes::BoostRational>
  {
    enum
    {
      IsComplex = 0,
      IsInteger = 1,
      IsSigned = 1,
      RequireInitialization = 1,
      ReadCost = 2,
      AddCost = 4,
      MulCost = 8
    };


    typedef NumericTypes::BoostRational Real;
    typedef NumericTypes::BoostRational NonInteger;
    typedef NumericTypes::BoostRational Nested;


    static inline Real
    epsilon()
    {
      return std::numeric_limits<NumericTypes::BoostRational>::epsilon();
    }


    static inline Real
    dummy_precision()
    {
      return Real(0);
    }


    static inline NumericTypes::BoostRational
    highest() {
      return (std::numeric_limits<NumericTypes::BoostRational>::max)();
    }


    static inline NumericTypes::BoostRational
    lowest()
    {
      return (std::numeric_limits<NumericTypes::BoostRational>::min)();
    }
  };
#else
  template<>
  struct NumTraits<NumericTypes::BoostRational> :
    NumTraits<NumericTypes::BuiltinInteger>
  {
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


    typedef NumericTypes::BoostRational Real;
    typedef NumericTypes::BoostRational NonInteger;
    typedef NumericTypes::BoostRational Nested;
  };
#endif // LP_WITH_MULTIPRECISION
}


#endif // EIGENEXTENSIONS_HXX
