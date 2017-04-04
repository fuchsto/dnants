project(project_dnants_libgos CXX)

include(${CMAKE_SOURCE_DIR}/CMakeExt/InstallFiles.cmake)


set(DNANTS_LIBGOS_TARGET_NAME gos)


file(GLOB_RECURSE DNANTS_LIBGOS_SOURCES
     "src/*.c" "src/*.h" "src/*.cc" "src/*.cpp" )
file(GLOB_RECURSE DNANTS_LIBGOS_HEADERS
     "include/*.h")

set(DNANTS_LIBGOS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DNANTS_LIBGOS_SOURCE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(
  ${DNANTS_LIBGOS_INCLUDE_DIRS}
)

add_library(
  ${DNANTS_LIBGOS_TARGET_NAME}
  SHARED
  ${DNANTS_LIBGOS_HEADERS}
  ${DNANTS_LIBGOS_SOURCES}
)

target_link_libraries(
  ${DNANTS_LIBGOS_TARGET_NAME}
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
  TARGETS ${DNANTS_LIBGOS_TARGET_NAME}
  DESTINATION lib/)
