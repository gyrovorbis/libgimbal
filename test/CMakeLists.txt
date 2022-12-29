cmake_minimum_required(VERSION 3.10)

set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED=NO)
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
#set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.test")
set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "Falco Girgis")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")

project(GimbalTest VERSION ${GIMBAL_VERSION} DESCRIPTION "LibGimbal Tests" LANGUAGES C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS OFF)

# Because fuck trying to build an Android APK via CMake without Qt
if(ANDROID)
    set(CMAKE_AUTOMOC ON)
    find_package(Qt6 COMPONENTS Core Svg REQUIRED)
endif()

set(GIMBAL_TEST_SOURCES
    include/algorithms/gimbal_sort_test_suite.h
    source/algorithms/gimbal_sort_test_suite.c
    include/algorithms/gimbal_hash_test_suite.h
    source/algorithms/gimbal_hash_test_suite.c
    include/allocators/gimbal_arena_allocator_test_suite.h
    source/allocators/gimbal_arena_allocator_test_suite.c
    include/allocators/gimbal_pool_allocator_test_suite.h
    source/allocators/gimbal_pool_allocator_test_suite.c
    include/allocators/gimbal_scope_allocator_test_suite.h
    source/allocators/gimbal_scope_allocator_test_suite.c
    include/containers/gimbal_linked_list_test_suite.h
    source/containers/gimbal_linked_list_test_suite.c
    include/containers/gimbal_doubly_linked_list_test_suite.h
    source/containers/gimbal_doubly_linked_list_test_suite.c
    include/containers/gimbal_array_map_test_suite.h
    source/containers/gimbal_array_map_test_suite.c
    include/containers/gimbal_tree_set_test_suite.h
    source/containers/gimbal_tree_set_test_suite.c
    include/containers/gimbal_byte_array_test_suite.h
    source/containers/gimbal_byte_array_test_suite.c
    include/containers/gimbal_array_list_test_suite.h
    source/containers/gimbal_array_list_test_suite.c
    include/containers/gimbal_hash_set_test_suite.h
    source/containers/gimbal_hash_set_test_suite.c
    include/containers/gimbal_nary_tree_test_suite.h
    source/containers/gimbal_nary_tree_test_suite.c
    include/containers/gimbal_ring_buffer_test_suite.h
    source/containers/gimbal_ring_buffer_test_suite.c
    include/containers/gimbal_ring_list_test_suite.h
    source/containers/gimbal_ring_list_test_suite.c
    include/containers/gimbal_array_deque_test_suite.h
    source/containers/gimbal_array_deque_test_suite.c
    include/containers/gimbal_array_heap_test_suite.h
    source/containers/gimbal_array_heap_test_suite.c
    include/meta/classes/gimbal_enum_test_suite.h
    source/meta/classes/gimbal_enum_test_suite.c
    include/meta/classes/gimbal_flags_test_suite.h
    source/meta/classes/gimbal_flags_test_suite.c
    include/meta/classes/gimbal_opaque_test_suite.h
    source/meta/classes/gimbal_opaque_test_suite.c
    include/meta/classes/gimbal_class_test_suite.h
    source/meta/classes/gimbal_class_test_suite.c
    include/meta/ifaces/gimbal_interface_test_suite.h
    source/meta/ifaces/gimbal_interface_test_suite.c
    include/meta/instances/gimbal_instance_test_suite.h
    source/meta/instances/gimbal_instance_test_suite.c
    include/meta/instances/gimbal_object_test_suite.h
    source/meta/instances/gimbal_object_test_suite.c
    include/meta/signals/gimbal_signal_test_suite.h
    source/meta/signals/gimbal_signal_test_suite.c
    include/meta/signals/gimbal_closure_test_suite.h
    source/meta/signals/gimbal_closure_test_suite.c
    include/meta/types/gimbal_type_test_suite.h
    source/meta/types/gimbal_type_test_suite.c
    include/meta/types/gimbal_variant_test_suite.h
    source/meta/types/gimbal_variant_test_suite.c
    include/meta/types/gimbal_pointer_test_suite.h
    source/meta/types/gimbal_pointer_test_suite.c
    include/strings/gimbal_quark_test_suite.h
    source/strings/gimbal_quark_test_suite.c
    include/strings/gimbal_string_list_test_suite.h
    source/strings/gimbal_string_list_test_suite.c
    include/strings/gimbal_string_view_test_suite.h
    source/strings/gimbal_string_view_test_suite.c
    include/strings/gimbal_string_ref_test_suite.h
    source/strings/gimbal_string_ref_test_suite.c
    include/strings/gimbal_string_buffer_test_suite.h
    source/strings/gimbal_string_buffer_test_suite.c
    include/strings/gimbal_string_pattern_test_suite.h
    source/strings/gimbal_string_pattern_test_suite.c
    include/utils/gimbal_uuid_test_suite.h
    source/utils/gimbal_uuid_test_suite.c
    include/utils/gimbal_ref_test_suite.h
    source/utils/gimbal_ref_test_suite.c
    include/utils/gimbal_version_test_suite.h
    source/utils/gimbal_version_test_suite.c
    include/utils/gimbal_option_group_test_suite.h
    source/utils/gimbal_option_group_test_suite.c
    include/utils/gimbal_cmd_parser_test_suite.h
    source/utils/gimbal_cmd_parser_test_suite.c
    source/gimbal_test_main.c
)

