#include "tablemodelstorage.hxx"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidget>

#include "tablemodel.hxx"
#include "../misc/utils.hxx"

using namespace Utils;

TableModelStorage::TableModelStorage() :
  _values(QVector<TableModel>(0)), _field(Field::Real) {}

TableModelStorage::TableModelStorage(const QVector<TableModel>& values, Field field) :
  _values(values), _field(field) {}

QVector<TableModel>& TableModelStorage::items()
{
  return _values;
}

const TableModel& TableModelStorage::valueAt(int idx) const
{
  return _values.at(idx);
}

int TableModelStorage::length() const
{
  return _values.length();
}

Field TableModelStorage::field() const
{
  return _field;
}

QString TableModelStorage::metadata() const
{
  return _metadataHeader + QString("%1").arg(int(_field));
}

void TableModelStorage::read(const QJsonObject& json)
{
  QString metadata = json["metadata"].toString();
  if (metadata != QStringLiteral(""))
  {
    QStringList metadataContents = metadata.split(";");
    if (metadataContents.at(0) + QStringLiteral(";") == _metadataHeader) //TODO: ~
    {
      _field = Field(metadataContents.at(1).toInt());
      _values.clear();
      QJsonArray values = json["values"].toArray();
      for (int i = 0; i < values.size(); ++i)
      {
        QJsonObject valueObject = values[i].toObject();
        TableModel value;
        value.read(valueObject);
        _values.append(value);
      }
    }
    else
    {
      qDebug() << "TableModelStorage::read: incompatible metadata key";
    }
  }
  else
  {
    qDebug() << "TableModelStorage::read: empty metadata key";
  }
}

void TableModelStorage::write(QJsonObject& json) const
{
  QJsonArray values;
  foreach (const TableModel& value, _values)
  {    
    QJsonObject valueObject;
    value.write(valueObject);
    values.append(valueObject);
  }
  json["metadata"] = _metadataHeader + QString("%1").arg(int(_field));
  json["values"] = values;
}
