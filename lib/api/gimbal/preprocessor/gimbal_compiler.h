/*! \file
 *  \brief Helper Definitions for language, compiler, and platform
 *  \ingroup preprocessor
 *  \sa gimbal_macro_utilities.h
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_COMPILER_H
#define GIMBAL_COMPILER_H

#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

// C Version
#ifdef __STDC_VERSION__
#   define GBL_C_89
#   if (__STDC_VERSION__ >= 199409L)
#       define GBL_C_95
#   endif
#   if (__STDC_VERSION__ >= 199901L)
#       define GBL_C_99
#   endif
#   if (__STDC_VERSION__ >= 201112L)
#       define GBL_C_11
#   endif
#   if (__STDC_VERSION__ >= 201710L)
#       define GBL_C_17
#   endif
#endif

//C++ Version
#ifdef __cplusplus
#   if (__cplusplus >= 199711L)
#       define GBL_CPP_98
#   endif
#   if (__cplusplus >= 201103L)
#       define GBL_CPP_11
#   endif
#   if (__cplusplus >= 201402L)
#       define GBL_CPP_14
#   endif
#   if (__cplusplus >= 201703L)
#       define GBL_CPP_17
#   endif
#   if (__cplusplus >= 202002L)
#       define GBL_CPP_20
#   endif
#endif

#ifdef GBL_CPP_11
#   define GBL_NULL nullptr
#else
#   define GBL_NULL NULL
#endif

#if UINTPTR_MAX == 0xffff
#   define GBL_16BIT
#elif UINTPTR_MAX == 0xffffffff
#   define GBL_32BIT
#elif UINTPTR_MAX == 0xffffffffffffffff
#   define GBL_64BIT
#else
    /* wtf, you're on your own, bro */
#endif

// Shared library symbol imports/exports
#ifdef _MSC_VER
#   define GBL_EXPORT_SHARED __declspec(dllexport)
#   define GBL_IMPORT_SHARED __declspec(dllimport)
#elif defined(__clang__) || defined(__GNUC__)
#   define GBL_EXPORT_SHARED __attribute__((visibility("default")))
#   define GBL_IMPORT_SHARED
#else
#   define GBL_EXPORT_SHARED
#   define GBL_IMPORT_SHARED
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
#   ifdef _WIN64
        #define GBL_INITIALIZER(f) GBL_INITIALIZER2_(f,"")
#   else
        #define GBL_INITIALIZER(f) GBL_INITIALIZER2_(f,"_")
#   endif
#else
#   define GBL_INITIALIZER(f) \
        static void f(void) __attribute__((constructor)); \
        static void f(void)
#endif

// Thread-local storage
#ifdef _MSC_VER
#   define GBL_THREAD_LOCAL __declspec(thread)
#else
#   if defined(__DREAMCAST__)
#       define GBL_THREAD_LOCAL _Thread_local
#   else
#       define GBL_THREAD_LOCAL __thread
#   endif
#endif

//====== C++ FEATURE SUPPORT ==============

// RTTI
#ifdef __cpp_rtti
#   define GBL_CPP_RTTI 1
#elif defined(__clang__)
#   if __has_feature(cxx_rtti)
#       define GBL_CPP_RTTI 1
#   else
#       define GBL_CPP_RTTI 0
#   endif
#elif defined(__GNUG__) && defined(__GXX_RTTI)
#   define GBL_CPP_RTTI 1
#elif defined(_MSC_VER) && defined(_CPPRTTI)
#   define GBL_CPP_RTTI 1
#else
#   define GBL_CPP_RTTI 0
#endif

// Exceptions
#ifdef __cpp_exceptions
#   define GBL_CPP_EXCEPTIONS  1
#   define GBL_NOEXCEPT noexcept
#else
#   define GBL_CPP_EXCEPTIONS  0
#   define GBL_NOEXCEPT
#endif

#ifdef GBL_CPP_11
#   define GBL_CONSTEXPR        constexpr
#   ifdef GBL_CPP_20
#       define GBL_CONSTEVAL    consteval
#   else
#       define GBL_CONSTEVAL    constexpr
#   endif
#else
#   define GBL_CONSTEXPR
#   define GBL_CONSTEVAL
#endif

