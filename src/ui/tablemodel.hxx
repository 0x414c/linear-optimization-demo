#ifndef TABLEMODEL_HXX
#define TABLEMODEL_HXX

#include <QAbstractTableModel>
#include <QDebug>
#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>

#include <eigen3/Eigen/Dense>

#include "../misc/ijsonserializable.hxx"

using namespace Eigen;

class TableModel : public QAbstractTableModel, public IJsonSerializable
{
  Q_OBJECT

  public:
    explicit TableModel(QObject* parent = 0);
    explicit TableModel(int rows, int columns, QObject* parent = 0);
    TableModel(const TableModel& tableModel);

    const TableModel& operator=(const TableModel& tableModel);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole) override;

    virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    virtual bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
    virtual bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;

    bool clear(const QModelIndex& parent = QModelIndex());
    bool clear(const QVariant& value, const QModelIndex& parent = QModelIndex());
    bool clear(int rowFirst, int columnFirst, int rowLast, int columnLast, const QVariant& value, const QModelIndex& parent = QModelIndex());

    bool resize(int newRows, int newColumns, const QModelIndex& parent = QModelIndex());

    QVariant valueAt(int row, int col, const QModelIndex& parent = QModelIndex()) const; //TODO: ? use QAbstractTableModel::index

    virtual void read(const QJsonObject& json) override;
    virtual void write(QJsonObject& json) const override;

  private:
    int _rows;
    int _columns;
    bool _hasCustomVerticalHeaderData = false;
    bool _hasCustomHorizontalHeaderData = false;
    QVector<QString> _horizontalHeaderData;
    QVector<QString> _verticalHeaderData;
    QVector<QVector<QString>> _values;
};

#endif // TABLEMODEL_HXX
