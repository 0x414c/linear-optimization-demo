﻿#pragma once

#ifndef TABLEMODELUTILS_HXX
#define TABLEMODELUTILS_HXX


#include <functional>
#include <stdexcept>

#include <QTableWidget>
#include <QString>
#include <QVariant>

#include "eigen3/Eigen/Core"

#include "simpletablemodel.hxx"
#include "../lp/simplextableau.hxx"
#include "../math/mathutils.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/dataconvertors.hxx"


namespace TableModelUtils
{
  using DataConvertors::numericCast;
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using GUI::SimpleTableModel;
  using LinearProgramming::SimplexTableau;
  using NumericTypes::Rational;
  using NumericTypes::Real;
  using Utils::AlwaysFalse;
  using std::function;
  using std::invalid_argument;


  enum struct FillMethod : int
  {
    Zeroes = 0,
    Random = 1,
    Sequential = 2
  };


  //TODO: ~? Make these functions members of the `SimpleTableModel' class
  bool fill(SimpleTableModel* const tableModel, FillMethod fillMethod)
  throw(invalid_argument);

  bool fill(SimpleTableModel* const tableModel, const QString& value)
  throw(invalid_argument);

  bool fill(
    SimpleTableModel* const tableModel,
    const function<QVariant(const QVariant&)>& callback
  ) throw(invalid_argument);


  template<typename T = Real>
  bool
  fill(
    SimpleTableModel* const tableModel,
    const Matrix<T, Dynamic, Dynamic>& matrix
  ) throw(invalid_argument)
  {
    if (tableModel == nullptr)
    {
      throw invalid_argument("`tableModel' == `nullptr'");
    }

    if (
      tableModel->rowCount() != matrix.rows() ||
      tableModel->columnCount() != matrix.cols()
    )
    {
      throw invalid_argument("tableModel->rowCount() != tableau.rows()"
                             " || tableModel->columnCount() != tableau.cols()");
    }

    for (DenseIndex row(0); row < matrix.rows(); ++row)
    {
      for (DenseIndex col(0); col < matrix.cols(); ++col)
      {
        const QModelIndex index(tableModel->index(row, col));
        tableModel->setData(
          index,
          numericCast<QString, T>(matrix(row, col))
        );
      }
    }

    return true;
  }


  template<typename T = Real>
  bool
  fill(
    SimpleTableModel* const tableModel,
    const SimplexTableau<T>& tableau
  ) throw(invalid_argument)
  {
    if (tableModel == nullptr)
    {
      throw invalid_argument("`tableModel' == `nullptr'");
    }

    if (
      tableModel->rowCount() != tableau.rows() ||
      tableModel->columnCount() != tableau.cols()
    )
    {
      throw invalid_argument("tableModel->rowCount() != tableau.rows()"
                             " || tableModel->columnCount() != tableau.cols()");
    }

    fill<T>(tableModel, tableau.entries());

    for (DenseIndex row(0); row < tableau.rows() - 1; ++row)
    {
      tableModel->setHeaderData(
        row,
        Qt::Vertical,
        QStringLiteral("x") +
        numericCast<QString, T>(tableau.basicVars().at(row) + T(1))
      );

      for (DenseIndex col(0); col < tableau.cols() - 1; ++col)
      {
        tableModel->setHeaderData(
          col,
          Qt::Horizontal,
          QStringLiteral("x") +
          numericCast<QString, T>(tableau.freeVars().at(col) + T(1))
        );
      }
    }

    tableModel->setHeaderData(
      tableau.rows() - 1,
      Qt::Vertical,
      QStringLiteral("P")
    );

    tableModel->setHeaderData(
      tableau.cols() - 1,
      Qt::Horizontal,
      QStringLiteral("β")
    );

    return true;
  }


  template<typename R, typename T>
  bool
  convert(SimpleTableModel* const tableModel) throw(invalid_argument)
  {
    static_assert(
      AlwaysFalse<R, T>::value,
      "TableModelUtils::convert<R, T>:"
      " You can only use one of the specified specializations!"
    );

    return false;
  }


