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
#ifdef LP_WITH_MULTIPRECISION
  namespace mp = NumericTypes::mp;
#endif // LP_WITH_MULTIPRECISION
  using NumericTypes::rational_t;
  using NumericTypes::real_t;
  using std::fabs;
  using std::is_integral;
  using std::is_floating_point;
  using std::isfinite;
  using std::make_pair;
  using std::max;
  using std::pair;
  using std::trunc;


#ifdef LP_WITH_MULTIPRECISION
  using NumericTypes::boost_integer_t;
  using NumericTypes::boost_real_t;


  template<>
  /**
   * @brief integerPart
   * @param x
   * TODO: !
   * @return
   */
  inline boost_integer_t
  integerPart(boost_real_t x)
  {
    return boost_integer_t(0);
  }
#endif // LP_WITH_MULTIPRECISION


#ifdef LP_WITH_MULTIPRECISION
  using NumericTypes::boost_real_t;


  template<>
  /**
   * @brief isFinite
   * @param x
   * @return
   */
  inline bool
  isFinite(boost_real_t x)
  {
    return true;
  }
#else // LP_WITH_MULTIPRECISION
  using NumericTypes::builtin_real_t;


  template<>
  /**
   * @brief isFinite
   * @param x
   * @return
   */
  inline bool
  isFinite(builtin_real_t x)
  {
    return isfinite(x);
  }
#endif // LP_WITH_MULTIPRECISION


  template<typename R, typename T>
  /**
   * @brief MathUtils::rationalize
   * Finds the "best" rational approximation of the given real number `x'
   * that satisfies given restrictions (relative error,
   * maximal denominator value and maximal iterations count)
   * by using Continued Fractions method.
   * For the reference see:
   *   `http://mathworld.wolfram.com/ContinuedFraction.html'.
   * The absolute error value will be: |p./q - x|.
   * NOTE: This function doesn't throw any exceptions if the sequence
   *         failed to converge due to too tight given restrictions.
   * TODO: Check for isFinite<T>(x).
   * @param x Real number to approximate.
   * @param tolerance Relative tolerance value
   *          (default is `Config::MathUtils::Epsilon').
   * @param maxDenominator Maximum denominator value.
   *          (default is `NumericLimits::max<R>()').
   * @param maxIterations Maximum iterations count allowed
   *          (23 is enough for the worst case when (x == Sqrt[2])).
   * @return Pair of integers 〈p; q〉 where (p./q ≈ x) ∧ (|q| <= maxDenominator).
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


    T r1(x); //See eq. (8): r[0] := x
#ifdef LP_WITH_MULTIPRECISION
    const T r1_integerPart(mp::trunc(r1));
#else // LP_WITH_MULTIPRECISION
    const T r1_integerPart(trunc(r1));
#endif // LP_WITH_MULTIPRECISION

    //Check for overflow (assuming that values range of `T' is larger than `R')
    if (r1_integerPart > T(NumericLimits::max<R>()))
    {
      qWarning() << "MathUtils::rationalize<R, T>:"
                    " overflow at the upper bound of `R'";

      return make_pair(NumericLimits::max<R>(), R(1));
    }
    else
    {
      if (r1_integerPart < T(NumericLimits::min<R>()))
      {
        qWarning() << "MathUtils::rationalize<R, T>:"
                      " overflow at the lower bound of `R'";

        return make_pair(NumericLimits::min<R>(), R(1));
      }
      else
      {
        //Now we can safely cast ⌊r0⌋ from `T' to `R'
#ifdef LP_WITH_MULTIPRECISION
        R a1(integerPart<R, T>(r0_integerPart)); //See eq. (5): a[0] := ⌊x⌋
#else // LP_WITH_MULTIPRECISION
        R a1(r1_integerPart); //See eq. (5): a[0] := ⌊x⌋
#endif // LP_WITH_MULTIPRECISION

        //If we got "almost integer" `x' or one of the trivial cases,
        //we should return pair 〈a0; 1〉
        //(or the loop will stuck at division by 0. later)
        if (
          isEqual<T>(T(a1), x, tolerance) ||
          maxDenominator == R(1) ||
          maxIterations == uint16_t(1)
        )
        {
          return make_pair(a1, R(1));
        }
        else
        {
          uint16_t iterCount(1);

          R p0(1),  q0(0); //See eq. (25): p[-1] := 1,    q[-1] := 0
          R p1(a1), q1(1); //See eq. (26): p[ 0] := a[0], q[ 0] := 1
          R p2(0),  q2(1); //              p[ 1] := 0,    q[ 1] := 1
          //NOTE: Indices in variables' names is shifted by 1.
          //We use only p[-1], q[-1] and p[0], q[0] as starting coeffs.
          //Rather than keeping entire sequence of p's and q's
          //we will keep and reuse coeffs computed on current iteration
          //(p[1] and q[1]) to compute new coeffs on next iteration.

          while (true)
          {
            ++iterCount;

            const T r2(T(1) / (r1 - T(a1))); //See eq. (9): r[n] := 1 ./ (r[n-1] - a[n-1])
#ifdef LP_WITH_MULTIPRECISION
            const R a2(integerPart<R, T>(mp::trunc(r2))); //See eq. (10): a[n] := ⌊r[n]⌋
#else // LP_WITH_MULTIPRECISION
            const R a2(trunc(r2)); //See eq. (10): a[n] := ⌊r[n]⌋
#endif // LP_WITH_MULTIPRECISION

            p2 = a2 * p1 + p0; //See eq. (27): p[n] == a[n] * p[n-1] + p[n-2]
            q2 = a2 * q1 + q0; //See eq. (28): q[n] == a[n] * q[n-1] + q[n-2]

            //Look at the n-th convergent (see eq. (11)): c[n] := p[n] ./ q[n]
            const T c2(T(p2) / T(q2));

            if (
              !isEqual<T>(c2, x, tolerance) &&
              absoluteValue<R>(q2) <= maxDenominator &&
              iterCount < maxIterations
            )
            {
              //Now "shift" all the coeffs to the left:
              //p[-1] := p[0], q[-1] := q[0],
              //p[ 0] := p[1], q[ 0] := q[1],
              //r[ 0] := r[1], a[ 0] := a[1]
              p0 = p1;
              q0 = q1;

              p1 = p2;
              q1 = q2;

              r1 = r2;
              a1 = a2;
            }
            else
            {
              break;
            }
          }

          if (absoluteValue<R>(q2) <= maxDenominator)
          {
            return make_pair(p2, q2);
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


#ifdef LP_WITH_MULTIPRECISION
  using NumericTypes::boost_real_t;

  template<>
  /**
   * @brief absoluteValue
   * @param x
   * @return |x|.
   */
  inline boost_real_t
  absoluteValue(boost_real_t x)
  {
    return boost_real_t(mp::fabs(x));
  }
