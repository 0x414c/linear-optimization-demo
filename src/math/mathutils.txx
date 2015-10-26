#ifndef MATHUTILS_TXX
#define MATHUTILS_TXX

#include "mathutils.hxx"

#include <limits>
#include <utility>

#include <QDebug>

#include "../misc/utils.hxx"

using namespace std;
using namespace Utils;

namespace MathUtils
{
  template<typename R>
  /**
   * @brief MathUtils::rationalize
   * Finds the "best" rational approximation of the given real number
   * within the given constraints (Relative Error and Maximal Denominator)
   * by using Continued Fractions method.
   * For the reference see `http://mathworld.wolfram.com/ContinuedFraction.html'.
   * @param x
   * @param tolerance
   * @param maxIterations
   * @param maxDenominator
   * @return
   */
  pair<R, R> rationalize(real_t x, real_t tolerance, uint16_t maxIterations, R maxDenominator)
  {
    static_assert(
      numeric_limits<R>::is_integer,
      "MathUtils::rationalize<R>: R should be a primitive integer type!"
    );

    real_t r0 = x; //See eq. (8)
    real_t integerPart = floor(r0);

    if (integerPart > real_t(numeric_limits<R>::max())) //Check for upper overflow
    {
      qDebug() << "MathUtils::rationalize<R>: overflow at upper bound of R";

      return make_pair(numeric_limits<R>::max(), 1);
    }
    else
    {
      if (integerPart < real_t(numeric_limits<R>::min())) //Check for lower overflow
      {
        qDebug() << "MathUtils::rationalize<R>: overflow at lower bound of R";

        return make_pair(numeric_limits<R>::min(), 1);
      }
      else //Now we can safely cast ⌊r0⌋ to R
      {
        R a0 = R(integerPart); //See eq. (5)

        if (fabs(r0 - a0) <= tolerance * fabs(r0)) //If we got "almost integer" `x', we should return `〈a_0; 1〉'
                                                   //(or the loop will stuck at division by 0.)
        {
          return make_pair(a0, 1);
        }
        else
        {
          R p0 = 1,  q0 = 0; //See eq. (25), (26)
          R p1 = a0, q1 = 1; //we use only p[0] and p[1] as starting coeffs
          R pn = 0,  qn = 1; //p[n] (where n==2) will be the scratch place
                             //(we will "shift" and reuse the coeffs rather than storing they all)

          uint16_t iterCount = 0;
          while (true)
          {
            real_t rn = 1. / (r0 - a0); //See eq. (9)
            R a_n = R(floor(rn)); //See eq. (10)
            pn = a_n * p1 + p0; //See eq. (27), (28)
            qn = a_n * q1 + q0;

            real_t cn = real_t(pn) / real_t(qn); //Look at the n-th convergent
            if (++iterCount <= maxIterations &&
                fabs(x - cn) > tolerance * fabs(x) &&
                qn < maxDenominator
            )
            {
              p0 = p1; q0 = q1; //Now "shift" the coeffs to the left
              p1 = pn; q1 = qn;
              a0 = a_n;
              r0 = rn;
            }
            else
            {
              break;
            }
          }

          if (iterCount > maxIterations)
          {
            qDebug() << "MathUtils::rationalize<R>: iterations limit exceeded: iterCount==" << iterCount << ">" << maxIterations << "; tolerance==" << tolerance;
          }
//          else
//          {
//            qDebug() << "MathUtils::rationalize<R>: iterations total:" << iterCount;
//          }

          if (qn <= maxDenominator)
          {
            return make_pair(pn, qn);
          }
          else //Try to prevent possible overflow and use result from previous iteration
               //(which have smaller absolute value)
          {
            return make_pair(p1, q1);
          }
        }
      }
    }
  }
}

#endif // MATHUTILS_TXX
