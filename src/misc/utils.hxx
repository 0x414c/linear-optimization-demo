#pragma once

#ifndef UTILS_HXX
#define UTILS_HXX


#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <QApplication>

#include "eigen3/Eigen/Core"
#include "cppformat/format.h"


namespace Utils
{
  template<typename T1, typename... TN>
  /**
   * @brief The AlwaysFalse struct
   */
  struct AlwaysFalse
  {
    enum : bool { value = false };
  };


  /**
   * @brief The OperationResult enum
   */
  enum struct OperationResult : int
  {
    Success = 0,
    Fail = 1,
    Nothing = 2
  };


  /**
   * @brief MathematicaFormat
   */
  const Eigen::IOFormat MathematicaFormat(
    Eigen::FullPrecision, 0, ", ", ",\n", "{", "}", "{", "}"
  );


  /**
   * @brief makeApplication
   * @param argc
   * @param argv
   * @return
   */
  QApplication* makeApplication(int& argc, char** argv);


  template<typename T>
  /**
   * @brief makeString
   * @param from
   * @return
   */
  std::string
  makeString(const std::vector<T>& from)
  {
    std::ostringstream oss;
    std::copy(
      from.begin(),
      from.end(),
      std::ostream_iterator<T>(oss, " ")
    );

    return oss.str();
  }


  template <typename... Args>
  /**
   * @brief printDebugLog
   * @param func
   * @param file
   * @param line
   * @param format
   * @param args
   */
  void
  printDebugLog(
    const char* const func, const char* const file, int line,
    const char* const format, const Args&... args
  )
  {
    fmt::print(std::cerr, "\n|*** {0}  {1}  {2}\n|    ", func, file, line);
    fmt::print(std::cerr, format, args...);
    fmt::print(std::cerr, "\n");
  }
}


#ifdef LP_WITH_DEBUG_LOG
#define LOG(fmt,...) \
do \
{ \
  Utils::printDebugLog(__FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
} while (false)
#else
#define LOG(fmt,...) do { } while (false)
#endif // LP_WITH_DEBUG_LOG


#endif // UTILS_HXX
