#include "numericvalidator.hxx"

#include <QObject>
#include <QString>


GUI::NumericValidator::NumericValidator(QObject* parent) :
  QValidator(parent)
{ }


//Just example usage of `QValidator::validate'
QValidator::State
GUI::NumericValidator::validate(QString& input, int& pos) const
{
  if (input.isEmpty()) {
    return Acceptable;
  }
  else
  {
    bool isOk(false);
    const int val(input.toInt(&isOk));

    if ((isOk == true) && (val > 0) && (val < 100))
    {
      return Acceptable;
    }
    else
    {
      return Invalid;
    }
  }
}
