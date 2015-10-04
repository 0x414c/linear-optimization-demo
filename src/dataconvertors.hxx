#ifndef DATACONVERTORS_HXX
#define DATACONVERTORS_HXX

#include <limits>

#include <QDebug>
#include <QLocale>
#include <QRegExp>
#include <QString>
#include <QStringList>

#include "utils.hxx"

using namespace std;
using namespace Utils;

namespace DataConvertors
{
  template<typename T>
  inline T toScalar(const QString& from)
  {
    static_assert(always_false<T>::value, "DataConvertors::toScalar<T>: You can only use one of the specified specializations!");

    return T(0);
  }

  template<>
  inline double toScalar<double>(const QString& from)
  {
    bool ok = false;
    QLocale cLocale(QLocale::C);
    double value = cLocale.toDouble(from, &ok);
    if (!ok) {
      qWarning(QString("toScalar<double>: could not convert %1").arg(from).toLatin1().data()); //TODO: possible crash here

      return numeric_limits<double>::lowest();
    }
    else
    {
      qDebug() << "toScalar<double>: converted" << from << "to" << value;

      return value;
    }
  }

  template<>
  inline int toScalar<int>(const QString &from)
  {
    bool ok = false;
    QLocale cLocale(QLocale::C);
    int value = cLocale.toInt(from, &ok);
    if (!ok) {
      qWarning(QString("toScalar<int>: could not convert %1").arg(from).toLatin1().data()); //TODO: same as above

      return numeric_limits<int>::lowest();
    }
    else
    {
      qDebug() << "toScalar<int>: converted" << from << "to" << value;

      return value;
    }
  }

  template<>
  inline llong_t toScalar<llong_t>(const QString &from)
  {
    bool ok = false;
    QLocale cLocale(QLocale::C);
    llong_t value = cLocale.toLongLong(from, &ok);
    if (!ok) {
      qWarning(QString("toScalar<llong>: could not convert %1").arg(from).toLatin1().data()); //TODO: same as above

      return numeric_limits<llong_t>::lowest();
    }
    else
    {
      qDebug() << "toScalar<llong>: converted" << from << "to" << value << "LL";

      return value;
    }
  }

  template<>
  inline rat_t toScalar<rat_t>(const QString& from)
  {
    QRegExp re(QStringLiteral("^([-+]?[0-9]+)(?:\\/([0-9]+))?$"));
    int idx = re.indexIn(from);
    if (idx > -1) {
        QString numCap = re.cap(1);
        QString denCap = re.cap(2);

        int_t num = toScalar<int_t>(numCap);
        int_t den = 1;
        if (denCap != "")
        {
          den = toScalar<int_t>(denCap);
        }
        rat_t value(num, den);

        qDebug() << "toScalar<frac>: converted" << from << "to frac(" << num << "/" << den << ")";

        return value;
    }
    else
    {
      qDebug() << "toScalar<frac>: invalid input" << from;
      rat_t value(numeric_limits<int_t>::lowest());

      return value;
    }
  }
}

#endif // DATACONVERTORS_HXX
