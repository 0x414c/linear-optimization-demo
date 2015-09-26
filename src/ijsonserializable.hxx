#ifndef IJSONSERIALIZABLE_HXX
#define IJSONSERIALIZABLE_HXX

#include <QJsonObject>

class IJsonSerializable
{
  public:
//    JsonSerializable();
    virtual ~IJsonSerializable() = 0;

    virtual void read(const QJsonObject& json) = 0;
    virtual void write(QJsonObject& json) const = 0;
};

#endif // IJSONSERIALIZABLE_HXX
