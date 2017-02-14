#include "test.hxx"

#include <stdexcept>
#include <utility>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "../lp/simplexsolver.hxx"
#include "../lp/linearprogramdata.hxx"
#include "../lp/linearprogrammingutils.hxx"
#include "../lp/linearprogramsolution.hxx"
#include "../lp/solutiontype.hxx"
#include "../math/mathutils.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"


namespace Test
{
  using namespace boost;
  using namespace Eigen;
  using namespace LinearProgramming;
  using namespace LinearProgrammingUtils;
  using namespace NumericTypes;
  using namespace MathUtils;
  using namespace std;


  void
  testReal(
    const Matrix<real_t, 1, Dynamic>& c,
    const Matrix<real_t, Dynamic, Dynamic>& A,
    const Matrix<real_t, Dynamic, 1>& b,
    real_t F_
  ) throw(logic_error)
  {
    const LinearProgramData<real_t> lpData(c, A, b);

    SimplexSolver<real_t> solver;

    solver.setLinearProgramData(lpData);

    const pair<SolutionType, optional<LinearProgramSolution<real_t>>>
    sol(solver.solve());

    switch (sol.first) {
      case SolutionType::Optimal:
        LOG(
          "Solution: x* == {0}\nF* == {1}",
          (*sol.second).extremePoint,
          (*sol.second).extremeValue
        );
//        if (!isEqual<real_t>((*sol.second).extremeValue, F_))
//        {
//          throw logic_error("Test::testReal: assertion failed.");
//        }
        break;
      case SolutionType::Unbounded:
        LOG("Unbounded");
        break;
      case SolutionType::Infeasible:
        LOG("Infeasible");
        break;
      default:
        LOG("...");
        break;
    }
  }


  void
  testRational(
    const Matrix<rational_t, 1, Dynamic>& c,
    const Matrix<rational_t, Dynamic, Dynamic>& A,
    const Matrix<rational_t, Dynamic, 1>& b,
    rational_t F_
  ) throw(logic_error)
  {
    const LinearProgramData<rational_t> lpData(c, A, b);

    SimplexSolver<rational_t> solver;

    solver.setLinearProgramData(lpData);

    const pair<SolutionType, optional<LinearProgramSolution<rational_t>>>
    sol(solver.solve());

    switch (sol.first) {
      case SolutionType::Optimal:
        LOG(
          "Solution: x* == {0}\nF* == {1}",
          (*sol.second).extremePoint,
          (*sol.second).extremeValue
        );
//        if (!isEqual<rational_t>((*sol.second).extremeValue, F_))
//        {
//          throw logic_error("Test::testRational: assertion failed.");
//        }
        break;
      case SolutionType::Unbounded:
        LOG("Unbounded");
        break;
      case SolutionType::Infeasible:
        LOG("Infeasible");
        break;
      default:
        LOG("...");
        break;
    }
  }


  void
  testSolvers()
  {
    //Example 5
    {
      Matrix<real_t, 1, Dynamic> c(1, 4);
      c <<
        -2, -1, -3, -1;

      Matrix<real_t, Dynamic, Dynamic> A(2, 4);
      A <<
        1, 2, 5, -1,
        1, -1, -1, 2;

      Matrix<real_t, Dynamic, 1> b(2, 1);
      b <<
        4,
        1;

      testReal(c, A, b);
    }

    //Example 2.2 - (3/2, 0, 0, 1) -> -18
    {
      Matrix<rational_t, 1, Dynamic> c(1, 4);
      c <<
        -10, 5, 7, -3;

      Matrix<rational_t, Dynamic, Dynamic> A(3, 4);
      A <<
        -1, -2, 3, 3,
        1, 1, 7, 2,
        2, 2, 8, 1;

      Matrix<rational_t, Dynamic, 1> b(3, 1);
      b <<
        rational_t(3, 2),
        rational_t(7, 2),
        4;

//      testRational(c, A, b);
    }

    //Example 2.1 - (28, 108, 0, 62) -> -38
    {
      Matrix<real_t, 1, Dynamic> c(1, 4);
      c <<
        -3, 1, -2, -1;

      Matrix<real_t, Dynamic, Dynamic> A(3, 4);
      A <<
        2, -1, 4, 1,
        -3, 2, 1, -2,
        4, -1, 2, 0;

      Matrix<real_t, Dynamic, 1> b(3, 1);
      b <<
        10,
        8,
        4;

//      testReal(c, A, b);
    }

    //Example 4.5 - (7/3, 0, 0, 2/3) -> -3
    {
      Matrix<real_t, 1, Dynamic> c(1, 4);
      c <<
        -1, 5, 1, -1;

      Matrix<real_t, Dynamic, Dynamic> A(2, 4);
      A <<
        1, 3, 3, 1,
        -2, 0, -3, 1;

      Matrix<real_t, Dynamic, 1> b(2, 1);
      b <<
        3,
        -4;

//      testReal(c, A, b, -3);
    }

    //Example -1
    {
      Matrix<rational_t, 1, Dynamic> c(1, 4);
      c <<
        -10, 57, 9, 24;

      Matrix<rational_t, Dynamic, Dynamic> A(3, 4);
      A <<
        rational_t(1, 2), rational_t(-11, 2), rational_t(-5, 2), 9,
        rational_t(1, 2), rational_t(-3, 2), rational_t(-1, 2), 1,
        1, 0, 0, 0;

      Matrix<rational_t, Dynamic, 1> b(3, 1);
      b <<
        0,
        0,
        1;

//      testRational(c, A, b);
    }

    //Example -2
    {
      Matrix<real_t, 1, Dynamic> c(1, 4);
      c <<
        1, -7, -1, -2;

      Matrix<real_t, Dynamic, Dynamic> A(3, 4);
      A <<
        1, 1, 1, 1,
        .5, -5.5, -2.5, 9,
        .5, -1.5, -.5, 1;

      Matrix<real_t, Dynamic, 1> b(3, 1);
      b <<
        1,
        0,
        0;

//      testReal(c, A, b);
    }

    //Example -3
    {
      Matrix<real_t, 1, Dynamic> c(1, 2);
      c <<
        1, 1;

      Matrix<real_t, Dynamic, Dynamic> A(1, 2);
      A <<
        1, 0;

      Matrix<real_t, Dynamic, 1> b(1, 1);
      b <<
        1;

//      testReal(c, A, b);
    }

    //Example -4 - Infeasible
    {
      Matrix<real_t, 1, Dynamic> c(1, 2);
      c <<
        1, 0;

      Matrix<real_t, Dynamic, Dynamic> A(2, 2);
      A <<
        1e-8 / 2., .5,
        1e-3, .1;

      Matrix<real_t, Dynamic, 1> b(2, 1);
      b <<
        0,
        10;

//      testReal(c, A, b);
    }
  }


