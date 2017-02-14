#pragma once

#ifndef BOOSTQTINTEROP_HXX
#define BOOSTQTINTEROP_HXX


#include <QDebug>

#include "../math/numerictypes.hxx"


namespace BoostQtInterop
{
  QDebug operator <<(QDebug debug, const NumericTypes::boost_rational_t& x);

#ifdef LP_WITH_MULTIPRECISION
  QDebug operator <<(QDebug debug, const NumericTypes::boost_real_t& x);
#endif // LP_WITH_MULTIPRECISION
}


#endif // BOOSTQTINTEROP_HXX
