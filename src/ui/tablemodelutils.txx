#ifndef TABLEMODELUTILS_TXX
#define TABLEMODELUTILS_TXX

#include <QTableWidget>

#include "tablemodel.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"

namespace TableModelUtils
{
  using namespace DataConvertors;
  using namespace Utils;

  template<>
  inline bool
  convert<Real, Rational>(SimpleTableModel* const tableModel)
  {
    if (tableModel != nullptr)
    {
      fill(
        tableModel,
        [](const QVariant& value)
        {
          return numericCast<QString>(
            numericCast<Real>(
              numericCast<Rational>(value.toString())
            )
          );
        }
      );

      return true;
    }
    else
    {
      qDebug() << "TableModelUtils::convert<Real, Rational>:"
                  " could not convert: `tableModel' argument is nullptr";

      return false;
    }
  }

  template<>
  inline bool
  convert<Rational, Real>(SimpleTableModel* const tableModel)
  {
    if (tableModel != nullptr)
    {
      fill(
        tableModel,
        [](const QVariant& value)
        {
          return numericCast<QString>(
            numericCast<Rational>(
              numericCast<Real>(value.toString())
            )
          );
        }
      );

      return true;
    }
    else
    {
      qDebug() << "TableModelUtils::convertTable<Rational, Real>:"
                  " could not convert: `tableModel' argument is nullptr";

      return false;
    }
  }
}

#endif // TABLEMODELUTILS_TXX
