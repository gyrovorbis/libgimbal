cmake_minimum_required(VERSION 3.10)

project(GimbalTest VERSION ${GIMBAL_VERSION} DESCRIPTION "Gimbal Tests" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTO_RCC ON)
set(CMAKE_AUTOIC ON)

find_package(Qt5 COMPONENTS Core Test REQUIRED)

add_executable(GimbalTests
    variant_tests.hpp
    test_main.cpp)

target_link_libraries(GimbalTests
    libGimbal
    Qt5::Core
    Qt5::Test)

add_test(NAME GimbalTests COMMAND GimbalTests)
