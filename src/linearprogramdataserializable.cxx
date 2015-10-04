#include "linearprogramdataserializable.hxx"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QTableWidget>

#include "tablewidgetserializable.hxx"

LinearProgramDataSerializable::LinearProgramDataSerializable() {}

LinearProgramDataSerializable::LinearProgramDataSerializable(const QVector<TableWidgetSerializable>& containers)
{
  _data = containers;
}

LinearProgramDataSerializable::LinearProgramDataSerializable(const QVector<QTableWidget*>& tableWidgets)
{
  QVector<TableWidgetSerializable> containers(tableWidgets.length());
  for (int i = 0; i < tableWidgets.length(); ++i)
  {
    TableWidgetSerializable container(tableWidgets[i]);
    containers[i] = container;
  }
  _data = containers;
}

QVector<TableWidgetSerializable> LinearProgramDataSerializable::items() const
{
  return _data;
}

TableWidgetSerializable& LinearProgramDataSerializable::itemAt(int idx)
{
  return _data[idx];
}

void LinearProgramDataSerializable::read(const QJsonObject& json)
{
  _data.clear();
  qDebug() << "ProblemDataSerializable: reading array [" << _dataId << "]";
  QJsonArray containers = json[_dataId].toArray();
  for (int i = 0; i < containers.size(); ++i)
  {
    qDebug() << "ProblemDataSerializable: reading container [" << i << "]";
    QJsonObject containerObject = containers[i].toObject();
    TableWidgetSerializable container;
    container.read(containerObject);
    _data.append(container);
  }
}

void LinearProgramDataSerializable::write(QJsonObject& json) const
{
  QJsonArray dataArray;
  foreach (const TableWidgetSerializable container, _data)
  {
    qDebug() << "ProblemDataSerializable: writing container ...";
    QJsonObject containerObject;
    container.write(containerObject);
    dataArray.append(containerObject);
  }
  qDebug() << "ProblemDataSerializable: writing array" << _dataId;
  json[_dataId] = dataArray;
}
