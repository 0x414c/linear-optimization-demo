#pragma once

#ifndef NUMERICVALIDATOR_HXX
#define NUMERICVALIDATOR_HXX


#include <QObject>
#include <QString>
#include <QValidator>


namespace Gui
{
  class NumericValidator :
    public QValidator
  {
    Q_OBJECT


    public:
      [[deprecated("Use `QRegExpValidator'!")]]
      explicit NumericValidator(QObject* parent = 0);

      virtual State validate(QString& input, int& pos) const override;
  };
}


#endif // NUMERICVALIDATOR_HXX
