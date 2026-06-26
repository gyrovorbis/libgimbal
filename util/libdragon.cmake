set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR mips64)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(N64_INST $ENV{N64_INST})
set(PLATFORM_N64 ON)

if(NOT N64_INST)
    message(FATAL_ERROR "N64_INST environment variable not set. Please point it to your libdragon toolchain directory.")
endif()

set(N64_TARGET mips64-elf)
set(CMAKE_SYSROOT "${N64_INST}/${N64_TARGET}")

set(CMAKE_C_COMPILER   "${N64_INST}/bin/${N64_TARGET}-gcc")
set(CMAKE_CXX_COMPILER "${N64_INST}/bin/${N64_TARGET}-g++")
set(CMAKE_AR           "${N64_INST}/bin/${N64_TARGET}-gcc-ar")
set(CMAKE_RANLIB       "${N64_INST}/bin/${N64_TARGET}-ranlib")

add_compile_definitions(N64=1)

add_compile_options(
    -std=gnu17
    -march=vr4300
    -mtune=vr4300
    -mabi=o64
    -O2
    -g
    -falign-functions=32
    -ffunction-sections
    -fdata-sections
    -fdiagnostics-color=always
    -ftrivial-auto-var-init=pattern
    -ffile-prefix-map=${CMAKE_SOURCE_DIR}=
)

link_libraries(dragon dragonsys m c)

add_link_options(
    -Tn64.ld
    -Wl,--gc-sections
    -Wl,--wrap=__do_global_ctors
)

# Usage:
#   add_n64_rom(target_name
#       [TITLE "My Game"]
#       [SAVETYPE none|eeprom4k|eeprom16|sram256k|sram768k|sram1m|flashram]
#       [FILESYSTEM path/to/assets]
#       [RTC]
#   )
function(add_n64_rom TARGET_NAME)
    set(options RTC)
    set(oneValueArgs TITLE SAVETYPE FILESYSTEM)
    cmake_parse_arguments(ROM "${options}" "${oneValueArgs}" "" ${ARGN})

    if(NOT ROM_TITLE)
        set(ROM_TITLE "Made with libdragon")
    endif()

    set(ELF_FILE $<TARGET_FILE:${TARGET_NAME}>)
    set(Z64_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.z64)
    set(SYM_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.sym)
    set(STRIPPED_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.stripped)

    set(N64TOOL_ARGS --title "${ROM_TITLE}" --toc)

    if(ROM_SAVETYPE)
        list(APPEND N64TOOL_ARGS --save ${ROM_SAVETYPE})
    endif()

    if(ROM_RTC)
        list(APPEND N64TOOL_ARGS --rtc)
    endif()

    set(BUILD_COMMANDS
        COMMAND ${N64_INST}/bin/n64sym ${ELF_FILE} ${SYM_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy ${ELF_FILE} ${STRIPPED_FILE}
        COMMAND ${N64_INST}/bin/${N64_TARGET}-strip -s ${STRIPPED_FILE}
        COMMAND ${N64_INST}/bin/n64elfcompress -o ${CMAKE_CURRENT_BINARY_DIR} -c 1 ${STRIPPED_FILE}
    )

    set(PACK_ARGS --align 256 ${STRIPPED_FILE})

    if(ROM_FILESYSTEM)
        if(NOT IS_ABSOLUTE ${ROM_FILESYSTEM})
            set(ROM_FILESYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/${ROM_FILESYSTEM}")
        endif()

        set(DFS_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.dfs)

        list(APPEND BUILD_COMMANDS
            COMMAND ${N64_INST}/bin/mkdfs ${DFS_FILE} ${ROM_FILESYSTEM}
        )
        list(APPEND PACK_ARGS --align 16 ${DFS_FILE})
    endif()

    list(APPEND PACK_ARGS --align 8 ${SYM_FILE} --align 8)

    list(APPEND BUILD_COMMANDS
        COMMAND ${N64_INST}/bin/n64tool ${N64TOOL_ARGS} --output ${Z64_FILE} ${PACK_ARGS}
    )

    add_custom_command(
        OUTPUT ${Z64_FILE}
        ${BUILD_COMMANDS}
        DEPENDS ${TARGET_NAME}
        COMMENT "Packaging N64 ROM: ${TARGET_NAME}.z64"
    )

    add_custom_target(${TARGET_NAME}_rom ALL DEPENDS ${Z64_FILE})
endfunction()