#pragma once

#ifndef MATHUTILS_TXX
#define MATHUTILS_TXX


#include "mathutils.hxx"

#include <cmath>

#include <QDebug>

#include <algorithm>
#include <utility>

#include "numerictypes.hxx"


namespace MathUtils
{
  using NumericTypes::Rational;
  using NumericTypes::Real;
  using std::fabs;
  using std::max;
  using std::pair;
  using std::make_pair;


  template<typename R, typename>
  /**
   * @brief MathUtils::rationalize
   * Finds the "best" rational approximation of the given real number `x'
   * within the given constraints (Relative Error (Tolerance) and
   * Maximal Denominator) by using Continued Fractions method.
   * For the reference see:
   *  `http://mathworld.wolfram.com/ContinuedFraction.html'.
   * @param x Number to approximate.
   * @param tolerance Tolerance value (default is `1E-16').
   * @param maxIterations Maximum iterations count allowed.
   * @param maxDenominator Maximum denominator value.
   * @return Pair 〈p; q〉 where (p / q ≈ x) ∧ (q < maxDenominator).
   */
  pair<R, R>
  rationalize(Real x, Real tolerance, uint16_t maxIterations, R maxDenominator)
  {
//    static_assert(
//      numeric_limits<R>::is_integer,
//      "MathUtils::rationalize<R>: `R' should be a primitive integer type!"
//    );

    Real r0(x); //See eq. (8)
    Real integerPart(floor(r0));

    //Check for overflow
    if (integerPart > Real(NumericLimits::max<R>()))
    {
      qWarning() << "MathUtils::rationalize<R>:"
                    " overflow at the upper bound of `R'";

      return make_pair(NumericLimits::max<R>(), 1);
    }
    else
    {
      if (integerPart < Real(NumericLimits::min<R>()))
      {
        qWarning() << "MathUtils::rationalize<R>:"
                      " overflow at the lower bound of `R'";

        return make_pair(NumericLimits::min<R>(), 1);
      }
      else
      {
        //Now we can safely cast ⌊r0⌋ to R
        R a0(integerPart); //See eq. (5)

        //If we got "almost integer" `x', we should return 〈a_0; 1〉
        //(or the loop will stuck at division by 0.)
        if (fabs(r0 - a0) <= tolerance * fabs(r0))
        {
          return make_pair(a0, 1);
        }
        else
        {
          R p0(1),  q0(0); //See eq. (25), (26)
          R p1(a0), q1(1); //we use only p[0] and p[1] as starting coeffs
          R pn(0),  qn(1); //p[n] (where n==2) will be the scratch place
                           //(we will "shift" and reuse the coeffs rather
                           //than storing they all)

          uint16_t iterCount(0);
          while (true)
          {
            Real rn(Real(1) / Real(r0 - a0)); //See eq. (9)
            R an(floor(rn)); //See eq. (10)
            pn = an * p1 + p0; //See eq. (27), (28)
            qn = an * q1 + q0;

            //Look at the n-th convergent `cn'
            const Real cn(Real(pn) / Real(qn));
            if (
              ++iterCount <= maxIterations &&
              fabs(x - cn) > tolerance * fabs(x) &&
              qn < maxDenominator
            )
            {
              //Now "shift" all the coeffs to the left
              p0 = p1; q0 = q1;
              p1 = pn; q1 = qn;
              a0 = an;
              r0 = rn;
            }
            else
            {
              break;
            }
          }

          if (iterCount > maxIterations)
          {
            qWarning() << "MathUtils::rationalize<R>:"
                          " iterations limit `maxIterations' exceeded:"
                          " `iterCount' ==" << iterCount << ">" <<
                          maxIterations << "; `tolerance' ==" << tolerance;
          }

          if (qn <= maxDenominator)
          {
            return make_pair(pn, qn);
          }
          else
          {
            //Try to prevent possible overflow and use result from previous
            //iteration (which have smaller absolute value)

            return make_pair(p1, q1);
          }
        }
      }
    }
  }


