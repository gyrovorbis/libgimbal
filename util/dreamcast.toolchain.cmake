SET(CMAKE_SYSTEM_NAME Generic)

SET(PLATFORM_DREAMCAST TRUE)

SET(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_CROSSCOMPILING TRUE)

IF(WIN32)
set(CMAKE_C_COMPILER $ENV{KOS_CC_BASE}/bin/sh-elf-gcc.exe)
set(CMAKE_CXX_COMPILER $ENV{KOS_CC_BASE}/bin/sh-elf-g++.exe)
ELSE()
set(CMAKE_C_COMPILER $ENV{KOS_CC_BASE}/bin/sh-elf-gcc)
set(CMAKE_CXX_COMPILER $ENV{KOS_CC_BASE}/bin/sh-elf-g++)
ENDIF()

add_compile_options(-ml -mfsca -mfsrra -funsafe-math-optimizations -ffinite-math-only -m4-single-only -ffast-math -ffunction-sections -fdata-sections )

IF($ENV{KOS_SUBARCH} MATCHES naomi)
set(CMAKE_EXE_LINKER_FLAGS " -ml -mfsca -mfsrra -funsafe-math-optimizations -ffinite-math-only -m4-single-only -ffast-math -Wl,-Ttext=0x8c020000 -Wl,--gc-sections -T$ENV{KOS_BASE}/utils/ldscripts/shlelf-naomi.xc -nodefaultlibs" CACHE INTERNAL "" FORCE)
ELSE()
set(CMAKE_EXE_LINKER_FLAGS " -ml -mfsca -mfsrra -funsafe-math-optimizations -ffinite-math-only -m4-single-only -ffast-math -Wl,-Ttext=0x8c010000 -Wl,--gc-sections -T$ENV{KOS_BASE}/utils/ldscripts/shlelf.xc -nodefaultlibs" CACHE INTERNAL "" FORCE)
ENDIF()

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_SYSTEM_INCLUDE_PATH "${CMAKE_SYSTEM_INCLUDE_PATH} $ENV{KOS_BASE}/include $ENV{KOS_BASE}/kernel/arch/dreamcast/include $ENV{KOS_BASE}/addons/include $ENV{KOS_BASE}/../kos-ports/include")

INCLUDE_DIRECTORIES(
    $ENV{KOS_BASE}/include
    $ENV{KOS_BASE}/kernel/arch/dreamcast/include
    $ENV{KOS_BASE}/addons/include
    $ENV{KOS_BASE}/../kos-ports/include
)

LINK_DIRECTORIES(
    $ENV{KOS_BASE}/addons/lib/dreamcast
    $ENV{KOS_PORTS}/lib
)

IF(${CMAKE_BUILD_TYPE} MATCHES Debug)
LINK_DIRECTORIES($ENV{KOS_BASE}/lib/dreamcast/debug)
ELSE()
LINK_DIRECTORIES($ENV{KOS_BASE}/lib/dreamcast)
ENDIF()


add_link_options(-L$ENV{KOS_BASE}/lib/dreamcast)
link_libraries(-Wl,--gc-sections -Wl,--start-group -lstdc++ -lkallisti -lc -lgcc -Wl,--end-group m)

SET(CMAKE_EXECUTABLE_SUFFIX ".elf")
SET(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

ADD_DEFINITIONS(
    -D__DREAMCAST__
    -DDREAMCAST
    -D_arch_dreamcast
    -D__arch_dreamcast
    -D_arch_sub_$ENV{KOS_SUBARCH} 
)

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    ADD_DEFINITIONS(-DNDEBUG)
    add_compile_options(-fomit-frame-pointer -O3)
else()
    add_compile_options(-g -DFRAME_POINTERS -fno-omit-frame-pointer -Os)
endif()

SET(CMAKE_ASM_FLAGS "")
SET(CMAKE_ASM_FLAGS_RELEASE "")


