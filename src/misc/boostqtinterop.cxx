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
  operator <<(QDebug debug, const NumericTypes::BoostRational& x)
  {
    QDebugStateSaver stateSaver(debug);

    debug.nospace() << numericCast<QString, NumericTypes::BoostRational>(x);

    return debug;
  }


#ifdef LP_WITH_MULTIPRECISION
  QDebug
  operator <<(QDebug debug, const NumericTypes::BoostReal& x)
  {
    QDebugStateSaver stateSaver(debug);

    debug.nospace() << numericCast<QString, NumericTypes::BoostReal>(x);

    return debug;
  }
#endif // LP_WITH_MULTIPRECISION
}
