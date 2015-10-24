#ifndef MATHUTILS_HXX
#define MATHUTILS_HXX

#include <limits>
#include <utility>

#include "../misc/utils.hxx"

using namespace std;
using namespace Utils;

namespace MathUtils
{
  pair<integer_t, integer_t> rationalize(real_t x,
                                         real_t eps = 1E-16,
                                         uint16_t maxIterations = 50,
                                         integer_t maxDenominator = numeric_limits<integer_t>::max());
}

#endif // MATHUTILS_HXX
