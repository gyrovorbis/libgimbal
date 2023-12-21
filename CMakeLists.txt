cmake_minimum_required(VERSION 3.10)

SET(ANDROID_NATIVE_API_LEVEL 33)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(GIMBAL_VERSION_MAJOR 0)
set(GIMBAL_VERSION_MINOR 1)
set(GIMBAL_VERSION_PATCH 0)
set(GIMBAL_VERSION
  ${GIMBAL_VERSION_MAJOR}.${GIMBAL_VERSION_MINOR}.${GIMBAL_VERSION_PATCH})

include(util/gimbal_utils.cmake)

project(LibGimbal 
    VERSION ${GIMBAL_VERSION}
    DESCRIPTION "Aggregate project for libGimbal subprojects" 
    HOMEPAGE_URL "libgimbal.elysianshadows.com"
    LANGUAGES C CXX)


kos_enable_sh4_math()

option(GBL_ENABLE_TESTS "Enable LibGimbal unit tests" OFF)
option(GBL_ENABLE_CPP   "Enable C++ libGimbal API"    OFF)

add_subdirectory(lib)

if(GBL_ENABLE_TESTS)
    add_subdirectory(test)
endif(GBL_ENABLE_TESTS)
