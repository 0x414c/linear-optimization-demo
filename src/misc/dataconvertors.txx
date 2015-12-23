#pragma once

#ifndef DATACONVERTORS_TXX
#define DATACONVERTORS_TXX


#include "dataconvertors.hxx"

#include <cstdint>
#include <utility>

#include <QDebug>
#include <QLocale>
#include <QRegExp>
#include <QString>

#include <cppformat/format.h>
#ifdef LP_WITH_MULTIPRECISION
#include <stdexcept>

#include "boost/multiprecision/cpp_bin_float.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#endif // LP_WITH_MULTIPRECISION

#include "boostextensions.hxx"
#include "../math/mathutils.hxx"
#include "../math/numericlimits.hxx"
#include "../math/numerictypes.hxx"
#include "../config.hxx"


namespace DataConvertors
{
  using namespace Config::MathUtils;
  using fmt::format;
  using MathUtils::rationalize;
  using NumericTypes::Rational;
  using NumericTypes::Real;
  using std::pair;


  template<>
  inline Real
  numericCast<Real, Real>(const Real& from)
  {
    return from;
  }


  template<>
  inline Rational
  numericCast<Rational, Rational>(const Rational& from)
  {
    return from;
  }


#ifdef LP_WITH_MULTIPRECISION
  using NumericTypes::BoostReal;
  using NumericTypes::BoostInteger;
  using NumericTypes::BoostRational;
  using namespace NumericTypes::mpr;
  using std::runtime_error;


  template<>
  inline BoostReal
  numericCast<BoostReal, BoostRational>(const BoostRational& from)
  {
    const BoostReal value(
      BoostReal(numerator(from)) / BoostReal(denominator(from))
    );

    return value;
  }


  template<>
  inline BoostRational
  numericCast<BoostRational, BoostReal>(const BoostReal& from)
  {
    const pair<BoostInteger, BoostInteger> rationalized =
      rationalize<BoostInteger>(
        from, Epsilon,
        MaxRationalizeIterations, DefaultRationalizeDemoninator
      );

    return BoostRational(rationalized.first, rationalized.second);
  }


  template<>
  inline QString
  numericCast<QString, BoostReal>(const BoostReal& from)
  {
    const QString value(QString::fromStdString(format("{}", from)));

    return value;
  }


  template<>
  inline QString
  numericCast<QString, BoostRational>(const BoostRational& from)
  {
    QString value; //TODO: ~ Remove assignment, just construct-and-return

    if (denominator(from) == BoostInteger(1))
    {
      value = QString::fromStdString(format("{}", numerator(from)));
    }
    else
    {
      if (numerator(from) == BoostInteger(0))
      {
        value = QStringLiteral("0");
      }
      else
      {
        value =
          QString::fromStdString(
            format("{}/{}", numerator(from), denominator(from))
          );
      }
    }

    return value;
  }


  template<>
  inline BoostReal
  numericCast<BoostReal>(const QString& from)
  {
    try
    {
      const BoostReal value(from.toStdString());

      return value;
    }
    catch (const std::runtime_error& ex)
    {
      return NumericLimits::min<BoostReal>();
    }
  }


  template<>
  inline BoostInteger
  numericCast<BoostInteger>(const QString& from)
  {
    try
    {
      const BoostInteger value(from.toStdString());

      return value;
    }
    catch (const std::runtime_error& ex)
    {
      return NumericLimits::min<BoostInteger>();
    }
  }


  template<>
  inline BoostRational
  numericCast<BoostRational>(const QString& from)
  {
    try
    {
      const BoostRational value(from.toStdString());

      return value;
    }
    catch (const std::runtime_error& ex)
    {
      return NumericLimits::min<BoostRational>();
    }
  }

#else
  using NumericTypes::BuiltinReal;
  using NumericTypes::BuiltinInteger;
  using NumericTypes::BoostRational;


  template<>
  inline BuiltinReal
  numericCast<BuiltinReal, BoostRational>(const BoostRational& from)
  {
    const BuiltinReal value(
      BuiltinReal(from.numerator()) / BuiltinReal(from.denominator())
    );

    return value;
  }


  template<>
  inline BoostRational
  numericCast<BoostRational, BuiltinReal>(const BuiltinReal& from)
  {
    const pair<BuiltinInteger, BuiltinInteger> rationalized =
      rationalize<BuiltinInteger>(
        from, Epsilon,
        MaxRationalizeIterations, DefaultRationalizeDemoninator
      );

    return BoostRational(rationalized.first, rationalized.second);
  }


  template<>
  inline QString
  numericCast<QString, BuiltinReal>(const BuiltinReal& from)
  {
    const QString value(QString::fromStdString(format("{}", from)));

    return value;
  }


  template<>
  inline QString
  numericCast<QString, BoostRational>(const BoostRational& from)
  {
    QString value; //TODO: ~ Remove assignment, just construct-and-return

    if (from.denominator() == BuiltinInteger(1))
    {
      value = QString::fromStdString(format("{}", from.numerator()));
    }
    else
    {
      if (from.numerator() == BuiltinInteger(0))
      {
        value = QStringLiteral("0");
      }
      else
      {
        value =
          QString::fromStdString(
            format("{}/{}", from.numerator(), from.denominator())
          );
      }
    }

    return value;
  }


  template<>
  inline BuiltinReal
  numericCast<BuiltinReal>(const QString& from)
  {
    bool isOk(false);
    const QLocale cLocale(QLocale::C);

    const BuiltinReal value(cLocale.toDouble(from, &isOk));
    if (!isOk) {
      qCritical() << "DataConvertors::numericCast<Real>:"
                     " could not convert" << from;

      return NumericLimits::min<BuiltinReal>();
    }
    else
    {
      return value;
    }
  }


  template<>
  inline BuiltinInteger
  numericCast<BuiltinInteger>(const QString& from)
  {
    bool isOk(false);
    const QLocale cLocale(QLocale::C);

    const BuiltinInteger value(cLocale.toLongLong(from, &isOk));
    if (!isOk) {
      qCritical() << "DataConvertors::numericCast<Integer>:"
                     " could not convert" << from;

      return NumericLimits::min<BuiltinInteger>();
    }
    else
    {
      return value;
    }
  }


  template<>
  inline BoostRational
  numericCast<BoostRational>(const QString& from)
  {
    //We assume that the input string `from' was already
    //validated and no zero denominator can appear in
    //the input string, so this regexp `re' can be simpler
    //(than the one contained in corresponding validator).
    const QRegExp re(QStringLiteral("^([-+]?[0-9]+)(?:\\/([0-9]*))?$"));

    const int idx(re.indexIn(from));
    if (idx > -1) {
      const QString numCapture(re.cap(1));
      const QString denCapture(re.cap(2));

      const BuiltinInteger numValue(numericCast<BuiltinInteger>(numCapture));
      BuiltinInteger denValue(1);
      if (denCapture != QStringLiteral(""))
      {
        denValue = numericCast<BuiltinInteger>(denCapture);
      }

      return BoostRational(numValue, denValue);
    }
    else
    {
      qCritical() << "DataConvertors::numericCast<Rational>:"
                     " could not convert" << from;

      return NumericLimits::min<BoostRational>();
    }
  }
#endif // LP_WITH_MULTIPRECISION
}


#endif // DATACONVERTORS_TXX
