#ifndef TABLEMODEL_HXX
#define TABLEMODEL_HXX

#include <QAbstractTableModel>
#include <QDebug>
#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>

#include "../misc/ijsonserializable.hxx"
#include "../misc/utils.hxx"

using namespace Utils;

class TableModel :
  virtual public QAbstractTableModel,
  virtual public IJsonSerializable
{
  Q_OBJECT

  public:
    explicit TableModel(QObject* parent = 0);
    TableModel(int rows, int cols, QObject* parent = 0);

    TableModel(const TableModel& tableModel);
    TableModel(TableModel&& tableModel);

    const TableModel& operator =(const TableModel& tableModel);
    const TableModel& operator =(TableModel&& tableModel);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const override;

    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

    virtual bool setData(const QModelIndex& index,
                         const QVariant& value, int role = Qt::EditRole) override;
    virtual bool setHeaderData(int section, Qt::Orientation orientation,
                               const QVariant& value, int role = Qt::EditRole) override;

    virtual bool insertRows(int row, int count,
                            const QModelIndex& parent = QModelIndex()) override;
    virtual bool removeRows(int row, int count,
                            const QModelIndex& parent = QModelIndex()) override;

    virtual bool insertColumns(int col, int count,
                               const QModelIndex& parent = QModelIndex()) override;
    virtual bool removeColumns(int col, int count,
                               const QModelIndex& parent = QModelIndex()) override;

    bool clear(const QModelIndex& parent = QModelIndex());
    bool clear(const QVariant& value, const QModelIndex& parent = QModelIndex());
    bool clear(int rowFirst, int colFirst, int rowLast, int colLast,
               const QVariant& value, const QModelIndex& parent = QModelIndex());

    bool resize(int newRows, int newCols,
                const QModelIndex& parent = QModelIndex());

     //TODO: ~? Use QAbstractTableModel::index
    QVariant valueAt(int row, int col, const QModelIndex& parent = QModelIndex()) const;

    virtual OperationResult read(const QJsonObject& jsonObject) override;
    virtual OperationResult write(QJsonObject& jsonObject) const override;

  private:
    int _rows = 0;
    int _cols = 0;

    bool _haveCustomVerticalHeaderData = false;
    bool _haveCustomHorizontalHeaderData = false;

    QVector<QVector<QString>> _values = QVector<QVector<QString>>(0);
    QVector<QString> _horizontalHeaderData = QVector<QString>(0);
    QVector<QString> _verticalHeaderData = QVector<QString>(0);
};

#endif // TABLEMODEL_HXX
