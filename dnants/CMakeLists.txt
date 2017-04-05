project(project_dnants_main CXX)

include(${CMAKE_SOURCE_DIR}/CMakeExt/InstallFiles.cmake)

# -- Dependencies --------------------------------------
find_package(SDL2     REQUIRED)
find_package(Pybind11 REQUIRED)
find_package(Python   REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG}   ${PYTHON_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} ${PYTHON_FLAGS}")

# set(Boost_USE_STATIC_LIBS    ON)
# set(Boost_USE_MULTITHREADED  ON)
# set(Boost_USE_STATIC_RUNTIME ON)
#
# find_package(Boost 1.58.0 REQUIRED
#              COMPONENTS filesystem regex)
#
# message(STATUS "Boost include directories: "
#         ${Boost_INCLUDE_DIRS})
# message(STATUS "Boost libraries: "
#         ${Boost_LIBRARIES})

# -- Target --------------------------------------------

set(DNANTS_MAIN_TARGET_NAME dnants)



file(GLOB_RECURSE DNANTS_MAIN_SOURCES
     "src/*.c" "src/*.h" "src/*.cc" "src/*.cpp" )
file(GLOB_RECURSE DNANTS_MAIN_HEADERS
     "include/*.h")
file(GLOB_RECURSE DNANTS_MAIN_RESOURCES
     "res/*.")

set(DNANTS_GOS_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/gos/include)


set(DNANTS_MAIN_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DNANTS_MAIN_SOURCE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(
  ${DNANTS_MAIN_INCLUDE_DIRS}
  ${DNANTS_GOS_INCLUDE_DIRS}
  ${DNANTS_LIBGOS_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIR}
  # Test
  ${PYBIND11_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
)

add_executable(
  ${DNANTS_MAIN_TARGET_NAME}
  ${DNANTS_MAIN_HEADERS}
  ${DNANTS_MAIN_SOURCES}
)

target_link_libraries(
  ${DNANTS_MAIN_TARGET_NAME}
# pygos
  gos
  ${SDL2_LIBRARY}
# Test
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
  TARGETS ${DNANTS_MAIN_TARGET_NAME}
  DESTINATION bin)

install(
  DIRECTORY res/
  DESTINATION bin)

