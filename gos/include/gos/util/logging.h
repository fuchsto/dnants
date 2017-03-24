#ifndef GOS__UTIL__LOGGING_H__INCLUDED
#define GOS__UTIL__LOGGING_H__INCLUDED

#include <iostream>
#include <iomanip>


#define GOS__LOG_DEBUG(scope, mesg) \
  do { \
    std::clog << "[ DEBUG  ] " << std::setw(16) << std::left \
                               << scope << " | " <<  mesg << '\n'; \
  } while(0)


#endif // GOS__UTIL__LOGGING_H__INCLUDED
