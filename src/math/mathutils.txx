#pragma once

#ifndef MATHUTILS_TXX
#define MATHUTILS_TXX


#include "mathutils.hxx"

#include <cmath>

#include <QDebug>

#include <algorithm>
#include <utility>

#include "mathutils.hxx"
#include "../misc/boostextensions.hxx"
#include "../misc/boostqtinterop.hxx"
#include "numerictypes.hxx"


namespace MathUtils
{
  using namespace BoostQtInterop;
  using MathUtils::isEqual;
  using NumericTypes::Rational;
  using NumericTypes::Real;
  using std::fabs;
  using std::floor;
  using std::make_pair;
  using std::max;
  using std::pair;


  template<typename R, typename T/*, typename*/>
  /**
   * @brief MathUtils::rationalize
   * Finds the "best" rational approximation of the given real number `x'
   * that satisfies given constraints (Relative Error (Tolerance) and
   * Maximal Denominator) by using Continued Fractions method.
   * For the reference see:
   *   `http://mathworld.wolfram.com/ContinuedFraction.html'.
   * @param x Number to approximate.
   * @param tolerance Tolerance value (default is `1E-16').
   * @param maxIterations Maximum iterations count allowed.
   * @param maxDenominator Maximum denominator value.
   * @return Pair 〈p; q〉 where (x ≈ p/q) ∧ (q < maxDenominator).
   */
  pair<R, R>
  rationalize(T x, T tolerance, uint16_t maxIterations, R maxDenominator)
  {
    T r0(x); //See eq. (8)
    T r0_integerPart(floor(r0));

    //Check for overflow (assuming that `T' is larger than `R')
    if (r0_integerPart > T(NumericLimits::max<R>()))
    {
      qWarning() << "MathUtils::rationalize<R>:"
                    " overflow at the upper bound of `R'";

      return make_pair(NumericLimits::max<R>(), 1);
    }
    else
    {
      if (r0_integerPart < T(NumericLimits::min<R>()))
      {
        qWarning() << "MathUtils::rationalize<R>:"
                      " overflow at the lower bound of `R'";

        return make_pair(NumericLimits::min<R>(), 1);
      }
      else
      {
        //Now we can safely cast ⌊r0⌋ from `T' to `R'
        R a0(r0_integerPart); //See eq. (5)

        //If we got "almost integer" `x', we should return 〈a_0; 1〉
        //(or the loop will stuck at division by 0.)
        if (isEqual<T>(r0, T(a0)))
        {
          return make_pair(a0, R(1));
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
            T rn(T(1) / T(r0 - T(a0))); //See eq. (9)
            R an(floor(rn)); //See eq. (10)
            pn = an * p1 + p0; //See eq. (27), (28)
            qn = an * q1 + q0;

            //Look at the n-th convergent `cn'
            const T cn(T(pn) / T(qn));
            if (
              ++iterCount <= maxIterations &&
              !isEqual<T>(x, cn) &&
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
   * @brief isGreaterThan
   * @param x
   * @param y
   * @return `true' if (x ≲ y), `false' otherwise.
   */
  inline bool
  isGreaterThan(Real x, Real y)
  {
    return (
      (x - y) >
      Epsilon * max<Real>(
        {Real(1), absoluteValue<Real>(y), absoluteValue<Real>(y)}
      )
    );
  }


  template<>
  /**
   * @brief isGreaterThan
   * @param x
   * @param y
   * @return `true' if (x < y), `false' otherwise.
   */
  inline bool
  isGreaterThan(Rational x, Rational y)
  {
    return (x > y);
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


#ifdef LP_WITH_MULTIPRECISION
  template<>
  /**
   * @brief isEqualToZero
   * @param x
   * @return `true' if (x == 0), `false' otherwise.
   */
  inline bool
  isEqualToZero(NumericTypes::BoostRational x)
  {
    return (numerator(x) == NumericTypes::BoostInteger(0));
  }
#else
  template<>
  /**
   * @brief isEqualToZero
   * @param x
   * @return `true' if (x == 0), `false' otherwise.
   */
  inline bool
  isEqualToZero(NumericTypes::BoostRational x)
  {
    return (x.numerator() == NumericTypes::BuiltinInteger(0));
  }
#endif // LP_WITH_MULTIPRECISION


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
