/*! \file
 *  \brief Platform and architecture definitions
 *  \ingroup preprocessor
 *
 *  \author 2026 Falco Girgis
 *  \author 2026 Agustín Bellagamba
 *  \copyright MIT License
 */
#ifndef GIMBAL_PLATFORM_H
#define GIMBAL_PLATFORM_H

#include "gimbal_compiler.h"
#include "gimbal_macro_composition.h"
#include <stdint.h>

// ======== Operating System ========

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    // Common for all Windows platforms
#   define GBL_WIN  1
#   ifdef _WIN64
        // 64-bit Windows only
#       define GBL_WIN64    1
#   else
        // 32-bit Windows only
#       define GBL_WIN32    1
#   endif
#elif __APPLE__
    // Common to all Apple platforms
#   define GBL_APPLE
#   include <TargetConditionals.h>
#   if TARGET_IPHONE_SIMULATOR
        // iOS, tvOS, or watchOS Simulator
#       define GBL_IOS_SIMULATOR     1
#   elif TARGET_OS_MACCATALYST
         // Mac's Catalyst (ports iOS API into Mac, like UIKit).
#       define GBL_MACCATALYST  1
#   elif TARGET_OS_IPHONE
        // iOS, tvOS, or watchOS device
#       define GBL_IOS  1
#   elif TARGET_OS_MAC
        // Other kinds of Apple platforms
#       define GBL_MACOS    1
#   else
#       error "Unknown Apple platform"
#   endif
#elif __ANDROID__
    // Below __linux__ check should be enough to handle Android,
    // but something may be unique to Android.
#   define GBL_ANDROID  1
#elif defined(__DREAMCAST__)
#   define GBL_DREAMCAST    1
#elif defined(__GAMECUBE__)
#   define GBL_GAMECUBE 1
#elif defined(__PSP__)
#   define GBL_PSP  1
#elif defined(VITA)
#   define GBL_PSVITA   1
#elif __linux__
#   define GBL_LINUX    1
#elif __unix__ // all unices not caught above
    // Unix
#   define GBL_UNIX 1
#elif defined(_POSIX_VERSION)
    // POSIX
#   define GBL_POSIX
#endif

// ======== CPU Architecture ========

#if defined(__x86_64__) || defined(_M_X64)
#   define GBL_X86_64   1
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#   define GBL_X86_32       1
#elif defined(__ARM_ARCH_2__)
#   define GBL_ARM2         1
#elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
#   define GBL_ARM3         1
#elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
#   define GBL_ARM4T        1
#elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
#   define GBL_ARM5         1
#elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
#   define GBL_ARM6T2       1
#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
#   define GBL_ARM6         1
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
#   define GBL_ARM7         1
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
#   define GBL_ARM7A        1
#elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
#   define GBL_ARM7R        1
#elif defined(__ARM_ARCH_7M__)
#   define GBL_ARM7M        1
#elif defined(__ARM_ARCH_7S__)
#   define GBL_ARM7S        1
#elif defined(__aarch64__) || defined(_M_ARM64)
#   define GBL_ARM64        1
#elif defined(mips) || defined(__mips__) || defined(__mips)
#   define GBL_MIPS         1
#elif defined(__sh__)
#   define GBL_SUPERH       1
#elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
#   define GBL_POWERPC      1
#elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
#   define GBL_POWERPC64    1
#elif defined(__sparc__) || defined(__sparc)
#   define GBL_SPARC        1
#elif defined(__m68k__)
#   define GBL_M68K         1
#endif

// ======== Pointer Width ========

#if UINTPTR_MAX == 0xffff
#   define GBL_16BIT
#elif UINTPTR_MAX == 0xffffffff
#   define GBL_32BIT
#elif UINTPTR_MAX == 0xffffffffffffffff
#   define GBL_64BIT
#else
    /* wtf, you're on your own, bro */
#endif

// ======== Compiler Support ========

#ifdef _WIN64
#   define GBL_INITIALIZER_PREFIX_ ""
#else
#   define GBL_INITIALIZER_PREFIX_ "_"
#endif

#if defined(__DREAMCAST__) || defined(__GAMECUBE__)
#   define GBL_THREAD_LOCAL_ _Thread_local
#elif defined(__PSP__)
#   define GBL_THREAD_LOCAL_
#else
#   define GBL_THREAD_LOCAL_ __thread
#endif

#ifdef __cplusplus
#   define GBL_INITIALIZER(f) \
        static void f(void); \
        struct f##_t_ { f##_t_(void) { f(); } }; static f##_t_ f##_; \
        static void f(void)