// Concepts
#ifdef __cpp_concepts
#   define GBL_CPP_CONCEPTS 1
#else
#   define GBL_CPP_CONCEPTS 0
#endif


// Source Location
#ifdef __cpp_lib_source_location
#   define GBL_CPP_SOURCE_LOCATION 1
#else
#   define GBL_CPP_SOURCE_LOCATION 0
#endif

// Deprecated
#if defined(__has_cpp_attribute)
#   if __has_cpp_attribute(deprecated)
#       define GBL_DEPRECATED(msg) [[deprecated(msg)]]
#   else
#       define GBL_DEPRECATED(msg)
#   endif
#else
#   define GBL_DEPRECATED(msg)
#endif

// Fallthrough
#if defined(__has_cpp_attribute)
#   if __has_cpp_attribute(fallthrough)
#       define GBL_FALLTHROUGH [[fallthrough]]
#   else
#       define GBL_FALLTHROUGH
#   endif
#else
#   define GBL_FALLTHROUGH
#endif

// Likely
#if defined(__has_cpp_attribute)
#   if __has_cpp_attribute(likely)
#       define GBL_LIKELY [[likely]]
#   else
#       define GBL_LIKELY
#   endif
#else
#   define GBL_LIKELY
#endif


// Maybe_Unused
#if defined(__has_cpp_attribute)
#   if __has_cpp_attribute(maybe_unused)
#       define GBL_MAYBE_UNUSED [[maybe_unused]]
#   else
#       define GBL_MAYBE_UNUSED
#   endif
#elif defined(__clang__) || defined(__GNUC__)
#   define GBL_MAYBE_UNUSED __attribute__((unused))
#else
#   define GBL_MAYBE_UNUSED
#endif

// No_Unique_Address
#if defined(__has_cpp_attribute)
#   if __has_cpp_attribute(no_unique_address)
#       define GBL_NO_UNIQUE_ADDRESS [[no_unique_address]]
#   else
#       define GBL_NO_UNIQUE_ADDRESS
#   endif
#else
#   define GBL_NO_UNIQUE_ADDRESS
#endif

// No_Discard
#if defined(__has_cpp_attribute)
#   if __has_cpp_attribute(nodiscard)
#       define GBL_NODISCARD [[nodiscard]]
#   else
#       define GBL_NODISCARD
#   endif
#else
#   define GBL_NODISCARD
#endif

// No_Return (Cpp)
#if defined(__has_cpp_attribute)
#   if __has_cpp_attribute(noreturn)
#       define GBL_NORETURN [[noreturn]]
#   else
#       define GBL_NORETURN
#   endif
# else
#   ifdef GBL_C_11
#       define GBL_NORETURN _Noreturn
#   else
#       define GBL_NORETURN
#   endif
#endif

// Unlikely
#if defined(__has_cpp_attribute)
#   if __has_cpp_attribute(unlikely)
#       define GBL_UNLIKELY [[unlikely]]
#   else
#       define GBL_UNLIKELY
#   endif
#else
#   define GBL_UNLIKELY
#endif

#if __cpp_static_assert
#   if __cpp_static_assert > 201400
#       define GBL_STATIC_ASSERT(cond)      static_assert(cond)
#   else
#       define GBL_STATIC_ASSERT(cond)      static_assert(cond, #cond)
#   endif
#   define GBL_STATIC_ASSERT_MSG(cond, msg) static_assert(cond, msg)
#elif defined(GBL_C_STD_11)
#   define GBL_STATIC_ASSERT(cond)          _Static_assert(cond, #cond)
#   define GBL_STATIC_ASSERT_MSG(cond, msg) _Static_assert(cond, msg)
#else
#   define GBL_STATIC_ASSERT(cond)
#   define GBL_STATIC_ASSERT_MSG(cond, msg)
#endif

#ifdef GBL_C_99
#   define GBL_RESTRICT restrict
#else
#   define GBL_RESTRICT
#endif

