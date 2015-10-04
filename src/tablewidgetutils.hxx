#ifndef TABLEWIDGETUTILS_HXX
#define TABLEWIDGETUTILS_HXX

#include <QTableWidget>

#include <src/tablewidgetserializable.hxx>
#include <src/linearprogramdataserializable.hxx>

namespace TableWidgetUtils
{
  enum struct FillMethod { Zeroes, Random, Sequential };

  void clearTable(QTableWidget* tableWidget, int rowStart, int columnStart);
  void clearTable(QTableWidget* tableWidget);

  void fillTable(QTableWidget* tableWidget, FillMethod FillMethod);
  void fillTable(QTableWidget* tableWidget, TableWidgetSerializable& container);

  void FillTables(QVector<QTableWidget*>& tableWidgets, LinearProgramDataSerializable& problemContainer);
}

#include "tablewidgetutils.txx"

#endif // TABLEWIDGETUTILS_HXX
