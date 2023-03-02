function(GBL_ESCAPE_REGEX OUT_NAME INPUT)
    foreach(CHAR "^$[]()\"\\")
        string(REPLACE "${CHAR}" "\\${CHAR}" INPUT ${INPUT})
    endforeach()
    set(${OUT_NAME} ${INPUT} PARENT_SCOPE)
endfunction()

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
    set(GBL_CONFIGURATION_DEFINES ${GBL_CONFIGURATION_DEFINES} PARENT_SCOPE)
    if(${${name}})
    list(APPEND
        GBL_CONFIGURATION_ENABLED
        ${name})
    set(GBL_CONFIGURATION_ENABLED ${GBL_CONFIGURATION_ENABLED} PARENT_SCOPE)
    endif()
endfunction()

function(GBL_BUILD_DEFINE buildDefine var)
    list(APPEND
        GBL_BUILD_DEFINES
        ${buildDefine}=${var})
    set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
endfunction()

function(GBL_GITLAB_CI_DEFINE buildVar envVar)
    set(env $ENV{${envVar}})
    if("${env}" STREQUAL "")
        set(env "?")
    endif()
    set(env "${env}")
    GBL_ESCAPE_REGEX(env, "${env}")
    GBL_BUILD_DEFINE(${buildVar} "${env}")
    set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
endFunction()

function(GBL_BUILD_TIMESTAMP_DEFINE varName)
    string(TIMESTAMP BUILD_TIME "%d/%m/%Y %H:%M:%S")
    GBL_BUILD_DEFINE(${varName} "\"${BUILD_TIME}\"")
    set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
endfunction()

#include(TestBigEndian)

function(GBL_BUILD_BIG_ENDIAN_DEFINE varName)
#    TEST_BIG_ENDIAN(${varName})
    if(CMAKE_C_BYTE_ORDER EQUAL BIG_ENDIAN)
        GBL_BUILD_DEFINE(${varName} 1)
    else()
        GBL_BUILD_DEFINE(${varName} 0)
    endif()
    set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
endfunction()

function(GBL_BUILD_HOST_INFO_DEFINE buildVar keyName)
    cmake_host_system_information(RESULT ${buildVar} QUERY ${keyName})
    if (NOT ${${buildVar}} MATCHES "^[0-9]+$")
        set(${buildVar} "\"${${buildVar}}\"")
    endif()
    GBL_BUILD_DEFINE(${buildVar} "${${buildVar}}")
    set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
endfunction()

function(GBL_TARGET_PROPERTY_DEFINE def tgt prop)
    get_target_property(${def} ${tgt} ${prop})
    GBL_BUILD_DEFINE(${def} "\"${${def}}\"")
    set(${def} ${${def}} PARENT_SCOPE)
    set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
endfunction()