#elif defined(_MSC_VER)
#   pragma section(".CRT$XCU",read)
#   define GBL_INITIALIZER2_(f,p) \
        static void f(void); \
        __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
        __pragma(comment(linker,"/include:" p #f "_")) \
        static void f(void)
#   define GBL_INITIALIZER(f) GBL_INITIALIZER2_(f, GBL_INITIALIZER_PREFIX_)
#else
#   define GBL_INITIALIZER(f) \
        static void f(void) __attribute__((constructor)); \
        static void f(void)
#endif

// ======== Thread-local storage ========
#if defined(TINYCTHREAD_ENABLE_THREADS) && !TINYCTHREAD_ENABLE_THREADS
#   define GBL_THREAD_LOCAL
#elif defined(_MSC_VER)
#   define GBL_THREAD_LOCAL __declspec(thread)
#else
#   define GBL_THREAD_LOCAL GBL_THREAD_LOCAL_
#endif

#if defined(_MSC_VER) || defined(__MINGW64__)
#   define GBL_ALIGNED_ALLOC(a, s)      _aligned_malloc(s, a)
#   define GBL_ALIGNED_REALLOC(p, a, s) _aligned_realloc(p, s, a)
#   define GBL_ALIGNED_FREE(p)          _aligned_free(p)
#elif defined(__MINGW32__)
#   define GBL_ALIGNED_ALLOC(a, s)      __mingw_aligned_malloc(s, a)
#   define GBL_ALIGNED_REALLOC(p, a, s) __mingw_aligned_realloc(p, s, a)
#   define GBL_ALIGNED_FREE(p)          __mingw_aligned_free(p)
#elif defined(GBL_C11) || defined(GBL_CPP17)
#   include <stdlib.h>
#   define GBL_ALIGNED_ALLOC(a, s)      aligned_alloc(a, s)
#   define GBL_ALIGNED_REALLOC(p, a, s) realloc(p, s)
#   define GBL_ALIGNED_FREE(p)          free(p)
#endif

#if GBL_CONFIG_PREFETCH_ENABLED
#   ifndef GBL_PREFETCH
#       if defined(_MSC_VER) || defined(__MINGW64__)
#           if defined(_M_ARM64) || defined(_M_ARM64EC)
#               include <intrin.h>
#           else
#               include <immintrin.h>
#           endif
#           define GBL_PREFETCH(addr) _mm_prefetch(addr, _MM_HINT_T0)
#       elif defined(__GNUC__)
#           define GBL_PREFETCH __builtin_prefetch
#       endif
#   else
#       define GBL_PREFETCH(...)
#   endif
#else
#   define GBL_PREFETCH(...)
#endif

// ======== alloca() ========
#ifndef GBL_ALLOCA
#   if defined(__APPLE__)      || defined(__GLIBC__)    || \
       defined(__sun)          || defined(__CYGWIN__)   || \
       defined(__EMSCRIPTEN__) || defined(VITA)         || \
       defined(__DREAMCAST__)  || defined(PSP)          || \
       defined(__GAMECUBE__)   || defined(GEKKO)        || \
       defined(__DEVKITARM__)
#       include <alloca.h>        // Sane platforms
#   elif defined(_WIN32)
#       include <malloc.h>        // Windoez
#       ifndef alloca
#           define alloca _alloca // Clang with MS Codegen
#       endif
#   else
#       include <stdlib.h>        // BSD extension
#   endif
#   define GBL_ALLOCA alloca
#endif

#if __APPLE__  // at very least fucking MacOS headers are missing it!
#   define GBL_QUICK_EXIT(c) exit(c)
#elif defined(GBL_CPP11)
#   define GBL_QUICK_EXIT(c) quick_exit(c)
#elif defined(GBL_C11)
#   define GBL_QUICK_EXIT(c) quick_exit(c)
#else
#   define GBL_QUICK_EXIT(c) exit(c)
#endif

/* ==========================================================
 * Provide simple macro statement wrappers (adapted from Perl):
 *  G_STMT_START { statements; } G_STMT_END;
 *  can be used as a single statement, as in
 *  if (x) G_STMT_START { ... } G_STMT_END; else ...
 *
 *  For gcc we will wrap the statements within `({' and `})' braces.
 *  For SunOS they will be wrapped within `if (1)' and `else (void) 0',
 *  and otherwise within `do' and `while (0)'.
 *  ========================================================
 *  Graciously stolen from GLib. Imitation is the most sincere form of flattery.
 *          --Falco Girgis
 */
/* REMOVED WHILE TRYING TO AVOID CLANG WARNINGS AND TO BE STANDARD COMPLIANT
 * #  if defined (__GNUC__) && !defined (__STRICT_ANSI__) && !defined (__cplusplus)
#    define GBL_STMT_START    (void) __extension__ (
#    define GBL_STMT_END      )
# */
#if !(defined (GBL_STMT_START) && defined (GBL_STMT_END))
#  if defined (__GNUC__) && !defined (__STRICT_ANSI__) && !defined (__cplusplus)
#    define GBL_STMT_START    do
#    define GBL_STMT_END      while(0)
#  else
#    if (defined (sun) || defined (__sun__))
#      define GBL_STMT_START  if(1)
#      define GBL_STMT_END    else(void)0
#    else
#      define GBL_STMT_START  do
#      define GBL_STMT_END    while(0)
#    endif
#  endif
#endif

// ======== Size Types ========

#ifdef GBL_64BIT
#   define GBL_SIZE_TYPE GBL_UINT64_TYPE
#   define GBL_SIZE_BITS 64
#else
#   define GBL_SIZE_TYPE GBL_UINT32_TYPE
#   define GBL_SIZE_BITS 32
#endif

// ======== Variant ========

#if !defined(GBL_DREAMCAST) && !defined(GBL_GAMECUBE) && !defined(GBL_PSP)
#   define GBL_VARIANT_CONSTRUCT_GENERIC_PLATFORM_ENTRIES()
#else
#   define GBL_VARIANT_CONSTRUCT_GENERIC_PLATFORM_ENTRIES() \
    (int, GblVariant_constructInt32),
#endif


// ======== Time ========

#include <time.h>

#if !defined(GBL_NO_TIMESPEC_)
#   if defined(ANDROID) || defined(__APPLE__) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(VITA) || defined(PSP)
#       define GBL_NO_TIMESPEC_
#   endif
#endif

#ifdef GBL_NO_TIMESPEC_
#   ifndef TIME_UTC
#       define TIME_UTC 1
#   endif

GBL_INLINE int gblTimespecGet_(struct timespec* pSpec, int base) {
    if(base != TIME_UTC)
        return 0;

    if(clock_gettime(CLOCK_REALTIME, pSpec) == -1)
        return 0;

    return TIME_UTC;
}

#   define timespec_get gblTimespecGet_
#endif

// ======== C++ ========

#ifdef GBL_CPP_17
#   if defined(__APPLE__) || defined(__ANDROID__) || defined(__wasm__)
#       define GBL_PMR_HEADER_    <experimental/memory_resource>
#       define GBL_PMR_NAMESPACE_ std::experimental::pmr
#   else
#       define GBL_PMR_HEADER_    <memory_resource>
#       define GBL_PMR_NAMESPACE_ std::pmr
#       define GBL_PMR_VECTOR
#       define GBL_PMR_STRING
#       define GBL_LEX_CMP_3WAY
#   endif
#endif

#ifdef GBL_PMR_VECTOR
#   define GBL_PMR_VECTOR_ONLY_ GBL_EVAL0
#else
#   define GBL_PMR_VECTOR_ONLY_ GBL_MACRO_CALL_NULL
#endif

#ifdef GBL_PMR_STRING
#   define GBL_PMR_STRING_ONLY_ GBL_EVAL0
#else
#   define GBL_PMR_STRING_ONLY_ GBL_MACRO_CALL_NULL
#endif

#ifdef GBL_LEX_CMP_3WAY
#   define GBL_LEXICOGRAPHICAL_COMPARE_ std::lexicographical_compare_three_way
#else
#   define GBL_LEXICOGRAPHICAL_COMPARE_ std::lexicographical_compare
#endif

// ======== Endianness ========

#if GBL_BIG_ENDIAN
#   define GBL_BIT_FIELDS_ENDIAN_XFORM_(...) (__VA_ARGS__)
#else
#   define GBL_BIT_FIELDS_ENDIAN_XFORM_(...) (GBL_REVERSE(__VA_ARGS__))
#endif

// Select the big-endian or little-endian alternative at preprocessing time.
#if GBL_BIG_ENDIAN
#   define GBL_ENDIAN_SELECT_ GBL_ARG_1
#   define GBL_FROM_BIG_ENDIAN_U32_(value)  (value)
#   define GBL_TO_LITTLE_ENDIAN_U16_(value) GBL_BSWAP_U16(value)
#   define GBL_TO_LITTLE_ENDIAN_U32_(value) GBL_BSWAP_U32(value)
#else
#   define GBL_ENDIAN_SELECT_ GBL_ARG_2
#   define GBL_FROM_BIG_ENDIAN_U32_(value)  GBL_BSWAP_U32(value)
#   define GBL_TO_LITTLE_ENDIAN_U16_(value) (value)
#   define GBL_TO_LITTLE_ENDIAN_U32_(value) (value)
#endif

// ======== Unaligned Access ========

// Load a native-endian word from a possibly unaligned address.
// DO NOT DO UNALIGNED WORD ACCESSES ON THE RISC/EMBEDDED CPUS
#if !defined(__DREAMCAST__) && !defined(__GAMECUBE__) && !defined(__VITA__)
GBL_INLINE uint32_t gblLoadUnalignedU32_(const void* pData) {
    return *(const uint32_t*)pData;
}
#else
#   include <string.h>

GBL_INLINE uint32_t gblLoadUnalignedU32_(const void* pData) {
    uint32_t value;
    memcpy(&value, pData, sizeof(value));
    return value;
}
#endif

#endif // GIMBAL_PLATFORM_H
