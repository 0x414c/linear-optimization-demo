#ifndef LINEARPROGRAMSOLUTION_HXX
#define LINEARPROGRAMSOLUTION_HXX

#include "eigen3/Eigen/Core"

#include "../misc/utils.hxx"
#include "../math/numerictypes.hxx"

namespace LinearProgramming
{
  using namespace Eigen;
  using namespace NumericTypes;
  using namespace Utils;

  /**
   * @brief The SolutionType enum
   */
  enum struct SolutionType : int
  {
    Optimal = 0,
    Unbounded = 1,
    Inconsistent = 2,
    Incomplete = 4,
    Unknown = 8
  };

  template<typename T = Real>
  /**
   * @brief The LinearProgramSolution struct
   */
  struct LinearProgramSolution
  {
    LinearProgramSolution()/* = delete*/;
    LinearProgramSolution(const LinearProgramSolution<T>& other);
    LinearProgramSolution(LinearProgramSolution<T>&& other);
    LinearProgramSolution(
      SolutionType solutionType,
      const Matrix<T, 1, Dynamic>& extremePoint,
      const T& extremeValue
    );
    LinearProgramSolution(
      SolutionType solutionType,
      Matrix<T, 1, Dynamic>&& extremePoint,
      const T& extremeValue
    );

    const LinearProgramSolution<T>& operator =(
      const LinearProgramSolution<T>& other
    );
    const LinearProgramSolution<T>& operator =(
      LinearProgramSolution<T>&& other
    );

    SolutionType solutionType = SolutionType::Unknown;
    Matrix<T, 1, Dynamic> extremePoint;
    T extremeValue = T(0);
  };
}

#include "linearprogramsolution.txx"

#endif // LINEARPROGRAMSOLUTION_HXX
