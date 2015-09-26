#ifndef TABLEWIDGETSERIALIZABLE_HXX
#define TABLEWIDGETSERIALIZABLE_HXX

#include <QJsonObject>
#include <QString>
#include <QTableWidget>
#include <QVector>

#include "ijsonserializable.hxx"

class TableWidgetSerializable : public IJsonSerializable
{
  public:
    TableWidgetSerializable();
    TableWidgetSerializable(const QVector<QString>& data, int rows, int cols);
    TableWidgetSerializable(const QTableWidget* tableWidget);
    ~TableWidgetSerializable() {}

    int rows() const;
    int cols() const;
    QString itemAt(int idx) const;

    virtual void read(const QJsonObject& json) override;
    virtual void write(QJsonObject& json) const override;

  private:
    QString _dataId = QStringLiteral("data");
    QVector<QString> _data;
    int _rows;
    int _cols;
};

#endif // TABLEWIDGETSERIALIZABLE_HXX