  void
  testUtilities()
  {
    const uint16_t maxIterations(25);
//    const Integer maxDenominator(1000000000);
    const integer_t maxDenominator(1000000);
    const real_t tolerance(1E-16);
    const real_t Pi(3.141592653589793238462643383279502884);

    {
      const real_t x(0.3);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.33);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.3333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.33333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.333333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.3333333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.33333333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.333333333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.3333333333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.33333333333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.333333333333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.3333333333333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.33333333333333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.333333333333333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(0.3333333333333333);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }

    {
      const real_t x(Pi);
      const auto rat (
        MathUtils::rationalize<integer_t, real_t>(
          x, tolerance, maxDenominator, maxIterations
        )
      );
      LOG("x={0}, rat={1}, x={2}", x, rat, rat.first / real_t(rat.second));
    }
  }


  void
  testRREF()
  {
    {
      Matrix<real_t, Dynamic, Dynamic> A(3, 4);
      A <<
        1, 2, -1, -4,
        2, 3, -1, -11,
        -2, 0, -3, 22;

      LOG("A==\n{0}", A);

      RREF<real_t> rref(reducedRowEchelonForm<real_t>(A));

      LOG("A^==\n{0}\nrank=={1}", rref.rref, rref.rank);
    }

    {
      Matrix<real_t, Dynamic, Dynamic> A(3, 4);
      A <<
        1, 2, -1, -4,
        2, 3, -1, -11,
        0, 0, 0, 0;

      LOG("A==\n{0}", A);

      RREF<real_t> rref(reducedRowEchelonForm<real_t>(A));

      LOG("A^==\n{0}\nrank=={1}", rref.rref, rref.rank);
    }

    {
      Matrix<real_t, Dynamic, Dynamic> A(3, 4);
      A <<
        1, 2, -1, -4,
        2, 3, -1, -11,
        2, 3, -1, -11;

      LOG("A==\n{0}", A);

      RREF<real_t> rref(reducedRowEchelonForm<real_t>(A));

      LOG("A^==\n{0}\nrank=={1}", rref.rref, rref.rank);
    }

    {
      Matrix<real_t, Dynamic, Dynamic> A(3, 4);
      A <<
        2, 3, -1, -11,
        2, 3, -1, -11,
        2, 3, -1, -11;

      LOG("A==\n{0}", A);

      RREF<real_t> rref(reducedRowEchelonForm<real_t>(A));

      LOG("A^==\n{0}\nrank=={1}", rref.rref, rref.rank);
    }

    {
      Matrix<real_t, Dynamic, Dynamic> A(3, 4);
      A <<
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0;

      LOG("A==\n{0}", A);

      RREF<real_t> rref(reducedRowEchelonForm<real_t>(A));

      LOG("A^==\n{0}\nrank=={1}", rref.rref, rref.rank);
    }

    {
      Matrix<real_t, Dynamic, Dynamic> A(3, 4);
      A <<
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0;

      LOG("A==\n{0}", A);

      RREF<real_t> rref(reducedRowEchelonForm<real_t>(A));

      LOG("A^==\n{0}\nrank=={1}", rref.rref, rref.rank);
    }
  }
}
