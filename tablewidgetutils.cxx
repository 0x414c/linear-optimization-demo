#include "tablewidgetutils.hxx"

#include <string>
#include <random>
#include <QDebug>
#include <QString>
#include <QTableWidgetItem>

using namespace std;

namespace TableWidgetUtils
{
  void clearTable(QTableWidget* tableWidget, int rowStart, int colStart) {
    for (int i = rowStart; i < tableWidget->rowCount(); ++i) {
      for (int j = colStart; j < tableWidget->columnCount(); ++j) {
        QTableWidgetItem* item = new QTableWidgetItem;
        item->setText(QString("0"));
        tableWidget->setItem(i, j, item);
      }
    }
  }

  void clearTable(QTableWidget* tableWidget) {
    clearTable(tableWidget, 0, 0);
  }

  void fillTable(QTableWidget* tableWidget, FillMethod FillMethod) {
    tableWidget->clear();
    minstd_rand rand;
    uniform_real_distribution<double> dist(-25, 25);
    int startIdx = 0;
    for (int i = 0; i < tableWidget->rowCount(); ++i) {
      for (int j = 0; j < tableWidget->columnCount(); ++j) {
        QTableWidgetItem* item = new QTableWidgetItem;
        switch (FillMethod) {
          case Zeroes:
            item->setText(QString("%1").arg(0));
            break;
          case Random:
            item->setText(QString("%1").arg(dist(rand)));
            break;
          case Sequential:
            item->setText(QString("%1").arg(++startIdx));
            break;
          default:
            break;
        }
        tableWidget->setItem(i, j, item);
      }
    }
  }
}
