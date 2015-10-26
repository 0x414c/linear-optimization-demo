#ifndef TABLEMODELUTILS_HXX
#define TABLEMODELUTILS_HXX

#include <functional>

#include <QTableWidget>
#include <QVariant>

#include "eigen3/Eigen/Dense"

#include "tablemodel.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"

using namespace DataConvertors;
using namespace Eigen;
using namespace std;
using namespace Utils;

namespace TableModelUtils
{
  enum struct FillMethod { Zeroes, Random, Sequential };

  //TODO: ~? Make these functions members of `TableModel'
  bool fillTableModel(TableModel* const tableModel, FillMethod fillMethod);
  bool fillTableModel(TableModel* const tableModel, const function<QVariant(const QVariant&)>& callback);

  template<typename R, typename T>
  bool convertTableModel(TableModel* const tableModel)
  {
    static_assert(
      False<R, T>::value,
      "TableModelUtils::convertTable<R, T>: You can only use one of the specified specializations!"
    );

    return false;
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

#include "tablemodelutils.txx"

#endif // TABLEMODELUTILS_HXX
