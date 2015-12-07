#pragma once

#ifndef DANTZIGNUMERICSOLVERCONTROLLER_HXX
#define DANTZIGNUMERICSOLVERCONTROLLER_HXX


#include <cstdint>

#include <list>
#include <memory>
#include <stdexcept>
#include <utility>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "dantzignumericsolver_fwd.hxx"
#include "linearprogramsolution.hxx"
#include "simplextableau_fwd.hxx"
#include "solutionphase.hxx"
#include "../math/numerictypes.hxx"
#include "../config.hxx"


namespace LinearProgramming
{
  using namespace boost;
  using namespace Config::LinearProgramming;
  using namespace Eigen;
  using namespace NumericTypes;
  using namespace std;

  template<class T>
  using shared_ptr = std::shared_ptr<T>;


  template<typename T = Real>
  class DantzigNumericSolverController
  {
    public:
      DantzigNumericSolverController() = default;


      DantzigNumericSolverController(DantzigNumericSolver<T>* const solver) :
        _solver(make_shared<DantzigNumericSolver<T>>(solver))
      { }


      DantzigNumericSolverController(
        const shared_ptr<DantzigNumericSolver<T>>& solver
      ) :
        _solver(solver)
      { }


      const shared_ptr<DantzigNumericSolver<T>>&
      solver() const
      {
        return _solver;
      }


      void
      setSolver(const shared_ptr<DantzigNumericSolver<T>>& solver)
      {
        _solver = solver;
        reset();
      }


      void
      reset()
      {
        _solver->reset();
        _tableausList.clear();
        _hasNext = false;
        _wasAdvanced = false;
      }


      void
      start()
      {
        if (isEmpty())
        {
          _solver->reset();
          _tableausList.push_back(
            std::move(
              SimplexTableau<T>::makePhaseOne(_solver->_linearProgramData)
            )
          );
          _hasNext = true;
          _wasAdvanced = true;
        }
      }


      const SimplexTableau<T>&
      current() const throw(out_of_range)
      {
        if (!isEmpty())
        {
          return _tableausList.back();
        }
        else
        {
          throw out_of_range("There is no current item. Generator is empty.");
        }
      }


      pair<SolutionType, optional<pair<DenseIndex, DenseIndex>>>
      pivot() const throw(out_of_range)
      {
        return _solver->computePivotIdx(current());
      }


      void
      next(optional<pair<DenseIndex, DenseIndex>> pivotIdx) throw(out_of_range)
      {
        if (hasNext())
        {
          SimplexTableau<T> tableau(current());
          SolutionType solutionType;

          if (pivotIdx)
          {
            solutionType = _solver->iterate(tableau, *pivotIdx);
          }
          else
          {
            solutionType = _solver->iterate(tableau);
          }

          switch (solutionType)
          {
            case SolutionType::Optimal:
              {
                const SolutionPhase phase(tableau.phase());
                switch (phase)
                {
                  case SolutionPhase::One:
                    {
                      if (iterationsCount() > 0)
                      {
                        ++_solver->_iterCount;
                        _tableausList.push_back(
                          SimplexTableau<T>::makePhaseTwo(
                            _solver->linearProgramData(), tableau
                          )
                        );
                        _hasNext = true;
                        _wasAdvanced = true;
                      }
                      else
                      {
                        goto end;
                      }
                    }
                    break;

                  case SolutionPhase::Two:
                    goto end;

                  default:
                    break;
                }
              }
              break;

            case SolutionType::Unbounded:
              goto end;

            case SolutionType::Infeasible:
              goto end;

            case SolutionType::Incomplete:
              {
                _tableausList.push_back(std::move(tableau));
                _hasNext = true;
                _wasAdvanced = true;
              }
              break;

            case SolutionType::Unknown:
              goto end;

            end:
              _hasNext = false;
              _wasAdvanced = false;
              break;

            default:
              break;
          }
        }
        else
        {
          throw out_of_range("There is no next item. Generator is exhausted.");
        }
      }


      void
      previous() throw(out_of_range)
      {
        if (hasPrevious())
        {
          --_solver->_iterCount;
          _tableausList.pop_back();
          _hasNext = true;
          _wasAdvanced = true;
        }
        else
        {
          throw out_of_range("There is no previous item. Generator is empty.");
        }
      }


      bool
      hasNext() const
      {
        return _hasNext;
      }


      bool
      hasPrevious() const
      {
        return (_tableausList.size() >= 2);
      }


      bool
      stateChanged() const
      {
        return _wasAdvanced;
      }


      bool
      isEmpty() const
      {
        return _tableausList.empty();
      }


      uint16_t
      iterationsCount() const
      {
        return _solver->_iterCount;
      }


      size_t
      elementsCount() const
      {
        return _tableausList.size();
      }


    private:
      shared_ptr<DantzigNumericSolver<T>> _solver;

      list<SimplexTableau<T>> _tableausList = list<SimplexTableau<T>>(0);

      bool _hasNext = false;

      bool _wasAdvanced = false;
  };
}


#endif // DANTZIGNUMERICSOLVERCONTROLLER_HXX
