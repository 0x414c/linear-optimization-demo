#pragma once

#ifndef LINEARPROGRAMMINGUTILS_TXX
#define LINEARPROGRAMMINGUTILS_TXX


#include "linearprogrammingutils.hxx"

#include <functional>
#include <iostream>
#include <utility>

#include <QDebug>
#include <QPointF>
#include <QVector>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "linearprogramdata.hxx"
#include "../math/mathutils.hxx"
#include "../misc/utils.hxx"


namespace LinearProgrammingUtils
{
  using namespace boost;
  using namespace Eigen;
  using namespace MathUtils;
  using namespace std;
  using namespace Utils;


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
      x.unaryExpr(std::ref(isGreaterThanOrEqualToZero<T>)).all() &&
      ((A * x).array() <= b.array()).all()
    );
  }


  template<typename T>
  /**
   * @brief findIntersection
   * Finds intersection point `x' of two lines
   * given as the following matrix equation
   *   Ax == b.
   * The formula is
   *   x := {{(A11*b0 - A01*b1) / (-A01*A10 + A00*A11)},
   *         {(A10*b0 - A00*b1) / ( A01*A10 - A00*A11)}}.
   * NOTE: This function handles only two-dimensional case.
   * @param coeffs 1 × 2 matrix `A'
   * @param RHS N × 1 matrix `b'
   * @return (optional) intersection point `x' as 2 × 1 column-vector.
   */
  optional<Matrix<T, 2, 1>>
  findIntersection(const Matrix<T, 2, 2>& A, const Matrix<T, 2, 1>& b)
  {
    Matrix<T, 2, 1> sol(2, 1);
    optional<Matrix<T, 2, 1>> ret;

    const T q((-A(0, 1) * A(1, 0) + A(0, 0) * A(1, 1))),
            s((A(0, 1) * A(1, 0) - A(0, 0) * A(1, 1)));

    if (!isEqualToZero<T>(q) && !isEqualToZero<T>(s))
    {
      const T p((A(1, 1) * b(0) - A(0, 1) * b(1))),
              r((A(1, 0) * b(0) - A(0, 0) * b(1)));

      sol <<
        p / q,
        r / s;

      ret = sol;
    }

    return ret;
  }


  template<typename T>
  /**
   * @brief rowReducedEchelonForm
   * Algorithm: Transforming a matrix to row canonical/reduced
   * row echelon form (RREF).
   * INPUT: n × m matrix A.
   * OUTPUT: n × m matrix A' in reduced row echelon form.
   * 1. Set j ← 1.
   * 2. For each row i from 1 to n do
   *  a. While column j has all zero elements, set j ← j + 1.
   *     If j > m return A'.
   *  b. If element a[i, j] is zero, then interchange row i
   *     with a row x > i that has a[x, j] ≠ 0.
   *  c. Divide each element of row i by a[i, j],
   *     thus making the pivot a[i, j] equal to one.
   *  d. For each row k from 1 to n, with k ≠ i, subtract row i
   *     multiplied by a[k, j] from row k.
   * 3. Return transformed matrix A'.
   * There are many, many variants of the above algorithm.
   * For example, in step 2a you could always select the largest
   * element of  the column as the pivot to help reduce rounding errors,
   * and you could combine steps 2c and 2d.
   * For the reference see:
   *  `http://www.di-mgt.com.au/matrixtransform.html';
   *  `http://www.millersville.edu/~bikenaga/linear-algebra/row-reduction/
   *   row-reduction.html'.
   * @param matrix Matrix A to reduce.
   * @return Row-reduced echelon form of A' along w/ rank of A'.
   */
  pair<Matrix<T, Dynamic, Dynamic>, DenseIndex>
  reducedRowEchelonForm(const Matrix<T, Dynamic, Dynamic>& A)
  {
    const DenseIndex n(A.rows());
    const DenseIndex m(A.cols());
    DenseIndex rank(0);

    Matrix<T, Dynamic, Dynamic> RREF(n, m);
    RREF << A;

    DenseIndex i(0), j(0);
    //1. Deal with each row i from 1 to n in turn, ...
    while (true)
    {
      //If we had reached the end
      if (i >= n || j >= m)
      {
        return make_pair(RREF, rank);
      }
      else
      {
        //3. Interchange rows, if necessary, so that the
        //pivot element A(i, j) is nonzero.
        DenseIndex x(i);
        DenseIndex xMax(i);
        T maxValue(0);
        bool isColNonZero(false);
        //... and work across the columns j from 1 to m
        //skipping any column of all zero entries.
        while (true)
        {
          if (isEqualToZero<T>(RREF(x, j)))
          {
            ++x;
          }
          else
          {
            if (absoluteValue<T>(RREF(x, j)) > absoluteValue<T>(maxValue))
            {
              xMax = x;
              maxValue = RREF(x, j);
            }
            isColNonZero = true;
            ++x;
          }

          if (x >= n)
          {
            if (isColNonZero)
            {
              x = xMax;

              break;
            }
            else
            {
              ++j;
              if (j >= m)
              {
                return make_pair(RREF, rank);
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
          RREF.row(x).swap(RREF.row(i));
        }

        //4. Make the pivot equal to 1 by dividing each element
        //in the pivot row by the value of the pivot.
        const T pivot(RREF(i, j));

//        LOG("A.row({0}) / pivot <=> [{1}] / {2}", i, A.row(i), pivot);

        RREF.row(i) /= pivot;

//        LOG("A.row({0}) = [{1}]", i, A.row(i));

        //5. Make all elements above and below the pivot equal to 0 by
        //subtracting a suitable multiple of the pivot row from each other row.
        const Matrix<T, 1, Dynamic> pivotRow(RREF.row(i));
        for (DenseIndex k(0); k < i; ++k)
        {
          const T factor(RREF(k, j));

//          LOG("A.row({0}) - factor * pivotRow <=> [{1}] - ({2} * [{3}])",
//              k, A.row(k), factor, pivotRow);

          RREF.row(k) -= factor * pivotRow;

//          LOG("A.row({0}) = [{1}]", k, A.row(k));
        }
        for (DenseIndex k(i + 1); k < n; ++k)
        {
          const T factor(RREF(k, j));

//          LOG("A.row({0}) - factor * pivotRow <=> [{1}] - ({2} * [{3}])",
//              k, A.row(k), factor, pivotRow);

          RREF.row(k) -= factor * pivotRow;

//          LOG("A.row({0}) = [{1}]", k, A.row(k));
        }

//        LOG("A = \n{0}", A.format(MathematicaFormat));

        //Jump to the next column
        ++j;
      }
      //Increase rank (we now have 1 as the leading element in the current row)
      ++rank;

      //Jump to the next row
      ++i;
    }
  }
}


#endif // LINEARPROGRAMMINGUTILS_TXX
