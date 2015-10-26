#ifndef DANTZIGNUMERICSOLVER_TXX
#define DANTZIGNUMERICSOLVER_TXX

#include "dantzignumericsolver.hxx"

#include <QDebug>

#include "linearprogramdata.hxx"
#include "../misc/utils.hxx"

using namespace Utils;

template<typename T>
DantzigNumericSolver<T>::DantzigNumericSolver() {}

template<typename T>
DantzigNumericSolver<T>::DantzigNumericSolver(const LinearProgramData<T>& linearProgramData) :
  _linearProgramData(linearProgramData) {}

template<typename T>
void DantzigNumericSolver<T>::solve()
{
  qDebug() << "DantzigNumericSolver<T>: solving...";
}

#endif // DANTZIGNUMERICSOLVER_TXX
