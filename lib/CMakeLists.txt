cmake_minimum_required(VERSION 3.10)

SET(CMAKE_ANDROID_API 28)

project(LibGimbal 
    VERSION 
        ${GIMBAL_VERSION} 
    DESCRIPTION 
        "Lightweight cross-platform framework for creating static/dynamic libraries with a run-time extensible C API"
    HOMEPAGE_URL
        "elysianshadows.com"
    LANGUAGES C)

#set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum OS X Deployment Version")

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(VITA)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVITA")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVITA")
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

#set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*,-hicpp-braces-around-statements;")
#set(CMAKE_C_CLANG_TIDY "clang-tidy;-checks=*;")

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
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_RUNNER_TAGS CI_RUNNER_TAGS)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_RUNNER_ARCH CI_RUNNER_EXECUTABLE_ARCH)

GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_USER_NAME GITLAB_USER_NAME)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_USER_LOGIN GITLAB_USER_LOGIN)
GBL_GITLAB_CI_DEFINE(GBL_BUILD_CI_USER_EMAIL GITLAB_USER_EMAIL)


GBL_CONFIG_VARIABLE(GBL_CONFIGURATION_INPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/api/gimbal/core/gimbal_config.h.in FILEPATH "Path to user API configuration file")

GBL_CONFIG_OPTION(GBL_CONFIG_LAST_CALL_RECORD_ENABLED "Track most recent call record in GblHandle" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_ERRNO_CHECKS              "Enable C errno verification macros" ON)

GBL_CONFIG_OPTION(GBL_CONFIG_LOG_PARTIAL_ENABLED   "Log all partial success results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LOG_ERROR_ENABLED             "Log all error results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LOG_UNKNOWN_ENABLED             "Log all unknown results from API calls" ON)

GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_PARTIAL_LEVEL GBL_LOG_LEVEL_WARNING STRING "Log level for reporting partial results")
GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_ERROR_LEVEL GBL_LOG_LEVEL_ERROR STRING "Log level for reporting error results")
GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_UNKNOWN_LEVEL GBL_LOG_LEVEL_ERROR STRING "Log level for reporting unknown results")

GBL_CONFIG_OPTION(GBL_CONFIG_ASSERT_PARTIAL_ENABLED "Assert on all partial success results from API calls" OFF)
GBL_CONFIG_OPTION(GBL_CONFIG_ASSERT_ERROR_ENABLED          "Assert on all error results from API calls" OFF)
GBL_CONFIG_OPTION(GBL_CONFIG_ASSERT_UNKNOWN_ENABLED          "Assert on all unknown results from API calls" OFF)

