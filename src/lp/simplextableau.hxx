#ifndef SIMPLEXTABLEAU_HXX
#define SIMPLEXTABLEAU_HXX

#include <cstdlib>

#include <vector>

#include <QDebug>

#include "eigen3/Eigen/Core"

#include "linearprogramdata.hxx"
#include "../math/numerictypes.hxx"

namespace LinearProgramming
{
  using namespace Eigen;
  using namespace NumericTypes;
  using namespace std;

  /**
   * @brief The Phase enum
   */
  enum struct Phase : int { One = 1, Two = 2 };

  template<typename T = Real>
  /**
   * @brief The SimplexTableau class
   */
  class SimplexTableau
  {
    public:
      SimplexTableau(const SimplexTableau<T>& simplexTableau);
      SimplexTableau(SimplexTableau<T>&& simplexTableau);

      T& operator ()(DenseIndex rowIdx, DenseIndex colIdx);
      const T& operator ()(DenseIndex rowIdx, DenseIndex colIdx) const;

      Block<Matrix<T, Dynamic, Dynamic>>
//      Matrix<T, Dynamic, Dynamic>&
      entries();
      Block<const Matrix<T, Dynamic, Dynamic>>
//      const Matrix<T, Dynamic, Dynamic>&
      entries() const;

      typename
      Matrix<T, Dynamic, Dynamic>::RowXpr row(DenseIndex rowIdx);
      typename
      Matrix<T, Dynamic, Dynamic>::ConstRowXpr row(DenseIndex rowIdx) const;

      typename
      Matrix<T, Dynamic, Dynamic>::ColXpr col(DenseIndex colIdx);
      typename
      Matrix<T, Dynamic, Dynamic>::ConstColXpr col(DenseIndex colIdx) const;

      DenseIndex rows() const;

      DenseIndex cols() const;

      Phase phase() const;

      vector<DenseIndex>& basicVars();
      const vector<DenseIndex>& basicVars() const;

      vector<DenseIndex>& freeVars();
      const vector<DenseIndex>& freeVars() const;

      size_t basicVarsCount() const;

      size_t freeVarsCount() const;

      Matrix<T, 1, Dynamic> extremePoint() const;

      T extremeValue() const;

      static SimplexTableau<T> makePhase1(
        const LinearProgramData<T>& linearProgramData
      );

      static SimplexTableau<T> makePhase2(
        const LinearProgramData<T>& linearProgramData,
        const SimplexTableau<T>& phase1Tableau
      ) throw(invalid_argument);

//      std::ostream& operator<< (
//        std::ostream& os, const SimplexTableau<T>& tableau
//      );

    private:
      SimplexTableau()/* = delete*/;

      Phase _phase;
      vector<DenseIndex> _basicVars;
      vector<DenseIndex> _freeVars;
      Matrix<T, Dynamic, Dynamic> _entries;
  };
}

#include "simplextableau.txx"

#endif // SIMPLEXTABLEAU_HXX
