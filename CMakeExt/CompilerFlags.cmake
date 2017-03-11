
find_package(OpenMP)

set (CC_WARN_FLAG  "${CC_WARN_FLAG}  -Wall -Wextra -Wpedantic")
set (CXX_WARN_FLAG "${CXX_WARN_FLAG} -Wall -Wextra -Wpedantic")

set (CXX_STD_FLAG "--std=c++11"
     CACHE STRING "C++ compiler std flag")

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
  set (CXX_GDB_FLAG "-g"
       CACHE STRING "C++ compiler GDB debug symbols flag")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  set (CXX_GDB_FLAG "-ggdb3 -rdynamic"
       CACHE STRING "C++ compiler GDB debug symbols flag")
endif()

set (CXX_OMP_FLAG ${OpenMP_CXX_FLAGS})
set (CC_OMP_FLAG  ${OpenMP_CC_FLAGS})