#ifdef GBL_C_99
#   define GBL_STATIC_ARRAY(idx) static idx
#else
#   define GBL_STATIC_ARRAY(idx) idx
#endif

#ifdef __cplusplus
#   define GBL_STRUCT_INIT(type, ...)   (type{ __VA_ARGS__ })
#else
#   define GBL_STRUCT_INIT(type, ...)   ((type){ __VA_ARGS__ })
#endif

#ifdef __cplusplus
#   define GBL_ALIGNAS(e) alignas(e)
#   define GBL_ALIGNOF(e) alignof(e)
#elif defined(GBL_C_11)
#   define GBL_ALIGNAS(e) _Alignas(e)
#   define GBL_ALIGNOF(e) _Alignof(e)
#endif

#if defined(_MSC_VER) || defined(__MINGW64__)
#   define GBL_ALIGNED_ALLOC(a, s)      _aligned_malloc(s, a)
#   define GBL_ALIGNED_REALLOC(p, a, s) _aligned_realloc(p, s, a)
#   define GBL_ALIGNED_FREE(p)          _aligned_free(p)
#elif defined(__MINGW32__)
#   define GBL_ALIGNED_ALLOC(a, s)      __mingw_aligned_malloc(s, a)
#   define GBL_ALIGNED_REALLOC(p, a, s) __mingw_aligned_realloc(p, s, a)
#   define GBL_ALIGNED_FREE(p)          __mingw_aligned_free(p)
#elif defined(GBL_C_11) || defined(GBL_CPP_17)
#   include <stdlib.h>
#   define GBL_ALIGNED_ALLOC(a, s)      aligned_alloc(a, s)
#   define GBL_ALIGNED_REALLOC(p, a, s) realloc(p, s)
#   define GBL_ALIGNED_FREE(p)          free(p)
#endif

#if GBL_CONFIG_PREFETCH_ENABLED
#   ifndef GBL_PREFETCH
#       if defined(_MSC_VER) || defined(__MINGW64__)
#           include <immintrin.h>
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

#ifdef __cplusplus
#   define GBL_INLINE_ inline
#else
#   define GBL_INLINE_ static inline
#endif

#ifdef __cplusplus
#   define GBL_INLINE GBL_MAYBE_UNUSED GBL_INLINE_
#else
#   define GBL_INLINE GBL_MAYBE_UNUSED GBL_INLINE_
#endif

#ifdef __GNUC__
#   define GBL_FORCE_INLINE __attribute__((always_inline)) GBL_INLINE_
#elif defined(_MSC_VER)
#   define GBL_FORCE_INLINE __forceinline
#else
#   define GBL_FORCE_INLINE GBL_INLINE
#endif

// ====== NONSTANDARD COMPILER-SPECIFIC C FUNCTIONS ==========

// "Safer" C11 functions with boundaries
#if 0
#ifdef __STDC_WANT_LIB_EXT1__
#define GBL_C11_EXT1    1
#define GBL_VPRINTF     vprintf_s
#define GBL_VFPRINTF    vfprintf_s
#define GBL_VSPRINTF    vsprintf_s
#define GBL_VSNPRINTF   vsnprintf_s
#define GBL_MEMSET      memset_s
#define GBL_MEMCPY      memcpy_s
#else
#define GBL_C11_EXT1    0
#define GBL_VPRINTF     vprintf
#define GBL_VFPRINTF    vfprintf
#define GBL_VSPRINTF    vsprintf
#define GBL_VSNPRINTF   vsnprintf
#define GBL_MEMSET      memset
#define GBL_MEMCPY      memcpy
#endif
#endif

// alloca()
#ifndef GBL_ALLOCA
//#   ifndef alloca
#       if defined(__APPLE__) || defined(__GLIBC__) || defined(__sun) || defined(__CYGWIN__) || defined(__EMSCRIPTEN__) || defined(VITA) || defined(__DREAMCAST__)
#           include <alloca.h>     // alloca
#       elif defined(_WIN32)
#           include <malloc.h>     // alloca
#           ifndef alloca
#               define alloca _alloca  // for clang with MS Codegen
#           endif
#       else
#           include <stdlib.h>     // alloca
#       endif
//#   endif
#   define GBL_ALLOCA alloca
#endif

