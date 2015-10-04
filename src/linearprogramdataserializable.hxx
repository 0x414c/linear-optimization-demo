#ifndef LINEARPROGRAMDATASERIALIZABLE_HXX
#define LINEARPROGRAMDATASERIALIZABLE_HXX

#include <QJsonObject>
#include <QString>
#include <QTableWidget>
#include <QVector>

#include "tablewidgetserializable.hxx"
#include "ijsonserializable.hxx"

class TableWidgetSerializable;

class LinearProgramDataSerializable : public IJsonSerializable
{
  public:
    LinearProgramDataSerializable();
    LinearProgramDataSerializable(const QVector<TableWidgetSerializable>& containers);
    LinearProgramDataSerializable(const QVector<QTableWidget*>& tableWidgets);
    ~LinearProgramDataSerializable() {}

    QVector<TableWidgetSerializable> items() const;
    TableWidgetSerializable& itemAt(int idx);

    virtual void read(const QJsonObject& json) override;
    virtual void write(QJsonObject& json) const override;

  private:
    QString _dataId = QStringLiteral("values");
    QVector<TableWidgetSerializable> _data;
    //TODO: ?datatype tag
};

#endif // LINEARPROGRAMDATASERIALIZABLE_HXX
