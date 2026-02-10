


function(GBL_CREATE_APP target)
    if(VITA)
        add_compile_definitions(ElysianVmuTests PUBLIC VITA=1)
        include("${VITASDK}/share/vita.cmake" REQUIRED)

        vita_create_self(eboot.bin ElysianVmuTests UNSAFE)

        vita_create_vpk("EvmuTests_v${EVMU_CORE_VERSION}.vpk" EVMUTESTS0 eboot.bin
            VERSION "01.00"
            NAME "ElysianVMU Tests v${EVMU_CORE_VERSION}"
        )
    elseif(PSP)
        add_compile_definitions(GimbalTests PUBLIC BUILD_PRX=1)

        create_pbp_file(TARGET GimbalTests # CMake executable target
          TITLE "${CMAKE_PROJECT_NAME}" # displayed in game selection
          # ICON_PATH
          # BACKGROUND_PATH
          # PREVIEW_PATH
          )
    elseif(EMSCRIPTEN)
        # Set compile and link flags for the WASM build
        set_target_properties(GimbalTests PROPERTIES
            COMPILE_FLAGS "-pthread -s USE_PTHREADS=1"
            LINK_FLAGS "-pthread -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=8"
        )
    elseif(ANDROID)
        set(CMAKE_AUTOMOC ON)
        find_package(Qt6 COMPONENTS Core Svg REQUIRED)

        qt_add_executable(GimbalTests
            MANUAL_FINALIZATION
            ${GIMBAL_TEST_SOURCES}
        )

        list(APPEND
             GBL_TEST_LINK_LIBS
             Qt6::Core
             Qt6::Svg)

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
endfunction()

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
    GBL_BUILD_DEFINE(${buildVar} "\"${env}\"")
    set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
endFunction()

function(GBL_BUILD_TIMESTAMP_DEFINE varName)
    string(TIMESTAMP BUILD_TIME "%d/%m/%Y %H:%M:%S")
    GBL_BUILD_DEFINE(${varName} "\"${BUILD_TIME}\"")
    set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
endfunction()

function(GBL_BUILD_BIG_ENDIAN_DEFINE varName)
    if(CMAKE_C_BYTE_ORDER STREQUAL "BIG_ENDIAN")
        GBL_BUILD_DEFINE(${varName} 1)
    else()
        GBL_BUILD_DEFINE(${varName} 0)
    endif()
    set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
endfunction()

#function(GBL_BUILD_HOST_INFO_DEFINE buildVar keyName)
#    cmake_host_system_information(RESULT ${buildVar} QUERY ${keyName})
#    if (NOT ${${buildVar}} MATCHES "^[0-9]+$")
#        set(${buildVar} "\"${${buildVar}}\"")
#    endif()
#    GBL_BUILD_DEFINE(${buildVar} "${${buildVar}}")
#    set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
#endfunction()

function(GBL_TARGET_PROPERTY_DEFINE def tgt prop)
    get_target_property(${def} ${tgt} ${prop})
    GBL_BUILD_DEFINE(${def} "\"${${def}}\"")
    set(${def} ${${def}} PARENT_SCOPE)
    set(GBL_BUILD_DEFINES ${GBL_BUILD_DEFINES} PARENT_SCOPE)
endfunction()

