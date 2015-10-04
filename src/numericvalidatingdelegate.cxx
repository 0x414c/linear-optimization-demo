#include "numericvalidatingdelegate.hxx"

#include <QObject>
#include <QStyledItemDelegate>

ValidatingDelegate::ValidatingDelegate(QObject* parent) : QStyledItemDelegate(parent) {}
