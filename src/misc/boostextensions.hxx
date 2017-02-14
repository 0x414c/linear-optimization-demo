#pragma once

#ifndef BOOSTEXTENSIONS_HXX
#define BOOSTEXTENSIONS_HXX


/**
 * Include this header to all files that use
 * operations on `NumericTypes::rational_t' data type.
 * Contents of this header is not intended for end-user.
 * TODO: [0;0] Unused file.
 */


//#ifdef LP_WITH_MULTIPRECISION
//#include "boost/multiprecision/cpp_bin_float.hpp"
//#include "boost/multiprecision/cpp_int.hpp"
//#else // LP_WITH_MULTIPRECISION
//#include <cmath>
//#endif // LP_WITH_MULTIPRECISION

//#include "dataconvertors.hxx"
//#include "../math/numerictypes.hxx"


//namespace boost
//{
//  using DataConvertors::numericCast;


//#ifdef LP_WITH_MULTIPRECISION
//  using NumericTypes::boost_rational_t;
//  using NumericTypes::boost_real_t;


//  inline boost_rational_t
//  log(const boost_rational_t& x)
//  {
//    return (
//      boost_rational_t(
//        multiprecision::log(boost_real_t(x.numerator())) /
//        multiprecision::log(boost_real_t(x.denominator()))
//      )
//    );
//  }


//  inline boost_rational_t
//  sqrt(const boost_rational_t& x)
//  {
//    return (
//      boost_rational_t(
//        multiprecision::sqrt(boost_real_t(x.numerator())) /
//        multiprecision::sqrt(boost_real_t(x.denominator()))
//      )
//    );
//  }


//  inline boost_rational_t
//  ceil(const boost_rational_t& x)
//  {
//    return (
//      boost_rational_t(
//        multiprecision::ceil(
//          boost_real_t(x.numerator()) / boost_real_t(x.denominator())
//        )
//      )
//    );
//  }
//#else
//  using NumericTypes::boost_rational_t;
//  using NumericTypes::builtin_real_t;


//  inline boost_rational_t
//  log(const boost_rational_t& x)
//  {

//    return 0;/*(
//      numericCast<boost_rational_t, builtin_real_t>(
//        builtin_real_t(
//          std::log(double(x.numerator())) / std::log(double(x.denominator()))
//        )
//      )
//    );*/
//  }


//  inline boost_rational_t
//  sqrt(const boost_rational_t& x)
//  {
//    return 0;/*(
//      numericCast<boost_rational_t, builtin_real_t>(
//        builtin_real_t(
//          std::sqrt(double(x.numerator())) / std::sqrt(double(x.denominator()))
//        )
//      )
//    );*/
//  }


//  inline boost_rational_t
//  ceil(const boost_rational_t& x)
//  {
//    return 0;/*(
//      numericCast<boost_rational_t, builtin_real_t>(
//        builtin_real_t(
//          std::ceil(double(x.numerator()) / double(x.denominator()))
//        )
//      )
//    );*/
//  }
//#endif // LP_WITH_MULTIPRECISION
//}


#endif // BOOSTEXTENSIONS_HXX
