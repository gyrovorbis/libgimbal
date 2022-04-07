cmake_minimum_required(VERSION 3.10)

project(LibGimbal 
    VERSION 
        ${GIMBAL_VERSION} 
    DESCRIPTION 
        "Lightweight cross-platform framework for creating static/dynamic libraries with a run-time extensible C API"
    HOMEPAGE_URL
        "elysianshadows.com"
    LANGUAGES C)


SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)


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
GBL_CONFIG_OPTION(GBL_CONFIG_SOURCE_TRACKING_ENABLED      "Track calling source code location" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_ERRNO_CHECKS              "Enable C errno verification macros" ON)

GBL_CONFIG_OPTION(GBL_CONFIG_LOG_SUCCESS_ENABLED           "Log all success results from API calls" OFF)
GBL_CONFIG_OPTION(GBL_CONFIG_LOG_PARTIAL_ENABLED   "Log all partial success results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LOG_ERROR_ENABLED             "Log all error results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LOG_UNKNOWN_ENABLED             "Log all unknown results from API calls" ON)

GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_SUCCESS_LEVEL GBL_LOG_LEVEL_DEBUG STRING "Log level for reporting success results")
GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_PARTIAL_LEVEL GBL_LOG_LEVEL_WARNING STRING "Log level for reporting partial results")
GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_ERROR_LEVEL GBL_LOG_LEVEL_ERROR STRING "Log level for reporting error results")
GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_UNKNOWN_LEVEL GBL_LOG_LEVEL_ERROR STRING "Log level for reporting unknown results")

