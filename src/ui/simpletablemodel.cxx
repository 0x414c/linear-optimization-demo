#include "simpletablemodel.hxx"

#include <utility>

#include <QAbstractTableModel>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QString>
#include <QVariant>
#include <QVector>

#include "../misc/utils.hxx"


GUI::SimpleTableModel::SimpleTableModel(QObject* parent) :
  QAbstractTableModel(parent)
{ }


GUI::SimpleTableModel::SimpleTableModel(int rows, int cols, QObject* parent) :
  QAbstractTableModel(parent),
  _rows(rows),
  _cols(cols),
  _values(
    QVector<QVector<QString>>(rows, QVector<QString>(cols, QStringLiteral("")))
  )
{ }


GUI::SimpleTableModel::SimpleTableModel(const SimpleTableModel& other) :
  QAbstractTableModel(other.parent()),
  _rows(other._rows),
  _cols(other._cols),
  _isReadOnly(other._isReadOnly),
  _haveCustomVerticalHeaderData(other._haveCustomVerticalHeaderData),
  _haveCustomHorizontalHeaderData(other._haveCustomHorizontalHeaderData),
  _values(other._values),
  _horizontalHeaderData(other._horizontalHeaderData),
  _verticalHeaderData(other._verticalHeaderData)
{ }


GUI::SimpleTableModel::SimpleTableModel(SimpleTableModel&& other) :
  QAbstractTableModel(other.parent()),
  _rows(other._rows),
  _cols(other._cols),
  _isReadOnly(other._isReadOnly),
  _haveCustomVerticalHeaderData(other._haveCustomVerticalHeaderData),
  _haveCustomHorizontalHeaderData(other._haveCustomHorizontalHeaderData),
  _values(std::move(other._values)),
  _horizontalHeaderData(std::move(other._horizontalHeaderData)),
  _verticalHeaderData(std::move(other._verticalHeaderData))
{
//We don't need to re-initialize object at rvalue.
//  other._rows = 0;
//  other._cols = 0;
//  other._isReadOnly = false;
//  other._haveCustomHorizontalHeaderData = false;
//  other._haveCustomVerticalHeaderData = false;
//  other._values = QVector<QVector<QString>>(0);
//  other._horizontalHeaderData = QVector<QString>(0);
//  other._verticalHeaderData = QVector<QString>(0);
//  other.setParent(0);
}


const GUI::SimpleTableModel&
GUI::SimpleTableModel::operator =(const SimpleTableModel& other)
{
  if (this != &other)
  {
    beginResetModel();
    _rows = other._rows;
    _cols = other._cols;
    _isReadOnly = other._isReadOnly;
    _haveCustomHorizontalHeaderData = other._haveCustomHorizontalHeaderData;
    _haveCustomVerticalHeaderData = other._haveCustomVerticalHeaderData;
    _values = other._values;
    _horizontalHeaderData = other._horizontalHeaderData;
    _verticalHeaderData = other._verticalHeaderData;
    setParent(other.parent());
    endResetModel();
  }

  return *this;
}


const GUI::SimpleTableModel&
GUI::SimpleTableModel::operator =(SimpleTableModel&& other)
{
  if (this != &other)
  {
    beginResetModel();
    _rows = other._rows;
    _cols = other._cols;
    _isReadOnly = other._isReadOnly;
    _haveCustomHorizontalHeaderData = other._haveCustomHorizontalHeaderData;
    _haveCustomVerticalHeaderData = other._haveCustomVerticalHeaderData;
    _values = std::move(other._values);
    _horizontalHeaderData = std::move(other._horizontalHeaderData);
    _verticalHeaderData = std::move(other._verticalHeaderData);
    setParent(other.parent());
    endResetModel();

//We don't need to re-initialize object at rvalue.
//    other._rows = 0;
//    other._cols = 0;
//    other._isReadOnly = false;
//    other._haveCustomHorizontalHeaderData = false;
//    other._haveCustomVerticalHeaderData = false;
//    other._values = QVector<QVector<QString>>(0);
//    other._horizontalHeaderData = QVector<QString>(0);
//    other._verticalHeaderData = QVector<QString>(0);
//    other.setParent(0);
  }

  return *this;
}


int
GUI::SimpleTableModel::rowCount(const QModelIndex& parent) const
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


int
GUI::SimpleTableModel::columnCount(const QModelIndex& parent) const
{
  if (!parent.isValid())
  {
    return _cols;
  }
  else
  {
    return 0;
  }
}


