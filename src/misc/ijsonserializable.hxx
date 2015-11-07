#ifndef IJSONSERIALIZABLE_HXX
#define IJSONSERIALIZABLE_HXX

#include <QJsonObject>

#include "utils.hxx"

class IJsonSerializable
{
  public:
    virtual Utils::OperationResult read(const QJsonObject& jsonObject) = 0;
    virtual Utils::OperationResult write(QJsonObject& jsonObject) const = 0;

  protected:
    virtual ~IJsonSerializable() = 0;
};

#endif // IJSONSERIALIZABLE_HXX
