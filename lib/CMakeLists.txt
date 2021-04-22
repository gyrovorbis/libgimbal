cmake_minimum_required(VERSION 3.10)

project(LibGimbal 
    VERSION 
        ${GIMBAL_VERSION} 
    DESCRIPTION 
        "Lightweight cross-platform framework used for creating low-overhead shared or dynamic libraries with an extensible C API." 
    LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

function(GBL_CONFIG_VARIABLE name value type desc)
    set(${name} ${value} CACHE ${type} ${desc})
    option(${name} ${desc} ${default})
    list(APPEND
        GBL_CONFIGURATION_DEFINES
        ${name}=${value})
    set(GBL_CONFIGURATION_DEFINES  ${GBL_CONFIGURATION_DEFINES} PARENT_SCOPE)
endfunction()

function(GBL_CONFIG_OPTION name desc default)
    option(${name} ${desc} ${default})
    list(APPEND
        GBL_CONFIGURATION_DEFINES
        ${name}=$<BOOL:${${name}}>)
    set(GBL_CONFIGURATION_DEFINES  ${GBL_CONFIGURATION_DEFINES} PARENT_SCOPE)
endfunction()

function(GBL_BUILD_DEFINE var buildDefine)
    #if(NOT ${buildDefine})
        #if(NOT "${var}" STREQUAL "")
            list(APPEND
                GBL_BUILD_DEFINES
                ${buildDefine}=${var})
            set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
        #endif()
   # endif()
endfunction()


function(GBL_GITLAB_CI_DEFINE suffix)
    set(buildVar "GBL_BUILD_CI_${suffix}")
    set(envVar "CI_${suffix}")
    set(env $ENV{${envVar}})
    if("${env}" STREQUAL "")
        set(env "\"?\"")
    endif()
    GBL_BUILD_DEFINE("${env}" ${buildVar})
    set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
    message(${buildVar})
endFunction()

string(TIMESTAMP BUILD_TIME "%Y-%m-%dT%H%M")
    GBL_BUILD_DEFINE("\"${BUILD_TIME}\"" GBL_BUILD_TIMESTAMP)

    GBL_BUILD_DEFINE($<BOOL:$ENV{GITLAB_CI}> GBL_BUILD_CI)

    GBL_BUILD_DEFINE($ENV{PATH} PATH)

    GBL_GITLAB_CI_DEFINE(PROJECT_TITLE)

    GBL_GITLAB_CI_DEFINE(COMMIT_SHA)
    GBL_GITLAB_CI_DEFINE(COMMIT_TAG)
    GBL_GITLAB_CI_DEFINE(COMMIT_REF_NAME)

    GBL_GITLAB_CI_DEFINE(JOB_ID)
    GBL_GITLAB_CI_DEFINE(JOB_NAME)
    GBL_GITLAB_CI_DEFINE(JOB_STAGE)
    GBL_GITLAB_CI_DEFINE(JOB_MANUAL)
    GBL_GITLAB_CI_DEFINE(JOB_TRIGGERED)

    GBL_GITLAB_CI_DEFINE(RUNNER_DESCRIPTION)
    GBL_GITLAB_CI_DEFINE(USER_EMAIL)

    message(${GBL_BUILD_DEFINES})

GBL_CONFIG_VARIABLE(GBL_CONFIGURATION_INPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/api/gimbal/gimbal_config.h.in FILEPATH "Path to user API configuration file")

GBL_CONFIG_OPTION(GBL_CONFIG_LAST_ERROR_ENABLED "Track most recent error in GblHandle" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_SOURCE_TRACKING_ENABLED      "Track calling source code location" ON)

GBL_CONFIG_OPTION(GBL_CONFIG_LOG_SUCCESS_ENABLED           "Log all success results from API calls" OFF)
GBL_CONFIG_OPTION(GBL_CONFIG_LOG_PARTIAL_ENABLED   "Log all partial success results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LOG_ERROR_ENABLED             "Log all error results from API calls" ON)

GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_SUCCESS_LEVEL GBL_LOG_LEVEL_DEBUG STRING "Log level for reporting success results")
GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_PARTIAL_LEVEL GBL_LOG_LEVEL_WARNING STRING "Log level for reporting partial results")
GBL_CONFIG_VARIABLE(GBL_CONFIG_LOG_ERROR_LEVEL GBL_LOG_LEVEL_ERROR STRING "Log level for reporting error results")

