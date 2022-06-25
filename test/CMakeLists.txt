cmake_minimum_required(VERSION 3.10)

set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.LibGimbalTests")
set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "esteam")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")

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
    include/meta/test_gimbal_type.h
    include/types/test_gimbal_ref.h
    include/strings/test_gimbal_string_ref.h
    include/strings/test_gimbal_string_view.h
    include/types/test_gimbal_ref.hpp
    include/strings/test_gimbal_string_ref.hpp
    include/strings/test_gimbal_string_view.hpp
    include/types/test_gimbal_variant.hpp
    include/strings/test_gimbal_string.hpp
    include/containers/test_gimbal_vector.hpp
    include/containers/test_gimbal_hash_map.hpp
    include/test_gimbal_api.hpp
    include/objects/test_gimbal_context.hpp
    include/meta/test_gimbal_type.hpp
    include/objects/test_gimbal_object.hpp
    include/algorithms/test_gimbal_algorithms.hpp
    include/strings/test_gimbal_quark.hpp
    source/objects/test_gimbal_object.cpp
    source/meta/test_gimbal_type.cpp
    source/strings/test_gimbal_string.cpp
    source/meta/test_gimbal_type.c
    source/types/test_gimbal_ref.c
    source/strings/test_gimbal_string_ref.c
    source/strings/test_gimbal_string_view.c
    include/types/test_gimbal_closure.h
    include/types/test_gimbal_closure.hpp
    source/types/test_gimbal_closure.c
    include/containers/test_gimbal_linked_list.h
    include/containers/test_gimbal_linked_list.hpp
    source/containers/test_gimbal_linked_list.c
    include/containers/test_gimbal_array_map.h
    include/containers/test_gimbal_array_map.hpp
    source/containers/test_gimbal_array_map.c
    include/containers/test_gimbal_tree_set.h
    include/containers/test_gimbal_tree_set.hpp
    source/containers/test_gimbal_tree_set.c
    include/types/test_gimbal_variant.h
    source/types/test_gimbal_variant.c
    include/meta/test_gimbal_enum.h
    include/meta/test_gimbal_enum.hpp
    source/meta/test_gimbal_enum.c
    include/meta/test_gimbal_opaque.h
    include/meta/test_gimbal_opaque.hpp
    source/meta/test_gimbal_opaque.c
    include/meta/test_gimbal_boxed.h
    include/meta/test_gimbal_boxed.hpp
    source/meta/test_gimbal_boxed.c
    include/meta/test_gimbal_flags.h
    include/meta/test_gimbal_flags.hpp
    source/meta/test_gimbal_flags.c
    include/containers/test_gimbal_doubly_linked_list.h
    include/containers/test_gimbal_doubly_linked_list.hpp
    source/containers/test_gimbal_doubly_linked_list.c
    include/containers/test_gimbal_byte_array.h
    include/containers/test_gimbal_byte_array.hpp
    source/containers/test_gimbal_byte_array.c
    include/strings/test_gimbal_quark.h
    source/strings/test_gimbal_quark.c
    source/test_main.cpp)


target_link_libraries(GimbalTests
    libGimbal
    ElysianTest)

target_include_directories(GimbalTests
    PRIVATE include)

set_target_properties(GimbalTests PROPERTIES
    MACOSX_BUNDLE_INFO_STRING "Gimbal Tests v${GIMBAL_VERSION}"
    MACOSX_BUNDLE_BUNDLE_NAME "LibGimbalTests"
    MACOSX_BUNDLE_BUNDLE_VERSION ${GIMBAL_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${GIMBAL_VERSION}
    XCODE_ATTRIBUTE_PRODUCT_NAME "LibGimbalTests")

add_test(NAME GimbalTests COMMAND GimbalTests)
