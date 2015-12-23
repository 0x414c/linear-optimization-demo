#pragma once

#ifndef BOOSTEXTENSIONS_HXX
#define BOOSTEXTENSIONS_HXX


/**
 * Include this header to all files that use
 * operations on `NumericTypes::Rational' data type.
 * Contents of this header is not intended for end-user.
 */


#ifdef LP_WITH_MULTIPRECISION
#include "boost/multiprecision/cpp_bin_float.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#else
#include <cmath>
#endif // LP_WITH_MULTIPRECISION

#include "dataconvertors.hxx"
#include "../math/numerictypes.hxx"


namespace boost
{
//  using DataConvertors::numericCast;


//#ifdef LP_WITH_MULTIPRECISION
//  using NumericTypes::BoostRational;
//  using NumericTypes::BoostReal;


//  inline BoostRational
//  log(const BoostRational& x)
//  {
//    return (
//      BoostRational(
//        multiprecision::log(BoostReal(x.numerator())) /
//        multiprecision::log(BoostReal(x.denominator()))
//      )
//    );
//  }


//  inline BoostRational
//  sqrt(const BoostRational& x)
//  {
//    return (
//      BoostRational(
//        multiprecision::sqrt(BoostReal(x.numerator())) /
//        multiprecision::sqrt(BoostReal(x.denominator()))
//      )
//    );
//  }


//  inline BoostRational
//  ceil(const BoostRational& x)
//  {
//    return (
//      BoostRational(
//        multiprecision::ceil(
//          BoostReal(x.numerator()) / BoostReal(x.denominator())
//        )
//      )
//    );
//  }
//#else
//  using NumericTypes::BoostRational;
//  using NumericTypes::BuiltinReal;


//  inline BoostRational
//  log(const BoostRational& x)
//  {

//    return 0;/*(
//      numericCast<BoostRational, BuiltinReal>(
//        BuiltinReal(
//          std::log(double(x.numerator())) / std::log(double(x.denominator()))
//        )
//      )
//    );*/
//  }


//  inline BoostRational
//  sqrt(const BoostRational& x)
//  {
//    return 0;/*(
//      numericCast<BoostRational, BuiltinReal>(
//        BuiltinReal(
//          std::sqrt(double(x.numerator())) / std::sqrt(double(x.denominator()))
//        )
//      )
//    );*/
//  }


//  inline BoostRational
//  ceil(const BoostRational& x)
//  {
//    return 0;/*(
//      numericCast<BoostRational, BuiltinReal>(
//        BuiltinReal(
//          std::ceil(double(x.numerator()) / double(x.denominator()))
//        )
//      )
//    );*/
//  }
//#endif // LP_WITH_MULTIPRECISION
}


#endif // BOOSTEXTENSIONS_HXX
