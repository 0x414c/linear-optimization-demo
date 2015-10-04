#ifndef NUMERICVALIDATINGDELEGATE_TXX
#define NUMERICVALIDATINGDELEGATE_TXX

#include <QItemDelegate>
#include <QLineEdit>
#include <QModelIndex>
#include <QObject>
#include <QRegExp>
#include <QRegExpValidator>
#include <QString>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QWidget>

#include "utils.hxx"

using namespace Utils;

template<typename T>
NumericValidatingDelegate<T>::NumericValidatingDelegate(QObject* parent) :
  ValidatingDelegate(parent) {}

template<typename T>
QWidget* NumericValidatingDelegate<T>::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  static_assert(always_false<T>::value, "NumericValidatingDelegate<T>: You can only use one of the specified specializations!");

  return nullptr;
}

template<>
inline QWidget* NumericValidatingDelegate<real_t>::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* lineEdit = new QLineEdit(parent);
  //NumericValidator *validator = new NumericValidator;
  QRegExp re(QStringLiteral("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$"));
  QRegExpValidator* validator = new QRegExpValidator(re, parent);
  lineEdit->setValidator(validator);

  return lineEdit;
}

template<>
inline QWidget* NumericValidatingDelegate<int_t>::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* lineEdit = new QLineEdit(parent);
  QRegExp re(QStringLiteral("^[-+]?[0-9]+$"));
  QRegExpValidator* validator = new QRegExpValidator(re, parent);
  lineEdit->setValidator(validator);

  return lineEdit;
}

template<>
inline QWidget* NumericValidatingDelegate<rat_t>::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* lineEdit = new QLineEdit(parent);
  QRegExp re(QStringLiteral("^[-+]?[0-9]+(?:\\/[0-9]+)?$"));
  QRegExpValidator* validator = new QRegExpValidator(re, parent);
  lineEdit->setValidator(validator);

  return lineEdit;
}

#endif // NUMERICVALIDATINGDELEGATE_TXX
