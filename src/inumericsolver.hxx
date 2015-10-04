#ifndef INUMERICSOLVER_HXX
#define INUMERICSOLVER_HXX

class INumericSolver
{
  public:
    virtual ~INumericSolver() = 0;

    virtual void solve() = 0;
};

#endif // INUMERICSOLVER_HXX
