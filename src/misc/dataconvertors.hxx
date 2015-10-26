#ifndef DATACONVERTORS_HXX
#define DATACONVERTORS_HXX

#include <QString>

#include "utils.hxx"

using namespace Utils;

namespace DataConvertors
{
  template<typename R>
  inline R numericCast(const QString& from) //TODO: ~? Use the second template only
  {
    static_assert(
      False<R>::value,
      "DataConvertors::numericCast<R>: You can only use one of the specified specializations!"
    );

    return R(0);
  }

  template<typename R, typename T>
  inline R numericCast(const T& from)
  {
    static_assert(
      False<R, T>::value,
      "DataConvertors::numericCast<R, T>: You can only use one of the specified specializations!"
    );

    return R(0);
  }  
}

#include "dataconvertors.txx"

#endif // DATACONVERTORS_HXX
