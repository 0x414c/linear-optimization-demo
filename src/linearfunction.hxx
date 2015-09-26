#ifndef LINEARFUNCTION_HXX
#define LINEARFUNCTION_HXX

#include <initializer_list>

#include "lib/eigen3/Eigen/Dense"

using namespace std;
using namespace Eigen;

/**
 * @brief The `LinearFunction' represents immutable templated
 * class for linear functions on a field w/ elements of type `T':
 *   F(X1, X2, ..., XN) := C1*X1 + C2 * X2 + ... + CN*XN == (C, X),
 * where X := <X1, ..., XN>, C := <C1, ..., CN>,
 * (*, *) denotes the dot product.
 * This class is using `Eigen' library for internal
 * representation of function coefficents vector C.
 * The grad(F) defines normal vector
 *   X := <X1, ..., XN>
 * to hyperplane that is defined by equation
 *   F(X1, ..., XN) == 0.
 * This vector defines direction in which function
 * value increases.
 * We can use this property to solve LP.
 * For reference see:
 * `https://en.wikipedia.org/wiki/Field_(mathematics)',
 * `https://en.wikipedia.org/wiki/Linear_form',
 * `https://en.wikipedia.org/wiki/Hyperplane'.
 */
template<typename T = double, DenseIndex _dim = 1>
class LinearFunction
{
  public:
    LinearFunction() : _coeffs(Matrix<T, 1, dim>()) {}

    LinearFunction(const Matrix<T, 1, _dim>& coeffs) : _coeffs(coeffs) {}

    LinearFunction(initializer_list<T> initList) : _coeffs(initList) {}

    DenseIndex dim() {
      return _dim;
    }

    Matrix<T, 1, _dim>& coeffs() const {
      return _coeffs;
    }

    T coeffAt(DenseIndex coeffIdx) const {
      return _coeffs[coeffIdx];
    }

    T evaluateAt(const Matrix<T, 1, _dim>& args) {
      return _coeffs.dot(args);
//      T value = T(0);
//      for (int i = 0; i < _dim; ++i) { value += _coeffs(i) * args(i); }
//      return value;
    }

  private:
    Matrix<T, 1, _dim> _coeffs;
};

#endif // LINEARFUNCTION_HXX
