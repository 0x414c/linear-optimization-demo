﻿#pragma once

#ifndef SIMPLEXSOLVERCONTROLLER_HXX
#define SIMPLEXSOLVERCONTROLLER_HXX


#include <cstddef>
#include <cstdint>

#include <list>
#include <memory>
#include <stdexcept>
#include <utility>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"

#include "simplexsolver_fwd.hxx"
#include "linearprogramsolution.hxx"
#include "simplextableau_fwd.hxx"
#include "solutionphase.hxx"
#include "../math/numerictypes.hxx"
#include "../config.hxx"


namespace LinearProgramming
{
  using namespace Config::LinearProgramming;
  using boost::optional;
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using NumericTypes::real_t;
  using std::list;
  using std::make_shared;
  using std::shared_ptr;
  using std::out_of_range; // using std::length_error;
  using std::pair;


  template<typename T = real_t>
  class SimplexSolverController
  {
    public:
      SimplexSolverController() = default;


      SimplexSolverController(SimplexSolver<T>* const solver) :
        solver_(make_shared<SimplexSolver<T>>(solver))
      { }


      SimplexSolverController(
        const shared_ptr<SimplexSolver<T>>& solver
      ) :
        solver_(solver)
      { }


      const shared_ptr<SimplexSolver<T>>&
      solver() const
      {
        return solver_;
      }


      void
      setSolver(const shared_ptr<SimplexSolver<T>>& solver)
      {
        solver_ = solver;
        reset();
      }


      void
      reset()
      {
        solver_->reset();
        tableaus_.clear();
        hasNext_ = false;
        wasAdvanced_ = false;
      }


      void
      start()
      {
        if (isEmpty())
        {
          solver_->reset();
          tableaus_.push_back(
            std::move(
              SimplexTableau<T>::makePhaseOne(solver_->linearProgramData_)
            )
          );
          hasNext_ = true;
          wasAdvanced_ = true;
        }
      }


      const SimplexTableau<T>&
      current() const throw(out_of_range)
      {
        if (!isEmpty())
        {
          return tableaus_.back();
        }
        else
        {
          throw out_of_range("There is no current item. Generator is empty.");
        }
      }


      pair<SolutionType, optional<pair<DenseIndex, DenseIndex>>>
      pivot() const throw(out_of_range)
      {
        return solver_->computePivotIdx(current());
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
            solutionType = solver_->iterate(tableau, *pivotIdx);
          }
          else
          {
            solutionType = solver_->iterate(tableau);
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
                      if (iterationsCount() > 0) //TODO: [2;2] Workaround.
                      {
                        ++solver_->iterCount_;
                        tableaus_.push_back(
                          SimplexTableau<T>::makePhaseTwo(
                            solver_->linearProgramData(), tableau
                          )
                        );
                        hasNext_ = true;
                        wasAdvanced_ = true;
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
                tableaus_.push_back(std::move(tableau));
                hasNext_ = true;
                wasAdvanced_ = true;
              }
              break;

            case SolutionType::Unknown:
              goto end;

            end:
              hasNext_ = false;
              wasAdvanced_ = false;
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
          --solver_->iterCount_;
          tableaus_.pop_back();
          hasNext_ = true;
          wasAdvanced_ = true;
        }
        else
        {
          throw out_of_range("There is no previous item. Generator is empty.");
        }
      }


      bool
      hasNext() const
      {
        return hasNext_;
      }


      bool
      hasPrevious() const
      {
        return (tableaus_.size() >= 2);
      }


      bool
      stateChanged() const
      {
        return wasAdvanced_;
      }


      bool
      isEmpty() const
      {
        return tableaus_.empty();
      }


      uint16_t
      iterationsCount() const
      {
        return solver_->iterCount_;
      }


      size_t
      elementsCount() const
      {
        return tableaus_.size();
      }


    private:
      shared_ptr<SimplexSolver<T>> solver_;

      list<SimplexTableau<T>> tableaus_ = list<SimplexTableau<T>>(0);

      bool hasNext_ = false;

      bool wasAdvanced_ = false;
  };
}


#endif // SIMPLEXSOLVERCONTROLLER_HXX
