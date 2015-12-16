﻿#include "tablemodelstorage.hxx"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QStringList>
#include <QTableWidget>

#include "simpletablemodel.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"


namespace GUI
{
//  using namespace NumericTypes;
//  using namespace Utils;
}


GUI::TableModelStorage::TableModelStorage()
{ }


GUI::TableModelStorage::TableModelStorage(
  const QVector<SimpleTableModel>& items, Field field
) :
  _items(items),
  _field(field)
{ }


GUI::SimpleTableModel&
GUI::TableModelStorage::operator [](int idx)
{
  return _items[idx];
}


int
GUI::TableModelStorage::count() const
{
  return _items.count();
}


const QVector<GUI::SimpleTableModel>&
GUI::TableModelStorage::items()
{
  return _items;
}


const GUI::SimpleTableModel&
GUI::TableModelStorage::itemAt(int idx) const
{
  return _items.at(idx);
}


NumericTypes::Field
GUI::TableModelStorage::field() const
{
  return _field;
}


QString
GUI::TableModelStorage::metadata() const
{
  return (_metadataHeader + QString("%1").arg(int(_field)));
}


Utils::ResultType
GUI::TableModelStorage::read(const QJsonObject& jsonObject)
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
              SimpleTableModel item;
              const ResultType res(item.read(itemObject));
              if (res == ResultType::Success)
              {
                _items.append(item);
              }
              else
              {
                qCritical() << "TableModelStorage::read: cannot read `item':"
                               " stopping operation";

                return res;
              }
            }
            else
            {
              qCritical() << "TableModelStorage::read: cannot read `item':"
                             " `item' == `Undefined', stopping operation";

              return ResultType::Nothing;
            }
          }

          return ResultType::Success;
        }
        else
        {
          qWarning() << "TableModelStorage::read: cannot read:"
                        " `items' == `Undefined'";

          return ResultType::Nothing;
        }
      }
      else
      {
        qWarning() << "TableModelStorage::read: cannot read: "
                      "`metadata' != `this->metadata()'";

        return ResultType::Nothing;
      }
    }
    else
    {
      qWarning() << "TableModelStorage::read: cannot read:"
                    " `metadata' == (empty)";

      return ResultType::Nothing;
    }
  }
  else
  {
    qWarning() << "TableModelStorage::read: cannot read:"
                  " `metadata' == `Undefined'";

    return ResultType::Nothing;
  }
}


Utils::ResultType
GUI::TableModelStorage::write(QJsonObject& jsonObject) const
{
  QJsonArray itemsArray;
  for (const SimpleTableModel& item : _items)
  {
    QJsonObject itemObject;
    const ResultType res(item.write(itemObject));
    if (res == ResultType::Success)
    {
      itemsArray.append(itemObject);
    }
    else
    {
      qCritical() << "TableModelStorage::write: could not write `item',"
                     " stopping operation";

      return res;
    }
  }

  jsonObject[QStringLiteral("items")] = itemsArray;

  jsonObject[QStringLiteral("metadata")] = _metadataHeader +
                                           QString("%1").
                                           arg(int(_field));

  return ResultType::Success;
}
