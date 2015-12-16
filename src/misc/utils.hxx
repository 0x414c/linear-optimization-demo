#pragma once

#ifndef UTILS_HXX
#define UTILS_HXX


#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include <QApplication>

#include "eigen3/Eigen/Core"
#include "cppformat/format.h"
#include "prettyprint/prettyprint.hpp"


namespace Utils
{
  using fmt::print;
  using std::cerr;
  using std::copy;
  using std::list;
  using std::ostream_iterator;
  using std::ostringstream;
  using std::string;
  using std::vector;


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
  enum struct ResultType : int
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
  [[deprecated("Use `cxx-prettyprint'!")]]
  string
  makeString(const list<T>& from)
  {
    ostringstream oss;

    oss << '[';

    copy(
      from.cbegin(),
      from.cend(),
      ostream_iterator<T>(oss, " ")
    );

    oss << ']';

    return oss.str();
  }


  template<typename T>
  /**
   * @brief makeString
   * @param from
   * @return
   */
  [[deprecated("Use `cxx-prettyprint'!")]]
  string
  makeString(const vector<T>& from)
  {
    ostringstream oss;

    oss << '[';

    copy(
      from.cbegin(),
      from.cend(),
      ostream_iterator<T>(oss, " ")
    );

    oss << ']';

    return oss.str();
  }


  template <typename... Args>
  /**
   * @brief printDebugLog
   * TODO: ~? Use `Q_FUNC_INFO' or
   *   `BOOST_CURRENT_FUNCTION' (<boost/current_function.hpp>).
   * TODO: ~? Use `std::clog'.
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
    print(cerr, "\n|*** {0}  {1}  {2}\n|    ", func, file, line);
    print(cerr, format, args...);
    print(cerr, "\n");
  }
}


#ifdef LP_WITH_DEBUG_LOG
#define LOG(fmt,...) \
do \
{ \
  Utils::printDebugLog(__FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
} \
while (false)
#else
#define LOG(fmt,...) do { } while (false)
#endif // LP_WITH_DEBUG_LOG


#endif // UTILS_HXX
