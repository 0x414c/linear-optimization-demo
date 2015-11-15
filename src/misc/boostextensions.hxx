#ifndef BOOSTEXTENSIONS_HXX
#define BOOSTEXTENSIONS_HXX

#include <cmath>

#include <QDebug>
#include <QDebugStateSaver>
#include <QString>

#include "dataconvertors.hxx"
#include "../math/numerictypes.hxx"

/**
 * Include this header to all files that use
 * operations on `Rational' type.
 * Contents of this header is not intended for end-user.
 */

namespace boost
{
  inline NumericTypes::Rational
  log(const NumericTypes::Rational& x)
  {
    return (
      DataConvertors::numericCast<NumericTypes::Rational, NumericTypes::Real>(
        NumericTypes::Real(std::log(double(x.numerator())))
        /
        NumericTypes::Real(std::log(double(x.denominator())))
      )
    );
  }

  inline NumericTypes::Rational
  sqrt(const NumericTypes::Rational& x)
  {
    return (
      DataConvertors::numericCast<NumericTypes::Rational, NumericTypes::Real>(
        NumericTypes::Real(std::sqrt(double(x.numerator())))
        /
        NumericTypes::Real(std::sqrt(double(x.denominator())))
      )
    );
  }

  inline QDebug
  operator <<(QDebug debug, const NumericTypes::Rational& x)
  {
    QDebugStateSaver stateSaver(debug);
    debug.nospace() <<
      DataConvertors::numericCast<QString, NumericTypes::Rational>(x);

    return debug;
  }
}

#endif // BOOSTEXTENSIONS_HXX

