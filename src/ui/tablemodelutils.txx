#ifndef TABLEMODELUTILS_TXX
#define TABLEMODELUTILS_TXX

#include <QTableWidget>

#include "tablemodel.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"

using namespace DataConvertors;
using namespace Utils;

namespace TableModelUtils
{
  template<>
  inline bool convertTableModel<real_t, rational_t>(TableModel* const tableModel)
  {
    if (tableModel != nullptr)
    {
      fillTableModel(
        tableModel,
        [](const QVariant& value)
        {
          return numericCast<QString>(
            numericCast<real_t>(
              numericCast<rational_t>(value.toString())
            )
          );
        }
      );

      return true;
    }
    else
    {
      qDebug() << "TableModelUtils::convertTable<real_t, rational_t>: could not convert: `tableModel' argument is nullptr";

      return false;
    }
  }

  template<>
  inline bool convertTableModel<rational_t, real_t>(TableModel* const tableModel)
  {
    if (tableModel != nullptr)
    {
      fillTableModel(
        tableModel,
        [](const QVariant& value)
        {
          return numericCast<QString>(
            numericCast<rational_t>(
              numericCast<real_t>(value.toString())
            )
          );
        }
      );

      return true;
    }
    else
    {
      qDebug() << "TableModelUtils::convertTable<rational_t, real_t>: could not convert: `tableModel' argument is nullptr";

      return false;
    }
  }  
}

#endif // TABLEMODELUTILS_TXX
