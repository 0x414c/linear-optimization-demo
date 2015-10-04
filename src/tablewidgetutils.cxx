#include "tablewidgetutils.hxx"

#include <string>
#include <random>

#include <QDebug>
#include <QString>
#include <QTableWidgetItem>
#include <QVector>

#include "tablewidgetserializable.hxx"
#include "linearprogramdataserializable.hxx"

using namespace std;

namespace TableWidgetUtils
{
  void clearTable(QTableWidget* tableWidget, int rowStart, int colStart)
  {
    for (int i = rowStart; i < tableWidget->rowCount(); ++i)
    {
      for (int j = colStart; j < tableWidget->columnCount(); ++j)
      {
        QTableWidgetItem* item = new QTableWidgetItem;
        item->setText(QString("0"));
        tableWidget->setItem(i, j, item);
      }
    }
  }

  void clearTable(QTableWidget* tableWidget)
  {
    clearTable(tableWidget, 0, 0);
  }

  void fillTable(QTableWidget* tableWidget, FillMethod fillMethod)
  {
    minstd_rand rand;
    uniform_real_distribution<double> dist(-25, 25);
    int startIdx = 0;
    tableWidget->clear();
    for (int i = 0; i < tableWidget->rowCount(); ++i)
    {
      for (int j = 0; j < tableWidget->columnCount(); ++j)
      {
        QTableWidgetItem* item = new QTableWidgetItem;
        switch (fillMethod) {
          case FillMethod::Zeroes:
            item->setText(QString("%1").arg(0));
            break;
          case FillMethod::Random:
            item->setText(QString("%1").arg(dist(rand)));
            break;
          case FillMethod::Sequential:
            item->setText(QString("%1").arg(++startIdx));
            break;
          default:
            break;
        }
        tableWidget->setItem(i, j, item);
      }
    }
  }

  void fillTable(QTableWidget* tableWidget, TableWidgetSerializable& container)
  {
    if (tableWidget->rowCount() != container.rows())
    {
      qDebug() << "TableWidgetUtils: updating rowCount :=" << container.rows();
      tableWidget->setRowCount(container.rows());
    }
    if (tableWidget->columnCount() != container.cols())
    {
      qDebug() << "TableWidgetUtils: updating columnCount :=" << container.cols();
      tableWidget->setColumnCount(container.cols());
    }

    int startIdx = 0;
    for (int i = 0; i < container.rows(); ++i)
    {
      for (int j = 0; j < container.cols(); ++j)
      {
        qDebug() << "TableWidgetUtils: setting text [" << i << ", " << j << "] :=" << container.itemAt(startIdx);
        if (tableWidget->item(i, j) != 0)
        {
          tableWidget->item(i, j)->setText(container.itemAt(startIdx));
        }
        else
        {
          QTableWidgetItem* item = new QTableWidgetItem;
          item->setText(container.itemAt(startIdx));
          tableWidget->setItem(i, j, item);
        }
        startIdx += 1;
      }
    }
  }

  void FillTables(QVector<QTableWidget*>& tableWidgets, LinearProgramDataSerializable& problemContainer)
  {
    for (int i = 0; i < tableWidgets.length(); ++i)
    {
      fillTable(tableWidgets[i], problemContainer.itemAt(i));
    }
  }
}
