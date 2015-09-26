#include "tablewidgetserializable.hxx"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVector>

TableWidgetSerializable::TableWidgetSerializable() {}

TableWidgetSerializable::TableWidgetSerializable(const QVector<QString>& data, int rows, int cols) {
  _data = data;
  _rows = rows;
  _cols = cols;
}

TableWidgetSerializable::TableWidgetSerializable(const QTableWidget* tableWidget) {
  QVector<QString> items(tableWidget->rowCount() * tableWidget->columnCount());
  int startIdx = 0;
  for (int i = 0; i < tableWidget->rowCount(); ++i) {
    for (int j = 0; j < tableWidget->columnCount(); ++j) {
      items[startIdx++] = tableWidget->item(i, j)->text();
      qDebug() << "TableWidgetSerializable: init container [" << startIdx - 1 << "] :=" << tableWidget->item(i, j)->text();
    }
  }
  _data = items;
  _rows = tableWidget->rowCount();
  _cols = tableWidget->columnCount();
}

int TableWidgetSerializable::rows() const
{
  return _rows;
}

int TableWidgetSerializable::cols() const
{
  return _cols;
}

QString TableWidgetSerializable::itemAt(int idx) const
{
  return _data[idx];
}


void TableWidgetSerializable::read(const QJsonObject& json)
{
  _data.clear();
  qDebug() << "TableWidgetSerializable: reading array [" << _dataId << "]";
  QJsonArray strings = json[_dataId].toArray();
  for (int i = 0; i < strings.size(); ++i) {
    QString string = strings[i].toString();
    qDebug() << "TableWidgetSerializable: read string [" << i << "] :=" << string;
    _data.append(string);
  }
  _rows = json["rows"].toInt();
  _cols = json["cols"].toInt();
}

void TableWidgetSerializable::write(QJsonObject& json) const
{
  QJsonArray dataArray;
  foreach (const QString string, _data) {
    qDebug() << "TableWidgetSerializable: writing string" << string;
    dataArray.append(string);
  }
  qDebug() << "TableWidgetSerializable: writing array" << _dataId;
  json[_dataId] = dataArray;
  json["rows"] = _rows;
  json["cols"] = _cols;
}
