#ifndef TABLEWIDGETUTILS_TXX
#define TABLEWIDGETUTILS_TXX

#include <QTableWidget>
#include "eigen3/Eigen/Dense"

using namespace Eigen;

namespace TableWidgetUtils {
  template<typename T>
  auto getColumnVector(QTableWidget* tableWidget) {
    auto vec = Matrix<T, Dynamic, 1> (tableWidget->rowCount());

    for (int i = 0; i < vec.rows(); ++i) {
      vec(i) = tableWidget->item(i, 0)->text().toDouble();
    }

    return vec;
  }

  template<typename T>
  auto getRowVector(QTableWidget* tableWidget) {
    auto vec = Matrix<T, 1, Dynamic> (tableWidget->rowCount());

    for (int i = 0; i < vec.rows(); ++i) {
      vec(i) = tableWidget->item(0, i)->text().toDouble();
    }

    return vec;
  }

  template<typename T>
  auto getMatrix(QTableWidget* tableWidget) {
    auto mat = Matrix<T, Dynamic, Dynamic> (tableWidget->rowCount(), tableWidget->columnCount());

    for (int i = 0; i < mat.rows(); ++i) {
      for (int j = 0; j < mat.cols(); ++j) {
        mat(i, j) = tableWidget->item(i, j)->text().toDouble();
      }
    }

    return mat;
  }
}

#endif // TABLEWIDGETUTILS_TXX
