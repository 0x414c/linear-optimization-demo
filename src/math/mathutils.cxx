#include "mathutils.hxx"

#include <limits>
#include <utility>

#include <QDebug>

#include "../misc/utils.hxx"

using namespace std;
using namespace Utils;

/**
 * @brief MathUtils::rationalize
 * Finds the "best" rational approximation of the given real number
 * within given constraints (Relative error and Maximal Denominator)
 * by using Continued Fractions method.
 * For reference see `http://mathworld.wolfram.com/ContinuedFraction.html'.
 * @param x
 * @return
 */
pair<integer_t, integer_t> MathUtils::rationalize(real_t x, real_t eps, uint16_t maxIterations, integer_t maxDenominator)
{
  real_t r_0 = x; //See eq. (8)
  integer_t a_0 = integer_t(floor(r_0)); //See eq. (5)

  if (fabs(a_0 - x) < eps) {
//    qDebug() << "MathUtils::rationalize: approximated" << x << "by" << a_0;

    return make_pair(a_0, 1);
  }
  else
  {
    integer_t p_1 = 1, q_1 = 0; //See eq. (25), (26)
    integer_t p_0 = a_0, q_0 = 1; //we use only p[0] and p[n-1] as starting coeffs
    integer_t p_n = 0, q_n = 1; //p[n] (where n==1) will be the scratch place

    uint16_t n = 0;
    for (;;) {
//      qDebug() << "\t>>>" << n + 1 << ":" << p_1 << q_1 << p_0 << q_0 << p_n << q_n;
      real_t r_n = 1.0 / (r_0 - a_0); //See eq. (9)
      integer_t a_n = integer_t(floor(r_n)); //See eq. (10)
      p_n = a_n * p_0 + p_1; //See eq. (27), (28)
      q_n = a_n * q_0 + q_1;

      real_t c_n = real_t(p_n) / real_t(q_n); //look at the nth convergent
      if (n++ < maxIterations && fabs(c_n - x) > eps && q_n < maxDenominator)
      {
        p_1 = p_0; q_1 = q_0; //"shift" the coeffs to the left
        p_0 = p_n; q_0 = q_n;
        a_0 = a_n;
        r_0 = r_n;
      } else {
        break;
      }
    }

    if (n >= maxIterations) {
//      qDebug() << "MathUtils::rationalize: iterations limit exceeded (maxIterations ==" << maxIterations << ")";
    }

    //prevent possible overflow and use results from previous iteration
    if (q_n < maxDenominator)
    {
//      qDebug() << "MathUtils::rationalize: approximated" << x << "by" << p_n << "/" << q_n;

      return make_pair(p_n, q_n);
    }
    else
    {
//      qDebug() << "MathUtils::rationalize: approximated" << x << "by" << p_0 << "/" << q_0;

      return make_pair(p_0, q_0);
    }
  }
}
