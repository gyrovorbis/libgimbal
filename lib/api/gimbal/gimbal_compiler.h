#ifndef GIMBAL_COMPILER_H
#define GIMBAL_COMPILER_H

// C Version
#ifndef __cplusplus
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
#else

//C++ Version
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
#else
#   define GBL_CPP_EXCEPTIONS  0
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
#       define GBL_NO_DISCARD [[nodiscard]]
#   else
#       define GBL_NO_DISCARD
#   endif
#else
#   define GBL_NO_DISCARD
#endif

// No_Return
#if defined(__has_cpp_attribute)
#   if __has_cpp_attribute(noreturn)
#       define GBL_NO_RETURN [[noreturn]]
#   else
#       define GBL_NO_RETURN
#   endif
#else
#   define GBL_NO_RETURN
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
#else
#   define GBL_STATIC_ASSERT(cond)
#   define GBL_STATIC_ASSERT_MSG(cond, msg)
#endif


// ====== NONSTANDARD COMPILER-SPECIFIC C FUNCTIONS ==========

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






#endif // GIMBAL_COMPILER_H