  template<>
  /**
   * @brief absoluteValue
   * @param x
   * @return |x|.
   */
  inline Real
  absoluteValue(Real x)
  {
    return Real(fabs(double(x)));
  }


  template<>
  /**
   * @brief absoluteValue
   * @param x
   * @return |x|.
   */
  inline Rational
  absoluteValue(Rational x)
  {
    return ((x < Rational(0)) ? -x : x);
  }


  template<>
  /**
   * @brief isEqual
   * @param x
   * @param y
   * @return `true' if (x ~= y), `false' otherwise.
   */
  inline bool
  isEqual(Real x, Real y)
  {
    return (
      absoluteValue<Real>(x - y) <=
      Epsilon * max<Real>(
        {Real(1), absoluteValue<Real>(x), absoluteValue<Real>(y)}
      )
    );
  }


  template<>
  /**
   * @brief isEqual
   * @param x
   * @param y
   * @return `true' if (x == y), `false' otherwise.
   */
  inline bool
  isEqual(Rational x, Rational y)
  {
    return (x == y);
  }


  template<>
  /**
   * @brief isLessThan
   * @param x
   * @param y
   * @return `true' if (x ≲ y), `false' otherwise.
   */
  inline bool
  isLessThan(Real x, Real y)
  {
    return (
      (x - y) <
      (-Epsilon) * max<Real>(
        {Real(1), absoluteValue<Real>(y), absoluteValue<Real>(y)}
      )
    );
  }


  template<>
  /**
   * @brief isLessThan
   * @param x
   * @param y
   * @return `true' if (x < y), `false' otherwise.
   */
  inline bool
  isLessThan(Rational x, Rational y)
  {
    return (x < y);
  }


  template<>
  /**
   * @brief isEqualToZero
   * @param x
   * @return `true' if (x ~= 0), `false' otherwise.
   */
  inline bool
  isEqualToZero(Real x)
  {
    return (
      absoluteValue<Real>(x) <=
      Epsilon * max<Real>(Real(1), absoluteValue<Real>(x))
    );
  }


  template<>
  /**
   * @brief isEqualToZero
   * @param x
   * @return `true' if (x == 0), `false' otherwise.
   */
  inline bool
  isEqualToZero(Rational x)
  {
    return (x.numerator() == Integer(0));
  }


  template<>
  /**
   * @brief isGreaterThanZero
   * @param x
   * @return `true' if (x ≳ 0), `false' otherwise.
   */
  inline bool
  isGreaterThanZero(Real x)
  {
    return (
      x > Epsilon * max<Real>(Real(1), absoluteValue<Real>(x))
    );
  }


  template<>
  /**
   * @brief isGreaterThanZero
   * @param x
   * @return `true' if (x > 0), `false' otherwise.
   */
  inline bool
  isGreaterThanZero(Rational x)
  {
    return (x > Rational(0));
  }


  template<>
  /**
   * @brief isLessThanZero
   * @param x
   * @return `true' if (x ≲ 0), `false' otherwise.
   */
  inline bool
  isLessThanZero(Real x)
  {
    return (
      x < (-Epsilon) * max<Real>(Real(1), absoluteValue<Real>(x))
    );
  }


  template<>
  /**
   * @brief isLessThanZero
   * @param x
   * @return `true' if (x < 0), `false' otherwise.
   */
  inline bool
  isLessThanZero(Rational x)
  {
    return (x < Rational(0));
  }


  template<>
  /**
   * @brief isGreaterOrEqualToZero
   * @param x
   * @return `true' if (x >= 0), `false' otherwise.
   */
  inline bool
  isGreaterThanOrEqualToZero(Real x)
  {
    return (x >= Real(0));
  }


  template<>
  /**
   * @brief isGreaterOrEqualToZero
   * @param x
   * @return `true' if (x >= 0), `false' otherwise.
   */
  inline bool
  isGreaterThanOrEqualToZero(Rational x)
  {
    return (x >= Rational(0));
  }
}


#endif // MATHUTILS_TXX
