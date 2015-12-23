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
#include <QtDebug>
#include <QVariant>
#include <QVector>

#include "../misc/utils.hxx"


GUI::SimpleTableModel::SimpleTableModel(QObject* parent) :
  QAbstractTableModel(parent)
{ }


GUI::SimpleTableModel::SimpleTableModel(int rows, int cols, QObject* parent) :
  QAbstractTableModel(parent),
  rows_(rows),
  cols_(cols),
  values_(
    QVector<QVector<QString>>(rows, QVector<QString>(cols, QStringLiteral("")))
  ),
  flags_(QVector<QVector<Qt::ItemFlags>>(rows, QVector<Qt::ItemFlags>(cols)))
{
  _clearFlags();
}


GUI::SimpleTableModel::SimpleTableModel(const SimpleTableModel& other) :
  QAbstractTableModel(other.parent()),
  rows_(other.rows_),
  cols_(other.cols_),
  isEditable_(other.isEditable_),
  isSelectable_(other.isSelectable_),
  haveCustomVerticalHeaderData_(other.haveCustomVerticalHeaderData_),
  haveCustomHorizontalHeaderData_(other.haveCustomHorizontalHeaderData_),
  values_(other.values_),
  horizontalHeaderData_(other.horizontalHeaderData_),
  verticalHeaderData_(other.verticalHeaderData_),
  flags_(other.flags_)
{ }


GUI::SimpleTableModel::SimpleTableModel(SimpleTableModel&& other) :
  QAbstractTableModel(other.parent()),
  rows_(other.rows_),
  cols_(other.cols_),
  isEditable_(other.isEditable_),
  isSelectable_(other.isSelectable_),
  haveCustomVerticalHeaderData_(other.haveCustomVerticalHeaderData_),
  haveCustomHorizontalHeaderData_(other.haveCustomHorizontalHeaderData_),
  values_(std::move(other.values_)),
  horizontalHeaderData_(std::move(other.horizontalHeaderData_)),
  verticalHeaderData_(std::move(other.verticalHeaderData_)),
  flags_(std::move(other.flags_))
{ }


const GUI::SimpleTableModel&
GUI::SimpleTableModel::operator =(const SimpleTableModel& other)
{
  if (this != &other)
  {
    beginResetModel();
    rows_ = other.rows_;
    cols_ = other.cols_;
    isEditable_ = other.isEditable_;
    isSelectable_ = other.isSelectable_;
    haveCustomHorizontalHeaderData_ = other.haveCustomHorizontalHeaderData_;
    haveCustomVerticalHeaderData_ = other.haveCustomVerticalHeaderData_;
    values_ = other.values_;
    horizontalHeaderData_ = other.horizontalHeaderData_;
    verticalHeaderData_ = other.verticalHeaderData_;
    flags_ = other.flags_;
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
    rows_ = other.rows_;
    cols_ = other.cols_;
    isEditable_ = other.isEditable_;
    isSelectable_ = other.isSelectable_,
    haveCustomHorizontalHeaderData_ = other.haveCustomHorizontalHeaderData_;
    haveCustomVerticalHeaderData_ = other.haveCustomVerticalHeaderData_;
    values_ = std::move(other.values_);
    horizontalHeaderData_ = std::move(other.horizontalHeaderData_);
    verticalHeaderData_ = std::move(other.verticalHeaderData_);
    flags_ = std::move(other.flags_);
    setParent(other.parent());
    endResetModel();
  }

  return *this;
}


