#ifndef GOS__UTIL__TIMESTAMP_H__INCLUDED
#define GOS__UTIL__TIMESTAMP_H__INCLUDED

#include <chrono>


namespace gos {

static unsigned long long timestamp_ns() {
  auto ts_now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<
           std::chrono::nanoseconds
         >(ts_now.time_since_epoch()).count();
}

}

#endif // GOS__UTIL__TIMESTAMP_H__INCLUDED
