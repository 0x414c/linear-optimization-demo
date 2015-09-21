#include "numericvalidator.hxx"

QValidator::State NumericValidator::validate(QString &input, int &pos) const
{
    if (input.isEmpty()) {
      return Acceptable;
    }

    bool b = false;
    int val = input.toInt(&b);

    if ((b == true) && (val > 0) && (val < 76))
    {
      return Acceptable;
    }
    else
    {
      return Invalid;
    }
}
