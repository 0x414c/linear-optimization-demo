#pragma once

#ifndef TABLEMODELUTILS_TXX
#define TABLEMODELUTILS_TXX


#include "tablemodelutils.hxx"

#include <QTableWidget>

#include "simpletablemodel.hxx"
#include "../misc/dataconvertors.hxx"
#include "../math/numerictypes.hxx"


namespace TableModelUtils
{
  using namespace DataConvertors;
  using namespace NumericTypes;


  template<>
  inline bool
  convert<Real, Rational>(SimpleTableModel* const tableModel)
  {
    if (tableModel != nullptr)
    {
      const bool ret(
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
        )
      );

      return ret;
    }
    else
    {
      qDebug() << "TableModelUtils::convert<Real, Rational>:"
                  " could not convert: `tableModel' == `nullptr'";

      return false;
    }
  }


  template<>
  inline bool
  convert<Rational, Real>(SimpleTableModel* const tableModel)
  {
    if (tableModel != nullptr)
    {
      const bool ret(
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
        )
      );

      return ret;
    }
    else
    {
      qDebug() << "TableModelUtils::convertTable<Rational, Real>:"
                  " could not convert: `tableModel' == `nullptr'";

      return false;
    }
  }
}


#endif // TABLEMODELUTILS_TXX
