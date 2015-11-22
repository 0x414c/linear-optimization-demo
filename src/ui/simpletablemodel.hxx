﻿#pragma once

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
  using Utils::OperationResult;


  class SimpleTableModel :
    public virtual QAbstractTableModel,
    public virtual IJsonSerializable
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

      bool readOnly() const;

      bool setReadOnly(bool isReadOnly = true);

      virtual OperationResult read(const QJsonObject& jsonObject) override;

      virtual OperationResult write(QJsonObject& jsonObject) const override;


    private:
      int _rows = 0;
      int _cols = 0;

      bool _isReadOnly = false;

      bool _haveCustomVerticalHeaderData = false;
      bool _haveCustomHorizontalHeaderData = false;

      QVector<QVector<QString>> _values = QVector<QVector<QString>>(0);

      QVector<QString> _horizontalHeaderData = QVector<QString>(0);
      QVector<QString> _verticalHeaderData = QVector<QString>(0);
  };
}


#endif // SIMPLETABLEMODEL_HXX
