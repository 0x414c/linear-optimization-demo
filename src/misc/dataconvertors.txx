#ifndef DATACONVERTORS_TXX
#define DATACONVERTORS_TXX

#include "dataconvertors.hxx"

#include <cstdint>
#include <limits>
#include <utility>

#include <QDebug>
#include <QLocale>
#include <QRegExp>
#include <QString>
#include <QStringList>

#include "utils.hxx"
#include "../math/mathutils.hxx"
#include "../math/numerictypes.hxx"

namespace DataConvertors
{
  using namespace std;
  using namespace MathUtils;
  using namespace NumericTypes;
  using namespace Utils;

  template<>
  inline Real
  numericCast<Real, Real>(const Real& from)
  {
//    qDebug() << "DataConvertors::numericCast<Real, Real>: converted"
//             << from << "=>" << from;

    return from;
  }

  template<>
  inline Rational
  numericCast<Rational, Rational>(const Rational& from)
  {
//    qDebug() << "DataConvertors::numericCast<Rational, Rational>: converted"
//             << from.numerator() << "/" << from.denominator() << "=>"
//             << from.numerator() << "/" << from.denominator();

    return from;
  }

  template<>
  inline Real
  numericCast<Real, Rational>(const Rational& from)
  {
    Real value(Real(from.numerator()) / Real(from.denominator()));

//    qDebug() << "DataConvertors::numericCast<Real, Rational>: converted"
//             << from.numerator() << "/" << from.denominator() << "=>" << value;

    return value;
  }

  template<>
  inline Rational
  numericCast<Rational, Real>(const Real& from)
  {
    pair<Integer, Integer> rationalized = rationalize<Integer>(from);

//    qDebug() << "DataConvertors::numericCast<Rational, Real>: converted"
//             << from << "=>" << rationalized.first << "/" << rationalized.second;

    return Rational(rationalized.first, rationalized.second);
  }

  template<>
  inline QString
  numericCast<QString, Real>(const Real& from)
  {
    QString value(QString("%1").arg(from));

//    qDebug() << "DataConvertors::numericCast<QString, Real>: converted"
//             << from << "=>" << value;

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

//    qDebug() << "DataConvertors::numericCast<QString, Rational>: converted"
//             << from.numerator() << "/" << from.denominator() << "=>" << value;

    return value;
  }

  template<>
  inline Real
  numericCast<Real>(const QString& from)
  {
    bool isOk(false);
    QLocale cLocale(QLocale::C);
    Real value(cLocale.toDouble(from, &isOk));
    if (!isOk) {
      qWarning() <<
        QString("DataConvertors::numericCast<Real>: could not convert '%1'").
        arg(from).toLatin1().data(); //TODO: ~? Possible crash here

      return numeric_limits<Real>::lowest();
    }
    else
    {
//      qDebug() << "DataConvertors::numericCast<Real>: converted"
//               << from << "=>" << value;

      return value;
    }
  }

  template<>
  inline Integer
  numericCast<Integer>(const QString& from)
  {
    bool isOk(false);
    QLocale cLocale(QLocale::C);
    Integer value(cLocale.toLongLong(from, &isOk));
    if (!isOk) {
      qWarning() <<
        QString("DataConvertors::numericCast<Integer>: could not convert '%1'").
        arg(from).toLatin1().data(); //TODO: ~? The same as above

      return numeric_limits<Integer>::lowest();
    }
    else
    {
//      qDebug() << "DataConvertors::numericCast<Integer>: converted"
//      << from << "=>" << value;

      return value;
    }
  }

  template<>
  inline Rational
  numericCast<Rational>(const QString& from)
  {
    //We assume that the input string `from' was
    //already validated and no zero denominator
    //can appear in the input string, so this regexp
    //`re' can be simpler
    //(than the one contained in corresponding validator).
    QRegExp re(QStringLiteral("^([-+]?[0-9]+)(?:\\/([0-9]*))?$"));
    int idx(re.indexIn(from));
    if (idx > -1) {
      QString numCapture(re.cap(1));
      QString denCapture(re.cap(2));

      Integer numValue(numericCast<Integer>(numCapture));
      Integer denValue(1);
      if (denCapture != QStringLiteral(""))
      {
        denValue = numericCast<Integer>(denCapture);
      }
      Rational value(numValue, denValue);

//      qDebug() << "DataConvertors::numericCast<rat_t>: converted"
//      << from << "=>" << numValue << "/" << denValue;

      return value;
    }
    else
    {
      qDebug() << "DataConvertors::numericCast<rat_t>:"
                  " could not convert" << from;

      Rational value(numeric_limits<Integer>::lowest());

      return value;
    }
  }
}

#endif // DATACONVERTORS_TXX
