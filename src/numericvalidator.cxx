#include "numericvalidator.hxx"

#include <QObject>
#include <QString>

NumericValidator::NumericValidator(QObject* parent) :
  QValidator(parent) {}

// Just example usage of `QValidator::validate'
QValidator::State NumericValidator::validate(QString& input, int& pos) const
{
  if (input.isEmpty()) {
    return Acceptable;
  }
  else
  {
    bool b = false;
    int val = input.toInt(&b);

    if ((b == true) && (val > 0) && (val < 100))
    {
      return Acceptable;
    }
    else
    {
      return Invalid;
    }
  }
}
