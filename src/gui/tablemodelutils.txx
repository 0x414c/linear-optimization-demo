#pragma once

#ifndef TABLEMODELUTILS_TXX
#define TABLEMODELUTILS_TXX


#include "tablemodelutils.hxx"

#include <stdexcept>

#include <QString>
#include <QTableWidget>
#include <QVariant>

#include "simpletablemodel.hxx"
#include "../misc/dataconvertors.hxx"
#include "../math/numerictypes.hxx"


namespace TableModelUtils
{
  using DataConvertors::numericCast;
  using GUI::SimpleTableModel;
  using NumericTypes::Rational;
  using NumericTypes::Real;
  using std::invalid_argument;


  template<>
  inline bool
  convert<Real, Rational>(SimpleTableModel* const tableModel)
  throw(invalid_argument)
  {
    if (tableModel == nullptr)
    {
      throw invalid_argument("`tableModel' == `nullptr'");
    }

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


  template<>
  inline bool
  convert<Rational, Real>(SimpleTableModel* const tableModel)
  throw(invalid_argument)
  {
    if (tableModel == nullptr)
    {
      throw invalid_argument("`tableModel' == `nullptr'");
    }

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
}


#endif // TABLEMODELUTILS_TXX
