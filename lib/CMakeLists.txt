cmake_minimum_required(VERSION 3.10)

SET(ANDROID_NATIVE_API_LEVEL 28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

project(LibGimbal 
    VERSION 
        ${GIMBAL_VERSION} 
    DESCRIPTION 
        "Lightweight langauge independent cross-platform framework for creating static/dynamic libraries with a run-time extensible C API"
    HOMEPAGE_URL
        "libgimbal.elysianshadows.com"
    LANGUAGES C CXX)

#set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum OS X Deployment Version")

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(VITA)
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVITA")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVITA")
    GBL_BUILD_DEFINE(VITA 1)
    # This file, unfortunately, needs to be manually included.
    # And on this lists file specifically (its not passed down from the parent/aggregate of course)
    include("${VITASDK}/share/vita.cmake" REQUIRED)
endif()

GBL_BUILD_TIMESTAMP_DEFINE(GBL_BUILD_TIMESTAMP)
GBL_BUILD_BIG_ENDIAN_DEFINE(GBL_BIG_ENDIAN)

GBL_BUILD_HOST_INFO_DEFINE(GBL_BUILD_HOST_NAME HOSTNAME)
GBL_BUILD_HOST_INFO_DEFINE(GBL_BUILD_HOST_DOMAIN_NAME FQDN)
GBL_BUILD_HOST_INFO_DEFINE(GBL_BUILD_HOST_PROCESSOR_NAME PROCESSOR_NAME)
GBL_BUILD_HOST_INFO_DEFINE(GBL_BUILD_HOST_PROCESSOR_DESCRIPTION PROCESSOR_DESCRIPTION)
GBL_BUILD_HOST_INFO_DEFINE(GBL_BUILD_HOST_PROCESSOR_CORES_PHYSICAL NUMBER_OF_PHYSICAL_CORES)
GBL_BUILD_HOST_INFO_DEFINE(GBL_BUILD_HOST_PROCESSOR_CORES_LOGICAL NUMBER_OF_LOGICAL_CORES)
GBL_BUILD_HOST_INFO_DEFINE(GBL_BUILD_HOST_MEMORY_PHYSICAL_TOTAL TOTAL_PHYSICAL_MEMORY)
GBL_BUILD_HOST_INFO_DEFINE(GBL_BUILD_HOST_MEMORY_PHYSICAL_AVAILABLE AVAILABLE_PHYSICAL_MEMORY)

GBL_BUILD_DEFINE(GBL_BUILD_CI $<BOOL:$ENV{GITLAB_CI}>)

GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_PROJECT_TITLE CI_PROJECT_TITLE)

GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_COMMIT_TITLE CI_COMMIT_TITLE)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_COMMIT_SHA CI_COMMIT_SHA)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_COMMIT_TAG CI_COMMIT_TAG)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_COMMIT_BRANCH CI_COMMIT_BRANCH)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_COMMIT_TIMESTAMP CI_COMMIT_TIMESTAMP)

GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_JOB_ID CI_JOB_ID)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_JOB_NAME CI_JOB_NAME)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_JOB_STAGE CI_JOB_STAGE)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_JOB_MANUAL CI_JOB_MANUAL)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_JOB_TRIGGERED CI_JOB_TRIGGERED)

GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_RUNNER_DESCRIPTION CI_RUNNER_DESCRIPTION)
#GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_RUNNER_TAGS CI_RUNNER_TAGS)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_RUNNER_ARCH CI_RUNNER_EXECUTABLE_ARCH)

GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_USER_NAME GITLAB_USER_NAME)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_USER_LOGIN GITLAB_USER_LOGIN)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_USER_EMAIL GITLAB_USER_EMAIL)

GBL_CONFIG_VARIABLE(GBL_CONFIGURATION_INPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/api/gimbal/core/gimbal_config.h.in FILEPATH "Path to user API configuration file")

GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_PARTIAL_LEVEL    GBL_LOG_LEVEL_WARNING STRING   "Log level for reporting partial results")
GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_ERROR_LEVEL      GBL_LOG_LEVEL_ERROR STRING     "Log level for reporting error results")
GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_UNKNOWN_LEVEL    GBL_LOG_LEVEL_ERROR STRING     "Log level for reporting unknown results")

GBL_CONFIG_OPTION(GBL_CONFIG_PREFETCH_ENABLED       "Enable prefetching macro"                             ON)
GBL_CONFIG_OPTION(GBL_CONFIG_ERRNO_CHECKS           "Enable C errno verification macros"                   ON)

