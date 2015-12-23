#pragma once

#ifndef GRAPHICALSOLVER2D_HXX
#define GRAPHICALSOLVER2D_HXX


#include <utility>

#include "boost/optional.hpp"

#include "inumericsolver.hxx"
#include "linearprogramdata.hxx"
#include "plotdatareal2d.hxx"
#include "solutiontype.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using boost::optional;
  using NumericTypes::Real;
  using std::pair;


  template<typename T = Real>
  class GraphicalSolver2D :
    public INumericSolver<PlotDataReal2D>
  {
    public:
      GraphicalSolver2D() = default;

      explicit GraphicalSolver2D(const LinearProgramData<T>& other);
      explicit GraphicalSolver2D(LinearProgramData<T>&& other);

      void setLinearProgramData(const LinearProgramData<T>& other);
      void setLinearProgramData(LinearProgramData<T>&& other);

      virtual pair<SolutionType, optional<PlotDataReal2D>> solve() override;


    private:
      LinearProgramData<T> linearProgramData_;
  };
}


#include "graphicalsolver2d.txx"


#endif // GRAPHICALSOLVER2D_HXX
