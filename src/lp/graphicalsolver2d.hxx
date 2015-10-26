#ifndef GRAPHICALSOLVER2D_HXX
#define GRAPHICALSOLVER2D_HXX

#include "boost/optional.hpp"

#include "linearprogramdata.hxx"
#include "plotdata2d.hxx"
#include "../misc/utils.hxx"

using namespace boost;
using namespace Utils;

template<typename T = real_t>
class GraphicalSolver2D
{
  public:
    GraphicalSolver2D() = delete;
    explicit GraphicalSolver2D(const LinearProgramData<T>& linearProgramData);

    optional<PlotData2D> solve();

  private:
    LinearProgramData<T> _linearProgramData;
};

#include "graphicalsolver2d.txx"

#endif // GRAPHICALSOLVER2D_HXX