GBL_CONFIG_OPTION(GBL_CONFIG_ASSERT_PARTIAL_ENABLED "Assert on all partial success results from API calls" OFF)
GBL_CONFIG_OPTION(GBL_CONFIG_ASSERT_ERROR_ENABLED          "Assert on all error results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_ASSERT_UNKNOWN_ENABLED          "Assert on all unknown results from API calls" OFF)

GBL_CONFIG_OPTION(GBL_CONFIG_LAST_CALL_RECORD_SUCCESS_ENABLED    "Update Handle's LastCallRecord on all success results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LAST_CALL_RECORD_PARTIAL_ENABLED "Update Handle's LastCallRecord on all partial success results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LAST_CALL_RECORD_ERROR_ENABLED      "Update Handle's LastCallRecord on all error results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LAST_CALL_RECORD_UNKNOWN_ENABLED      "Update Handle's LastCallRecord on all unknown results from API calls" ON)

GBL_CONFIG_OPTION(GBL_CONFIG_EXT_CONTEXT_CALLBACKS_ENABLED    "Context supports runtime user callback EXT functions provided upon creation" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_EXT_CONTEXT_DEFAULT_ENABLED         "Context falls-back to default EXT implementation using standard C library calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_EXT_CONTEXT_PARENT_ENABLED          "Context falls-back to calling parent's EXT implementation" ON)

GBL_CONFIG_OPTION(GBL_FEATURE_VARIANTS      "Enable Variants" ON)
GBL_CONFIG_OPTION(GBL_FEATURE_STRINGS       "Enable Strings" ON)
GBL_CONFIG_OPTION(GBL_FEATURE_CONTAINERS    "Enable Containers" ON)

set(GIMBAL_CONFIGURATION_OUTPUT_PATH "${PROJECT_BINARY_DIR}/api/gimbal/core/gimbal_config.h")

set(GIMBAL_SOURCES
     source/gimbal_version.c
     source/gimbal_context.c
#     source/gimbal_test.c
     source/gimbal_handle.c
     source/gimbal_thread.c
     source/gimbal_ivariant.c
     source/gimbal_variant.c
#     source/gimbal_meta.c
#     source/gimbal_object.c
#     source/gimbal_context_object.c
     source/gimbal_ext.c
     source/gimbal_type.c
     source/gimbal_value_types.c
     source/gimbal_hash_set.c
     source/gimbal_tree_set.c
     source/gimbal_generics.cpp
#     source/meta2/gimbal_object.c
     lib/hashmap.c/hashmap.c
     lib/btree.c/btree.c
     lib/jenkins/lookup3.c
)

set(GIMBAL_INCLUDES
    api/gimbal/core/gimbal_api_generators.h
    #api/gimbal/gimbal_version.h
    api/gimbal/objects/gimbal_context.h
    api/gimbal/types/gimbal_buffer.h
    api/gimbal/types/gimbal_buffer.hpp
    api/gimbal/types/gimbal_result.h
    api/gimbal/core/gimbal_call_stack.hpp
    api/gimbal/core/gimbal_api_generators.hpp
    api/gimbal/types/gimbal_version.hpp
    api/gimbal/types/gimbal_exception.hpp
    #api/gimbal/gimbal_os.h
    api/gimbal/core/gimbal_api_frame.h
    api/gimbal/core/gimbal_api_frame.hpp
    api/gimbal/containers/gimbal_generics.hpp
    api/gimbal/containers/gimbal_container.hpp
    api/gimbal/core/gimbal_call_stack.h
    api/gimbal/containers/gimbal_list.h
    api/gimbal/meta/gimbal_interface.h
    api/gimbal/meta/gimbal_object.h
    api/gimbal/types/gimbal_result.hpp
    api/gimbal/types/gimbal_typedefs.h
    api/gimbal/preprocessor/gimbal_compiler.h
    api/gimbal/preprocessor/gimbal_macro_composition.h
    api/gimbal/preprocessor/gimbal_macro_utils.h
    ${GBL_CONFIGURATION_INPUT_PATH}
    ${GIMBAL_CONFIGURATION_OUTPUT_PATH}
    api/gimbal/preprocessor/gimbal_macro_sequences.h
    api/gimbal/core/gimbal_ext.h
    api/gimbal/types/gimbal_variant.h
    api/gimbal/util/gimbal_test.h
    api/gimbal/objects/gimbal_handle.h
    api/gimbal/containers/gimbal_hash_set.h
    api/gimbal/containers/gimbal_hash_set.hpp
    api/gimbal/containers/gimbal_hash_map.hpp
    api/gimbal/containers/gimbal_tree_set.h
    api/gimbal/containers/gimbal_table.h
    api/gimbal/meta/gimbal_type.h
#    api/gimbal/util/gimbal_cmdline_parser.h
#    api/gimbal/core/gimbal_runtime.h
    api/gimbal/core/gimbal_log.h
    api/gimbal/core/gimbal_thread.h
    api/gimbal/core/gimbal_thread.hpp
    api/gimbal/types/gimbal_typedefs.hpp
    api/gimbal/types/gimbal_variant.hpp
#    api/gimbal/objects/gimbal_instance.h
    api/gimbal/objects/gimbal_object.h
    api/gimbal/meta/gimbal_plugin.h
#    api/gimbal/objects/gimbal_context_object.h
#    api/gimbal/util/gimbal_monitor.hpp
    api/gimbal/containers/gimbal_table.h
    api/gimbal/containers/gimbal_vector.h
    api/gimbal/algorithms/gimbal_sort.h
    api/gimbal/algorithms/gimbal_numeric.h
    api/gimbal/algorithms/gimbal_hash.h
    api/gimbal/containers/gimbal_vector.hpp
#    api/gimbal/meta/gimbal_boxed.h
#    api/gimbal/meta/gimbal_weak_ref.h
#    api/gimbal/util/gimbal_lson.h
    api/gimbal/types/gimbal_string.h
    #api/gimbal/ifaces/gimbal_icomponent.h
    #api/gimbal/ifaces/gimbal_ichild.h
    #api/gimbal/ifaces/gimbal_iparent.h
    #api/gimbal/ifaces/gimbal_iallocator.h
    #api/gimbal/ifaces/gimbal_ilogger.h
    #api/gimbal/ifaces/gimbal_ievent_handler.h
    #api/gimbal/ifaces/gimbal_ilast_error.h
    #api/gimbal/ifaces/gimbal_ivalue.h
    #api/gimbal/ifaces/gimbal_icontext.h
    #api/gimbal/ifaces/gimbal_iobject.h
#    api/gimbal/meta2/gimbal_object.h
#    api/gimbal/meta2/gimbal_class.h
    api/gimbal/meta2/gimbal_type.h
    api/gimbal/meta/gimbal_type.hpp
    api/gimbal/meta/gimbal_instance.hpp
    api/gimbal/meta/gimbal_interface.hpp
    api/gimbal/meta/gimbal_class.h
    api/gimbal/meta/gimbal_class.hpp
    api/gimbal/meta/gimbal_instance.h
    api/gimbal/meta/gimbal_ivariant.h
    api/gimbal/meta/gimbal_variant.h
#    api/gimbal/meta2/gimbal_primitive.h
    #api/gimbal/ifaces/gimbal_inamed.h
    #api/gimbal/ifaces/gimbal_icontainer.h
    #api/gimbal/ifaces/gimbal_iuserdata.h
    #api/gimbal/ifaces/gimbal_iiterable.h
    #api/gimbal/ifaces/gimbal_itable.h
#    api/gimbal/types/gimbal_dispatcher.h
    api/gimbal/types/gimbal_version.h
#    api/gimbal/types/gimbal_value.h
#    api/gimbal/util/gimbal_alloc.h
    api/gimbal/types/gimbal_string.hpp
#    api/gimbal/containers/gimbal_container.h
    api/gimbal/objects/gimbal_object.hpp
    api/gimbal/objects/gimbal_handle.hpp
    api/gimbal/objects/gimbal_context.hpp
    lib/hashmap.c/hashmap.h
    lib/btree.c/btree.h
    lib/jenkins/lookup3.h)

add_library(libGimbal STATIC
    ${GIMBAL_SOURCES}
    ${GIMBAL_INCLUDES})

add_subdirectory(lib/interface99)
target_link_libraries(libGimbal interface99)

#set_property(TARGET libGimbal PROPERTY C_STANDARD 11)
#set_property(TARGET libGimbal PROPERTY C_STANDARD_REQUIRED ON)

GBL_TARGET_PROPERTY_DEFINE(GBL_BUILD_C_STANDARD libGimbal C_STANDARD)
GBL_TARGET_PROPERTY_DEFINE(GBL_BUILD_CPP_STANDARD libGimbal CXX_STANDARD)
GBL_TARGET_PROPERTY_DEFINE(GBL_BUILD_C_EXTENSIONS libGimbal C_EXTENIONS)
GBL_TARGET_PROPERTY_DEFINE(GBL_BUILD_CPP_EXTENSIONS libGimbal CXX_EXTENSIONS)

list(APPEND
    GBL_DEFINES
    ${GBL_CONFIGURATION_DEFINES}
    ${GBL_BUILD_DEFINES})


target_include_directories(libGimbal
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>
        $<INSTALL_INTERFACE:api>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/api>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/metalang99/include>
        $<INSTALL_INTERFACE:lib/metalang99/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/lib/metalang99/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/C99-Lambda>
        $<INSTALL_INTERFACE:lib/C99-Lambda>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/lib/C99-Lambda>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
        $<INSTALL_INTERFACE:lib>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/lib>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/api
)

target_compile_definitions(libGimbal
    PUBLIC
    ${GBL_DEFINES})

configure_file(${GBL_CONFIGURATION_INPUT_PATH}
    ${GIMBAL_CONFIGURATION_OUTPUT_PATH})


