cmake_minimum_required(VERSION 3.10)

set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED=NO)
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
#set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.test")
set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "Falco Girgis")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")

project(GimbalTest VERSION ${GIMBAL_VERSION} DESCRIPTION "Gimbal Tests" LANGUAGES C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(GimbalTests
    include/containers/gimbal_linked_list_test_suite.h
    source/containers/gimbal_linked_list_test_suite.c
    include/containers/gimbal_doubly_linked_list_test_suite.h
    source/containers/gimbal_doubly_linked_list_test_suite.c
    include/containers/gimbal_array_map_test_suite.h
    source/containers/gimbal_array_map_test_suite.c
    include/containers/gimbal_tree_set_test_suite.h
    source/containers/gimbal_tree_set_test_suite.c
    include/types/gimbal_ref_test_suite.h
    source/types/gimbal_ref_test_suite.c
    include/containers/gimbal_byte_array_test_suite.h
    source/containers/gimbal_byte_array_test_suite.c
    include/strings/gimbal_quark_test_suite.h
    source/strings/gimbal_quark_test_suite.c
    include/strings/gimbal_string_view_test_suite.h
    source/strings/gimbal_string_view_test_suite.c
    include/strings/gimbal_string_ref_test_suite.h
    source/strings/gimbal_string_ref_test_suite.c
    include/meta/gimbal_type_test_suite.h
    source/meta/gimbal_type_test_suite.c
    include/types/gimbal_variant_test_suite.h
    source/types/gimbal_variant_test_suite.c
    include/meta/gimbal_enum_test_suite.h
    source/meta/gimbal_enum_test_suite.c
    include/meta/gimbal_flags_test_suite.h
    source/meta/gimbal_flags_test_suite.c
    include/meta/gimbal_opaque_test_suite.h
    source/meta/gimbal_opaque_test_suite.c
    include/meta/gimbal_boxed_test_suite.h
    source/meta/gimbal_boxed_test_suite.c
    include/objects/gimbal_object_test_suite.h
    source/objects/gimbal_object_test_suite.c
    include/containers/gimbal_vector_test_suite.h
    source/containers/gimbal_vector_test_suite.c
    include/containers/gimbal_hash_set_test_suite.h
    source/containers/gimbal_hash_set_test_suite.c
    include/strings/gimbal_string_buffer_test_suite.h
    source/strings/gimbal_string_buffer_test_suite.c
    include/meta/gimbal_class_test_suite.h
    source/meta/gimbal_class_test_suite.c
    include/meta/gimbal_interface_test_suite.h
    source/meta/gimbal_interface_test_suite.c
    include/meta/gimbal_instance_test_suite.h
    source/meta/gimbal_instance_test_suite.c
    source/test_main.c)

target_link_libraries(GimbalTests
    libGimbal)

target_include_directories(GimbalTests
    PRIVATE include)

set_target_properties(GimbalTests PROPERTIES
    MACOSX_BUNDLE_INFO_STRING "Gimbal Tests v${GIMBAL_VERSION}"
    MACOSX_BUNDLE_BUNDLE_NAME "GimbalTests"
    MACOSX_BUNDLE_BUNDLE_VERSION ${GIMBAL_VERSION}
    MACOSX_BUNDLE_GUI_IDENTIFIER "GimbalTests"
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${GIMBAL_VERSION}
    XCODE_ATTRIBUTE_PRODUCT_NAME "GimbalTests"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")

add_test(NAME GimbalTests COMMAND GimbalTests)
