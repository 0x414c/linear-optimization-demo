#pragma once

#ifndef POINTREAL2D_HXX
#define POINTREAL2D_HXX


#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using namespace NumericTypes;


  struct PointReal2D
  {
    PointReal2D() = default;

    PointReal2D(Real x, Real y) :
      x(x), y(y)
    { }


    Real x;

    Real y;
  };
}


#endif // POINTREAL2D_HXX
