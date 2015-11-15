#ifndef NUMERICVALIDATOR_HXX
#define NUMERICVALIDATOR_HXX

#include <QObject>
#include <QString>
#include <QValidator>

class NumericValidator :
  public virtual QValidator
{
  Q_OBJECT

  public:
    [[deprecated("To be removed in final version!"
                 " We use `QRegExpValidator' for now!")]]
    explicit NumericValidator(QObject* parent = 0);

    virtual State validate(QString& input, int& pos) const override;
};

#endif // NUMERICVALIDATOR_HXX