  template<typename T = Real>
  bool
  setFlags(
    SimpleTableModel* const tableModel,
    const Matrix<T, Dynamic, Dynamic>& matrix,
    const function<bool(const T&)>& callback,
    Qt::ItemFlags flags) throw(invalid_argument)
  {
    if (tableModel == nullptr)
    {
      throw invalid_argument("`tableModel' == `nullptr'");
    }

    if (
      tableModel->rowCount() != matrix.rows() ||
      tableModel->columnCount() != matrix.cols()
    )
    {
      throw invalid_argument("tableModel->rowCount() != matrix.rows()"
                             " || tableModel->columnCount() != matrix.cols()");
    }

    if (!callback)
    {
      throw invalid_argument("`callback' == (empty)");
    }

    for (DenseIndex row(0); row < matrix.rows(); ++row)
    {
      for (DenseIndex col(0); col < matrix.cols(); ++col)
      {
        const QModelIndex index(tableModel->index(row, col));
        const bool res(callback(matrix(row, col)));
        const Qt::ItemFlags oldFlags(tableModel->flags(index));

        if (res)
        {
          tableModel->setFlags(index, oldFlags | flags);
        }
        else
        {
          tableModel->setFlags(index, oldFlags & ~flags);
        }
      }
    }

    return true;
  }


  template<typename T = Real>
  Matrix<T, Dynamic, 1>
  makeColumnVector(SimpleTableModel* const tableModel) throw(invalid_argument)
  {
    if (tableModel == nullptr)
    {
      throw invalid_argument("`tableModel' == `nullptr'");
    }

    if (tableModel->columnCount() != 1 || tableModel->rowCount() == 0)
    {
      throw invalid_argument("tableModel->columnCount() != 1"
                             " || tableModel->rowCount() == 0");
    }

    Matrix<T, Dynamic, 1> vec(tableModel->rowCount(), 1);

    for (DenseIndex i(0); i < vec.rows(); ++i)
    {
      vec(i) =
        numericCast<T>(
          tableModel->data(tableModel->index(i, 0)).
          toString()
        );
    }

    return vec;
  }


  template<typename T = Real>
  Matrix<T, 1, Dynamic>
  makeRowVector(SimpleTableModel* const tableModel) throw(invalid_argument)
  {
    if (tableModel == nullptr)
    {
      throw invalid_argument("`tableModel' == `nullptr'");
    }

    if (tableModel->rowCount() != 1 || tableModel->columnCount() == 0)
    {
      throw invalid_argument("tableModel->rowCount() != 1"
                             " || tableModel->columnCount() == 0");
    }

    Matrix<T, 1, Dynamic> vec(1, tableModel->columnCount());

    for (DenseIndex i(0); i < vec.cols(); ++i)
    {
      vec(i) =
        numericCast<T>(
          tableModel->data(tableModel->index(0, i)).
          toString()
        );
    }

    return vec;
  }


  template<typename T = Real>
  Matrix<T, Dynamic, Dynamic>
  makeMatrix(SimpleTableModel* const tableModel) throw(invalid_argument)
  {
    if (tableModel == nullptr)
    {
      throw invalid_argument("`tableModel' == `nullptr'");
    }

    if (tableModel->rowCount() == 0 || tableModel->columnCount() == 0)
    {
      throw invalid_argument("tableModel->rowCount() == 0"
                             " || tableModel->columnCount() == 0");
    }

    Matrix<T, Dynamic, Dynamic> mat(
      tableModel->rowCount(), tableModel->columnCount()
    );

    for (DenseIndex i(0); i < mat.rows(); ++i)
    {
      for (DenseIndex j(0); j < mat.cols(); ++j)
      {
        mat(i, j) =
          numericCast<T>(
            tableModel->data(tableModel->index(i, j)).
            toString()
          );
      }
    }

    return mat;
  }
}


#include "tablemodelutils.txx"


#endif // TABLEMODELUTILS_HXX
