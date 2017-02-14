#pragma once

#ifndef DATACONVERTORS_TXX
#define DATACONVERTORS_TXX


#include "dataconvertors.hxx"

#include <cstdint>

#ifdef LP_WITH_MULTIPRECISION
#include <stdexcept>
#endif // LP_WITH_MULTIPRECISION
#include <utility>

#include <QDebug>
#include <QLocale>
#include <QRegExp>
#include <QString>

#ifdef LP_WITH_MULTIPRECISION
#include "boost/multiprecision/cpp_bin_float.hpp"
#include "boost/multiprecision/cpp_int.hpp"
#endif // LP_WITH_MULTIPRECISION
#include <fmt/format.h>

#include "boostextensions.hxx"
#include "../math/mathutils.hxx"
#include "../math/numericlimits.hxx"
#include "../math/numerictypes.hxx"
#include "../config.hxx"


namespace DataConvertors
{
  using namespace Config::MathUtils;
  using Config::App::Locale;
  using fmt::format;
  using MathUtils::rationalize;
  using NumericTypes::rational_t;
  using NumericTypes::real_t;
  using std::pair;


  template<>
  inline real_t
  numericCast<real_t, real_t>(const real_t& from)
  {
    return from;
  }


  template<>
  inline rational_t
  numericCast<rational_t, rational_t>(const rational_t& from)
  {
    return from;
  }


#ifdef LP_WITH_MULTIPRECISION
  using NumericTypes::boost_real_t;
  using NumericTypes::boost_integer_t;
  using NumericTypes::boost_rational_t;
  using std::runtime_error;


  template<>
  inline boost_real_t
  numericCast<boost_real_t, boost_rational_t>(const boost_rational_t& from)
  {
    const boost_real_t value(
      boost_real_t(numerator(from)) / boost_real_t(denominator(from))
    );

    return value;
  }


  template<>
  inline boost_rational_t
  numericCast<boost_rational_t, boost_real_t>(const boost_real_t& from)
  {
    const pair<boost_integer_t, boost_integer_t> rationalized(
      rationalize<boost_integer_t, boost_real_t>(
        from, Epsilon,
        MaxRationalizeDenominator, MaxRationalizeIterations
      )
    );

    return boost_rational_t(rationalized.first, rationalized.second);
  }


  template<>
  inline QString
  numericCast<QString, boost_real_t>(const boost_real_t& from)
  {
    const QString value(QString::fromStdString(format("{}", from)));

    return value;
  }


  template<>
  inline QString
  numericCast<QString, boost_rational_t>(const boost_rational_t& from)
  {
    QString value; //TODO: [0;0] Remove assignment, just construct-and-return.

    if (denominator(from) == boost_integer_t(1))
    {
      value = QString::fromStdString(format("{}", numerator(from)));
    }
    else
    {
      if (numerator(from) == boost_integer_t(0))
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
  inline boost_real_t
  numericCast<boost_real_t>(const QString& from)
  {
    try
    {
      const boost_real_t value(from.toStdString());

      return value;
    }
    catch (const std::runtime_error& re)
    {
      return NumericLimits::min<boost_real_t>();
    }
  }


  template<>
  inline boost_integer_t
  numericCast<boost_integer_t>(const QString& from)
  {
    try
    {
      const boost_integer_t value(from.toStdString());

      return value;
    }
    catch (const std::runtime_error& re)
    {
      return NumericLimits::min<boost_integer_t>();
    }
  }


  template<>
  inline boost_rational_t
  numericCast<boost_rational_t>(const QString& from)
  {
    try
    {
      const boost_rational_t value(from.toStdString());

      return value;
    }
    catch (const std::runtime_error& re)
    {
      return NumericLimits::min<boost_rational_t>();
    }
  }
#else // LP_WITH_MULTIPRECISION
  using NumericTypes::builtin_real_t;
  using NumericTypes::builtin_integer_t;
  using NumericTypes::boost_rational_t;


  template<>
  inline builtin_real_t
  numericCast<builtin_real_t, boost_rational_t>(const boost_rational_t& from)
  {
    const builtin_real_t value(
      builtin_real_t(from.numerator()) / builtin_real_t(from.denominator())
    );

    return value;
  }


  template<>
  inline boost_rational_t
  numericCast<boost_rational_t, builtin_real_t>(const builtin_real_t& from)
  {
    const pair<builtin_integer_t, builtin_integer_t> rationalized =
      rationalize<builtin_integer_t, builtin_real_t>(
        from, Epsilon,
        MaxRationalizeDenominator,
        MaxRationalizeIterations
      );

    return boost_rational_t(rationalized.first, rationalized.second);
  }


  template<>
  inline QString
  numericCast<QString, builtin_real_t>(const builtin_real_t& from)
  {
    const QString value(QString::fromStdString(format("{}", from)));

    return value;
  }


  template<>
  inline QString
  numericCast<QString, boost_rational_t>(const boost_rational_t& from)
  {
    QString value; //TODO: [0;0] Remove assignment, just construct-and-return.

    if (from.denominator() == builtin_integer_t(1))
    {
      value = QString::fromStdString(format("{}", from.numerator()));
    }
    else
    {
      if (from.numerator() == builtin_integer_t(0))
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
  inline builtin_real_t
  numericCast<builtin_real_t>(const QString& from)
  {
    bool isOk(false);
    const builtin_real_t value(Locale.toDouble(from, &isOk));
    if (!isOk) {
      qCritical() << "DataConvertors::numericCast<builtin_real_t>:"
                     " could not convert" << from;

      return NumericLimits::min<builtin_real_t>();
    }
    else
    {
      return value;
    }
  }


  template<>
  inline builtin_integer_t
  numericCast<builtin_integer_t>(const QString& from)
  {
    bool isOk(false);
    const builtin_integer_t value(Locale.toLongLong(from, &isOk));
    if (!isOk) {
      qCritical() << "DataConvertors::numericCast<builtin_integer_t>:"
                     " could not convert" << from;

      return NumericLimits::min<builtin_integer_t>();
    }
    else
    {
      return value;
    }
  }


  template<>
  inline boost_rational_t
  numericCast<boost_rational_t>(const QString& from)
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

      const builtin_integer_t numValue(numericCast<builtin_integer_t>(numCapture));
      builtin_integer_t denValue(1);
      if (denCapture != QStringLiteral(""))
      {
        denValue = numericCast<builtin_integer_t>(denCapture);
      }

      return boost_rational_t(numValue, denValue);
    }
    else
    {
      qCritical() << "DataConvertors::numericCast<boost_rational_t>:"
                     " could not convert" << from;

      return NumericLimits::min<boost_rational_t>();
    }
  }
#endif // LP_WITH_MULTIPRECISION
}


#endif // DATACONVERTORS_TXX
