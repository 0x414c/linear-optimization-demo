#ifndef NUMERICVALIDATORDELEGATE_HXX
#define NUMERICVALIDATORDELEGATE_HXX

#include <QItemDelegate>
#include <QLineEdit>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QWidget>

//#include "numericvalidator.hxx"

class NumericValidatorDelegate : public QItemDelegate
{
  public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // NUMERICVALIDATORDELEGATE_HXX
