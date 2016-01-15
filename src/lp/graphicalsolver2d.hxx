#pragma once

#ifndef GRAPHICALSOLVER2D_HXX
#define GRAPHICALSOLVER2D_HXX


#include <utility>

#include "boost/optional.hpp"

#include "isolver.hxx"
#include "linearprogramdata.hxx"
#include "plotdata2d.hxx"
#include "solutiontype.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using boost::optional;
  using NumericTypes::Real;
  using std::pair;


  template<typename TCoeff = Real/*, DenseIndex TDim = 2*/>
  class GraphicalSolver2D :
    public ISolver<PlotData2D<TCoeff>>
  {
    public:
      GraphicalSolver2D() = default;

      explicit GraphicalSolver2D(const LinearProgramData<TCoeff>& other);
      explicit GraphicalSolver2D(LinearProgramData<TCoeff>&& other);

      void setLinearProgramData(const LinearProgramData<TCoeff>& other);
      void setLinearProgramData(LinearProgramData<TCoeff>&& other);

      virtual pair<SolutionType, optional<PlotData2D<TCoeff>>> solve() override;


    private:
      LinearProgramData<TCoeff> linearProgramData_;
  };
}


#include "graphicalsolver2d.txx"


#endif // GRAPHICALSOLVER2D_HXX
