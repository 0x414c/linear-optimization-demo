#include "linearprogramdata.hxx"

#include "utils.hxx"

using namespace Utils;

template<typename T>
LinearProgramData<T>::LinearProgramData() {}

template
struct LinearProgramData<real_t>;

template
struct LinearProgramData<rat_t>;
