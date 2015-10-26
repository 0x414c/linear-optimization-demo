#include "tablemodel.hxx"

#include <QAbstractTableModel>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QString>
#include <QVariant>
#include <QVector>

TableModel::TableModel(QObject* parent) :
  QAbstractTableModel(parent), _rows(0), _columns(0)
{
  _values = QVector<QVector<QString>>(0);
  _verticalHeaderData = QVector<QString>(0);
  _horizontalHeaderData = QVector<QString>(0);
}

TableModel::TableModel(int rows, int columns, QObject* parent) :
  QAbstractTableModel(parent), _rows(rows), _columns(columns)
{
  _values = QVector<QVector<QString>>(rows, QVector<QString>(columns, QStringLiteral("")));
  _verticalHeaderData = QVector<QString>(0);
  _horizontalHeaderData = QVector<QString>(0);
}

TableModel::TableModel(const TableModel& tableModel) :
  QAbstractTableModel(tableModel.parent()), _rows(tableModel._rows),
  _columns(tableModel._columns), _values(tableModel._values) {}

const TableModel& TableModel::operator=(const TableModel& otherTableModel)
{
  if (this != &otherTableModel)
  {
    beginResetModel();
    _rows = otherTableModel._rows;
    _columns = otherTableModel._columns;
    _values = otherTableModel._values;
    _hasCustomHorizontalHeaderData = otherTableModel._hasCustomHorizontalHeaderData;
    _hasCustomVerticalHeaderData = otherTableModel._hasCustomVerticalHeaderData;
    _horizontalHeaderData = otherTableModel._horizontalHeaderData;
    _verticalHeaderData = otherTableModel._verticalHeaderData;
    setParent(otherTableModel.parent());
    endResetModel();
  }

  return *this;
}

int TableModel::rowCount(const QModelIndex& parent) const
{
  if (!parent.isValid())
  {
    return _rows;
  }
  else
  {
    return 0;
  }
}

int TableModel::columnCount(const QModelIndex& parent) const
{
  if (!parent.isValid())
  {
    return _columns;
  }
  else
  {
    return 0;
  }
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
  {
    return QVariant();
  }
  else
  {
    if (index.row() < _rows && index.column() < _columns)
    {
      switch (role)
      {
//        case Qt::SizeHintRole:
//          return QSize(10, 5); //TODO: ~
        case Qt::DisplayRole:
        case Qt::EditRole:
          return _values.at(index.row()).at(index.column());
        default:
          return QVariant();
      }
    }
    else
    {
      return QVariant();
    }
  }
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  switch (role)
  {
//    case Qt::SizeHintRole:
//      switch (orientation) {
//        case Qt::Horizontal:
//          return QSize(16, 18); //TODO: ~
//        case Qt::Vertical:
//          return QSize(12, 16);
//        default:
//          return QSize();
//      }
    case Qt::DisplayRole:
      switch (orientation)
      {
        case Qt::Horizontal:
          if (_hasCustomHorizontalHeaderData)
          {
            if (section >= 0 && section < _horizontalHeaderData.length())
            {
              QVariant ret = _horizontalHeaderData.at(section);
              if (ret != QStringLiteral(""))
              {
                return ret;
              }
            }
          }
          goto default_case;
        case Qt::Vertical:
          if (_hasCustomVerticalHeaderData)
          {
            if (section >= 0 && section < _verticalHeaderData.length())
            {
              QVariant ret = _verticalHeaderData.at(section);
              if (ret != QStringLiteral(""))
              {
                return ret;
              }
            }
          }
          goto default_case;
        default:
        default_case:
          return QString("%1").arg(section + 1);
      }
    default:
      return QVariant();
  }
}

Qt::ItemFlags TableModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  else
  {
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
  }
}

bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (!index.isValid())
  {
    return false;
  }
  else
  {
    switch (role)
    {
      case Qt::EditRole:
        if (index.row() < _rows && index.column() < _columns)
        {
          _values[index.row()][index.column()] = value.toString();
          emit dataChanged(index, index, QVector<int>{role}); //TODO: ~?

          return true;
        }
        else
        {
          return false;
        }
      default:
        return false;
    }
  }
}

bool TableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
  switch (role)
  {
    case Qt::EditRole:
      switch (orientation)
      {
        case Qt::Horizontal:
          if (section >= 0 && section < _columns)
          {
            int oldLength = _horizontalHeaderData.length();
            _horizontalHeaderData.resize(_columns);
            _horizontalHeaderData[section] = value.toString();
            _hasCustomHorizontalHeaderData = true;
            emit headerDataChanged(orientation, oldLength, _columns - 1);

            return true;
          }
        case Qt::Vertical:
          if (section >= 0 && section < _rows)
          {
            int oldLength = _verticalHeaderData.length();
            _verticalHeaderData.resize(_rows);
            _verticalHeaderData[section] = value.toString();
            _hasCustomVerticalHeaderData = true;
            emit headerDataChanged(orientation, oldLength, _rows - 1);

            return true;
          }
        default:
          return false;
      }
    default:
      return false;
  }
}

