/*! \file
 *  \brief Helper Definitions for language and compiler
 *  \ingroup preprocessor
 *  \sa gimbal_macro_utilities.h
 *
 *  This header contains an assortment of preprocessor definitions
 *  for the following:
 *      - Compiler type
 *      - C/C++ language revision
 *      - C/C++ language features
 *      - Compiler extensions and features
 *
 *  \author    2023, 2025 Falco Girgis
 *  \copyright MIT LIicense
 */

#ifndef GIMBAL_COMPILER_H
#define GIMBAL_COMPILER_H

#define __STDC_WANT_LIB_EXT1__ 1

#include <stddef.h>
#include <stdint.h>

// C Version
#ifdef __STDC_VERSION__
#   define GBL_C89      1
#   if (__STDC_VERSION__ >= 199409L)
#       define GBL_C95  1
#   endif
#   if (__STDC_VERSION__ >= 199901L)
#       define GBL_C99  1
#   endif
#   if (__STDC_VERSION__ >= 201112L)
#       define GBL_C11  1
#   endif
#   if (__STDC_VERSION__ >= 201710L)
#       define GBL_C17  1
#   endif
#   if (__STDC_VERSION__ >= 202311L)
#       define GBL_C23  1
#   endif
#endif

//C++ Version
#ifdef __cplusplus
#   if (__cplusplus >= 199711L)
#       define GBL_CPP98    1
#   endif
#   if (__cplusplus >= 201103L)
#       define GBL_CPP11    1
#   endif
#   if (__cplusplus >= 201402L)
#       define GBL_CPP14    1
#   endif
#   if (__cplusplus >= 201703L)
#       define GBL_CPP17    1
#   endif
#   if (__cplusplus >= 202002L)
#       define GBL_CPP20    1
#   endif
#   if (__cplusplus >= 202302L)
#       define GBL_CPP23    1
#   endif
#endif

#ifdef __GNUC__
#   define GBL_GNUC 1
#endif

#ifdef _MSC_VER
#   define GBL_MSVC 1
#elif defined(__ICC)
#   define GBL_ICC 1
#elif defined(__clang__)
#   define GBL_CLANG 1
#elif defined(__MINGW32__)
#   define GBL_MINGW32  1
#elif defined(__MINGW64__)
#   define GBL_MINGW64  1
#elif defined(__EMSCRIPTEN__)
#   define GBL_EMSCRIPTEN 1
#elif defined(__GNUC__)
#   define GBL_GCC  1
#elif defined(__ghs__)
#   define GBL_GREENHILL    1
#elif defined(__CC_ARM)
#   define GBL_ARMCC    1
#elif defined(__IAR_SYSTEMS__ICC__)
#   define GBL_IAR  1
#endif

#ifdef GBL_CPP11
#   define GBL_NULL nullptr
#else
#   define GBL_NULL NULL
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

// Floating-point precision directives
#ifdef _MSC_VER
#   define GBL_FP_FAST \
        _Pragma("float_control(except, off)")       /* disable environment sensitivity */\
        _Pragma("fenv_access(off)")                 /* disable exception semantics */\
        _Pragma("float_control(precise, off)")      /* disable precise semantics */\
        _Pragma("fp_contract(on)")                  /* enable contractions */
#   define GBL_FP_PRECISE \
        _Pragma("float_control(precise, on)")       /* enable precise semantics */\
        _Pragma("fenv_access(on)")                  /* enable environment sensitivity */\
        _Pragma("float_control(except, on)")        /* enable exception semantics */
#else
#   define GBL_FP_FAST      __attribute__((optimize("-ffast-math")))
#   define GBL_FP_PRECISE   __attribute__((optimize("-fno-fast-math")))
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

#ifdef GBL_CPP11
#   define GBL_CONSTEXPR        constexpr
#   ifdef GBL_CPP20
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
#       define GBL_LIKELY(exp) (exp) [[likely]]
#   endif
#endif
#ifndef GBL_LIKELY
#   ifdef GBL_GNUC
#       define GBL_LIKELY(exp)  (__builtin_expect(!!(exp), 1))
#   else
#       define GBL_LIKELY(exp) (exp)
#   endif
#endif

// unlikely
#if defined(__has_cpp_attribute)
#   if __has_cpp_attribute(unlikely)
#       define GBL_UNLIKELY(exp) (exp) [[unlikely]]
#   endif
#endif
#ifndef GBL_UNLIKELY
#   ifdef GBL_GNUC
#       define GBL_UNLIKELY(exp)  (__builtin_expect(!!(exp), 0))
#   else
#       define GBL_UNLIKELY(exp) (exp)
#   endif
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
#   ifdef GBL_C11
#       define GBL_NORETURN _Noreturn
#   else
#       define GBL_NORETURN
#   endif
#endif

#if __cpp_static_assert
#   if __cpp_static_assert > 201400
#       define GBL_STATIC_ASSERT(cond)      static_assert(cond)
#   else
#       define GBL_STATIC_ASSERT(cond)      static_assert(cond, #cond)
#   endif
#   define GBL_STATIC_ASSERT_MSG(cond, msg) static_assert(cond, msg)
#elif defined(GBL_C11)
#   define GBL_STATIC_ASSERT(cond)          _Static_assert(cond, #cond);
#   define GBL_STATIC_ASSERT_MSG(cond, msg) _Static_assert(cond, msg);
#else
#   define GBL_STATIC_ASSERT(cond)
#   define GBL_STATIC_ASSERT_MSG(cond, msg)
#endif

#ifdef GBL_C99
#   define GBL_RESTRICT restrict
#else
#   define GBL_RESTRICT
#endif

#ifdef GBL_C99
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
#elif defined(GBL_C11)
#   define GBL_ALIGNAS(e) _Alignas(e)
#   define GBL_ALIGNOF(e) _Alignof(e)
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

#ifdef __GNUC__
#   define GBL_NO_INLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#   define GBL_NO_INLINE __declspec(noinline)
#else
#   define GBL_NO_INLINE
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

#define GBL_NULL_TERMINATED

#if GBL_MSVC
#   include <stdlib.h>
#   define GBL_BSWAP_U16(v) _byteswap_ushort(v)
#   define GBL_BSWAP_U32(v) _byteswap_ulong(v)
#   define GBL_BSWAP_U64(v) _byteswap_uint64(v)
#else
#   define GBL_BSWAP_U16(v) __builtin_bswap16(v)
#   define GBL_BSWAP_U32(v) __builtin_bswap32(v)
#   define GBL_BSWAP_U64(v) __builtin_bswap64(v)
#endif

// Low-Level BitMask Operations
#ifdef GBL_CPP20
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
