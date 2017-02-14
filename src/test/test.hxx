#pragma once

#ifndef TEST_HXX
#define TEST_HXX


#include <stdexcept>

#include "eigen3/Eigen/Core"

#include "../math/numerictypes.hxx"


namespace Test
{
  using Eigen::Matrix;
  using Eigen::Dynamic;
  using NumericTypes::rational_t;
  using NumericTypes::real_t;
  using std::logic_error;


  void testReal(
    const Matrix<real_t, 1, Dynamic>& c,
    const Matrix<real_t, Dynamic, Dynamic>& A,
    const Matrix<real_t, Dynamic, 1>& b,
    real_t F_ = 0
  ) throw(logic_error);

  void testRational(
    const Matrix<rational_t, 1, Dynamic>& c,
    const Matrix<rational_t, Dynamic, Dynamic>& A,
    const Matrix<rational_t, Dynamic, 1>& b,
    rational_t F_ = 0
  ) throw(logic_error);

  void testSolvers();

  void testUtilities();

  void testRREF();
}


#endif // TEST_HXX
