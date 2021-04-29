#ifndef GIMBAL_COMPILER_H
#define GIMBAL_COMPILER_H

#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
#       define GBL_C_18
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
#       define GBL_STATIC_ASSERT(cond) static_assert(cond)
#   else
#       define GBL_STATIC_ASSERT(cond) static_assert(cond, #cond)
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

#if defined(GBL_C_11)
#   include<stdalign.h>
#   define GBL_ALIGNAS(e)           alignas(e)
#   define GBL_ALIGNOF(e)           alignof(e)
#   define GBL_ALLOC_ALIGNED(s, a)  aligned_alloc(s, a)
#elif defined(GBL_CPP_11)
#   define GBL_ALIGNAS(e)           alignas(e)
#   define GBL_ALIGNOF(e)           alignof(e)
#   ifdef GBL_CPP_17
#       define GBL_ALLOC_ALIGNED(s, a)  std::aligned_alloc(s, a)
#   else
#       define GBL_ALLOC_ALIGNED(s, a)  std::malloc(s * a)
#   endif
#else
#   define GBL_ALIGNAS(e)
#   define GBL_ALIGNOF(e)
#   define GBL_ALLOC_ALIGNED(s, a)  malloc(s * a)
#endif

#define GBL_INLINE \
    static inline

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
#   ifndef alloca
#       if defined(__GLIBC__) || defined(__sun) || defined(__CYGWIN__)
#           include <alloca.h>     // alloca
#       elif defined(_WIN32)
#           include <malloc.h>     // alloca
#           ifndef alloca
#               define alloca _alloca  // for clang with MS Codegen
#           endif
#       else
#           include <stdlib.h>     // alloca
#       endif
#   endif
#   define GBL_ALLOCA alloca
#endif

#ifdef GBL_CPP_11
#   define GBL_QUICK_EXIT(c) quick_exit(c)
#elif defined(GBL_C_11)
#   define GBL_QUICK_EXIT(c) quick_exit(c)
#else
#   define GBL_QUICK_EXIT(c) exit(c)
#endif



#endif // GIMBAL_COMPILER_H