#else // LP_WITH_MULTIPRECISION
  using NumericTypes::builtin_real_t;
  using NumericTypes::builtin_integer_t;


  template<>
  /**
   * @brief absoluteValue
   * @param x
   * @return |x|.
   */
  inline builtin_real_t
  absoluteValue(builtin_real_t x)
  {
    return builtin_real_t(fabs(x));
  }


  template<>
  /**
   * @brief absoluteValue
   * @param x
   * @return |x|.
   */
  inline builtin_integer_t
  absoluteValue(builtin_integer_t x)
  {
    return ((x < builtin_integer_t(0)) ? x * builtin_integer_t(-1): x);
  }
#endif // LP_WITH_MULTIPRECISION


  template<>
  /**
   * @brief absoluteValue
   * @param x
   * @return |x|.
   */
  inline rational_t
  absoluteValue(rational_t x)
  {
    return ((x < rational_t(0)) ? x * rational_t(-1) : x);
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
  isEqual(real_t x, real_t y, real_t tolerance)
  {
    if (isFinite<real_t>(x) && isFinite<real_t>(y))
    {
      return (
        absoluteValue<real_t>(x - y) <=
        tolerance * max<real_t>(
          {real_t(1), absoluteValue<real_t>(x), absoluteValue<real_t>(y)}
        )
      );
    }
    else
    {
      return (x == y);
    }
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
  isEqual(rational_t x, rational_t y, rational_t tolerance)
  {
    return (absoluteValue<rational_t>(x - y) <= tolerance);
  }


  template<>
  /**
   * @brief isEqual
   * @param x
   * @param y
   * @return `true' if (x ≊ y), `false' otherwise.
   */
  inline bool
  isEqual(real_t x, real_t y)
  {
    if (isFinite<real_t>(x) && isFinite<real_t>(y))
    {
      return (
        absoluteValue<real_t>(x - y) <=
        Epsilon * max<real_t>(
          {real_t(1), absoluteValue<real_t>(x), absoluteValue<real_t>(y)}
        )
      );
    }
    else
    {
      return (x == y);
    }
  }


  template<>
  /**
   * @brief isEqual
   * @param x
   * @param y
   * @return `true' if (x == y), `false' otherwise.
   */
  inline bool
  isEqual(rational_t x, rational_t y)
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
  isLessThan(real_t x, real_t y)
  {
    if (isFinite<real_t>(x) && isFinite<real_t>(y))
    {
      return (
        (x - y) <
        real_t(-1) * Epsilon * max<real_t>(
          {real_t(1), absoluteValue<real_t>(x), absoluteValue<real_t>(y)}
        )
      );
    }
    else
    {
      return (x < y);
    }
  }


  template<>
  /**
   * @brief isLessThan
   * @param x
   * @param y
   * @return `true' if (x < y), `false' otherwise.
   */
  inline bool
  isLessThan(rational_t x, rational_t y)
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
  isGreaterThan(real_t x, real_t y)
  {
    if (isFinite<real_t>(x) && isFinite<real_t>(y))
    {
      return (
        (x - y) >
        Epsilon * max<real_t>(
          {real_t(1), absoluteValue<real_t>(x), absoluteValue<real_t>(y)}
        )
      );
    }
    else
    {
      return (x > y);
    }
  }


  template<>
  /**
   * @brief isGreaterThan
   * @param x
   * @param y
   * @return `true' if (x > y), `false' otherwise.
   */
  inline bool
  isGreaterThan(rational_t x, rational_t y)
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
  isEqualToZero(real_t x)
  {
    if (isFinite<real_t>(x))
    {
      return (
        absoluteValue<real_t>(x) <=
        Epsilon * max<real_t>(real_t(1), absoluteValue<real_t>(x))
      );
    }
    else
    {
      return (x == real_t(0));
    }
  }


  template<>
  /**
   * @brief isEqualToZero
   * @param x
   * @return `true' if (x == 0), `false' otherwise.
   */
  inline bool
  isEqualToZero(rational_t x)
  {
    return (x == rational_t(0));
  }


  template<>
  /**
   * @brief isGreaterThanZero
   * @param x
   * @return `true' if (x ≳ 0), `false' otherwise.
   */
  inline bool
  isGreaterThanZero(real_t x)
  {
    if (isFinite<real_t>(x))
    {
      return (
        x > Epsilon * max<real_t>(real_t(1), absoluteValue<real_t>(x))
      );
    }
    else
    {
      return (x > real_t(0));
    }
  }


  template<>
  /**
   * @brief isGreaterThanZero
   * @param x
   * @return `true' if (x > 0), `false' otherwise.
   */
  inline bool
  isGreaterThanZero(rational_t x)
  {
    return (x > rational_t(0));
  }


  template<>
  /**
   * @brief isLessThanZero
   * @param x
   * @return `true' if (x ≲ 0), `false' otherwise.
   */
  inline bool
  isLessThanZero(real_t x)
  {
    if (isFinite<real_t>(x))
    {
      return (
        x < (-Epsilon) * max<real_t>(real_t(1), absoluteValue<real_t>(x))
      );
    }
    else
    {
      return (x < real_t(0));
    }
  }


  template<>
  /**
   * @brief isLessThanZero
   * @param x
   * @return `true' if (x < 0), `false' otherwise.
   */
  inline bool
  isLessThanZero(rational_t x)
  {
    return (x < rational_t(0));
  }


  template<>
  /**
   * @brief isGreaterOrEqualToZero
   * @param x
   * @return `true' if (x >= 0), `false' otherwise.
   */
  inline bool
  isGreaterThanOrEqualToZero(real_t x)
  {
    return (x >= real_t(0));
  }


  template<>
  /**
   * @brief isGreaterOrEqualToZero
   * @param x
   * @return `true' if (x >= 0), `false' otherwise.
   */
  inline bool
  isGreaterThanOrEqualToZero(rational_t x)
  {
    return (x >= rational_t(0));
  }
}


#endif // MATHUTILS_TXX
