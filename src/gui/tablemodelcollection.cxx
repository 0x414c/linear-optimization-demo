﻿#include "tablemodelcollection.hxx"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QStringList>
#include <QTableWidget>

#include "stringtablemodel.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"


namespace Gui
{
  using NumericTypes::Field;
  using Utils::ResultType;
}


Gui::TableModelCollection::TableModelCollection()
{ }


Gui::TableModelCollection::TableModelCollection(
  const QVector<StringTableModel>& items, Field field
) :
  items_(items),
  field_(field)
{ }


Gui::StringTableModel&
Gui::TableModelCollection::operator [](int idx)
{
  return items_[idx];
}


int
Gui::TableModelCollection::count() const
{
  return items_.count();
}


const QVector<Gui::StringTableModel>&
Gui::TableModelCollection::items()
{
  return items_;
}


const Gui::StringTableModel&
Gui::TableModelCollection::at(int idx) const
{
  return items_.at(idx);
}


NumericTypes::Field
Gui::TableModelCollection::field() const
{
  return field_;
}


QString
Gui::TableModelCollection::metadata() const
{
  return (fileMetadataHeader_ + QString("%1").arg(int(field_)));
}


Utils::ResultType
Gui::TableModelCollection::read(const QJsonObject& jsonObject)
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
        field_ = Field(re.cap(1).toInt());
        items_.clear();
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
              StringTableModel item;
              const ResultType res(item.read(itemObject));
              if (res == ResultType::Success)
              {
                items_.append(item);
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
Gui::TableModelCollection::write(QJsonObject& jsonObject) const
{
  QJsonArray itemsArray;
  for (const StringTableModel& item : items_)
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

  jsonObject[QStringLiteral("metadata")] = fileMetadataHeader_ +
                                           QString("%1").
                                           arg(int(field_));

  return ResultType::Success;
}