QVariant
GUI::SimpleTableModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
  {
    return QVariant();
  }
  else
  {
    if (index.row() < _rows && index.column() < _cols)
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


QVariant
GUI::SimpleTableModel::headerData(
  int section, Qt::Orientation orientation, int role
) const
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
          if (_haveCustomHorizontalHeaderData)
          {
            if (section >= 0 && section < _horizontalHeaderData.count())
            {
              QVariant ret(_horizontalHeaderData.at(section));
              if (ret != QStringLiteral(""))
              {
                return ret;
              }
            }
          }
          goto defaultCase;
        case Qt::Vertical:
          if (_haveCustomVerticalHeaderData)
          {
            if (section >= 0 && section < _verticalHeaderData.count())
            {
              QVariant ret(_verticalHeaderData.at(section));
              if (ret != QStringLiteral(""))
              {
                return ret;
              }
            }
          }
          goto defaultCase;
        default:
        defaultCase:
          return QString("%1").arg(section + 1);
      }
    default:
      return QVariant();
  }
}


Qt::ItemFlags
GUI::SimpleTableModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
  {
    return Qt::NoItemFlags;
  }
  else
  {
    if (_isReadOnly)
    {
      return (QAbstractTableModel::flags(index) & ~Qt::ItemIsEditable);
    }
    else
    {
      return (QAbstractTableModel::flags(index) | Qt::ItemIsEditable);
    }
  }
}