GBL_CONFIG_OPTION(GBL_CONFIG_LOG_PARTIAL_ENABLED    "Log all partial success results from API calls"       ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LOG_ERROR_ENABLED      "Log all error results from API calls"                 ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LOG_UNKNOWN_ENABLED    "Log all unknown results from API calls"               ON)

GBL_CONFIG_OPTION(GBL_CONFIG_ASSERT_PARTIAL_ENABLED "Assert on all partial success results from API calls" OFF)
GBL_CONFIG_OPTION(GBL_CONFIG_ASSERT_ERROR_ENABLED   "Assert on all error results from API calls"           OFF)
GBL_CONFIG_OPTION(GBL_CONFIG_ASSERT_UNKNOWN_ENABLED "Assert on all unknown results from API calls"         OFF)

set(GIMBAL_CONFIGURATION_OUTPUT_PATH "${PROJECT_BINARY_DIR}/api/gimbal/core/gimbal_config.h")

if ((CMAKE_COMPILER_IS_GNUCC))
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Os -g")
endif()

set(GIMBAL_INCLUDES
    ${GBL_CONFIGURATION_INPUT_PATH}
    ${GIMBAL_CONFIGURATION_OUTPUT_PATH}
    api/gimbal/gimbal_allocators.h
    api/gimbal/gimbal_algorithms.h
    api/gimbal/gimbal_containers.h
    api/gimbal/gimbal_core.h
    api/gimbal/gimbal_meta.h
    api/gimbal/gimbal_preprocessor.h
    api/gimbal/gimbal_strings.h
    api/gimbal/gimbal_test.h
    api/gimbal/gimbal_utils.h
    api/gimbal/algorithms/gimbal_sort.h
    api/gimbal/algorithms/gimbal_numeric.h
    api/gimbal/algorithms/gimbal_hash.h
    api/gimbal/allocators/gimbal_allocation_tracker.h
    api/gimbal/allocators/gimbal_arena_allocator.h
    api/gimbal/allocators/gimbal_pool_allocator.h
    api/gimbal/allocators/gimbal_scope_allocator.h
    api/gimbal/containers/gimbal_linked_list.h
    api/gimbal/containers/gimbal_hash_set.h
    api/gimbal/containers/gimbal_tree_set.h
    api/gimbal/containers/gimbal_array_map.h
    api/gimbal/containers/gimbal_doubly_linked_list.h
    api/gimbal/containers/gimbal_array_list.h
    api/gimbal/containers/gimbal_nary_tree.h
    api/gimbal/containers/gimbal_ring_buffer.h
    api/gimbal/containers/gimbal_ring_list.h
    api/gimbal/containers/gimbal_array_deque.h
    api/gimbal/containers/gimbal_array_heap.h
    api/gimbal/core/gimbal_decls.h
    api/gimbal/core/gimbal_ctx.h
    api/gimbal/core/gimbal_stack_frame.h
    api/gimbal/core/gimbal_result.h
    api/gimbal/core/gimbal_typedefs.h
    api/gimbal/core/gimbal_thd.h
    api/gimbal/core/gimbal_thread.h
    api/gimbal/core/gimbal_atomics.h
    api/gimbal/core/gimbal_app.h
    api/gimbal/core/gimbal_main_loop.h
    api/gimbal/core/gimbal_task.h
    api/gimbal/core/gimbal_logger.h
    api/gimbal/core/gimbal_module.h
    api/gimbal/core/gimbal_exception.h
    api/gimbal/meta/classes/gimbal_class.h
    api/gimbal/meta/classes/gimbal_class.hpp
    api/gimbal/meta/classes/gimbal_opaque.h
    api/gimbal/meta/classes/gimbal_enum.h
    api/gimbal/meta/classes/gimbal_flags.h
    api/gimbal/meta/classes/gimbal_primitives.h
    api/gimbal/meta/ifaces/gimbal_ivariant.h
    api/gimbal/meta/ifaces/gimbal_itable_variant.h
    api/gimbal/meta/ifaces/gimbal_ievent_handler.h
    api/gimbal/meta/ifaces/gimbal_ievent_filter.h
    api/gimbal/meta/ifaces/gimbal_iplugin.h
    api/gimbal/meta/ifaces/gimbal_iallocator.h
    api/gimbal/meta/ifaces/gimbal_ilogger.h
    api/gimbal/meta/ifaces/gimbal_interface.h
    api/gimbal/meta/instances/gimbal_context.h
    api/gimbal/meta/instances/gimbal_event.h
    api/gimbal/meta/instances/gimbal_instance.h
    api/gimbal/meta/instances/gimbal_object.h
    api/gimbal/meta/instances/gimbal_box.h
    api/gimbal/meta/properties/gimbal_property.h
    api/gimbal/meta/signals/gimbal_signal.h
    api/gimbal/meta/signals/gimbal_marshal.h
    api/gimbal/meta/signals/gimbal_closure.h
    api/gimbal/meta/signals/gimbal_c_closure.h
    api/gimbal/meta/signals/gimbal_class_closure.h
    api/gimbal/meta/signals/gimbal_signal_closure.h
    api/gimbal/meta/types/gimbal_type.h
    api/gimbal/meta/types/gimbal_pointer.h
    api/gimbal/meta/types/gimbal_variant.h
    api/gimbal/meta/types/gimbal_builtin_types.h
    source/meta/types/gimbal_type_.h
    api/gimbal/platform/gimbal_time_.h
    api/gimbal/platform/gimbal_thread_.h
    api/gimbal/platform/gimbal_system_.h
    api/gimbal/preprocessor/gimbal_compiler.h
    api/gimbal/preprocessor/gimbal_macro_composition.h
    api/gimbal/preprocessor/gimbal_macro_utils.h
    api/gimbal/preprocessor/gimbal_macro_sequences.h
    api/gimbal/strings/gimbal_quark.h
    api/gimbal/strings/gimbal_string_buffer.h
    api/gimbal/strings/gimbal_string_view.h
    api/gimbal/strings/gimbal_string_ref.h
    api/gimbal/strings/gimbal_string.h
    api/gimbal/strings/gimbal_string_list.h
    api/gimbal/strings/gimbal_pattern.h
    api/gimbal/test/gimbal_test_macros.h
    api/gimbal/test/gimbal_test_suite.h
    api/gimbal/test/gimbal_test_scenario.h
    api/gimbal/utils/gimbal_cmd_parser.h
    api/gimbal/utils/gimbal_timer.h
    api/gimbal/utils/gimbal_uuid.h
    api/gimbal/utils/gimbal_ref.h
    api/gimbal/utils/gimbal_version.h
    api/gimbal/utils/gimbal_option_group.h
    api/gimbal/utils/gimbal_date_time.h
    api/gimbal/utils/gimbal_date_time.hpp
    api/gimbal/utils/gimbal_byte_array.h
    api/gimbal/utils/gimbal_bit_view.h
    api/gimbal/utils/gimbal_scanner.h
    api/gimbal/utils/gimbal_uri.h
    api/gimbal/utils/gimbal_settings.h
)


