project(project_dnants)

cmake_minimum_required (VERSION 2.8.11)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeExt)


if(POLICY CMP0054)
  cmake_policy(SET CMP0054 OLD)
endif(POLICY CMP0054)
if(POLICY CMP0053)
  cmake_policy(SET CMP0053 OLD)
endif(POLICY CMP0053)
if(POLICY CMP0007)
  cmake_policy(SET CMP0007 OLD)
endif(POLICY CMP0007)
if(POLICY CMP0003)
  cmake_policy(SET CMP0003 OLD)
endif(POLICY CMP0003)
if(POLICY CMP0060)
  cmake_policy(SET CMP0060 OLD)
endif(POLICY CMP0060)
if(POLICY CMP0004)
  cmake_policy(SET CMP0004 OLD)
endif(POLICY CMP0004)
if(POLICY CMP0016)
  cmake_policy(SET CMP0016 OLD)
endif(POLICY CMP0016)


## Build options
option(ENABLE_LOGGING
       "Specify whether logging should be enabled" off)

set(CMAKE_RULE_MESSAGES    ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

## Install path
if (INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX})
else()
  set(CMAKE_INSTALL_PREFIX "./install/")
endif()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

## Set compiler flags (depend on CMake options)
include(${CMAKE_SOURCE_DIR}/CMakeExt/CompilerFlags.cmake)

## Build results output directories:
# file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Base library
add_subdirectory(gos)

# Python binding shared library
add_subdirectory(pygos)

# Main application executable
add_subdirectory(dnants)

