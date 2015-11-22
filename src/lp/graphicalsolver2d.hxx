#pragma once

#ifndef GRAPHICALSOLVER2D_HXX
#define GRAPHICALSOLVER2D_HXX


#include "boost/optional.hpp"

#include "linearprogramdata.hxx"
#include "plotdata2d.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using namespace boost;
  using namespace NumericTypes;


  template<typename T = Real>
  class GraphicalSolver2D
  {
    public:
      GraphicalSolver2D()/* = delete*/;

      explicit GraphicalSolver2D(const LinearProgramData<T>& other);
      explicit GraphicalSolver2D(LinearProgramData<T>&& other);

      void setLinearProgramData(const LinearProgramData<T>& other);
      void setLinearProgramData(LinearProgramData<T>&& other);

      optional<PlotData2D> solve() const;


    private:
      LinearProgramData<T> _linearProgramData = LinearProgramData<T>();
  };
}


#include "graphicalsolver2d.txx"


#endif // GRAPHICALSOLVER2D_HXX
