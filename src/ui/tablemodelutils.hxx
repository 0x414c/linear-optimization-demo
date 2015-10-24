#ifndef TABLEMODELUTILS_HXX
#define TABLEMODELUTILS_HXX

#include <functional>

#include <QTableWidget>
#include <QVariant>

#include "tablemodel.hxx"

using namespace std;

namespace TableModelUtils
{
  enum struct FillMethod { Zeroes, Random, Sequential };

  //TODO: ~ make these functions members of `TableModel'
  bool fillTableModel(TableModel* const tableModel, FillMethod fillMethod);
  bool fillTableModel(TableModel* const tableModel, const function<QVariant(const QVariant&)>& callback);
}

#include "tablemodelutils.txx"

#endif // TABLEMODELUTILS_HXX
