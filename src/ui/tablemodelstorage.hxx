#pragma once

#ifndef TABLEMODELSTORAGE_HXX
#define TABLEMODELSTORAGE_HXX


#include <QJsonObject>
#include <QString>
#include <QTableWidget>
#include <QVector>

#include "simpletablemodel.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/ijsonserializable.hxx"
#include "../misc/utils.hxx"


namespace GUI
{
  using namespace NumericTypes;
  using Utils::OperationResult;


  class TableModelStorage :
    public virtual IJsonSerializable
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
}


#endif // TABLEMODELSTORAGE_HXX
