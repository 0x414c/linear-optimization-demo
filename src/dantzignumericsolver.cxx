#include "dantzignumericsolver.hxx"

#include <QDebug>

#include "utils.hxx"

using namespace Utils;

template<typename T>
DantzigNumericSolver<T>::DantzigNumericSolver() {}

template<typename T>
void DantzigNumericSolver<T>::solve()
{
  qDebug() << "DantzigNumericSolver<T>: solving...";
}

template
class DantzigNumericSolver<real_t>;

template
class DantzigNumericSolver<rat_t>;