#if __APPLE__  // at very least fucking MacOS headers are missing it!
#   define GBL_QUICK_EXIT(c) exit(c)
#elif defined(GBL_CPP_11)
#   define GBL_QUICK_EXIT(c) quick_exit(c)
#elif defined(GBL_C_11) 
#   define GBL_QUICK_EXIT(c) quick_exit(c)
#else
#   define GBL_QUICK_EXIT(c) exit(c)
#endif

#ifndef GBL_PRAGMA_MACRO_PUSH
#   define GBL_PRAGMA_MACRO_PUSH(X) push_macro(X)
#endif

#ifndef GBL_PRAGMA_MACRO_POP
#   define GBL_PRAGMA_MACRO_POP(X) pop_macro(X)
#endif

#ifdef _MSC_VER
#   define GBL_MAX_ALIGN_T double
#else
#   define GBL_MAX_ALIGN_T max_align_t
#endif

#define GBL_ALLOC_MIN_SIZE GBL_ALIGNOF(GBL_MAX_ALIGN_T)

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

#define GBL_NULL_TERMINATED

// Low-Level BitMask Operations
#ifdef GBL_CPP_20
#   include <bit>
#   define GBL_BITMASK_CLZ(mask) std::countl_zero(mask)
#   define GBL_BITMASK_CTZ(mask) std::countr_zero(mask)
    inline constexpr auto GBL_BITMASK_FFS(auto mask) noexcept {
        const auto idx = GBL_BITMASK_CLZ(mask);
        return idx? idx + 1 : 0;
    }
#   define GBL_BITMASK_POPCOUNT(mask) std::popcount(mask)
#else
#   define GBL_BITMASK_POPCOUNT_SW(mask)                                           \
        (((mask) >= sizeof(unsigned long) * CHAR_BIT) ?                            \
            (unsigned long) -1 : (1u << (mask)) - 1)

#   if defined(__clang__) || defined(__GNUC__)
#       define GBL_BITMASK_CLZ(mask)        __builtin_clz(mask)         // count leading zeroes
#       define GBL_BITMASK_CTZ(mask)        __builtin_ctz(mask)         // count trailing zeroes
#       define GBL_BITMASK_FFS(mask)        __builtin_ffs(mask)         // find first set
#       define GBL_BITMASK_POPCOUNT(mask)   __builtin_popcount(mask)    // count number of 1s
#   elif defined(_MSC_VER)
#       include <intrin.h>
        GBL_CONSTEXPR GBL_INLINE unsigned GBL_BITMASK_CLZ(unsigned mask) GBL_NOEXCEPT {
            unsigned long idx = 0;
            return _BitScanReverse(&idx, mask)? ((sizeof(unsigned) * 8) - idx) : 0;
        }
        GBL_CONSTEXPR GBL_INLINE unsigned GBL_BITMASK_CTZ(unsigned mask) GBL_NOEXCEPT {
            unsigned long idx = 0;
            return _BitScanForward(&idx, mask)? idx : 0;
        }
        GBL_CONSTEXPR GBL_INLINE unsigned GBL_BITMASK_FFS(unsigned mask) GBL_NOEXCEPT {
            const unsigned idx = GBL_BITMASK_CTZ(mask);
            return idx? idx + 1 : 0;
        }
#       define GBL_BITMASK_POPCOUNT(mask) GBL_BITMASK_POPCOUNT_SW(mask)
#   else
#      define GBL_BITMASK_CLZ(mask)     // implement me in software later TODO
#      define GBL_BITMASK_CTZ(mask)
#      define GBL_BITMASK_FFS(mask)
#      define GBL_BITMASK_POPCOUNT(mask) GBL_BITMASK_POPCOUNT_SW(mask)
#   endif
#endif

#endif // GIMBAL_COMPILER_H
