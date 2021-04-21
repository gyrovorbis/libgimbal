cmake_minimum_required(VERSION 3.10)

set(GIMBAL_VERSION_MAJOR 2)
set(GIMBAL_VERSION_MINOR 0)
set(GIMBAL_VERSION_PATCH 0)
set(GIMBAL_VERSION
  ${GIMBAL_VERSION_MAJOR}.${GIMBAL_VERSION_MINOR}.${GIMBAL_VERSION_PATCH})

project(LibGimbal 
    VERSION ${GIMBAL_VERSION}
    DESCRIPTION "Aggregate project for libGimbal subprojects" 
    LANGUAGES C CXX)


add_subdirectory(lib)
add_subdirectory(test)
