#ifndef GRAPHICALSOLVER2D_HXX
#define GRAPHICALSOLVER2D_HXX

#include "boost/optional.hpp"

#include "linearprogramdata.hxx"
#include "plotdata2d.hxx"
#include "../misc/utils.hxx"

using namespace boost;
using namespace Utils;

class GraphicalSolver2D
{
  public:
    GraphicalSolver2D() = delete;
    explicit GraphicalSolver2D(const LinearProgramData<real_t>& linearProgramData);

    optional<PlotData2D> solve();

  private:
    LinearProgramData<real_t> _linearProgramData;
};

#endif // GRAPHICALSOLVER2D_HXX
