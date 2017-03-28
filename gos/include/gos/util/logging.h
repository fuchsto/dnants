#ifndef GOS__UTIL__LOGGING_H__INCLUDED
#define GOS__UTIL__LOGGING_H__INCLUDED

#include <iostream>
#include <iomanip>

#if defined(GOS_DEBUG)
#  define GOS__LOG__LEVEL(level, scope, mesg) \
     do { \
       std::clog << "[ " \
                 << std::setw(7)  << std::left << level \
                 << " ] " \
                 << std::setw(16) << std::left << scope \
                 << " | " <<  mesg << '\n'; \
     } while(0)
#else
#  define GOS__LOG__LEVEL(level,scope,mesg) do { } while(0)
#endif


#define GOS__LOG_DEBUG(scope, mesg) GOS__LOG__LEVEL("DEBUG", scope, mesg)
#define GOS__LOG_WARN(scope, mesg)  GOS__LOG__LEVEL("WARN",  scope, mesg)
#define GOS__LOG_ERROR(scope, mesg) GOS__LOG__LEVEL("ERROR", scope, mesg)


#endif // GOS__UTIL__LOGGING_H__INCLUDED
