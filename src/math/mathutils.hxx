#ifndef MATHUTILS_HXX
#define MATHUTILS_HXX

#include <limits>
#include <utility>

#include "../misc/utils.hxx"

using namespace std;
using namespace Utils;

namespace MathUtils
{
  template<typename R = integer_t>
  /**
   * @brief rationalize
   * @param x
   * @param tolerance
   * @param maxIterations
   * @param maxDenominator
   * @return
   */
  pair<R, R> rationalize(real_t x, real_t tolerance = 1E-16,
                         uint16_t maxIterations = 22, R maxDenominator = numeric_limits<R>::max());
}

#include "mathutils.txx"

#endif // MATHUTILS_HXX
