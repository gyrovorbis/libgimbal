cmake_minimum_required(VERSION 3.10)

set(EVMU_VERSION_MAJOR 2)
set(EVMU_VERSION_MINOR 0)
set(EVMU_VERSION_PATCH 0)
set(EVMU_VERSION
  ${EVMU_VERSION_MAJOR}.${EVMU_VERSION_MINOR}.${EVMU_VERSION_PATCH})

project(LibElysianVMU VERSION ${EVMU_VERSION} DESCRIPTION "Sega Dreamcast Visual Memory Unit Emulation Core Library" LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(EVMU_RESULT_ERROR_ASSERT             "Assert upon encountering API errors" ON)
option(EVMU_RESULT_ERROR_LOG                "Log API errors" ON)
option(EVMU_RESULT_CONTEXT_TRACK_LAST_ERROR "Track most recent error in EVMUContext" ON)
option(EVMU_RESULT_CALL_STACK_TRACKING      "Track calling source code location" ON)

set(EVMU_SOURCES
    source/evmu_version.c
    source/util/evmu_context.c
    source/os/evmu_os_callbacks.c)

set(EVMU_INCLUDES
    api/evmu/evmu_defines.h
    api/evmu/evmu_version.h
    api/evmu/util/evmu_context.h
    api/evmu/util/evmu_result.h
    api/evmu/os/evmu_os.h)

#if(FALSE)
#    api/evmu/hw/evmu_battery.h
#    api/evmu/hw/evmu_buzzer.h
#    api/evmu/hw/evmu_cpu.h
#    api/evmu/hw/evmu_device.h
#    api/evmu/hw/evmu_display.h
#    api/evmu/hw/evmu_flash.h
#    api/evmu/hw/evmu_gamepad.h
#    api/evmu/hw/evmu_instr.h
#    api/evmu/hw/evmu_battery.h
#endif()

if(EVMU_RESULT_ERROR_ASSERT)
    list(APPEND
        EVMU_DEFINES
        EVMU_RESULT_ERROR_ASSERT)
endif()

if(EVMU_RESULT_ERROR_LOG)
    list(APPEND
        EVMU_DEFINES
        EVMU_RESULT_ERROR_LOG)
endif()

if(EVMU_RESULT_CONTEXT_TRACK_LAST_ERROR)
    list(APPEND
        EVMU_DEFINES
        EVMU_RESULT_CONTEXT_TRACK_LAST_ERROR)
endif()

if(EVMU_RESULT_CALL_STACK_TRACKING)
    list(APPEND
        EVMU_DEFINES
        EVMU_RESULT_CALL_STACK_TRACKING)
endif()

add_library(libLibElysianVMU STATIC
    ${EVMU_SOURCES}
    ${EVMU_INCLUDES})

target_include_directories(libLibElysianVMU
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>
        $<INSTALL_INTERFACE:api>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/api
)

target_compile_definitions(libLibElysianVMU
    PUBLIC
    ${EVMU_DEFINES})


