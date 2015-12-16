#pragma once

#ifndef BOOSTEXTENSIONS_HXX
#define BOOSTEXTENSIONS_HXX


/**
 * Include this header to all files that use
 * operations on `NumericTypes::Rational' data type.
 * Contents of this header is not intended for end-user.
 */


#include <cmath>

#include <QDebug>
#include <QDebugStateSaver>
#include <QString>

#include "dataconvertors.hxx"
#include "../math/numerictypes.hxx"


namespace boost
{
  using DataConvertors::numericCast;
  using NumericTypes::Rational;
  using NumericTypes::Real;
  using std::log;
  using std::sqrt;


  inline Rational
  log(const Rational& x)
  {
    return (
      numericCast<Rational, Real>(
        Real(log(double(x.numerator()))) / Real(log(double(x.denominator())))
      )
    );
  }


  inline Rational
  sqrt(const Rational& x)
  {
    return (
      numericCast<Rational, Real>(
        Real(sqrt(double(x.numerator()))) / Real(sqrt(double(x.denominator())))
      )
    );
  }


  inline QDebug
  operator <<(QDebug debug, const Rational& x)
  {
    QDebugStateSaver stateSaver(debug);

    debug.nospace() << numericCast<QString, NumericTypes::Rational>(x);

    return debug;
  }
}


#endif // BOOSTEXTENSIONS_HXX
