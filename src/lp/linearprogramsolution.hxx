#ifndef LINEARPROGRAMSOLUTION_HXX
#define LINEARPROGRAMSOLUTION_HXX

#include "eigen3/Eigen/Dense"

#include "../misc/utils.hxx"
#include "../math/numerictypes.hxx"

using namespace Eigen;
using namespace NumericTypes;
using namespace Utils;

/**
 * @brief The SolutionType enum
 * TODO: ~ Move to its own namespace
 */
enum struct SolutionType : int
{
  Optimal = 0,
  Unbounded = 1,
  Incomplete = 2,
  Inconsistent = 4,
  Unknown = 8
};

template<typename T = Real>
/**
 * @brief The Solution struct
 */
struct LinearProgramSolution
{
  LinearProgramSolution()/* = delete*/;
  LinearProgramSolution(SolutionType resultType,
                        const Matrix<T, 1, Dynamic>& extremePoint,
                        const T& extremeValue);

  SolutionType resultType = SolutionType::Unknown;
  Matrix<T, 1, Dynamic> extremePoint/* = Matrix<T, 1, Dynamic>::Zero(1, 1)*/;
  T extremeValue = T(0);
};

#include "linearprogramsolution.txx"

#endif // LINEARPROGRAMSOLUTION_HXX
