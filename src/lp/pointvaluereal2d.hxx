#pragma once

#ifndef POINTVALUEREAL2D_HXX
#define POINTVALUEREAL2D_HXX


#include "pointreal2d.hxx"
#include "../math/numerictypes.hxx"


namespace LinearProgramming
{
  using namespace NumericTypes;


  [[deprecated]]
  struct PointValueReal2D
  {
    PointValueReal2D() = default;

    PointValueReal2D(const PointReal2D& point, Real value) :
      point(point), value(value)
    { }


    PointReal2D point;

    Real value;
  };
}


#endif // POINTVALUEREAL2D_HXX
