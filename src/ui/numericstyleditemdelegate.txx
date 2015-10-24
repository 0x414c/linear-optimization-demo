#ifndef NUMERICSTYLEDITEMDELEGATE_TXX
#define NUMERICSTYLEDITEMDELEGATE_TXX

#include <QItemDelegate>
#include <QLineEdit>
#include <QModelIndex>
#include <QObject>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSize>
#include <QString>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QWidget>

#include "../misc/utils.hxx"

using namespace Utils;

template<typename T>
NumericStyledItemDelegate<T>::NumericStyledItemDelegate(QObject* parent) :
  StyledItemDelegate(parent) {}

template<typename T>
QWidget* NumericStyledItemDelegate<T>::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  static_assert(AlwaysFalse<T>::value, "NumericStyledItemDelegate<T>: You can only use one of the specified specializations!");

  return nullptr;
}

template<>
inline QWidget* NumericStyledItemDelegate<real_t>::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* lineEdit = new QLineEdit(parent);
  QRegExp re(QStringLiteral("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$"));
  QRegExpValidator* validator = new QRegExpValidator(re, parent);
  lineEdit->setValidator(validator);

  return lineEdit;
}

template<>
inline QWidget* NumericStyledItemDelegate<integer_t>::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* lineEdit = new QLineEdit(parent);
  QRegExp re(QStringLiteral("^[-+]?[0-9]+$"));
  QRegExpValidator* validator = new QRegExpValidator(re, parent);
  lineEdit->setValidator(validator);

  return lineEdit;
}

template<>
inline QWidget* NumericStyledItemDelegate<rational_t>::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* lineEdit = new QLineEdit(parent);
  QRegExp re(QStringLiteral("^[-+]?[0-9]+(?:\\/[0-9]*[1-9]+[0-9]*)?$"));
  QRegExpValidator* validator = new QRegExpValidator(re, parent);
  lineEdit->setValidator(validator);

  return lineEdit;
}

template<typename T>
void NumericStyledItemDelegate<T>::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  QString value = index.model()->data(index, Qt::EditRole).toString();
  QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
  Q_ASSERT(lineEdit);
  lineEdit->setText(value);
}

template<typename T>
void NumericStyledItemDelegate<T>::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
  Q_ASSERT(lineEdit);
  QString value = lineEdit->text();
  model->setData(index, value, Qt::EditRole);
}

//template<typename T>
//void NumericStyledItemDelegate<T>::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
//{
//  editor->setGeometry(option.rect);
//}

//template<typename T>
//QSize NumericStyledItemDelegate<T>::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
//{
//  return QSize(10, 10);
//}

#endif // NUMERICSTYLEDITEMDELEGATE_TXX
