#ifndef NUMERICVALIDATOR_HXX
#define NUMERICVALIDATOR_HXX

#include <QValidator>

class NumericValidator : public QValidator
{
  Q_OBJECT
  public:
    explicit NumericValidator(QObject *parent = 0);

    virtual State validate(QString &input, int &pos) const;
};

#endif // NUMERICVALIDATOR_HXX
