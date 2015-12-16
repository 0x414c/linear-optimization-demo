#pragma once

#ifndef DATACONVERTORS_HXX
#define DATACONVERTORS_HXX


#include <QString>

#include "utils.hxx"


//TODO: ~? Merge w/ `NumericTypes'
namespace DataConvertors
{
  using Utils::AlwaysFalse;


  template<typename R>
  //TODO: ~? Use the second template only
  R numericCast(const QString& from)
  {
    static_assert(
      AlwaysFalse<R>::value,
      "DataConvertors::numericCast<R>:"
      " You can only use one of the specified specializations!"
    );

    return R(0);
  }


  template<typename R, typename T>
  R numericCast(const T& from)
  {
    static_assert(
      AlwaysFalse<R, T>::value,
      "DataConvertors::numericCast<R, T>:"
      " You can only use one of the specified specializations!"
    );

    return R(0);
  }
}


#include "dataconvertors.txx"


#endif // DATACONVERTORS_HXX
