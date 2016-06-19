#pragma once

#ifndef TABLEMODELCOLLECTION_HXX
#define TABLEMODELCOLLECTION_HXX


#include <QJsonObject>
#include <QString>
#include <QTableWidget>
#include <QVector>

#include "stringtablemodel.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/ijsonserializable.hxx"
#include "../misc/utils.hxx"


namespace GUI
{
  using NumericTypes::Field;
  using NumericTypes::rational_t;
  using NumericTypes::real_t;
  using Utils::ResultType;


  class TableModelCollection :
    public IJsonSerializable
  {
    public:
      TableModelCollection();

      explicit TableModelCollection(
        const QVector<StringTableModel>& items, Field field = Field::Real
      );

      StringTableModel& operator [](int idx);

      int count() const;

      const QVector<StringTableModel>& items();

      const StringTableModel& at(int idx) const;

      Field field() const;

      QString metadata() const;

      virtual ResultType read(const QJsonObject& jsonObject) override;

      virtual ResultType write(QJsonObject& jsonObject) const override;


    private:
      const QString fileMetadataHeader_ = QStringLiteral("1;");

      QVector<StringTableModel> items_ = QVector<StringTableModel>(0);

      Field field_ = Field::Real;
  };
}


#endif // TABLEMODELCOLLECTION_HXX