bool TableModel::insertRows(int row, int count, const QModelIndex& parent)
{
  if (!parent.isValid())
  {
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
    {
      _values.insert(row, QVector<QString>(_columns));
      for (int col = 0; col < _columns; ++col)
      {
        _values[row][col] = QStringLiteral("");
      }
    }
    _rows += count;
    endInsertRows();

    return true;
  }
  else
  {
    return false;
  }
}

bool TableModel::removeRows(int row, int count, const QModelIndex& parent)
{
  if (!parent.isValid())
  {
    if (count <= _rows)
    {
      beginRemoveRows(parent, row, row + count - 1);
      for (int i = 0; i < count; ++i)
      {
        _values.removeAt(row);
      }
      _rows -= count;
      endRemoveRows();

      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

bool TableModel::insertColumns(int column, int count, const QModelIndex& parent)
{
  if (!parent.isValid())
  {
    beginInsertColumns(parent, column, column + count - 1);
    for (int row = 0; row < _rows; ++row)
    {
      for (int i = 0; i < count; ++i)
      {
        _values[row].insert(column, QStringLiteral(""));
      }
    }
    _columns += count;
    endInsertColumns();

    return true;
  }
  else
  {
    return false;
  }
}

bool TableModel::removeColumns(int column, int count, const QModelIndex& parent)
{
  if (!parent.isValid())
  {
    if (count <= _columns)
    {
      beginRemoveColumns(parent, column, column + count - 1);
      for (int row = 0; row < _rows; ++row)
      {
        for (int i = 0; i < count; ++i)
        {
          _values[row].removeAt(column);
        }
      }
      _columns -= count;
      endRemoveColumns();

      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

bool TableModel::clear(const QModelIndex& parent)
{
  if (!parent.isValid())
  {
    beginResetModel();
    for (int row = 0; row < _rows; ++row)
    {
      _values[row].fill(QStringLiteral(""));
    }
    endResetModel();

    return true;
  }
  else
  {
    return false;
  }
}

bool TableModel::clear(const QVariant& value, const QModelIndex& parent)
{
  if (!parent.isValid())
  {
    beginResetModel();
    for (int row = 0; row < _rows; ++row)
    {
      _values[row].fill(value.toString());
    }
    endResetModel();

    return true;
  }
  else
  {
    return false;
  }
}

bool TableModel::clear(int rowFirst, int columnFirst, int rowLast, int columnLast, const QVariant& value, const QModelIndex& parent)
{
  if (!parent.isValid())
  {
    if (rowFirst >= 0 && rowLast <= _rows && columnFirst >= 0 && columnLast <= _columns && rowFirst < rowLast && columnFirst < columnLast)
    {
      for (int row = rowFirst; row < rowLast; ++row)
      {
        for (int col = columnFirst; col < columnLast; ++col)
        {
          _values[row][col] = value.toString();
        }
      }
      emit dataChanged(
        QAbstractTableModel::index(rowFirst, columnFirst, parent),
        QAbstractTableModel::index(rowLast, columnLast, parent),
        QVector<int>{Qt::DisplayRole | Qt::EditRole}
      );

      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

bool TableModel::resize(int newRows, int newColumns, const QModelIndex& parent)
{
  if (!parent.isValid())
  {
    bool ret = false;

    if (newRows != 0)
    {
      int diffRows = newRows - _rows;
      if (diffRows > 0)
      {
        insertRows(_rows, diffRows, parent);
        ret = true;
      }
      else
      {
        if (diffRows < 0)
        {
          removeRows(_rows + diffRows, -diffRows, parent);
          ret = true;
        }
      }
    }

    if (newColumns != 0)
    {
      int diffCols = newColumns - _columns;
      if (diffCols > 0)
      {
        insertColumns(_columns, diffCols, parent);
        ret = true;
      }
      else
      {
        if (diffCols < 0)
        {
          removeColumns(_columns + diffCols, -diffCols, parent);
          ret = true;
        }
      }
    }

    return ret;
  }
  else
  {
    return false;
  }
}

QVariant TableModel::valueAt(int row, int col, const QModelIndex &parent) const
{
  if (!parent.isValid())
  {
    if (row >= 0 && row < _rows && col >= 0 && col < _columns)
    {
      return _values.at(row).at(col);
    }
    else
    {
      return QVariant();
    }
  }
  else
  {
    return QVariant();
  }
}

void TableModel::read(const QJsonObject& json)
{
  const int rows = json["rows"].toInt();
  const int cols = json["cols"].toInt();
  const QJsonArray values = json["values"].toArray();
  if (values.size() == rows * cols)
  {
    int startIdx = 0;
    beginResetModel();
    _values.clear();
    _values.resize(rows);
    for (int row = 0; row < rows; ++row)
    {
      for (int col = 0; col < cols; ++col)
      {
        QString value = values[startIdx++].toString();
        _values[row].append(value);
      }
    }
    _rows = rows;
    _columns = cols;
    endResetModel();
  }
  else
  {
    qDebug() << "TableModel::read: inconsistent items count" << values.size() << "!=" << rows * cols;
  }
}


void TableModel::write(QJsonObject& json) const
{
  QJsonArray values;
  foreach (const QVector<QString>& row, _values)
  {
    foreach (const QString& col, row)
    {
      values.append(col);
    }
  }
  json["values"] = values;
  json["rows"] = _rows;
  json["cols"] = _columns;
}
