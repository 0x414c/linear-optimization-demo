#include "tablemodelutils.hxx"

#include <functional>
#include <stdexcept>
#include <string>
#include <random>

#include <QDebug>
#include <QString>
#include <QVariant>

#include "stringtablemodel.hxx"
#include "../config.hxx"


namespace TableModelUtils
{
  using namespace Config::TableModelUtils;
  using GUI::StringTableModel;
  using std::function;
  using std::invalid_argument;
  using std::minstd_rand;
  using std::uniform_real_distribution;


  bool
  fill(StringTableModel* const tableModel, FillMethod fillMethod)
  throw(invalid_argument)
  {
    if (tableModel == nullptr)
    {
      throw invalid_argument("`tableModel' == `nullptr'");
    }

    minstd_rand rnd(RandSeed);
    uniform_real_distribution<double> dist(RandDistMin, RandDistMax);
    int startIdx(0);
    tableModel->clear();
    for (int row(0); row < tableModel->rowCount(); ++row)
    {
      for (int col(0); col < tableModel->columnCount(); ++col)
      {
        switch (fillMethod) {
          case FillMethod::Zeroes:
            tableModel->setData(
              tableModel->index(row, col), QString("%1").arg(0)
            );
            break;

          case FillMethod::Random:
            tableModel->setData(
              tableModel->index(row, col), QString("%1").arg(dist(rnd))
            );
            break;

          case FillMethod::Sequential:
            tableModel->setData(
              tableModel->index(row, col), QString("%1").arg(startIdx++)
            );
            break;

          default:
            break;
        }
      }
    }

    return true;
  }


  bool
  fill(StringTableModel* const tableModel, const QString& value)
  throw(invalid_argument)
  {
    if (tableModel == nullptr)
    {
      throw invalid_argument("`tableModel' == `nullptr'");
    }

    for (int row(0); row < tableModel->rowCount(); ++row)
    {
      for (int col(0); col < tableModel->columnCount(); ++col)
      {
        const QModelIndex index(tableModel->index(row, col));
        tableModel->setData(index, value);
      }
    }

    return true;
  }


  bool
  fill(
    StringTableModel* const tableModel,
    const function<QVariant(const QVariant&)>& callback
  ) throw(invalid_argument)
  {
    if (tableModel == nullptr)
    {
      throw invalid_argument("`tableModel' == `nullptr'");
    }

    if (!callback)
    {
      throw invalid_argument("`callback' == (empty)");
    }

    for (int row(0); row < tableModel->rowCount(); ++row)
    {
      for (int col(0); col < tableModel->columnCount(); ++col)
      {
        const QModelIndex index(tableModel->index(row, col));
        const QVariant data(tableModel->data(index));
        if (data != QStringLiteral(""))
        {
          const QVariant value(callback(data));
          tableModel->setData(index, value);
        }
        else
        {
          qCritical() << "TableModelUtils::fill: tableModel->data(index(" <<
                         row << "," << col << ")) == (empty)";
        }
      }
    }

    return true;
  }
}