bool
GUI::SimpleTableModel::setData(
  const QModelIndex& index, const QVariant& value, int role
)
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
        if (index.row() < _rows && index.column() < _cols)
        {
          _values[index.row()][index.column()] = value.toString();
           //TODO: ~? Do we need to add `DisplayRole' too?
          emit dataChanged(index, index, QVector<int>{role});

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


bool
GUI::SimpleTableModel::setHeaderData(
  int section, Qt::Orientation orientation, const QVariant& value, int role
)
{
  switch (role)
  {
    case Qt::EditRole:
      switch (orientation)
      {
        case Qt::Horizontal:
          if (section >= 0 && section < _cols)
          {
            int oldLength(_horizontalHeaderData.length());
            _horizontalHeaderData.resize(_cols);
            _horizontalHeaderData[section] = value.toString();
            _haveCustomHorizontalHeaderData = true;
            emit headerDataChanged(orientation, oldLength, _cols - 1);

            return true;
          }
        case Qt::Vertical:
          if (section >= 0 && section < _rows)
          {
            int oldLength(_verticalHeaderData.length());
            _verticalHeaderData.resize(_rows);
            _verticalHeaderData[section] = value.toString();
            _haveCustomVerticalHeaderData = true;
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


bool
GUI::SimpleTableModel::insertRows(
  int row, int count, const QModelIndex& parent
)
{
  if (!parent.isValid())
  {
    beginInsertRows(parent, row, row + count - 1);
    for (int i(0); i < count; ++i)
    {
      _values.insert(row, QVector<QString>(_cols));
      for (int col(0); col < _cols; ++col)
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


bool
GUI::SimpleTableModel::removeRows(
  int row, int count, const QModelIndex& parent
)
{
  if (!parent.isValid())
  {
    if (count <= _rows)
    {
      beginRemoveRows(parent, row, row + count - 1);
      for (int i(0); i < count; ++i)
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


bool
GUI::SimpleTableModel::insertColumns(
  int col, int count, const QModelIndex& parent
)
{
  if (!parent.isValid())
  {
    beginInsertColumns(parent, col, col + count - 1);
    for (int row(0); row < _rows; ++row)
    {
      for (int i(0); i < count; ++i)
      {
        _values[row].insert(col, QStringLiteral(""));
      }
    }
    _cols += count;
    endInsertColumns();

    return true;
  }
  else
  {
    return false;
  }
}


bool
GUI::SimpleTableModel::removeColumns(
  int col, int count, const QModelIndex& parent
)
{
  if (!parent.isValid())
  {
    if (count <= _cols)
    {
      beginRemoveColumns(parent, col, col + count - 1);
      for (int row(0); row < _rows; ++row)
      {
        for (int i(0); i < count; ++i)
        {
          _values[row].removeAt(col);
        }
      }
      _cols -= count;
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


bool
GUI::SimpleTableModel::clear(const QModelIndex& parent)
{
  if (!parent.isValid())
  {
    beginResetModel();
    for (int row(0); row < _rows; ++row)
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


bool
GUI::SimpleTableModel::clear(const QVariant& value, const QModelIndex& parent)
{
  if (!parent.isValid())
  {
    beginResetModel();
    for (int row(0); row < _rows; ++row)
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


bool
GUI::SimpleTableModel::clear(
  int rowFirst, int colFirst, int rowLast, int colLast,
  const QVariant& value, const QModelIndex& parent
)
{
  if (!parent.isValid())
  {
    if (rowFirst >= 0 && rowLast <= _rows &&
        colFirst >= 0 && colLast <= _cols &&
        rowFirst < rowLast && colFirst < colLast
    )
    {
      for (int row(rowFirst); row < rowLast; ++row)
      {
        for (int col(colFirst); col < colLast; ++col)
        {
          _values[row][col] = value.toString();
        }
      }
      emit dataChanged(
        QAbstractTableModel::index(rowFirst, colFirst, parent),
        QAbstractTableModel::index(rowLast, colLast, parent),
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


bool
GUI::SimpleTableModel::resize(
  int newRows, int newCols, const QModelIndex& parent
)
{
  if (!parent.isValid())
  {
    bool ret(false);

    if (newRows != 0)
    {
      const int diffRows(newRows - _rows);
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

    if (newCols != 0)
    {
      const int diffCols(newCols - _cols);
      if (diffCols > 0)
      {
        insertColumns(_cols, diffCols, parent);
        ret = true;
      }
      else
      {
        if (diffCols < 0)
        {
          removeColumns(_cols + diffCols, -diffCols, parent);
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


QVariant
GUI::SimpleTableModel::valueAt(
  int row, int col, const QModelIndex &parent
) const
{
  if (!parent.isValid())
  {
    if (row >= 0 && row < _rows && col >= 0 && col < _cols)
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


bool
GUI::SimpleTableModel::readOnly() const
{
  return _isReadOnly;
}


bool
GUI::SimpleTableModel::setReadOnly(bool isReadOnly)
{
  if (_isReadOnly != isReadOnly)
  {
//    beginResetModel();
    _isReadOnly = isReadOnly;
//    endResetModel();

    return true;
  }
  else
  {
    return false;
  }
}


Utils::OperationResult
GUI::SimpleTableModel::read(const QJsonObject& jsonObject)
{
  const QJsonValue rowsValue(jsonObject[QStringLiteral("rows")]);
  if (rowsValue.type() != QJsonValue::Undefined)
  {
    const int rowsCount(rowsValue.toInt());
    const QJsonValue colsValue(jsonObject[QStringLiteral("cols")]);
    if (colsValue.type() != QJsonValue::Undefined)
    {
      const int colsCount(colsValue.toInt());
      const QJsonValue valuesValue(jsonObject[QStringLiteral("values")]);
      if (valuesValue.type() != QJsonValue::Undefined)
      {
        const QJsonArray valuesArray(valuesValue.toArray());
        if (valuesArray.count() == rowsCount * colsCount)
        {
          int startIdx(0);
          beginResetModel();
          _values.clear();
          _values.resize(rowsCount);
          for (int row(0); row < rowsCount; ++row)
          {
            for (int col(0); col < colsCount; ++col)
            {
              QJsonValue valueValue(valuesArray[startIdx++]);
              if (valueValue.type() != QJsonValue::Undefined)
              {
                QString valueString(valueValue.toString());
                _values[row].append(valueString);
              }
              else
              {
                qDebug() << "TableModel::read: cannot read `value':"
                            " `value' == `Undefined', stopping operation";

                return OperationResult::Fail;
              }
            }
          }
          _rows = rowsCount;
          _cols = colsCount;
          endResetModel();

          return OperationResult::Success;
        }
        else
        {
          qDebug() << "TableModel::read: inconsistent `values' count:"
                      " expected" << rowsCount * colsCount <<
                      ", actual is" << valuesArray.count();

          return OperationResult::Nothing;
        }
      }
      else
      {
        qDebug() << "TableModel::read: cannot read: `values' == `Undefined'";

        return OperationResult::Nothing;
      }
    }
    else
    {
      qDebug() << "TableModel::read: cannot read: `cols' == `Undefined'";

      return OperationResult::Nothing;
    }
  }
  else
  {
    qDebug() << "TableModel::read: cannot read: `rows' == `Undefined'";

    return OperationResult::Nothing;
  }
}


Utils::OperationResult
GUI::SimpleTableModel::write(QJsonObject& jsonObject) const
{
  QJsonArray valuesArray;
  for (const QVector<QString>& row : _values)
  {
    for (const QString& col : row)
    {
      valuesArray.append(col);
    }
  }

  jsonObject[QStringLiteral("values")] = valuesArray;
  jsonObject[QStringLiteral("rows")] = _rows;
  jsonObject[QStringLiteral("cols")] = _cols;

  return OperationResult::Success;
}
