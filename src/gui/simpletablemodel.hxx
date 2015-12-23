#pragma once

#ifndef SIMPLETABLEMODEL_HXX
#define SIMPLETABLEMODEL_HXX


#include <QAbstractTableModel>
#include <QDebug>
#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>

#include "../misc/ijsonserializable.hxx"
#include "../misc/utils.hxx"


namespace GUI
{
  using Utils::ResultType;


  class SimpleTableModel :
    public QAbstractTableModel,
    public IJsonSerializable
  {
    Q_OBJECT


    public:
      explicit SimpleTableModel(QObject* parent = 0);

      SimpleTableModel(int rows, int cols, QObject* parent = 0);

      SimpleTableModel(const SimpleTableModel& tableModel);
      SimpleTableModel(SimpleTableModel&& tableModel);

      const SimpleTableModel& operator =(const SimpleTableModel& other);
      const SimpleTableModel& operator =(SimpleTableModel&& other);

      virtual int rowCount(
        const QModelIndex& parent = QModelIndex()
      ) const override;

      virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

      virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

      bool setFlags(const QModelIndex& index, Qt::ItemFlags flags);

      virtual QVariant data(
        const QModelIndex& index, int role = Qt::DisplayRole
      ) const override;

      virtual bool setData(
        const QModelIndex& index, const QVariant& value,
        int role = Qt::EditRole
      ) override;

      virtual QVariant headerData(
        int section, Qt::Orientation orientation, int role = Qt::DisplayRole
      ) const override;

      virtual bool setHeaderData(
        int section, Qt::Orientation orientation, const QVariant& value,
        int role = Qt::EditRole
      ) override;

      virtual bool insertRows(
        int row, int count, const QModelIndex& parent = QModelIndex()
      ) override;

      virtual bool removeRows(
        int row, int count, const QModelIndex& parent = QModelIndex()
      ) override;

      virtual bool insertColumns(
        int col, int count, const QModelIndex& parent = QModelIndex()
      ) override;

      virtual bool removeColumns(
        int col, int count, const QModelIndex& parent = QModelIndex()
      ) override;

      bool clear(const QModelIndex& parent = QModelIndex());
      bool clear(
        const QVariant& value, const QModelIndex& parent = QModelIndex()
      );
      bool clear(
        int rowFirst, int colFirst, int rowLast, int colLast,
        const QVariant& value, const QModelIndex& parent = QModelIndex()
      );

      bool resize(
        int newRows, int newCols, const QModelIndex& parent = QModelIndex()
      );

     QVariant valueAt(
        int row, int col, const QModelIndex& parent = QModelIndex()
      ) const;

      bool editable() const;

      bool setEditable(bool isReadOnly = true);

      bool selectable() const;

      bool setSelectable(bool isSelectable = true);

      virtual ResultType read(const QJsonObject& jsonObject) override;

      virtual ResultType write(QJsonObject& jsonObject) const override;


    private:
      int rows_ = 0;
      int cols_ = 0;

      bool isEditable_ = true;
      bool isSelectable_ = true;

      bool haveCustomVerticalHeaderData_ = false;
      bool haveCustomHorizontalHeaderData_ = false;

      QVector<QVector<QString>> values_ = QVector<QVector<QString>>(0);

      QVector<QString> horizontalHeaderData_ = QVector<QString>(0);
      QVector<QString> verticalHeaderData_ = QVector<QString>(0);

      QVector<QVector<Qt::ItemFlags>> flags_ =
        QVector<QVector<Qt::ItemFlags>>(0);


      bool _clearFlags(const QModelIndex& parent = QModelIndex());

      bool _resetFlags(const QModelIndex& parent = QModelIndex());
  };
}


#endif // SIMPLETABLEMODEL_HXX
