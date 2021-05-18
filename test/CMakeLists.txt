cmake_minimum_required(VERSION 3.10)

project(GimbalTest VERSION ${GIMBAL_VERSION} DESCRIPTION "Gimbal Tests" LANGUAGES C CXX)

find_package(Qt6 COMPONENTS Core Test REQUIRED)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTO_RCC ON)
set(CMAKE_AUTOIC ON)


add_subdirectory(lib/elysian-test)

add_executable(GimbalTests
    include/test_gimbal.hpp
    include/variant_tests.hpp
    include/test_gimbal_string.hpp
    include/test_gimbal_api.hpp
    include/test_gimbal_context.hpp
    test_main.cpp)


target_link_libraries(GimbalTests
    libGimbal
    ElysianTest)

target_include_directories(GimbalTests
    PRIVATE include)

add_test(NAME GimbalTests COMMAND GimbalTests)
