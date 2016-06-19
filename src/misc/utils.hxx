#pragma once

#ifndef UTILS_HXX
#define UTILS_HXX


#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <QApplication>

#include "cxx-prettyprint/prettyprint.hpp"
#include "eigen3/Eigen/Core"
#include "fmt/format.h"
#include "fmt/ostream.h"


namespace Utils
{
  using fmt::print;
  using std::cerr;
  using std::copy;
  using std::false_type;
  using std::list;
  using std::ostream_iterator;
  using std::ostringstream;
  using std::string;
  using std::vector;


  template<typename...>
  /**
   * @brief The AlwaysFalse struct
   * Acts as a guard for templates which used to prevent
   * arbitrary template instantiations allowing defined
   * specializations only.
   */
  struct AlwaysFalse :
    false_type
  { };


  /**
   * @brief The ResultType enum
   */
  enum struct ResultType : int
  {
    Success = 0,
    Nothing = 1,
    Fail = 2
  };


  /**
   * @brief MathematicaFormat
   * NOTE: For debugging purposes.
   */
  const Eigen::IOFormat MathematicaFormat(
    Eigen::FullPrecision, 0,
    ", ", ",\n", "{", "}", "{", "}"
  );


  /**
   * @brief RowVectorFormat
   * NOTE: For printing.
   */
  const Eigen::IOFormat RowVectorFormat(
    Eigen::StreamPrecision, Eigen::DontAlignCols,
    ", ", "", "(", ")", "", ""
  );


  /**
   * @brief ColVectorFormat
   * NOTE: For printing.
   */
  const Eigen::IOFormat ColVectorFormat(
    Eigen::StreamPrecision, Eigen::DontAlignCols,
    "", ", ", "", "", "(", ")"
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
   *   `BOOST_CURRENT_FUNCTION' (<boost/current_function.hpp>)
   * TODO: ~? Use `std::clog'
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
#else // LP_WITH_DEBUG_LOG
#define LOG(fmt,...) do { } while (false)
#endif // LP_WITH_DEBUG_LOG


#endif // UTILS_HXX