GBL_CONFIG_OPTION(GBL_CONFIG_LAST_CALL_RECORD_PARTIAL_ENABLED "Update Handle's LastCallRecord on all partial success results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LAST_CALL_RECORD_ERROR_ENABLED      "Update Handle's LastCallRecord on all error results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LAST_CALL_RECORD_UNKNOWN_ENABLED      "Update Handle's LastCallRecord on all unknown results from API calls" ON)

set(GIMBAL_CONFIGURATION_OUTPUT_PATH "${PROJECT_BINARY_DIR}/api/gimbal/core/gimbal_config.h")

set(GIMBAL_SOURCES
     source/types/gimbal_version.c
     source/objects/gimbal_context.c
     source/core/gimbal_thread.c
     source/ifaces/gimbal_ivariant.c
     source/types/gimbal_variant.c
     source/types/gimbal_ref.c
     source/objects/gimbal_object.c
     source/core/gimbal_ext.c
     source/meta/gimbal_type.c
     source/meta/gimbal_primitives.c
     source/containers/gimbal_hash_set.c
     source/containers/gimbal_tree_set.c
     source/objects/gimbal_property.c
     source/meta/gimbal_enum.c
     source/ifaces/gimbal_itable.c
     source/strings/gimbal_quark.c
     source/ifaces/gimbal_ievent_handler.c
     source/ifaces/gimbal_ievent_filter.c
     source/core/gimbal_call_stack.c
     source/ifaces/gimbal_iallocator.c
     source/ifaces/gimbal_ilogger.c
     source/objects/gimbal_event.c
     source/meta/gimbal_class.c
     source/meta/gimbal_instance.c
     source/ifaces/gimbal_iplugin.c
     source/meta/gimbal_pointer.c
     source/objects/gimbal_module.c
     source/types/gimbal_closure.c
     source/containers/gimbal_linked_list.c
     source/containers/gimbal_array_map.c
     source/meta/gimbal_boxed.c
     source/meta/gimbal_flags.c
     source/containers/gimbal_byte_array.c
     source/containers/gimbal_vector.c
     source/test/gimbal_test_suite.c
     source/test/gimbal_test_scenario.c
 )

set(GIMBAL_INCLUDES
    api/gimbal/core/gimbal_api_generators.h
    api/gimbal/containers/gimbal_byte_array.h
    api/gimbal/meta/gimbal_enum.h
    api/gimbal/meta/gimbal_flags.h
    api/gimbal/meta/gimbal_pointer.h
    api/gimbal/containers/gimbal_byte_array.hpp
    api/gimbal/types/gimbal_result.h
    api/gimbal/core/gimbal_call_stack.hpp
    api/gimbal/core/gimbal_api_generators.hpp
    api/gimbal/types/gimbal_version.hpp
    api/gimbal/types/gimbal_exception.hpp
    api/gimbal/core/gimbal_api_frame.h
    api/gimbal/core/gimbal_api_frame.hpp
    api/gimbal/containers/gimbal_generics.hpp
    api/gimbal/containers/gimbal_container.hpp
    api/gimbal/core/gimbal_call_stack.h
    api/gimbal/containers/gimbal_linked_list.h
    api/gimbal/meta/gimbal_interface.h
    api/gimbal/objects/gimbal_object.h
    api/gimbal/ifaces/gimbal_iallocator.h
    api/gimbal/ifaces/gimbal_ilogger.h
    api/gimbal/objects/gimbal_context.h
    api/gimbal/objects/gimbal_object.hpp
    api/gimbal/objects/gimbal_module.h
    api/gimbal/types/gimbal_result.hpp
    api/gimbal/types/gimbal_typedefs.h
    api/gimbal/preprocessor/gimbal_compiler.h
    api/gimbal/preprocessor/gimbal_macro_composition.h
    api/gimbal/preprocessor/gimbal_macro_utils.h
    ${GBL_CONFIGURATION_INPUT_PATH}
    ${GIMBAL_CONFIGURATION_OUTPUT_PATH}
    api/gimbal/preprocessor/gimbal_macro_sequences.h
    api/gimbal/core/gimbal_ext.h
    api/gimbal/test/gimbal_test.h
    api/gimbal/types/gimbal_variant.h
    api/gimbal/strings/gimbal_quark.h
    api/gimbal/strings/gimbal_quark.hpp
    api/gimbal/containers/gimbal_hash_set.h
    api/gimbal/containers/gimbal_hash_set.hpp
    api/gimbal/containers/gimbal_hash_map.hpp
    api/gimbal/containers/gimbal_tree_set.h
    api/gimbal/containers/gimbal_array_map.h
    api/gimbal/containers/gimbal_doubly_linked_list.h
    api/gimbal/meta/gimbal_type.h
    api/gimbal/core/gimbal_thread.h
    api/gimbal/core/gimbal_thread.hpp
    api/gimbal/types/gimbal_typedefs.hpp
    api/gimbal/ifaces/gimbal_iplugin.h
    api/gimbal/containers/gimbal_vector.h
    api/gimbal/algorithms/gimbal_sort.h
    api/gimbal/algorithms/gimbal_numeric.h
    api/gimbal/algorithms/gimbal_hash.h
    api/gimbal/containers/gimbal_vector.hpp
    api/gimbal/strings/gimbal_string_buffer.h
    api/gimbal/strings/gimbal_string_view.h
    api/gimbal/strings/gimbal_string_ref.h
    api/gimbal/types/gimbal_ref.h
    api/gimbal/meta/gimbal_type.hpp
    api/gimbal/meta/gimbal_instance.hpp
    api/gimbal/meta/gimbal_interface.hpp
    api/gimbal/meta/gimbal_class.h
    api/gimbal/meta/gimbal_class.hpp
    api/gimbal/meta/gimbal_instance.h
    api/gimbal/meta/gimbal_builtin_types.h
    api/gimbal/meta/gimbal_primitives.h
    api/gimbal/ifaces/gimbal_ivariant.h
    api/gimbal/ifaces/gimbal_ievent_handler.h
    api/gimbal/ifaces/gimbal_ievent_filter.h
    api/gimbal/types/gimbal_closure.h
    api/gimbal/objects/gimbal_property.h
    api/gimbal/ifaces/gimbal_itable.h
    api/gimbal/types/gimbal_version.h
    api/gimbal/strings/gimbal_string.hpp
    api/gimbal/objects/gimbal_context.hpp
    api/gimbal/objects/gimbal_event.h
    api/gimbal/utils/gimbal_cmdline_parser.h
    api/gimbal/preprocessor/gimbal_atomics.h
    api/gimbal/meta/gimbal_boxed.h
    api/gimbal/preprocessor/gimbal_memory_resource.hpp
    api/gimbal/test/gimbal_test_suite.h
    api/gimbal/test/gimbal_test_scenario.h
    source/meta/gimbal_type_.h
)

add_library(libGimbal STATIC
    ${GIMBAL_SOURCES}
    ${GIMBAL_INCLUDES})

add_subdirectory(lib/tinycthread)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)

target_link_libraries(libGimbal tinycthread Threads::Threads)

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
    ${GBL_DEFINES})

configure_file(${GBL_CONFIGURATION_INPUT_PATH}
    ${GIMBAL_CONFIGURATION_OUTPUT_PATH})


