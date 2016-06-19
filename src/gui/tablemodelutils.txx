#pragma once

#ifndef TABLEMODELUTILS_TXX
#define TABLEMODELUTILS_TXX


#include "tablemodelutils.hxx"

#include <stdexcept>

#include <QString>
#include <QTableWidget>
#include <QVariant>

#include "stringtablemodel.hxx"
#include "../misc/dataconvertors.hxx"
#include "../math/numerictypes.hxx"


namespace TableModelUtils
{
  using DataConvertors::numericCast;
  using GUI::StringTableModel;
  using NumericTypes::rational_t;
  using NumericTypes::real_t;
  using std::invalid_argument;


  template<>
  inline bool
  convert<real_t, rational_t>(StringTableModel* const tableModel)
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
            numericCast<real_t>(
              numericCast<rational_t>(value.toString())
            )
          );
        }
      )
    );

    return ret;
  }


  template<>
  inline bool
  convert<rational_t, real_t>(StringTableModel* const tableModel)
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
            numericCast<rational_t>(
              numericCast<real_t>(value.toString())
            )
          );
        }
      )
    );

    return ret;
  }
}


#endif // TABLEMODELUTILS_TXX
