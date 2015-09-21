#ifndef TABLEDATAUTILS_HXX
#define TABLEDATAUTILS_HXX

#include <QTableWidget>

namespace TableWidgetUtils
{
  enum FillMethod { Zeroes, Random, Sequential };
  void clearTable(QTableWidget* tableWidget, int rowStart, int columnStart);
  void clearTable(QTableWidget* tableWidget);
  void fillTable(QTableWidget* tableWidget, FillMethod FillMethod);
}

#include "tablewidgetutils.txx"

#endif // TABLEDATAUTILS_HXX
