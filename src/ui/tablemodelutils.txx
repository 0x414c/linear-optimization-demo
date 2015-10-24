#ifndef TABLEMODELUTILS_TXX
#define TABLEMODELUTILS_TXX

#include <QTableWidget>

#include "eigen3/Eigen/Dense"

#include "tablemodel.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"

using namespace DataConvertors;
using namespace Eigen;
using namespace Utils;

namespace TableModelUtils
{
  template<typename R, typename T>
  bool convertTableModel(TableModel* const tableModel)
  {
    static_assert(
      AlwaysFalse<R, T>::value,
      "TableModelUtils::convertTable<R, T>: You can only use one of the specified specializations!"
    );

    return false;
  }

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

  template<typename T = real_t>
  auto getColumnVector(TableModel* const tableModel)
  {
    if (tableModel != nullptr)
    {
      Matrix<T, Dynamic, 1> vec(tableModel->rowCount(), 1);

      for (DenseIndex i = 0; i < vec.rows(); ++i)
      {
        vec(i) = numericCast<T>(tableModel->data(tableModel->index(i, 0)).toString());
      }

      return vec;
    }
    else
    {
      qDebug() << "TableModelUtils::getColumnVector: tableModel argument is nullptr";

      return Matrix<T, Dynamic, 1>();
    }
  }

  template<typename T = real_t>
  auto getRowVector(TableModel* const tableModel)
  {
    if (tableModel != nullptr)
    {
      Matrix<T, 1, Dynamic> vec(1, tableModel->columnCount());

      for (DenseIndex i = 0; i < vec.cols(); ++i)
      {
        vec(i) = numericCast<T>(tableModel->data(tableModel->index(0, i)).toString());
      }

      return vec;
    }
    else
    {
      qDebug() << "TableModelUtils::getRowVector: tableModel argument is nullptr";

      return Matrix<T, 1, Dynamic>();
    }
  }

  template<typename T = real_t>
  auto getMatrix(TableModel* const tableModel)
  {
    if (tableModel != nullptr)
    {
      Matrix<T, Dynamic, Dynamic> mat(tableModel->rowCount(), tableModel->columnCount());

      for (DenseIndex i = 0; i < mat.rows(); ++i)
      {
        for (DenseIndex j = 0; j < mat.cols(); ++j)
        {
          mat(i, j) = numericCast<T>(tableModel->data(tableModel->index(i, j)).toString());
        }
      }

      return mat;
    }
    else
    {
      qDebug() << "TableModelUtils::getMatrix: tableModel argument is nullptr";

      return Matrix<T, Dynamic, Dynamic>();
    }
  }
}

#endif // TABLEMODELUTILS_TXX
