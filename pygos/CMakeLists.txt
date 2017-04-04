project(project_dnants_pygos CXX)

include(${CMAKE_SOURCE_DIR}/CMakeExt/InstallFiles.cmake)

# -- Dependencies --------------------------------------
find_package(Pybind11 REQUIRED)
find_package(Python   REQUIRED)

# add_subdirectory(project_dnants_libgos)

# -- Target --------------------------------------------

set(DNANTS_PYGOS_TARGET_NAME pygos)


file(GLOB_RECURSE DNANTS_PYGOS_SOURCES
     "src/*.c" "src/*.h" "src/*.cc" "src/*.cpp" )
file(GLOB_RECURSE DNANTS_PYGOS_HEADERS
     "include/*.h")

set(DNANTS_GOS_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/gos/include)


set(DNANTS_PYGOS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DNANTS_PYGOS_SOURCE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(
  ${DNANTS_PYGOS_INCLUDE_DIRS}
  ${DNANTS_GOS_INCLUDE_DIRS}
  ${PYBIND11_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
)

add_library(
  ${DNANTS_PYGOS_TARGET_NAME}
  SHARED
  ${DNANTS_PYGOS_HEADERS}
  ${DNANTS_PYGOS_SOURCES}
  ${DNANTS_GOS_HEADERS}
)

target_link_libraries(
  ${DNANTS_PYGOS_TARGET_NAME}
  gos
  ${PYTHON_LIBRARY}
)

set_target_properties(
  ${exampletarget} PROPERTIES
  COMPILE_FLAGS
  "${VARIANT_ADDITIONAL_COMPILE_FLAGS} -Wno-unused"
)
set_target_properties(
  ${exampletarget} PROPERTIES
  CXX_STANDARD 11
)
set_target_properties(
  ${exampletarget} PROPERTIES
  CXX_STANDARD_REQUIRED 11
)

install(
  TARGETS ${DNANTS_PYGOS_TARGET_NAME}
  DESTINATION lib/)
install(
  DIRECTORY include/
  DESTINATION include)