GBL_CONFIG_OPTION(GBL_CONFIG_ASSERT_PARTIAL_ENABLED "Assert on all partial success results from API calls" OFF)
GBL_CONFIG_OPTION(GBL_CONFIG_ASSERT_ERROR_ENABLED          "Assert on all error results from API calls" ON)

GBL_CONFIG_OPTION(GBL_CONFIG_LAST_ERROR_SUCCESS_ENABLED    "Update Handle's LastError on all success results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LAST_ERROR_PARTIAL_ENABLED "Update Handle's LastError on all partial success results from API calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_LAST_ERROR_ERROR_ENABLED      "Update Handle's LastError on all error results from API calls" ON)

GBL_CONFIG_OPTION(GBL_CONFIG_EXT_CONTEXT_CALLBACKS_ENABLED    "Context supports runtime user callback EXT functions provided upon creation" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_EXT_CONTEXT_DEFAULT_ENABLED         "Context falls-back to default EXT implementation using standard C library calls" ON)
GBL_CONFIG_OPTION(GBL_CONFIG_EXT_CONTEXT_PARENT_ENABLED          "Context falls-back to calling parent's EXT implementation" ON)

GBL_CONFIG_OPTION(GBL_FEATURE_VARIANTS      "Enable Variants" ON)
GBL_CONFIG_OPTION(GBL_FEATURE_STRINGS       "Enable Strings" ON)
GBL_CONFIG_OPTION(GBL_FEATURE_CONTAINERS    "Enable Containers" ON)

set(GIMBAL_CONFIGURATION_OUTPUT_PATH "${PROJECT_BINARY_DIR}/api/gimbal/gimbal_config.h")

set(GIMBAL_SOURCES
     source/gimbal_version.c
     source/gimbal_context.c
     source/gimbal_test.c
     source/gimbal_handle.c
     source/gimbal_ext.c
)

set(GIMBAL_INCLUDES
    api/gimbal/gimbal_macros.h
    #api/gimbal/gimbal_version.h
    api/gimbal/gimbal_context.h
    api/gimbal/gimbal_result.h
    #api/gimbal/gimbal_os.h
    api/gimbal/gimbal_api.h
    api/gimbal/gimbal_api.hpp
    api/gimbal/gimbal_types.h
    ${GBL_CONFIGURATION_INPUT_PATH}
    ${GIMBAL_CONFIGURATION_OUTPUT_PATH}
    api/gimbal/gimbal_macros_map.h
    #api/gimbal/gimbal_generator.h
    api/gimbal/gimbal_ext.h
    api/gimbal/gimbal_variant.h
    api/gimbal/gimbal_test.h
    api/gimbal/gimbal_handle.h
    api/gimbal/gimbal_types.hpp
    api/gimbal/gimbal_variant.hpp
    api/gimbal/gimbal_object.h
    #api/gimbal/gimbal_table.h
    api/gimbal/gimbal_string.h
    api/gimbal/gimbal_container.h
    api/gimbal/gimbal_object.hpp
    api/gimbal/gimbal_handle.hpp
    api/gimbal/gimbal_context.hpp
    )




list(APPEND
    GBL_DEFINES
    ${GBL_CONFIGURATION_DEFINES}
    ${GBL_BUILD_DEFINES})


add_library(libGimbal STATIC
    ${GIMBAL_SOURCES}
    ${GIMBAL_INCLUDES})

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