set(GIMBAL_SOURCES
    source/algorithms/gimbal_hash.c
    source/algorithms/gimbal_numeric.c
    source/algorithms/gimbal_sort.c
    source/allocators/gimbal_arena_allocator.c
    source/allocators/gimbal_pool_allocator.c
    source/allocators/gimbal_allocation_tracker.c
    source/allocators/gimbal_scope_allocator.c
    source/containers/gimbal_hash_set.c
    source/containers/gimbal_tree_set.c
    source/containers/gimbal_linked_list.c
    source/containers/gimbal_doubly_linked_list.c
    source/containers/gimbal_nary_tree.c
    source/containers/gimbal_array_map.c
    source/containers/gimbal_array_list.c
    source/containers/gimbal_ring_buffer.c
    source/containers/gimbal_ring_list.c
    source/containers/gimbal_array_deque.c
    source/containers/gimbal_array_heap.c
    source/core/gimbal_thd.c
    source/core/gimbal_thread.c
    source/core/gimbal_app.c
    source/core/gimbal_module.c
    source/core/gimbal_logger.c
    source/core/gimbal_exception.c
    source/core/gimbal_main_loop.c
    source/meta/classes/gimbal_class.c
    source/meta/classes/gimbal_opaque.c
    source/meta/classes/gimbal_flags.c
    source/meta/classes/gimbal_enum.c
    source/meta/classes/gimbal_primitives.c
    source/meta/ifaces/gimbal_ievent_handler.c
    source/meta/ifaces/gimbal_ievent_filter.c
    source/meta/ifaces/gimbal_iallocator.c
    source/meta/ifaces/gimbal_ilogger.c
    source/meta/ifaces/gimbal_iplugin.c
    source/meta/ifaces/gimbal_interface.c
    source/meta/ifaces/gimbal_ivariant.c
    source/meta/ifaces/gimbal_itable_variant.c
    source/meta/instances/gimbal_context.c
    source/meta/instances/gimbal_object.c
    source/meta/instances/gimbal_event.c
    source/meta/instances/gimbal_instance.c
    source/meta/instances/gimbal_box.c
    source/meta/properties/gimbal_property.c
    source/meta/signals/gimbal_signal.c
    source/meta/signals/gimbal_marshal.c
    source/meta/signals/gimbal_closure.c
    source/meta/signals/gimbal_c_closure.c
    source/meta/signals/gimbal_class_closure.c
    source/meta/signals/gimbal_signal_closure.c
    source/meta/types/gimbal_variant.c
    source/meta/types/gimbal_type.c
    source/meta/types/gimbal_pointer.c
    source/platform/gimbal_time_.c
    source/strings/gimbal_quark.c
    source/strings/gimbal_string_ref.c
    source/strings/gimbal_string_buffer.c
    source/strings/gimbal_string.c
    source/strings/gimbal_string_list.c
    source/strings/gimbal_pattern.c
    source/strings/gimbal_string_view.c
    source/test/gimbal_test_suite.c
    source/test/gimbal_test_scenario.c
    source/utils/gimbal_cmd_parser.c
    source/utils/gimbal_uuid.c
    source/utils/gimbal_version.c
    source/utils/gimbal_ref.c
    source/utils/gimbal_option_group.c
    source/utils/gimbal_date_time.c
    source/utils/gimbal_byte_array.c
    source/utils/gimbal_bit_view.c
    source/utils/gimbal_uri.c
    source/utils/gimbal_scanner.c
    source/utils/gimbal_settings.c
    source/utils/gimbal_timer.c
)

