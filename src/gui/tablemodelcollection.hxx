#pragma once

#ifndef TABLEMODELCOLLECTION_HXX
#define TABLEMODELCOLLECTION_HXX


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
  using NumericTypes::Field;
  using NumericTypes::Rational;
  using NumericTypes::Real;
  using Utils::ResultType;


  class TableModelCollection :
    public IJsonSerializable
  {
    public:
      TableModelCollection();

      explicit TableModelCollection(
        const QVector<SimpleTableModel>& items, Field field = Field::Real
      );

      SimpleTableModel& operator [](int idx);

      int count() const;

      const QVector<SimpleTableModel>& items();

      const SimpleTableModel& at(int idx) const;

      Field field() const;

      QString metadata() const;

      virtual ResultType read(const QJsonObject& jsonObject) override;

      virtual ResultType write(QJsonObject& jsonObject) const override;


    private:
      const QString fileMetadataHeader_ = QStringLiteral("1;");

      QVector<SimpleTableModel> items_ = QVector<SimpleTableModel>(0);

      Field field_ = Field::Real;
  };
}


#endif // TABLEMODELCOLLECTION_HXX
