project(project_dnants_libgos CXX)

include(${CMAKE_SOURCE_DIR}/CMakeExt/InstallFiles.cmake)

# -- Dependencies --------------------------------------
find_package(Pybind11 REQUIRED)
find_package(Python   REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG}   ${PYTHON_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} ${PYTHON_FLAGS}")

# -- Target --------------------------------------------

set(GOS_LIBRARY gos)

set(DNANTS_LIBGOS_TARGET_NAME ${GOS_LIBRARY})


file(GLOB_RECURSE DNANTS_LIBGOS_SOURCES
     "src/*.c" "src/*.h" "src/*.cc" "src/*.cpp" )
file(GLOB_RECURSE DNANTS_LIBGOS_HEADERS
     "include/*.h")

# Test
file(GLOB_RECURSE DNANTS_PYGOS_SOURCES
     "../pygos/src/*.c" "../pygos/src/*.h" "../pygos/src/*.cc" "../pygos/src/*.cpp" )
file(GLOB_RECURSE DNANTS_PYGOS_HEADERS
     "../pygos/include/*.h")

set(DNANTS_PYGOS_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/pygos/include)


add_library(
  ${DNANTS_LIBGOS_TARGET_NAME}
# SHARED
  ${DNANTS_LIBGOS_HEADERS}
  ${DNANTS_LIBGOS_SOURCES}
)

set(DNANTS_LIBGOS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DNANTS_LIBGOS_SOURCE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

# include_directories(
#   ${DNANTS_LIBGOS_INCLUDE_DIRS}
# )
target_include_directories(
  ${DNANTS_LIBGOS_TARGET_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE
  ${DNANTS_LIBGOS_SOURCE_DIRS}
  ${PYBIND11_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
)

target_link_libraries(
  ${DNANTS_LIBGOS_TARGET_NAME}
  ${PYTHON_LIBRARY}
)

set_target_properties(
  ${DNANTS_LIBGOS_TARGET_NAME}
  PROPERTIES 
  CXX_STANDARD 11
)
set_target_properties(
  ${DNANTS_LIBGOS_TARGET_NAME}
  PROPERTIES
  CXX_STANDARD_REQUIRED 11
)

install(
  TARGETS ${DNANTS_LIBGOS_TARGET_NAME}
  EXPORT  libgosConfig
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)
install(
  DIRECTORY include/
  DESTINATION include
)
install(
  EXPORT libgosConfig
  DESTINATION share/libgos/cmake
)

export(
  TARGETS ${DNANTS_LIBGOS_TARGET_NAME}
  FILE libgosConfig.cmake
)

