#pragma once

#ifndef EIGENEXTENSIONS_HXX
#define EIGENEXTENSIONS_HXX


/**
 * Include this header to all files that use
 * operations on `Eigen::Matrix<NumericTypes::rational_t>' data type.
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
  struct NumTraits<NumericTypes::boost_rational_t>
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


    typedef NumericTypes::boost_rational_t Real;
    typedef NumericTypes::boost_rational_t NonInteger;
    typedef NumericTypes::boost_rational_t Nested;


    static inline Real
    epsilon()
    {
      return std::numeric_limits<NumericTypes::boost_rational_t>::epsilon();
    }


    static inline Real
    dummy_precision()
    {
      return NumericTypes::boost_rational_t(0);
    }


    static inline Real
    highest()
    {
      return std::numeric_limits<NumericTypes::boost_rational_t>::max();
    }


    static inline Real
    lowest()
    {
      return std::numeric_limits<NumericTypes::boost_rational_t>::min();
    }
  };
#else // LP_WITH_MULTIPRECISION
  template<>
  struct NumTraits<NumericTypes::boost_rational_t> :
    NumTraits<NumericTypes::builtin_integer_t>
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


    typedef NumericTypes::boost_rational_t Real;
    typedef NumericTypes::boost_rational_t NonInteger;
    typedef NumericTypes::boost_rational_t Nested;
  };
#endif // LP_WITH_MULTIPRECISION
}


#endif // EIGENEXTENSIONS_HXX
