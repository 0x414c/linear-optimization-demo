#include "numericvalidatordelegate.hxx"

QWidget* NumericValidatorDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QLineEdit *lineEdit = new QLineEdit(parent);
    //NumericValidator *validator = new NumericValidator;
    QRegExp re("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$"); //TODO: fractions
    QRegExpValidator *validator = new QRegExpValidator(re, parent);
    lineEdit->setValidator(validator);

    return lineEdit;
}
