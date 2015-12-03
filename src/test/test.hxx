﻿#ifndef TEST_HXX
#define TEST_HXX


#include <stdexcept>

#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"


namespace Test {
  using namespace Eigen;
  using namespace NumericTypes;
  using namespace std;


  void testReal(
    const Matrix<Real, 1, Dynamic>& c,
    const Matrix<Real, Dynamic, Dynamic>& A,
    const Matrix<Real, Dynamic, 1>& b,
    Real F_ = 0
  ) throw(logic_error);

  void testRational(
    const Matrix<Rational, 1, Dynamic>& c,
    const Matrix<Rational, Dynamic, Dynamic>& A,
    const Matrix<Rational, Dynamic, 1>& b,
    Rational F_ = 0
  ) throw(logic_error);

  void run();
}


#endif // TEST_HXX