#include "numericvalidatordelegate.hxx"

#include <QLineEdit>
#include <QModelIndex>
#include <QRegExp>
#include <QRegExpValidator>
#include <QString>
#include <QStyleOptionViewItem>
#include <QWidget>

QWidget* NumericValidatorDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QLineEdit *lineEdit = new QLineEdit(parent);
    //NumericValidator *validator = new NumericValidator;
    QRegExp re(QStringLiteral("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$")); //TODO: fractions
    QRegExpValidator *validator = new QRegExpValidator(re, parent);
    lineEdit->setValidator(validator);

    return lineEdit;
}
