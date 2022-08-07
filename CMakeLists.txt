cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

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

option(GBL_ENABLE_TESTS "Enable LibGimbal unit tests" OFF)
option(GBL_ENABLE_TESTS_QT "Enable LibGimbal Qt unit tests" OFF)

add_subdirectory(lib)

if(GBL_ENABLE_TESTS)
    add_subdirectory(test)
endif(GBL_ENABLE_TESTS)
