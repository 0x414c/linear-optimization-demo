#include "test.hxx"

#include <stdexcept>
#include <utility>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "../lp/simplexsolver.hxx"
#include "../lp/linearprogramdata.hxx"
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
  using namespace NumericTypes;
  using namespace MathUtils;
  using namespace std;


  void
  testReal(
    const Matrix<Real, 1, Dynamic>& c,
    const Matrix<Real, Dynamic, Dynamic>& A,
    const Matrix<Real, Dynamic, 1>& b,
    Real F_
  ) throw(logic_error)
  {
    const LinearProgramData<Real> lpData(c, A, b);

    SimplexSolver<Real> solver;

    solver.setLinearProgramData(lpData);

    const pair<SolutionType, optional<LinearProgramSolution<Real>>>
    sol(solver.solve());

    switch (sol.first) {
      case SolutionType::Optimal:
        LOG(
          "Solution: x* == {0}\nF* == {1}",
          (*sol.second).extremePoint,
          (*sol.second).extremeValue
        );
//        if (!isEqual<Real>((*sol.second).extremeValue, F_))
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
    const Matrix<Rational, 1, Dynamic>& c,
    const Matrix<Rational, Dynamic, Dynamic>& A,
    const Matrix<Rational, Dynamic, 1>& b,
    Rational F_
  ) throw(logic_error)
  {
    const LinearProgramData<Rational> lpData(c, A, b);

    SimplexSolver<Rational> solver;

    solver.setLinearProgramData(lpData);

    const pair<SolutionType, optional<LinearProgramSolution<Rational>>>
    sol(solver.solve());

    switch (sol.first) {
      case SolutionType::Optimal:
        LOG(
          "Solution: x* == {0}\nF* == {1}",
          (*sol.second).extremePoint,
          (*sol.second).extremeValue
        );
//        if (!isEqual<Rational>((*sol.second).extremeValue, F_))
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
  run()
  {
    //Example 5
    {
      Matrix<Real, 1, Dynamic> c(1, 4);
      c <<
        -2, -1, -3, -1;

      Matrix<Real, Dynamic, Dynamic> A(2, 4);
      A <<
        1, 2, 5, -1,
        1, -1, -1, 2;

      Matrix<Real, Dynamic, 1> b(2, 1);
      b <<
        4,
        1;

//      testReal(c, A, b);
    }

    //Example 2.2 - (3/2, 0, 0, 1) -> -18
    {
      Matrix<Rational, 1, Dynamic> c(1, 4);
      c <<
        -10, 5, 7, -3;

      Matrix<Rational, Dynamic, Dynamic> A(3, 4);
      A <<
        -1, -2, 3, 3,
        1, 1, 7, 2,
        2, 2, 8, 1;

      Matrix<Rational, Dynamic, 1> b(3, 1);
      b <<
        Rational(3, 2),
        Rational(7, 2),
        4;

//      testRational(c, A, b);
    }

    //Example 2.1 - (28, 108, 0, 62) -> -38
    {
      Matrix<Real, 1, Dynamic> c(1, 4);
      c <<
        -3, 1, -2, -1;

      Matrix<Real, Dynamic, Dynamic> A(3, 4);
      A <<
        2, -1, 4, 1,
        -3, 2, 1, -2,
        4, -1, 2, 0;

      Matrix<Real, Dynamic, 1> b(3, 1);
      b <<
        10,
        8,
        4;

//      testReal(c, A, b);
    }

    //Example 4.5 - (7/3, 0, 0, 2/3) -> -3
    {
      Matrix<Real, 1, Dynamic> c(1, 4);
      c <<
        -1, 5, 1, -1;

      Matrix<Real, Dynamic, Dynamic> A(2, 4);
      A <<
        1, 3, 3, 1,
        -2, 0, -3, 1;

      Matrix<Real, Dynamic, 1> b(2, 1);
      b <<
        3,
        -4;

//      testReal(c, A, b, -3);
    }

    //Example -1
    {
      Matrix<Rational, 1, Dynamic> c(1, 4);
      c <<
        -10, 57, 9, 24;

      Matrix<Rational, Dynamic, Dynamic> A(3, 4);
      A <<
        Rational(1, 2), Rational(-11, 2), Rational(-5, 2), 9,
        Rational(1, 2), Rational(-3, 2), Rational(-1, 2), 1,
        1, 0, 0, 0;

      Matrix<Rational, Dynamic, 1> b(3, 1);
      b <<
        0,
        0,
        1;

//      testRational(c, A, b);
    }

    //Example -2
    {
      Matrix<Real, 1, Dynamic> c(1, 4);
      c <<
        1, -7, -1, -2;

      Matrix<Real, Dynamic, Dynamic> A(3, 4);
      A <<
        1, 1, 1, 1,
        .5, -5.5, -2.5, 9,
        .5, -1.5, -.5, 1;

      Matrix<Real, Dynamic, 1> b(3, 1);
      b <<
        1,
        0,
        0;

//      testReal(c, A, b);
    }

    //Example -3
    {
      Matrix<Real, 1, Dynamic> c(1, 2);
      c <<
        1, 1;

      Matrix<Real, Dynamic, Dynamic> A(1, 2);
      A <<
        1, 0;

      Matrix<Real, Dynamic, 1> b(1, 1);
      b <<
        1;

//      testReal(c, A, b);
    }

    //Example -4 - Infeasible
    {
      Matrix<Real, 1, Dynamic> c(1, 2);
      c <<
        1, 0;

      Matrix<Real, Dynamic, Dynamic> A(2, 2);
      A <<
        1e-8 / 2., .5,
        1e-3, .1;

      Matrix<Real, Dynamic, 1> b(2, 1);
      b <<
        0,
        10;

//      testReal(c, A, b);
    }
  }
}
