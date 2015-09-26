#ifndef TABLEWIDGETUTILS_TXX
#define TABLEWIDGETUTILS_TXX

#include <QTableWidget>

#include "dataconvertors.hxx"

#include "lib/eigen3/Eigen/Dense"

using namespace Eigen;
using namespace DataConvertors;

namespace TableWidgetUtils {
  template<typename T = double>
  auto getColumnVector(QTableWidget* tableWidget) {
    auto vec = Matrix<T, Dynamic, 1> (tableWidget->rowCount());

    for (DenseIndex i = 0; i < vec.rows(); ++i) {
      vec(i) = toScalar<T>(tableWidget->item(i, 0)->text());
    }

    return vec;
  }

  template<typename T = double>
  auto getRowVector(QTableWidget* tableWidget) {
    auto vec = Matrix<T, 1, Dynamic> (tableWidget->rowCount());

    for (DenseIndex i = 0; i < vec.rows(); ++i) {
      vec(i) = toScalar<T>(tableWidget->item(0, i)->text());
    }

    return vec;
  }

  template<typename T = double>
  auto getMatrix(QTableWidget* tableWidget) {
    auto mat = Matrix<T, Dynamic, Dynamic> (tableWidget->rowCount(), tableWidget->columnCount());

    for (DenseIndex i = 0; i < mat.rows(); ++i) {
      for (DenseIndex j = 0; j < mat.cols(); ++j) {
        mat(i, j) = toScalar<T>(tableWidget->item(i, j)->text());
      }
    }

    return mat;
  }
}

#endif // TABLEWIDGETUTILS_TXX
