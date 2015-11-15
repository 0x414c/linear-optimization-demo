#ifndef TABLEMODELUTILS_HXX
#define TABLEMODELUTILS_HXX

#include <functional>

#include <QTableWidget>
#include <QVariant>

#include "eigen3/Eigen/Core"

#include "tablemodel.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"

namespace TableModelUtils
{
  using namespace DataConvertors;
  using namespace Eigen;
  using namespace std;
  using namespace Utils;

  enum struct FillMethod : int
  {
    Zeroes = 0,
    Random = 1,
    Sequential = 2
  };

  //TODO: ~ Make these functions members of `TableModel' class
  bool fill(SimpleTableModel* const tableModel, FillMethod fillMethod);
  bool fill(
    SimpleTableModel* const tableModel,
    const function<QVariant(const QVariant&)>& callback
  );

  template<typename R, typename T>
  bool convert(SimpleTableModel* const tableModel)
  {
    static_assert(
      AlwaysFalse<R, T>::value,
      "TableModelUtils::convert<R, T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }

  template<typename T = Real>
  Matrix<T, Dynamic, 1>
  makeColumnVector(SimpleTableModel* const tableModel)
  {
    if (tableModel != nullptr)
    {
      if (tableModel->columnCount() == 1)
      {
        Matrix<T, Dynamic, 1> vec(tableModel->rowCount(), 1);

        for (DenseIndex i(0); i < vec.rows(); ++i)
        {
          vec(i) =
            numericCast<T>(
              tableModel->data(tableModel->index(i, 0)).toString()
            );
        }

        return vec;
      }
      else
      {
        qDebug() << "TableModelUtils::makeColumnVector:"
                    " `tableModel' should have 1 column";

        return Matrix<T, Dynamic, 1>::Zero(1, 1);
      }
    }
    else
    {
      qDebug() << "TableModelUtils::makeColumnVector:"
                  " `tableModel' argument is `nullptr'";

      return Matrix<T, Dynamic, 1>::Zero(1, 1);
    }
  }

  template<typename T = Real>
  Matrix<T, 1, Dynamic>
  makeRowVector(SimpleTableModel* const tableModel)
  {
    if (tableModel != nullptr)
    {
      if (tableModel->rowCount() == 1)
      {
        Matrix<T, 1, Dynamic> vec(1, tableModel->columnCount());

        for (DenseIndex i(0); i < vec.cols(); ++i)
        {
          vec(i) =
            numericCast<T>(
              tableModel->data(tableModel->index(0, i)).toString()
            );
        }

        return vec;
      }
      else
      {
        qDebug() << "TableModelUtils::makeRowVector:"
                    " `tableModel' should have 1 row";

        return Matrix<T, 1, Dynamic>::Zero(1, 1);
      }
    }
    else
    {
      qDebug() << "TableModelUtils::makeRowVector:"
                  " `tableModel' argument is `nullptr'";

      return Matrix<T, 1, Dynamic>::Zero(1, 1);
    }
  }

  template<typename T = Real>
  Matrix<T, Dynamic, Dynamic>
  makeMatrix(SimpleTableModel* const tableModel)
  {
    if (tableModel != nullptr)
    {
      if (tableModel->rowCount() > 0 && tableModel->columnCount() > 0)
      {
        Matrix<T, Dynamic, Dynamic> mat(
          tableModel->rowCount(), tableModel->columnCount()
        );

        for (DenseIndex i(0); i < mat.rows(); ++i)
        {
          for (DenseIndex j(0); j < mat.cols(); ++j)
          {
            mat(i, j) =
              numericCast<T>(
                tableModel->data(tableModel->index(i, j)).toString()
              );
          }
        }

        return mat;
      }
      else
      {
        qDebug() << "TableModelUtils::makeMatrix:"
                    " `tableModel' should not be empty";

        return Matrix<T, Dynamic, Dynamic>::Zero(1, 1);
      }
    }
    else
    {
      qDebug() << "TableModelUtils::makeMatrix:"
                  " `tableModel' argument is `nullptr'";

      return Matrix<T, Dynamic, Dynamic>::Zero(1, 1);
    }
  }
}

#include "tablemodelutils.txx"

#endif // TABLEMODELUTILS_HXX
