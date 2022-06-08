cmake_minimum_required(VERSION 3.10)

project(GimbalTest VERSION ${GIMBAL_VERSION} DESCRIPTION "Gimbal Tests" LANGUAGES C CXX)

find_package(Qt6 COMPONENTS Core Test REQUIRED)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTO_RCC ON)
set(CMAKE_AUTOIC ON)


add_subdirectory(lib/elysian-test)

add_executable(GimbalTests
    include/test_gimbal.hpp
    include/test_gimbal_type.h
    include/test_gimbal_ref.h
    include/test_gimbal_string_ref.h
    include/test_gimbal_string_view.h
    include/test_gimbal_ref.hpp
    include/test_gimbal_string_ref.hpp
    include/test_gimbal_string_view.hpp
    include/test_gimbal_variant.hpp
    include/test_gimbal_string.hpp
    include/test_gimbal_vector.hpp
    include/test_gimbal_hash_map.hpp
    include/test_gimbal_buffer.hpp
    include/test_gimbal_byte_array.hpp
    include/test_gimbal_api.hpp
    include/test_gimbal_context.hpp
    include/test_gimbal_type.hpp
#    include/test_gimbal_meta.hpp
    include/test_gimbal_object.hpp
    include/test_gimbal_algorithms.hpp
    include/test_gimbal_quark.hpp
    source/test_gimbal_object.cpp
    source/test_gimbal_type.cpp
    source/test_gimbal_string.cpp
    source/test_gimbal_byte_array.cpp
    source/test_gimbal_type.c
    source/test_gimbal_ref.c
    source/test_gimbal_string_ref.c
    source/test_gimbal_string_view.c
    include/test_gimbal_closure.h
    include/test_gimbal_closure.hpp
    source/test_gimbal_closure.c
    include/test_gimbal_linked_list.h
    include/test_gimbal_linked_list.hpp
    source/test_gimbal_linked_list.c
    include/test_gimbal_array_map.h
    include/test_gimbal_array_map.hpp
    source/test_gimbal_array_map.c
    include/test_gimbal_tree_set.h
    include/test_gimbal_tree_set.hpp
    source/test_gimbal_tree_set.c
    source/test_main.cpp)


target_link_libraries(GimbalTests
    libGimbal
    ElysianTest)

target_include_directories(GimbalTests
    PRIVATE include)

add_test(NAME GimbalTests COMMAND GimbalTests)
