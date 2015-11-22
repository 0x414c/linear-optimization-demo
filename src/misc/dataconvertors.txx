#pragma once

#ifndef DATACONVERTORS_TXX
#define DATACONVERTORS_TXX


#include <cstdint>
#include <utility>

#include <QDebug>
#include <QLocale>
#include <QRegExp>
#include <QString>
#include <QStringList>

#include "../math/mathutils.hxx"
#include "../math/numericlimits.hxx"
#include "../math/numerictypes.hxx"
#include "../config.hxx"


namespace DataConvertors
{
  using namespace Config::MathUtils;
  using namespace MathUtils;
  using namespace NumericTypes;
  using namespace std;


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


  template<>
  inline Real
  numericCast<Real, Rational>(const Rational& from)
  {
    const Real value(Real(from.numerator()) / Real(from.denominator()));

    return value;
  }


  template<>
  inline Rational
  numericCast<Rational, Real>(const Real& from)
  {
    const pair<Integer, Integer> rationalized =
    rationalize<Integer>(from, Epsilon, MaxRationalizeIterations, 32768);

    return Rational(rationalized.first, rationalized.second);
  }


  template<>
  inline QString
  numericCast<QString, Real>(const Real& from)
  {
    const QString value(QString("%1").arg(from));

    return value;
  }


  template<>
  inline QString
  numericCast<QString, Rational>(const Rational& from)
  {
    QString value; //TODO: ~ Remove assignment, just construct-and-return
    if (from.denominator() == Integer(1))
    {
      value = QString("%1").arg(from.numerator());
    }
    else
    {
      if (from.numerator() == Integer(0))
      {
        value = QStringLiteral("0");
      }
      else
      {
        value = QString("%1/%2").arg(from.numerator()).arg(from.denominator());
      }
    }

    return value;
  }


  template<>
  inline Real
  numericCast<Real>(const QString& from)
  {
    bool isOk(false);
    const QLocale cLocale(QLocale::C);
    const Real value(cLocale.toDouble(from, &isOk));
    if (!isOk) {
      qWarning() <<
      QString("DataConvertors::numericCast<Real>: could not convert '%1'").
      arg(from).toLatin1().data(); //TODO: ~? Possible crash here

      return NumericLimits::min<Real>();
    }
    else
    {
      return value;
    }
  }


  template<>
  inline Integer
  numericCast<Integer>(const QString& from)
  {
    bool isOk(false);
    const QLocale cLocale(QLocale::C);
    const Integer value(cLocale.toLongLong(from, &isOk));
    if (!isOk) {
      qWarning() <<
      QString("DataConvertors::numericCast<Integer>: could not convert '%1'").
      arg(from).toLatin1().data(); //TODO: ~? The same as above

      return NumericLimits::min<Integer>();
    }
    else
    {
      return value;
    }
  }


  template<>
  inline Rational
  numericCast<Rational>(const QString& from)
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

      const Integer numValue(numericCast<Integer>(numCapture));
      Integer denValue(1);
      if (denCapture != QStringLiteral(""))
      {
        denValue = numericCast<Integer>(denCapture);
      }

      return Rational(numValue, denValue);
    }
    else
    {
      qDebug() << "DataConvertors::numericCast<Rational>:"
                  " could not convert" << from;

      return NumericLimits::min<Rational>();
    }
  }
}


#endif // DATACONVERTORS_TXX
