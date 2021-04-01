cmake_minimum_required(VERSION 3.10)

project(LibGimbal 
    VERSION 
        ${GIMBAL_VERSION} 
    DESCRIPTION 
        "Lightweight cross-platform framework used for creating low-overhead shared or dynamic libraries with an extensible C API." 
    LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(GIMBAL_RESULT_ERROR_ASSERT             "Assert upon encountering API errors" ON)
option(GIMBAL_RESULT_ERROR_LOG                "Log API errors" ON)
option(GIMBAL_RESULT_CONTEXT_TRACK_LAST_ERROR "Track most recent error in GIMBALContext" ON)
option(GIMBAL_RESULT_CALL_STACK_TRACKING      "Track calling source code location" ON)


option(GIMBAL_EXT_CALLBACKS "Register function pointers upon context creation for EXT logic" ON)
option(GIMBAL_EXT_EVENTS    "Use event system for EXT logic" ON)
option(GIMBAL_EXT_DEFAULTS  "Use default C standard implementation for EXT logic" ON)


option(GIMBAL_VARIANTS  "Enable variants" ON)
option(GIMBAL_TABLES "Enable Tables" ON)


set(GIMBAL_SOURCES
     source/gimbal_version.c
     source/gimbal_context.c
     source/gimbal_test.c
#    source/util/GIMBAL_context.c
#    source/os/GIMBAL_os_callbacks.c
)

set(GIMBAL_INCLUDES
    api/gimbal/gimbal_macros.h
    api/gimbal/gimbal_build.h
    #api/gimbal/gimbal_version.h
    api/gimbal/gimbal_context.h
    api/gimbal/gimbal_result.h
    #api/gimbal/gimbal_os.h
    api/gimbal/gimbal_api.h
    api/gimbal/gimbal_types.h
    api/gimbal/gimbal_config.h
    #api/gimbal/gimbal_generator.h
    api/gimbal/gimbal_ext.h
    api/gimbal/gimbal_variant.h
    api/gimbal/gimbal_test.h
    api/gimbal/gimbal_handle.h
    api/gimbal/gimbal_types.hpp
    api/gimbal/gimbal_variant.hpp
    api/gimbal/gimbal_object.h
    #api/gimbal/gimbal_table.h
    #api/gimbal/gimbal_string.h
    api/gimbal/gimbal_container.h
    )

#if(FALSE)
#    api/GIMBAL/hw/GIMBAL_battery.h
#    api/GIMBAL/hw/GIMBAL_buzzer.h
#    api/GIMBAL/hw/GIMBAL_cpu.h
#    api/GIMBAL/hw/GIMBAL_device.h
#    api/GIMBAL/hw/GIMBAL_display.h
#    api/GIMBAL/hw/GIMBAL_flash.h
#    api/GIMBAL/hw/GIMBAL_gamepad.h
#    api/GIMBAL/hw/GIMBAL_instr.h
#    api/GIMBAL/hw/GIMBAL_battery.h
#endif()

if(GIMBAL_RESULT_ERROR_ASSERT)
    list(APPEND
        GIMBAL_DEFINES
        GIMBAL_RESULT_ERROR_ASSERT)
endif()

if(GIMBAL_RESULT_ERROR_LOG)
    list(APPEND
        GIMBAL_DEFINES
        GIMBAL_RESULT_ERROR_LOG)
endif()

if(GIMBAL_RESULT_CONTEXT_TRACK_LAST_ERROR)
    list(APPEND
        GIMBAL_DEFINES
        GIMBAL_RESULT_CONTEXT_TRACK_LAST_ERROR)
endif()

if(GIMBAL_RESULT_CALL_STACK_TRACKING)
    list(APPEND
        GIMBAL_DEFINES
        GIMBAL_RESULT_CALL_STACK_TRACKING)
endif()

if(GIMBAL_EXT_CALLBACKS)
    list(APPEND
        GIMBAL_DEFINES
        GIMBAL_EXT_CALLBACKS)
endif()

if(GIMBAL_EXT_EVENTS)
    list(APPEND
        GIMBAL_DEFINES
        GIMBAL_EXT_EVENTS)
endif()

if(GIMBAL_EXT_DEFAULTS)
    list(APPEND
        GIMBAL_DEFINES
        GIMBAL_EXT_DEFAULTS)
endif()


add_library(libGimbal STATIC
    ${GIMBAL_SOURCES}
    ${GIMBAL_INCLUDES})

target_include_directories(libGimbal
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>
        $<INSTALL_INTERFACE:api>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/api
)

target_compile_definitions(libGimbal
    PUBLIC
    ${GIMBAL_DEFINES})


