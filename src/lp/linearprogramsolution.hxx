#ifndef LINEARPROGRAMSOLUTION_HXX
#define LINEARPROGRAMSOLUTION_HXX

#include "eigen3/Eigen/Dense"

#include "../misc/utils.hxx"

using namespace Eigen;
using namespace Utils;

/**
 * @brief The ResultType enum
 */
enum struct ResultType : int { Unknown = 1, Inconsistent = 2, Unbounded = 4, Optimal = 8 };

template<typename T = real_t>
/**
 * @brief The Solution struct
 */
struct Solution
{
    Solution() = delete;
    Solution(ResultType resultType, const Matrix<T, 1, Dynamic>& extremeVertex, const T& extremeValue) :
      resultType(resultType), extremeVertex(extremeVertex), extremeValue(extremeValue) {}

    ResultType resultType;
    Matrix<T, 1, Dynamic> extremeVertex;
    T extremeValue;
};

#endif // LINEARPROGRAMSOLUTION_HXX