add_library(libGimbal STATIC
    ${GIMBAL_SOURCES}
    ${GIMBAL_INCLUDES})

add_subdirectory(lib/tinycthread)
add_subdirectory(lib/tiny-regex-c)

list(APPEND
     GBL_TARGET_LINK_LIBS
     tinycthread
     TinyRegexC)

if (NOT DEFINED PLATFORM_DREAMCAST AND NOT DEFINED BUILD_VITA)

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    list(APPEND
         GBL_TARGET_LINK_LIBS
         Threads::Threads)

endif()

target_link_libraries(libGimbal ${GBL_TARGET_LINK_LIBS})

#set_property(TARGET libGimbal PROPERTY C_STANDARD 11)
#set_property(TARGET libGimbal PROPERTY C_STANDARD_REQUIRED ON)

GBL_TARGET_PROPERTY_DEFINE(GBL_BUILD_C_STANDARD libGimbal C_STANDARD)
GBL_TARGET_PROPERTY_DEFINE(GBL_BUILD_CPP_STANDARD libGimbal CXX_STANDARD)
GBL_TARGET_PROPERTY_DEFINE(GBL_BUILD_C_EXTENSIONS libGimbal C_EXTENIONS)
GBL_TARGET_PROPERTY_DEFINE(GBL_BUILD_CPP_EXTENSIONS libGimbal CXX_EXTENSIONS)

# /Zc:__cplusplus is required to make __cplusplus accurate
# /Zc:__cplusplus is available starting with Visual Studio 2017 version 15.7
# (according to https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus)
# That version is equivalent to _MSC_VER==1914
# (according to https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019)
# CMake's ${MSVC_VERSION} is equivalent to _MSC_VER
# (according to https://cmake.org/cmake/help/latest/variable/MSVC_VERSION.html#variable:MSVC_VERSION)
if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
        target_compile_options(libGimbal PUBLIC "/Zc:preprocessor")
endif()

if ((CMAKE_COMPILER_IS_GNUCC) OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
# Strip out full file paths for __FILE__ macros to save .rodata space/look prettier
    target_compile_options(libGimbal PUBLIC "-fmacro-prefix-map=${CMAKE_SOURCE_DIR}=...")
    target_compile_options(libGimbal PUBLIC "-Wno-deprecated-volatile")
endif()

if ((CMAKE_COMPILER_IS_GNUCC))
    target_compile_options(libGimbal PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>)
endif()

list(APPEND
    GBL_DEFINES
    ${GBL_CONFIGURATION_DEFINES}
    ${GBL_BUILD_DEFINES})


target_include_directories(libGimbal
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>
        $<INSTALL_INTERFACE:api>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/api>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/api
)

target_compile_definitions(libGimbal
    PUBLIC
    ${GBL_DEFINES}
    -DGIMBAL_VERSION_MAJOR=${GIMBAL_VERSION_MAJOR}
    -DGIMBAL_VERSION_MINOR=${GIMBAL_VERSION_MINOR}
    -DGIMBAL_VERSION_PATCH=${GIMBAL_VERSION_PATCH}
    -DGIMBAL_VERSION="${GIMBAL_VERSION}")

configure_file(${GBL_CONFIGURATION_INPUT_PATH}
    ${GIMBAL_CONFIGURATION_OUTPUT_PATH})


