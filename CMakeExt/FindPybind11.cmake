
find_path(
  PYBIND11_PREFIX
  NAMES include/pybind11.h
)

if(NOT PYBIND11_PREFIX AND NOT $ENV{PYBIND11_BASE} STREQUAL "")
	set(PYBIND11_PREFIX $ENV{PYBIND11_BASE})
endif()

message(STATUS "Searching for pybind11 in path " ${PYBIND11_PREFIX})

find_path(
  PYBIND11_INCLUDE_DIRS
  NAMES pybind11/pybind11.h
  HINTS ${PYBIND11_PREFIX}/include
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  PYBIND11 DEFAULT_MSG
  PYBIND11_INCLUDE_DIRS
)

mark_as_advanced(
  PYBIND11_PREFIX_DIRS
  PYBIND11_INCLUDE_DIRS
)

if (PYBIND11_FOUND)
  message(STATUS "pybind11 includes:  " ${PYBIND11_INCLUDE_DIRS})
endif()

