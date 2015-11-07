#include "tablemodelstorage.hxx"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QStringList>
#include <QTableWidget>

#include "tablemodel.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"

using namespace NumericTypes;
using namespace Utils;

TableModelStorage::TableModelStorage()
{ }

TableModelStorage::TableModelStorage(const QVector<TableModel>& items, Field field) :
  _items(items),
  _field(field)
{ }

TableModel& TableModelStorage::operator [](int idx)
{
  return _items[idx];
}

const QVector<TableModel>& TableModelStorage::items()
{
  return _items;
}

const TableModel& TableModelStorage::at(int idx) const
{
  return _items.at(idx);
}

int TableModelStorage::count() const
{
  return _items.count();
}

Field TableModelStorage::field() const
{
  return _field;
}

QString TableModelStorage::metadata() const
{
  return (_metadataHeader + QString("%1").arg(int(_field)));
}

OperationResult TableModelStorage::read(const QJsonObject& jsonObject)
{
  const QJsonValue metadataValue(jsonObject[QStringLiteral("metadata")]);
  if (metadataValue.type() != QJsonValue::Undefined)
  {
    const QString metadataString(metadataValue.toString());
    if (metadataString != QStringLiteral(""))
    {
      QRegExp re(QString("^1;([0-9]+)$"));
      int idx(re.indexIn(metadataString));
      if (idx > -1)
      {
        _field = Field(re.cap(1).toInt());
        _items.clear();
        const QJsonValue itemsValue(jsonObject[QStringLiteral("items")]);
        if (itemsValue.type() != QJsonValue::Undefined)
        {
          const QJsonArray itemsArray(itemsValue.toArray());
          for (int i(0); i < itemsArray.count(); ++i)
          {
            const QJsonValue itemValue(itemsArray[i]);
            if (itemValue.type() != QJsonValue::Undefined)
            {
              const QJsonObject itemObject(itemValue.toObject());
              TableModel item;
              const OperationResult res(item.read(itemObject));
              if (res == OperationResult::Success)
              {
                _items.append(item);
              }
              else
              {
                qDebug() << "TableModelStorage::read: cannot read item: stopping operation";

                return res;
              }
            }
            else
            {
              qDebug() << "TableModelStorage::read: cannot read item: item is `Undefined',"
                          " stopping operation";

              return OperationResult::Nothing;
            }
          }

          return OperationResult::Success;
        }
        else
        {
          qDebug() << "TableModelStorage::read: cannot read: `items' is `Undefined'";

          return OperationResult::Nothing;
        }
      }
      else
      {
        qDebug() << "TableModelStorage::read: cannot read: incompatible or invalid `metadata' key";

        return OperationResult::Nothing;
      }
    }
    else
    {
      qDebug() << "TableModelStorage::read: cannot read: empty `metadata' key";

      return OperationResult::Nothing;
    }
  }
  else
  {
    qDebug() << "TableModelStorage::read: cannot read: `metadata' is `Undefined'";

    return OperationResult::Nothing;
  }
}

OperationResult TableModelStorage::write(QJsonObject& jsonObject) const
{
  QJsonArray itemsArray;
  for (const TableModel& item : _items)
  {    
    QJsonObject itemObject;
    const OperationResult res(item.write(itemObject));
    if (res == OperationResult::Success)
    {
      itemsArray.append(itemObject);
    }
    else
    {
      qDebug() << "TableModelStorage::read: cannot write item, stopping operation";

      return res;
    }
  }
  jsonObject[QStringLiteral("metadata")] = _metadataHeader + QString("%1").arg(int(_field));
  jsonObject[QStringLiteral("items")] = itemsArray;

  return OperationResult::Success;
}
