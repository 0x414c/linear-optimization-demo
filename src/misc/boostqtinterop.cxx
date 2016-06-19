#include "boostqtinterop.hxx"

#include <QDebug>
#include <QDebugStateSaver>
#include <QString>

#include "dataconvertors.hxx"
#include "../math/numerictypes.hxx"


namespace BoostQtInterop
{
  using DataConvertors::numericCast;


  QDebug
  operator <<(QDebug debug, const NumericTypes::boost_rational_t& x)
  {
    QDebugStateSaver stateSaver(debug);

    debug.nospace() << numericCast<QString, NumericTypes::boost_rational_t>(x);

    return debug;
  }


#ifdef LP_WITH_MULTIPRECISION
  QDebug
  operator <<(QDebug debug, const NumericTypes::boost_real_t& x)
  {
    QDebugStateSaver stateSaver(debug);

    debug.nospace() << numericCast<QString, NumericTypes::boost_real_t>(x);

    return debug;
  }
#endif // LP_WITH_MULTIPRECISION
}
