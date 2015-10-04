#ifndef NUMERICVALIDATOR_HXX
#define NUMERICVALIDATOR_HXX

#include <QObject>
#include <QString>
#include <QValidator>

//[[deprecated("To be removed in final version!")]]
class NumericValidator : public QValidator
{
  Q_OBJECT

  public:
    explicit NumericValidator(QObject* parent = 0);

    virtual State validate(QString& input, int& pos) const override;
};

#endif // NUMERICVALIDATOR_HXX
