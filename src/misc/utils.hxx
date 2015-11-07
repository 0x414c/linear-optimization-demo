#ifndef UTILS_HXX
#define UTILS_HXX

#include <iostream>

#include "eigen3/Eigen/Core"
#include "cppformat/format.h"

//TODO: ~ Put away all the usings from headers
//TODO: ~ Use nested namespaces for all the modules
namespace Utils
{
  template<typename T1, typename... TN>
  struct False
  {
    enum : bool { value = false };
  };

  enum struct OperationResult : int
  {
    Success = 0, Fail = 1, Nothing = 2
  };

  const Eigen::IOFormat MathematicaFormat(
    Eigen::FullPrecision, 0, ", ", ",\n", "{", "}", "{", "}"
  );

  template <typename... Args>
  void printDebugLog(const char* const func, const char* const file, int line,
                     const char* const format, const Args&... args)
  {
    fmt::print(std::cerr, "\n|*** {0}  {1}  {2}\n|    ", func, file, line);
    fmt::print(std::cerr, format, args...);
    fmt::print(std::cerr, "\n");
  }
}

#ifdef __WITH_DEBUG_LOG__
#define LOG(fmt,...) do \
{ \
  Utils::printDebugLog(__FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
} while (false)
#else
#define LOG(fmt,...) do { } while (false)
#endif // __WITH_DEBUG_LOG__

#endif // UTILS_HXX
