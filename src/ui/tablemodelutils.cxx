#include "tablemodelutils.hxx"

#include <string>
#include <random>

#include <QDebug>
#include <QString>

#include <functional>

#include "tablemodel.hxx"

using namespace std;

namespace TableModelUtils
{
  bool fillTableModel(TableModel* const tableModel, FillMethod fillMethod)
  {
    if (tableModel != nullptr)
    {
      minstd_rand rand;
      uniform_real_distribution<double> dist(-10, 10);
      int startIdx = 0;
      tableModel->clear();
      for (int row = 0; row < tableModel->rowCount(); ++row)
      {
        for (int col = 0; col < tableModel->columnCount(); ++col)
        {
          switch (fillMethod) {
            case FillMethod::Zeroes:
              tableModel->setData(tableModel->index(row, col), QString("%1").arg(0));
              break;
            case FillMethod::Random:
              tableModel->setData(tableModel->index(row, col), QString("%1").arg(dist(rand)));
              break;
            case FillMethod::Sequential:
              tableModel->setData(tableModel->index(row, col), QString("%1").arg(++startIdx));
              break;
            default:
              break;
          }
        }
      }

      return true;
    }
    else
    {
      qDebug() << "TableModelUtils::fill: `tableModel' argument is nullptr";

      return false;
    }
  }

  bool fillTableModel(TableModel* const tableModel, const function<QVariant(const QVariant&)>& callback)
  {
    if (tableModel != nullptr)
    {
      if (callback)
      {
        for (int row = 0; row < tableModel->rowCount(); ++row)
        {
          for (int col = 0; col < tableModel->columnCount(); ++col)
          {
            auto index = tableModel->index(row, col);
            auto data = tableModel->data(index);
            if (data != QStringLiteral(""))
            {
              auto value = callback(data);
              tableModel->setData(index, value);
            }
            else
            {
              qDebug() << "TableModelUtils::fill: nothing to convert at index (" << row << ";" << col << ")";
            }
          }
        }

        return true;
      }
      else
      {
        qDebug() << "TableModelUtils::fill: `callback' argument is empty";
      }
    }
    else
    {
      qDebug() << "TableModelUtils::fill: `tableModel' argument is `nullptr'";

      return false;
    }
  }
}
