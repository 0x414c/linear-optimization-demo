#pragma once

#ifndef LINEARPROGRAMMINGUTILS_TXX
#define LINEARPROGRAMMINGUTILS_TXX


#include "linearprogrammingutils.hxx"

#include <functional>
#include <list>
#include <utility>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "../math/mathutils.hxx"
#include "../math/numericlimits.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgrammingUtils
{
  using boost::optional;
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using MathUtils::absoluteValue;
  using MathUtils::isEqual;
  using MathUtils::isEqualToZero;
  using MathUtils::isGreaterThanOrEqualToZero;
  using NumericTypes::Real;
  using std::list;
  using std::ref;


  template<typename T = Real>
  /**
   * @brief isSolutionFeasible
   * Checks if the point lies inside of
   * the feasible region which interior is
   * defined by the following matrix equation:
   *   {Ax <= b},
   * and if point is also satisfies mandatory
   * non-negativity constraints:
   *   (x[n] >= 0) for any `n' in [1; N].
   * @param x Point as N × 1 column-vector.
   * @param A Equations coefficients as M × N matrix
   * @param b Right-hand-side as M × 1 vector
   * @return `true' if point lies within given region,
   * `false' otherwise.
   */
  bool
  isSolutionFeasible(
    const Matrix<T, Dynamic, 1>& x,
    const Matrix<T, Dynamic, Dynamic>& A,
    const Matrix<T, Dynamic, 1>& b
  )
  {
    return (
      x.unaryExpr(ref(isGreaterThanOrEqualToZero<T>)).all() &&
      ((A * x).array() <= b.array()).all()
    );
  }


  template<typename TCoeff>
  struct findIntersectionImpl_<TCoeff, 2>
  {
    /**
     * @brief findIntersection
     * Finds intersection point `x' of two lines
     * given as the following matrix equation
     *   {Ax == b}.
     * The formula is
     *   x == {
     *         {(A11*b0 - A01*b1) / (-A01*A10 + A00*A11)},
     *         {(A10*b0 - A00*b1) / ( A01*A10 - A00*A11)}
     *        }.
     * NOTE: This function handles only two-dimensional case.
     * @param coeffs 2 × 2 coefficients matrix `A'
     * @param RHS 2 × 1 RHS matrix `b' as column-vector
     * @return (optional) intersection point `x' as 2 × 1 column-vector.
     */
    static optional<Matrix<TCoeff, 2, 1>>
    findIntersection(
      const Matrix<TCoeff, 2, 2>& A, const Matrix<TCoeff, 2, 1>& b
    )
    {
      Matrix<TCoeff, 2, 1> sol(2, 1);
      optional<Matrix<TCoeff, 2, 1>> ret;

      const TCoeff q(-A(0, 1) * A(1, 0) + A(0, 0) * A(1, 1)),
                   s( A(0, 1) * A(1, 0) - A(0, 0) * A(1, 1));

      if (!isEqualToZero<TCoeff>(q) && !isEqualToZero<TCoeff>(s))
      {
        const TCoeff p(A(1, 1) * b(0) - A(0, 1) * b(1)),
                     r(A(1, 0) * b(0) - A(0, 0) * b(1));

        sol <<
          p / q, //!
          r / s;

        ret = sol;
      }

      return ret;
    }
  };


  template<typename TCoeff>
  struct findIntersectionImpl_<TCoeff, 3>
  {
    /**
     * @brief findIntersection
     * Finds intersection point `x' of three lines
     * given as the following matrix equation
     *   {Ax == b}.
     * The formula is
     *   x == {
     *         {(A12*A21*b0  - A11*A22*b0  - A02*A21*b1 +
     *           A01*A22*b1  + A02*A11*b2  - A01*A12*b2) /
     *          (A02*A11*A20 - A01*A12*A20 - A02*A10*A21 +
     *           A00*A12*A21 + A01*A10*A22 - A00*A11*A22)},
     *         {(-A12*A20*b0 + A10*A22*b0  + A02*A20*b1 -
     *            A00*A22*b1 - A02*A10*b2  + A00*A12*b2) /
     *          (A02*A11*A20 - A01*A12*A20 - A02*A10*A21 +
     *           A00*A12*A21 + A01*A10*A22 - A00*A11*A22)},
     *         {(A11*A20*b0  - A10*A21*b0  - A01*A20*b1 +
     *           A00*A21*b1  + A01*A10*b2  - A00*A11*b2) /
     *          (A02*A11*A20 - A01*A12*A20 - A02*A10*A21 +
     *           A00*A12*A21 + A01*A10*A22 - A00*A11*A22)}
     *        }.
     * NOTE: This function handles only three-dimensional case.
     * @param coeffs 3 × 3 coefficients matrix `A'
     * @param RHS 3 × 1 RHS matrix `b' as column-vector
     * @return (optional) intersection point `x' as 3 × 1 column-vector.
     */
    static optional<Matrix<TCoeff, 3, 1>>
    findIntersection(
      const Matrix<TCoeff, 3, 3>& A, const Matrix<TCoeff, 3, 1>& b
    )
    {
      Matrix<TCoeff, 3, 1> sol(3, 1);
      optional<Matrix<TCoeff, 3, 1>> ret;

      const TCoeff q(A(0, 2) * A(1, 1) * A(2, 0) - A(0, 1) * A(1, 2) * A(2, 0) -
                     A(0, 2) * A(1, 0) * A(2, 1) + A(0, 0) * A(1, 2) * A(2, 1) +
                     A(0, 1) * A(1, 0) * A(2, 2) - A(0, 0) * A(1, 1) * A(2, 2)),
                   s(A(0, 2) * A(1, 1) * A(2, 0) - A(0, 1) * A(1, 2) * A(2, 0) -
                     A(0, 2) * A(1, 0) * A(2, 1) + A(0, 0) * A(1, 2) * A(2, 1) +
                     A(0, 1) * A(1, 0) * A(2, 2) - A(0, 0) * A(1, 1) * A(2, 2)),
                   u(A(0, 2) * A(1, 1) * A(2, 0) - A(0, 1) * A(1, 2) * A(2, 0) -
                     A(0, 2) * A(1, 0) * A(2, 1) + A(0, 0) * A(1, 2) * A(2, 1) +
                     A(0, 1) * A(1, 0) * A(2, 2) - A(0, 0) * A(1, 1) * A(2, 2));

      if (
        !isEqualToZero<TCoeff>(q) &&
        !isEqualToZero<TCoeff>(s) &&
        !isEqualToZero<TCoeff>(u)
      )
      {
        const TCoeff p(A(1, 2) * A(2, 1) * b(0)  - A(1, 1) * A(2, 2) * b(0) -
                       A(0, 2) * A(2, 1) * b(1)  + A(0, 1) * A(2, 2) * b(1) +
                       A(0, 2) * A(1, 1) * b(2)  - A(0, 1) * A(1, 2) * b(2)),
                     r(-A(1, 2) * A(2, 0) * b(0)  + A(1, 0) * A(2, 2) * b(0) +
                        A(0, 2) * A(2, 0) * b(1)  - A(0, 0) * A(2, 2) * b(1) -
                        A(0, 2) * A(1, 0) * b(2)  + A(0, 0) * A(1, 2) * b(2)),
                     t(A(1, 1) * A(2, 0) * b(0)  - A(1, 0) * A(2, 1) * b(0) -
                       A(0, 1) * A(2, 0) * b(1)  + A(0, 0) * A(2, 1) * b(1) +
                       A(0, 1) * A(1, 0) * b(2)  - A(0, 0) * A(1, 1) * b(2));

        sol <<
          p / q, //!
          r / s,
          t / u;

        ret = sol;
      }

      return ret;
    }
  };


  template<typename T>
  /**
   * @brief rowReducedEchelonForm
   * Algorithm: Transforming a matrix to row canonical/reduced
   * row echelon form aka RREF.
   * INPUT: M × N matrix `A'.
   * OUTPUT: M × N matrix `A^' in reduced row echelon form.
   * 1. Set j ← 1.
   * 2. For each row `i' from 1 to `M' do
   *  a. While column `j' has all zero elements, set j ← j + 1.
   *     If (j > N) return `A^'.
   *  b. If element A^[i, j] is zero, then interchange row `i'
   *     with a row (x > i) that has A^[x, j] ≠ 0.
   *  c. Divide each element of row `i' by A^[i, j],
   *     thus making the pivot a[i, j] equal to 1.
   *  d. For each row `k' from 1 to `M', with (k ≠ i), subtract row `i'
   *     multiplied by A^[k, j] from row `k'.
   * 3. Return transformed matrix `A^'.
   * There are many, many variants of the above algorithm.
   * For example, in step 2a you could always select the largest
   * element of the column as the pivot to help reduce rounding errors,
   * and you could combine steps 2c and 2d.
   * For the reference see:
   *  `http://www.di-mgt.com.au/matrixtransform.html';
   *  `http://www.millersville.edu/~bikenaga/linear-algebra/row-reduction/
   *   row-reduction.html'.
   * @param matrix Matrix `A' to reduce.
   * @return Struct containing row-reduced echelon form `A^'
   *         of matrix `A' along w/ rank of `A'.
   */
  RREF<T>
  reducedRowEchelonForm(const Matrix<T, Dynamic, Dynamic>& A)
  {
    const DenseIndex M(A.rows());
    const DenseIndex N(A.cols());
    DenseIndex rank(0);

    Matrix<T, Dynamic, Dynamic> A_(M, N);
    A_ << A;

    DenseIndex i(0), j(0);
    //1. Deal with each row `i' from 1 to `M' in turn, ...
    while (true)
    {
      //If we have reached the end
      if (i >= M || j >= N) // TODO: (i == M || j == N)
      {
        return RREF<T>(std::move(A_), rank);
      }
      else
      {
        //3. Interchange rows, if necessary, so that the
        //pivot element A^(i, j) is nonzero.
        DenseIndex x(i);
        DenseIndex xMax(i);
        T maxValue(0);
        bool isColNonZero(false);
        //... and work across the columns `j' from 1 to `N'
        //skipping any column of all zero entries.
        while (true)
        {
          if (isEqualToZero<T>(A_(x, j)))
          {
            ++x;
          }
          else
          {
            if (absoluteValue<T>(A_(x, j)) > absoluteValue<T>(maxValue))
            {
              xMax = x;
              maxValue = A_(x, j);
            }
            isColNonZero = true;
            ++x;
          }

          if (x >= M)
          {
            if (isColNonZero)
            {
              x = xMax;

              break;
            }
            else
            {
              ++j;
              if (j >= N)
              {
                return RREF<T>(std::move(A_), rank);
              }
              else
              {
                x = i;
                xMax = i;
                maxValue = T(0);
                isColNonZero = false;

                continue;
              }
            }
          }
        }

        if (x > i)
        {
          A_.row(x).swap(A_.row(i));
        }

        //4. Make the pivot equal to 1 by dividing each element
        //in the pivot row by the value of the pivot.
        const T pivot(A_(i, j));

        if (!isEqual<T>(pivot, T(1)))
        {
          A_.row(i) /= pivot; //!
        }

        //5. Make all elements above and below the pivot equal to 0 by
        //subtracting a suitable multiple of the pivot row from each other row.
        const Matrix<T, 1, Dynamic> pivotRow(A_.row(i));
        for (DenseIndex k(0); k < i; ++k)
        {
          const T factor(A_(k, j));

          A_.row(k) -= factor * pivotRow;
        }

        for (DenseIndex k(i + 1); k < M; ++k)
        {
          const T factor(A_(k, j));

          A_.row(k) -= factor * pivotRow;
        }

        //Jump to the next column
        ++j;
      }
      //Increase rank (now we have `1' as the leading element in the current row)
      ++rank;

      //Jump to the next row
      ++i;
    }
  }


  template<typename T>
  /**
   * @brief computeBoundingBox
   * TODO: ~? Use `std::minmax_element'.
   * @param points
   * @return
   */
  Matrix<T, 2, 2>
  computeBoundingBox(const list<Matrix<T, 2, 1>>& points)
  {
    Matrix<T, 2, 2> boundingBox(2, 2);
    T xMin(NumericLimits::max<T>()),
      xMax(NumericLimits::min<T>()),
      yMin(NumericLimits::max<T>()),
      yMax(NumericLimits::min<T>());

    for (auto it(points.cbegin()); it != points.cend(); ++it)
    {
      const Matrix<T, 2, 1> point(*it);

      if (point(0) < xMin)
      {
        xMin = point(0);
      }

      if (point(0) > xMax)
      {
        xMax = point(0);
      }

      if (point(1) < yMin)
      {
        yMin = point(1);
      }

      if (point(1) > yMax)
      {
        yMax = point(1);
      }
    }

    boundingBox <<
      xMin, xMax,
      yMin, yMax;

    return boundingBox;
  }
}


#endif // LINEARPROGRAMMINGUTILS_TXX
