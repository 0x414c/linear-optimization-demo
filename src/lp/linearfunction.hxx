#ifndef LINEARFUNCTION_HXX
#define LINEARFUNCTION_HXX

#include "eigen3/Eigen/Dense"

#include "../math/numerictypes.hxx"
#include "../misc/eigenextensions.hxx"
#include "../misc/utils.hxx"

using namespace Eigen;
using namespace NumericTypes;
using namespace Utils;

/**
 * @brief The `LinearFunction' templated class
 * represents linear forms on a field w/ elements of type `T':
 *   F(X1, X2, ..., XN) := C1*X1 + C2 * X2 + ... + CN*XN == (C, X),
 * where X := <X1, ..., XN>, C := <C1, ..., CN>,
 * and operation (*, *) denotes the dot product.
 * This class is using `Eigen' library for internal
 * representation of function coefficents vector `C'.
 * The grad(F) defines normal vector
 *   X := <X1, ..., XN>
 * to the hyperplane that is defined by equation
 *   F(X1, ..., XN) == 0.
 * This vector defines direction in which
 * function value increases.
 * We can use this property to solve LP.
 * For reference see:
 * `https://en.wikipedia.org/wiki/Field_(mathematics)',
 * `https://en.wikipedia.org/wiki/Linear_form',
 * `https://en.wikipedia.org/wiki/Hyperplane'.
 */
template<typename T = Real, DenseIndex N = Dynamic>
class LinearFunction
{
  public:
    LinearFunction() = delete;

    explicit LinearFunction(const Matrix<T, 1, N>& coeffs) :
      _coeffs(coeffs)
    { }

    DenseIndex dim() const
    {
      return _coeffs.cols();
    }

    const Matrix<T, 1, N>& coeffs()
    {
      return _coeffs;
    }

    T coeffAt(DenseIndex coeffIdx) const
    {
      return _coeffs(coeffIdx);
    }

    T operator ()(const Matrix<T, N, 1>& args) const
    {
      return valueAt(args);
    }

    T valueAt(const Matrix<T, N, 1>& args) const
    {
      return _coeffs.dot(args);
    }

  private:
    Matrix<T, 1, N> _coeffs;
};

#endif // LINEARFUNCTION_HXX
