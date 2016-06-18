#pragma once

#ifndef MATHUTILS_TXX
#define MATHUTILS_TXX


#include "mathutils.hxx"

#include <cmath>

#include <QDebug>

#include <algorithm>
#include <type_traits>
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
  using std::is_integral;
  using std::is_floating_point;
  using std::make_pair;
  using std::max;
  using std::pair;
  using std::trunc;


  template<typename R, typename T>
  /**
   * @brief MathUtils::rationalize
   * Finds the "best" rational approximation of the given real number `x'
   * that satisfies given constraints (relative error (tolerance),
   * maximal denominator and convergents (iterations) count)
   * by using Continued Fractions method.
   * For the reference see:
   *   `http://mathworld.wolfram.com/ContinuedFraction.html'.
   * The absolute error value will be: |p./q - x|.
   * TODO: check if `R' is integer type and break compilation if it isn't.
   * @param x Number to approximate.
   * @param tolerance Tolerance value (default is `Config::MathUtils::Epsilon').
   * @param maxDenominator Maximum denominator value.
   * @param maxIterations Maximum iterations (convergents) count allowed.
   * @return Pair of integers 〈p; q〉 where (p./q ≈ x) ∧ (q <= maxDenominator).
   */
  pair<R, R>
  rationalize(T x, T tolerance, R maxDenominator, uint16_t maxIterations)
  {
    static_assert(
      is_integral<R>::value,
      "MathUtils::rationalize<R, T>: R should be integral type."
    );

    static_assert(
      is_floating_point<T>::value,
      "MathUtils::rationalize<R, T>: T should be floating-point type."
    );


    T r0(x); //See eq. (8): r[0] := x
    const T r0_integerPart(trunc(r0));

    //Check for overflow (assuming that values range of `T' is larger than `R')
    if (r0_integerPart > T(NumericLimits::max<R>()))
    {
      qWarning() << "MathUtils::rationalize<R, T>:"
                    " overflow at the upper bound of `R'";

      return make_pair(NumericLimits::max<R>(), R(1));
    }
    else
    {
      if (r0_integerPart < T(NumericLimits::min<R>()))
      {
        qWarning() << "MathUtils::rationalize<R, T>:"
                      " overflow at the lower bound of `R'";

        return make_pair(NumericLimits::min<R>(), R(1));
      }
      else
      {
        //Now we can safely cast ⌊r0⌋ from `T' to `R'
        R a0(r0_integerPart); //See eq. (5): a[0] := ⌊x⌋

        //If we got "almost integer" `x' or one of the trivial cases,
        //we should return 〈a0; 1〉
        //(or the loop will stuck at division by 0. later)
        if (
          isEqual<T>(T(a0), x, tolerance) ||
          maxDenominator == R(1) ||
          maxIterations == uint16_t(1)
        )
        {
          return make_pair(a0, R(1));
        }
        else
        {
          uint16_t iterCount(1);

          R p0(1),  q0(0); //See eq. (25): p[-1] := 1,    q[-1] := 0
          R p1(a0), q1(1); //See eq. (26): p[ 0] := a[0], q[ 0] := 1
          R pn(0),  qn(1); //See eq. (24): p[-2] := 0,    q[-2] := 1
          //We use only {p, q}[0] and {p, q}[1] as starting coeffs;
          //p[n] (where n==2) will be the scratch place
          //(we will "shift" and reuse the coeffs rather than storing they all)

          while (true)
          {
            ++iterCount;

            const T rn(T(1) / (r0 - T(a0))); //See eq. (9): r[n] := 1 ./ (r[n-1] - a[n-1])
            const R an(trunc(rn)); //See eq. (10): a[n] := ⌊r[n]⌋

            pn = an * p1 + p0; //See eq. (27): p[n] == a[n] * p[n-1] + p[n-2]
            qn = an * q1 + q0; //See eq. (28): q[n] == a[n] * q[n-1] + q[n-2]

            //Look at the n-th convergent (see eq. (11)): c[n] := p[n] ./ q[n]
            const T cn(T(pn) / T(qn));

            if (
              !isEqual<T>(cn, x, tolerance) &&
              qn < maxDenominator &&
              iterCount < maxIterations
            )
            {
              //Now "shift" all the coeffs to the left
              p0 = p1;
              q0 = q1;

              p1 = pn;
              q1 = qn;

              r0 = rn;
              a0 = an;
            }
            else
            {
              break;
            }
          }

          if (iterCount > maxIterations)
          {
            qWarning() << "MathUtils::rationalize<R, T>:"
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
    return Real(fabs(x));
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
   * @param tolerance
   * @return `true' if (x ≊ y), `false' otherwise.
   */
  inline bool
  isEqual(Real x, Real y, Real tolerance)
  {
    return (
      absoluteValue<Real>(x - y) <=
      tolerance * max<Real>(
        {Real(1), absoluteValue<Real>(x), absoluteValue<Real>(y)}
      )
    );
  }


  template<>
  /**
   * @brief isEqual
   * @param x
   * @param y
   * @param tolerance
   * @return `true' if (x == y), `false' otherwise.
   */
  inline bool
  isEqual(Rational x, Rational y, Rational tolerance)
  {
    return (absoluteValue(x - y) <= tolerance);
  }


  template<>
  /**
   * @brief isEqual
   * @param x
   * @param y
   * @return `true' if (x ≊ y), `false' otherwise.
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
        {Real(1), absoluteValue<Real>(x), absoluteValue<Real>(y)}
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
   * @return `true' if (x ≳ y), `false' otherwise.
   */
  inline bool
  isGreaterThan(Real x, Real y)
  {
    return (
      (x - y) >
      Epsilon * max<Real>(
        {Real(1), absoluteValue<Real>(x), absoluteValue<Real>(y)}
      )
    );
  }


  template<>
  /**
   * @brief isGreaterThan
   * @param x
   * @param y
   * @return `true' if (x > y), `false' otherwise.
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


  template<>
  /**
   * @brief isEqualToZero
   * @param x
   * @return `true' if (x == 0), `false' otherwise.
   */
  inline bool
  isEqualToZero(Rational x)
  {
    return (x == Rational(0));
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
