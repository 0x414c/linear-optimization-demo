#pragma once

#ifndef GRAPHICALSOLVER2D_HXX
#define GRAPHICALSOLVER2D_HXX


#include <utility>

#include "boost/optional.hpp"

#include "inumericsolver.hxx"
#include "linearprogramdata.hxx"
#include "plotdata2d.hxx"
#include "solutiontype.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using namespace boost;
  using namespace NumericTypes;


  template<typename T = Real>
  class GraphicalSolver2D :
    public INumericSolver<PlotData2D>
  {
    public:
      GraphicalSolver2D() = default;

      explicit GraphicalSolver2D(const LinearProgramData<T>& other);
      explicit GraphicalSolver2D(LinearProgramData<T>&& other);

      void setLinearProgramData(const LinearProgramData<T>& other);
      void setLinearProgramData(LinearProgramData<T>&& other);

      virtual pair<SolutionType, optional<PlotData2D>> solve() override;


    private:
      LinearProgramData<T> _linearProgramData;
  };
}


#include "graphicalsolver2d.txx"


#endif // GRAPHICALSOLVER2D_HXX
