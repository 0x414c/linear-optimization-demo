#ifndef DATACONVERTORS_HXX
#define DATACONVERTORS_HXX

#include <cstdint>
#include <limits>

#include <QDebug>
#include <QLocale>
#include <QRegExp>
#include <QString>
#include <QStringList>

#include "utils.hxx"
#include "../math/mathutils.hxx"

using namespace std;
using namespace MathUtils;
using namespace Utils;

namespace DataConvertors
{
  template<typename R>
  inline R numericCast(const QString& from) //TODO: ~? use second template only
  {
    static_assert(
      AlwaysFalse<R>::value,
      "DataConvertors::numericCast<R>: You can only use one of the specified specializations!"
    );

    return R(0);
  }

  template<typename R, typename T>
  inline R numericCast(const T& from)
  {
    static_assert(
      AlwaysFalse<R, T>::value,
      "DataConvertors::numericCast<R, T>: You can only use one of the specified specializations!"
    );

    return R(0);
  }

  template<>
  inline real_t numericCast<real_t, real_t>(const real_t& from)
  {
//    qDebug() << "DataConvertors::numericCast<real_t, real_t>: converted"
//             << from << "=>" << from;

    return from;
  }

  template<>
  inline rational_t numericCast<rational_t, rational_t>(const rational_t& from)
  {
//    qDebug() << "DataConvertors::numericCast<rational_t, rational_t>: converted"
//             << from.numerator() << "/" << from.denominator() << "=>" << from.numerator() << "/" << from.denominator();

    return from;
  }

  template<>
  inline real_t numericCast<real_t, rational_t>(const rational_t& from)
  {
    real_t value = real_t(from.numerator()) / real_t(from.denominator());

//    qDebug() << "DataConvertors::numericCast<real_t, rational_t>: converted"
//             << from.numerator() << "/" << from.denominator() << "=>" << value;

    return value;
  }

  template<>
  inline rational_t numericCast<rational_t, real_t>(const real_t& from)
  {
    auto rationalized = rationalize(from);

//    qDebug() << "DataConvertors::numericCast<rational_t, real_t>: converted"
//             << from << "=>" << rationalized.first << "/" << rationalized.second;

    return rational_t(rationalized.first, rationalized.second);
  }

  template<>
  inline QString numericCast<QString, real_t>(const real_t& from)
  {
    QString value = QString("%1").arg(from);

//    qDebug() << "DataConvertors::numericCast<QString, real_t>: converted"
//             << from << "=>" << value;

    return value;
  }

  template<>
  inline QString numericCast<QString, rational_t>(const rational_t& from)
  {
    QString value;
    if (from.denominator() == 1)
    {
      value = QString("%1").arg(from.numerator());
    }
    else
    {
      if (from.numerator() == 0)
      {
        value = QStringLiteral("0");
      }
      else
      {
        value = QString("%1/%2").arg(from.numerator()).arg(from.denominator());
      }
    }

//    qDebug() << "DataConvertors::numericCast<QString, rational_t>: converted"
//             << from.numerator() << "/" << from.denominator() << "=>" << value;

    return value;
  }

  template<>
  inline real_t numericCast<real_t>(const QString& from)
  {
    bool ok = false;
    QLocale cLocale(QLocale::C);
    real_t value = cLocale.toDouble(from, &ok);
    if (!ok) {
      qWarning(QString("DataConvertors::numericCast<real_t>: could not convert '%1'").arg(from).toLatin1().data()); //TODO: ~? possible crash here

      return numeric_limits<double>::lowest();
    }
    else
    {
//      qDebug() << "DataConvertors::numericCast<real_t>: converted"
//               << from << "=>" << value;

      return value;
    }
  }

  template<>
  inline integer_t numericCast<integer_t>(const QString& from)
  {
    bool ok = false;
    QLocale cLocale(QLocale::C);
    integer_t value = cLocale.toLongLong(from, &ok);
    if (!ok) {
      qWarning(QString("DataConvertors::numericCast<integer_t>: could not convert '%1'").arg(from).toLatin1().data()); //TODO: same as above

      return numeric_limits<integer_t>::lowest();
    }
    else
    {
//      qDebug() << "DataConvertors::numericCast<integer_t>: converted"
//      << from << "=>" << value;

      return value;
    }
  }

  template<>
  inline rational_t numericCast<rational_t>(const QString& from)
  {
    //We assume that the input string `from'
    //was already validated
    //and no zero denominator can appear here
    //so this regexp `re' is simpler (than one
    //contained in corresponding validator)
    QRegExp re(QStringLiteral("^([-+]?[0-9]+)(?:\\/([0-9]*))?$"));
    int idx = re.indexIn(from);
    if (idx > -1) {
      QString numCapture = re.cap(1);
      QString denCapture = re.cap(2);

      integer_t numValue = numericCast<integer_t>(numCapture);
      integer_t denValue = 1;
      if (denCapture != QStringLiteral(""))
      {
        denValue = numericCast<integer_t>(denCapture);
      }
      rational_t value(numValue, denValue);

//      qDebug() << "DataConvertors::numericCast<rat_t>: converted"
//      << from << "=>" << numValue << "/" << denValue;

      return value;
    }
    else
    {
      qDebug() << "DataConvertors::numericCast<rat_t>: could not convert" << from;

      rational_t value(numeric_limits<integer_t>::lowest());

      return value;
    }
  }
}

#endif // DATACONVERTORS_HXX
