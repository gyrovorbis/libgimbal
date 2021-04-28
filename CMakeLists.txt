cmake_minimum_required(VERSION 3.10)

set(GIMBAL_VERSION_MAJOR 0)
set(GIMBAL_VERSION_MINOR 0)
set(GIMBAL_VERSION_PATCH 1)
set(GIMBAL_VERSION
  ${GIMBAL_VERSION_MAJOR}.${GIMBAL_VERSION_MINOR}.${GIMBAL_VERSION_PATCH})

include(util/gimbal_utils.cmake)

project(LibGimbal 
    VERSION ${GIMBAL_VERSION}
    DESCRIPTION "Aggregate project for libGimbal subprojects" 
    LANGUAGES C CXX)


add_subdirectory(lib)
add_subdirectory(test)
