#ifndef TABLEMODELSTORAGE_HXX
#define TABLEMODELSTORAGE_HXX

#include <QJsonObject>
#include <QString>
#include <QTableWidget>
#include <QVector>

#include "tablemodel.hxx"
#include "../misc/ijsonserializable.hxx"
#include "../misc/utils.hxx"

using namespace Utils;

class TableModelStorage : public IJsonSerializable
{
  public:
    TableModelStorage();
    explicit TableModelStorage(const QVector<TableModel>& values, Field field = Field::Real);

    QVector<TableModel>& items();
    const TableModel& valueAt(int idx) const;
    int length() const;

    Field field() const;
    QString metadata() const;

    virtual void read(const QJsonObject& json) override;
    virtual void write(QJsonObject& json) const override;

  private:
    const QString _metadataHeader = QStringLiteral("1;");
    QVector<TableModel> _values;
    Field _field;
};

#endif // TABLEMODELSTORAGE_HXX
