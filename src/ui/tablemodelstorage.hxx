#ifndef TABLEMODELSTORAGE_HXX
#define TABLEMODELSTORAGE_HXX

#include <QJsonObject>
#include <QString>
#include <QTableWidget>
#include <QVector>

#include "tablemodel.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/ijsonserializable.hxx"
#include "../misc/utils.hxx"

using namespace NumericTypes;
using namespace Utils;

class TableModelStorage :
  virtual public IJsonSerializable
{
  public:
    TableModelStorage();
    explicit TableModelStorage(
      const QVector<SimpleTableModel>& items, Field field = Field::Real
    );

    SimpleTableModel& operator [](int idx);

    int count() const;

    const QVector<SimpleTableModel>& items();

    const SimpleTableModel& itemAt(int idx) const;

    Field field() const;

    QString metadata() const;

    virtual OperationResult read(const QJsonObject& jsonObject) override;

    virtual OperationResult write(QJsonObject& jsonObject) const override;

  private:
    const QString _metadataHeader = QStringLiteral("1;");
    QVector<SimpleTableModel> _items = QVector<SimpleTableModel>(0);
    Field _field = Field::Real;
};

#endif // TABLEMODELSTORAGE_HXX
