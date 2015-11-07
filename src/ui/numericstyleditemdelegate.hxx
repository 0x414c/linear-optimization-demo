#ifndef NUMERICSTYLEDITEMDELEGATE_HXX
#define NUMERICSTYLEDITEMDELEGATE_HXX

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QWidget>

#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"

using namespace NumericTypes;
using namespace Utils;

//HACK: This shim makes `moc' happy about templated classes
class StyledItemDelegate :
  public QStyledItemDelegate
{
  Q_OBJECT

  public:
    explicit StyledItemDelegate(QObject* parent = 0);
};

template<typename T = Real>
class NumericStyledItemDelegate :
  public StyledItemDelegate
{
  public:
    explicit NumericStyledItemDelegate(QObject* parent = 0);

    virtual QWidget* createEditor(QWidget* parent,
                                  const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const override;

    virtual void setEditorData(QWidget* editor,
                               const QModelIndex& index) const override;

    virtual void setModelData(QWidget* editor,
                              QAbstractItemModel* model,
                              const QModelIndex& index) const override;

    virtual void updateEditorGeometry(QWidget* editor,
                                      const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const override;

//    virtual QSize sizeHint(const QStyleOptionViewItem& option,
//                           const QModelIndex& index) const override;
};

#include "numericstyleditemdelegate.txx"

#endif // NUMERICSTYLEDITEMDELEGATE_HXX
