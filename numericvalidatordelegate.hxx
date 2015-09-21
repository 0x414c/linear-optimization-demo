#ifndef NUMERICVALIDATORDELEGATE_HXX
#define NUMERICVALIDATORDELEGATE_HXX

#include <QWidget>
#include <QItemDelegate>
#include <QStyleOptionViewItem>
#include <QLineEdit>
//#include "numericvalidator.hxx"

class NumericValidatorDelegate : public QItemDelegate
{
  public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // NUMERICVALIDATORDELEGATE_HXX
