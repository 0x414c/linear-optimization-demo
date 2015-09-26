#ifndef PROBLEMDATASERIALIZABLE_HXX
#define PROBLEMDATASERIALIZABLE_HXX

#include <QJsonObject>
#include <QString>
#include <QTableWidget>
#include <QVector>

#include "tablewidgetserializable.hxx"
#include "ijsonserializable.hxx"

class TableWidgetSerializable;

class ProblemDataSerializable : public IJsonSerializable
{
  public:
    ProblemDataSerializable();
    ProblemDataSerializable(const QVector<TableWidgetSerializable>& containers);
    ProblemDataSerializable(const QVector<QTableWidget*>& tableWidgets);
    ~ProblemDataSerializable() {}

    QVector<TableWidgetSerializable> items() const;
    TableWidgetSerializable& itemAt(int idx);

    virtual void read(const QJsonObject& json) override;
    virtual void write(QJsonObject& json) const override;

  private:
    QString _dataId = QStringLiteral("data");
    QVector<TableWidgetSerializable> _data;
};

#endif // PROBLEMDATASERIALIZABLE_HXX
