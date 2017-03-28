#ifndef GOS__UTIL__TIMESTAMP_H__INCLUDED
#define GOS__UTIL__TIMESTAMP_H__INCLUDED

#include <chrono>


namespace gos {

typedef unsigned long long timestamp_t;

static inline timestamp_t timestamp_us() {
  auto ts_now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<
           std::chrono::milliseconds
         >(ts_now.time_since_epoch()).count();
}

static inline timestamp_t timestamp_ns() {
  auto ts_now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<
           std::chrono::nanoseconds
         >(ts_now.time_since_epoch()).count();
}

}

#endif // GOS__UTIL__TIMESTAMP_H__INCLUDED
