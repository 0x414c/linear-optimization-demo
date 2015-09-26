#ifndef DATACONVERTORS_HXX
#define DATACONVERTORS_HXX

#include <limits>

#include <QDebug>
#include <QLocale>
#include <QString>

#include "utils.hxx"

using namespace std;

namespace DataConvertors
{
  template<typename T>
  inline T toScalar(const QString& from)
  {
    static_assert(always_false<T>::value, "You must only use one of the specified specializations!");

    return T(0);
  }

  template<>
  inline double toScalar<double>(const QString& from) {
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
  inline int toScalar<int>(const QString &from) {
    bool ok = false;
    QLocale cLocale(QLocale::C);
    int value = cLocale.toInt(from, &ok);
    if (!ok) {
      qWarning(QString("toScalar<int>: could not convert %1").arg(from).toLatin1().data()); //TODO:

      return numeric_limits<int>::lowest();
    }
    else
    {
      qDebug() << "toScalar<int>: converted" << from << "to" << value;

      return value;
    }
  }
};

#endif // DATACONVERTORS_HXX
