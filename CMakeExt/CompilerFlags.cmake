
find_package(OpenMP)

set (CC_WARN_FLAG  "${CC_WARN_FLAG}  -Wall -Wpedantic -Wno-unused-function")
set (CXX_WARN_FLAG "${CXX_WARN_FLAG} -Wall -Wpedantic -Wno-unused-function")

set (CXX_STD_FLAG "--std=c++11"
     CACHE STRING "C++ compiler std flag")

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
  set (CXX_GDB_FLAG "-g"
       CACHE STRING "C++ compiler GDB debug symbols flag")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  set (CXX_GDB_FLAG "-ggdb3 -rdynamic"
       CACHE STRING "C++ compiler GDB debug symbols flag")
endif()

# Only works on unix systems:

execute_process(COMMAND python3.5-config --cflags
                OUTPUT_VARIABLE PYTHON_CFLAGS
                OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND python3.5-config --ldflags
                OUTPUT_VARIABLE PYTHON_LDFLAGS
                OUTPUT_STRIP_TRAILING_WHITESPACE)

set (PYTHON_FLAGS "${PYTHON_CFLAGS} ${PYTHON_LDFLAGS}")

set (CXX_OMP_FLAG ${OpenMP_CXX_FLAGS})
set (CC_OMP_FLAG  ${OpenMP_CC_FLAGS})

set (SDL2_FLAGS "-D_REENTRANT")

set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -Ofast ${CXX_ENV_SETUP_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG}   ${SDL2_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} ${SDL2_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_STD_FLAG} ${CXX_OMP_FLAG}")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} ${CXX_WARN_FLAG}")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -DGOS_DEBUG ${CXX_GDB_FLAG}")

set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} ${SDL2_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} ${CC_STD_FLAG} ${CC_OMP_FLAG}")
set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} ${CC_WARN_FLAG}")
set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} -DGOS_DEBUG ${CC_GDB_FLAG}")

set(CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE} -Ofast")
set(CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE} ${SDL2_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE} ${CC_STD_FLAG} ${CC_OMP_FLAG}")
set(CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE} ${CC_WARN_FLAG} -Ofast")

set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_STD_FLAG} ${CXX_OMP_FLAG}")
set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} ${CXX_WARN_FLAG} -Ofast")

message(STATUS "Python flags:        ${PYTHON_FLAGS}")

message(STATUS "CC  flags (Debug):   ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "CXX flags (Debug):   ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CC  flags (Release): ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "CXX flags (Release): ${CMAKE_CXX_FLAGS_RELEASE}")

