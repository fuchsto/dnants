project(project_dnants_main CXX)

include(${CMAKE_SOURCE_DIR}/CMakeExt/InstallFiles.cmake)

find_package(SDL2 REQUIRED)


file(GLOB_RECURSE DNANTS_MAIN_SOURCES
     "src/*.c" "src/*.h" "src/*.cc" "src/*.cpp" )
file(GLOB_RECURSE DNANTS_MAIN_HEADERS
     "include/*.h")

set(DNANTS_MAIN_TARGET_NAME dnants)

set(DNANTS_MAIN_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(DNANTS_MAIN_SOURCE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(
  ${DNANTS_MAIN_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIR}
)

add_executable(
  ${DNANTS_MAIN_TARGET_NAME}
  ${DNANTS_MAIN_HEADERS}
  ${DNANTS_MAIN_SOURCES}
)

target_link_libraries(
  ${DNANTS_MAIN_TARGET_NAME}
  ${SDL2_LIBRARY}
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
  DESTINATION bin/)
