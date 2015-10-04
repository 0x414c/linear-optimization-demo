#ifndef NUMERICVALIDATINGDELEGATE_HXX
#define NUMERICVALIDATINGDELEGATE_HXX

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QWidget>

#include "utils.hxx"

#include "numericvalidator.hxx"

//HACK: makes `moc' happy about templated classes
class ValidatingDelegate : public QStyledItemDelegate
{
  Q_OBJECT

  public:
    explicit ValidatingDelegate(QObject* parent = 0);
};

template<typename T>
class NumericValidatingDelegate : public ValidatingDelegate
{
  public:
    explicit NumericValidatingDelegate(QObject* parent = 0);

    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#include "numericvalidatingdelegate.txx"

#endif // NUMERICVALIDATINGDELEGATE_HXX