if(GBL_ENABLE_CPP)
    list(APPEND
         GIMBAL_TEST_SOURCES
         source/gimbal_test_cpp.cpp)
endif(GBL_ENABLE_CPP)

if(ANDROID)
    qt_add_executable(GimbalTests
        MANUAL_FINALIZATION
        ${GIMBAL_TEST_SOURCES}
    )
    #set_property(TARGET GimbalTests APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)
else()
    add_executable(GimbalTests
        ${GIMBAL_TEST_SOURCES}
    )
endif()

set(GBL_TEST_LINK_LIBS
    libGimbal)

if(ANDROID)
    list(APPEND
         GBL_TEST_LINK_LIBS
         Qt6::Core
         Qt6::Svg)
endif()

target_link_libraries(GimbalTests
    PRIVATE
    ${GBL_TEST_LINK_LIBS})

if(VITA)
    add_compile_definitions(GimbalTests PUBLIC VITA=1)
    include("${VITASDK}/share/vita.cmake" REQUIRED)

    vita_create_self(eboot.bin GimbalTests UNSAFE)

    vita_create_vpk("GimbalTests_v${GIMBAL_VERSION}.vpk" GBLTESTS0 eboot.bin
        VERSION "01.00"
        NAME "Gimbal Tests v${GIMBAL_VERSION}"
    )
endif()

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

if(ANDROID)
    # Figure ou the platform & architecture code we need
    set(ANDROID_PLATFORM_ARCHITECTURE_CODE "xxx")
    if (${ANDROID_ABI} STREQUAL "armeabi-v7a")
      set(ANDROID_PLATFORM_ARCHITECTURE_CODE "032")
    elseif (${ANDROID_ABI} STREQUAL "arm64-v8a")
      set(ANDROID_PLATFORM_ARCHITECTURE_CODE "064")
    elseif (${ANDROID_ABI} STREQUAL "x86")
      set(ANDROID_PLATFORM_ARCHITECTURE_CODE "132")
    elseif (${ANDROID_ABI} STREQUAL "x86_64")
      set(ANDROID_PLATFORM_ARCHITECTURE_CODE "164")
    endif()

    # Slap the above together with a version code (major and minor only)
    set(ANDROID_VERSION_CODE "${ANDROID_PLATFORM_ARCHITECTURE_CODE}${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}")
    set_property(TARGET GimbalTests APPEND PROPERTY QT_ANDROID_VERSION_CODE ${ANDROID_VERSION_CODE})
    message(STATUS ${ANDROID_VERSION_CODE})

    qt_android_generate_deployment_settings(GimbalTests)
    qt_finalize_executable(GimbalTests)
endif()