//NOTE: When implementing a table based model, `rowCount' should
//return `0' when the `parent' is valid.
int
GUI::SimpleTableModel::rowCount(const QModelIndex& parent) const
{
  if (!parent.isValid())
  {
    return rows_;
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
    return cols_;
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
    if (index.row() < rows_ && index.column() < cols_)
    {
      switch (role)
      {
//        case Qt::SizeHintRole:
//          return QSize(10, 5); //TODO: ~ Auto-resize

        case Qt::DisplayRole:
        case Qt::EditRole:
          return values_.at(index.row()).at(index.column());

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
        if (index.row() < rows_ && index.column() < cols_)
        {
          values_[index.row()][index.column()] = value.toString();
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
//          return QSize(16, 18); //TODO: ~ Auto-resize
//        case Qt::Vertical:
//          return QSize(12, 16);
//        default:
//          return QSize();
//      }

    case Qt::DisplayRole:
      switch (orientation)
      {
        case Qt::Horizontal:
          if (haveCustomHorizontalHeaderData_)
          {
            if (section >= 0 && section < horizontalHeaderData_.count())
            {
              QVariant ret(horizontalHeaderData_.at(section));
              if (ret != QStringLiteral(""))
              {
                return ret;
              }
            }
          }
          goto automatic;

        case Qt::Vertical:
          if (haveCustomVerticalHeaderData_)
          {
            if (section >= 0 && section < verticalHeaderData_.count())
            {
              QVariant ret(verticalHeaderData_.at(section));
              if (ret != QStringLiteral(""))
              {
                return ret;
              }
            }
          }
          goto automatic;

        default:
        automatic:
          return QString("%1").arg(section + 1);
      }

    default:
      return QVariant();
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
          if (section >= 0 && section < cols_)
          {
            const int oldLength(horizontalHeaderData_.length());
            horizontalHeaderData_.resize(cols_);
            horizontalHeaderData_[section] = value.toString();
            haveCustomHorizontalHeaderData_ = true;
            emit headerDataChanged(orientation, oldLength, cols_ - 1);

            return true;
          }

        case Qt::Vertical:
          if (section >= 0 && section < rows_)
          {
            const int oldLength(verticalHeaderData_.length());
            verticalHeaderData_.resize(rows_);
            verticalHeaderData_[section] = value.toString();
            haveCustomVerticalHeaderData_ = true;
            emit headerDataChanged(orientation, oldLength, rows_ - 1);

            return true;
          }

        default:
          return false;
      }

    default:
      return false;
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
    if (index.row() < rows_ && index.column() < cols_)
    {
      Qt::ItemFlags ret(flags_.at(index.row()).at(index.column()));

      if (isEditable_)
      {
        ret |= Qt::ItemIsEditable;
      }
      else
      {
        ret &= ~Qt::ItemIsEditable;
      }

      if (isSelectable_)
      {
        ret |= Qt::ItemIsSelectable;
      }
      else
      {
        ret &= ~Qt::ItemIsSelectable;
      }

      return ret;
    }
    else
    {
      return Qt::NoItemFlags;
    }
  }
}


bool
GUI::SimpleTableModel::setFlags(const QModelIndex& index, Qt::ItemFlags flags)
{
  if (!index.isValid())
  {
    return false;
  }
  else
  {
    if (index.row() < rows_ && index.column() < cols_)
    {
      flags_[index.row()][index.column()] = flags;

      return true;
    }
    else
    {
      return false;
    }
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
      values_.insert(row, QVector<QString>(cols_));
      for (int col(0); col < cols_; ++col)
      {
        values_[row][col] = QStringLiteral("");
      }
    }
    rows_ += count;

    endInsertRows();

    for (int i(0); i < count; ++i)
    {
      flags_.insert(row, QVector<Qt::ItemFlags>(cols_));
      for (int col(0); col < cols_; ++col)
      {
        flags_[row][col] =
          QAbstractTableModel::flags(
            QAbstractTableModel::index(row, col, parent)
          );
      }
    }

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
    if (count <= rows_)
    {
      beginRemoveRows(parent, row, row + count - 1);

      for (int i(0); i < count; ++i)
      {
        values_.removeAt(row);
      }
      rows_ -= count;

      endRemoveRows();

      for (int i(0); i < count; ++i)
      {
        flags_.removeAt(row);
      }

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

    for (int row(0); row < rows_; ++row)
    {
      for (int i(0); i < count; ++i)
      {
        values_[row].insert(col, QStringLiteral(""));
      }
    }
    cols_ += count;

    endInsertColumns();

    for (int row(0); row < rows_; ++row)
    {
      for (int i(0); i < count; ++i)
      {
        flags_[row].insert(
          col,
          QAbstractTableModel::flags(
            QAbstractTableModel::index(row, col, parent)
          )
        );
      }
    }

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
    if (count <= cols_)
    {
      beginRemoveColumns(parent, col, col + count - 1);

      for (int row(0); row < rows_; ++row)
      {
        for (int i(0); i < count; ++i)
        {
          values_[row].removeAt(col);
        }
      }
      cols_ -= count;

      endRemoveColumns();

      for (int row(0); row < rows_; ++row)
      {
        for (int i(0); i < count; ++i)
        {
          flags_[row].removeAt(col);
        }
      }

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

    for (int row(0); row < rows_; ++row)
    {
      values_[row].fill(QStringLiteral(""));
    }

    endResetModel();

    _clearFlags(parent);

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

    for (int row(0); row < rows_; ++row)
    {
      values_[row].fill(value.toString());
    }

    endResetModel();

    _clearFlags(parent);

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
    if (rowFirst >= 0 && rowLast <= rows_ &&
        colFirst >= 0 && colLast <= cols_ &&
        rowFirst < rowLast && colFirst < colLast
    )
    {
      for (int row(rowFirst); row < rowLast; ++row)
      {
        for (int col(colFirst); col < colLast; ++col)
        {
          values_[row][col] = value.toString();
        }
      }

      emit dataChanged(
        QAbstractTableModel::index(rowFirst, colFirst, parent),
        QAbstractTableModel::index(rowLast, colLast, parent),
        QVector<int>{Qt::DisplayRole, Qt::EditRole}
      );

      for (int row(rowFirst); row < rowLast; ++row)
      {
        for (int col(colFirst); col < colLast; ++col)
        {
          flags_[row][col] =
            QAbstractTableModel::flags(
              QAbstractTableModel::index(row, col, parent)
            );
        }
      }

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
      const int diffRows(newRows - rows_);
      if (diffRows > 0)
      {
        insertRows(rows_, diffRows, parent);
        ret = true;
      }
      else
      {
        if (diffRows < 0)
        {
          removeRows(rows_ + diffRows, -diffRows, parent);
          ret = true;
        }
      }
    }

    if (newCols != 0)
    {
      const int diffCols(newCols - cols_);
      if (diffCols > 0)
      {
        insertColumns(cols_, diffCols, parent);
        ret = true;
      }
      else
      {
        if (diffCols < 0)
        {
          removeColumns(cols_ + diffCols, -diffCols, parent);
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
    if (row >= 0 && row < rows_ && col >= 0 && col < cols_)
    {
      return values_.at(row).at(col);
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
GUI::SimpleTableModel::editable() const
{
  return isEditable_;
}


bool
GUI::SimpleTableModel::setEditable(bool isEditable)
{
  if (isEditable_ != isEditable)
  {
//    beginResetModel();
    isEditable_ = isEditable;
//    endResetModel();

    return true;
  }
  else
  {
    return false;
  }
}


bool
GUI::SimpleTableModel::selectable() const
{
  return isSelectable_;
}


bool
GUI::SimpleTableModel::setSelectable(bool isSelectable)
{
  if (isSelectable_ != isSelectable)
  {
//    beginResetModel();
    isSelectable_ = isSelectable;
//    endResetModel();

    return true;
  }
  else
  {
    return false;
  }
}


Utils::ResultType
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
          values_.clear();
          values_.resize(rowsCount);
          for (int row(0); row < rowsCount; ++row)
          {
            for (int col(0); col < colsCount; ++col)
            {
              QJsonValue valueValue(valuesArray[startIdx++]);
              if (valueValue.type() != QJsonValue::Undefined)
              {
                QString valueString(valueValue.toString());
                values_[row].append(valueString);
              }
              else
              {
                qCritical() << "SimpleTableModel::read: cannot read `value':"
                               " `value' == `Undefined', stopping operation";

                return ResultType::Fail;
              }
            }
          }
          rows_ = rowsCount;
          cols_ = colsCount;
          _resetFlags();
          endResetModel();

          return ResultType::Success;
        }
        else
        {
          qCritical() << "SimpleTableModel::read: inconsistent `values':"
                         " expected" << rowsCount * colsCount <<
                         ", actual is" << valuesArray.count();

          return ResultType::Nothing;
        }
      }
      else
      {
        qWarning() << "SimpleTableModel::read:"
                      " cannot read: `values' == `Undefined'";

        return ResultType::Nothing;
      }
    }
    else
    {
      qWarning() << "SimpleTableModel::read:"
                    " cannot read: `cols' == `Undefined'";

      return ResultType::Nothing;
    }
  }
  else
  {
    qWarning() << "SimpleTableModel::read:"
                  " cannot read: `rows' == `Undefined'";

    return ResultType::Nothing;
  }
}


Utils::ResultType
GUI::SimpleTableModel::write(QJsonObject& jsonObject) const
{
  QJsonArray valuesArray;
  for (const QVector<QString>& row : values_)
  {
    for (const QString& col : row)
    {
      valuesArray.append(col);
    }
  }

  jsonObject[QStringLiteral("values")] = valuesArray;
  jsonObject[QStringLiteral("rows")] = rows_;
  jsonObject[QStringLiteral("cols")] = cols_;

  return ResultType::Success;
}


bool
GUI::SimpleTableModel::_clearFlags(const QModelIndex& parent)
{
  if (!parent.isValid())
  {
    for (int row(0); row < rows_; ++row)
    {
      for (int col(0); col < cols_; ++col)
      {
        flags_[row][col] =
          QAbstractTableModel::flags(
            QAbstractTableModel::index(row, col, parent)
          );
      }
    }

    return true;
  }
  else
  {
    return false;
  }
}


bool
GUI::SimpleTableModel::_resetFlags(const QModelIndex& parent)
{
  if (!parent.isValid())
  {
    flags_.resize(rows_);
    for (int row = 0; row < rows_; ++row)
    {
      flags_[row].resize(cols_);
      for (int col(0); col < cols_; ++col)
      {
        flags_[row][col] =
          QAbstractTableModel::flags(
            QAbstractTableModel::index(row, col, parent)
          );
      }
    }

    return true;
  }
  else
  {
    return false;
  }
}
