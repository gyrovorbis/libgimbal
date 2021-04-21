#ifndef GIMBAL_MACROS_H
#define GIMBAL_MACROS_H

#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <gimbal/gimbal_config.h>
#include "gimbal_macros_map.h"

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
#else
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

#ifdef __cplusplus
extern "C" {
#endif

#define GBL_FORWARD_DECLARE_STRUCT(S)  \
    struct S;                          \
    typedef struct S S

#ifndef __cplusplus
#   define GBL_DECLARE_ENUM(E)     \
        typedef GBL_ENUM E;        \
        enum E
#else
#   define GBL_DECLARE_ENUM(E) \
    enum E : GBL_ENUM
#endif

#define GBL_FORWARD_DECLARE_ENUM(E) \
    typedef GBL_ENUM E

#define GBL_HANDLE void*
#define GBL_HANDLE_INVALID  NULL

#define GBL_DECLARE_OPAQUE(S)   \
    struct S##_;                \
    typedef struct S##_* S

//sizeof(emptyStruct) is 1 in Cpp, 0 in C!
#define GBL_DEFINE_EMPTY_STRUCT(S)  \
    typedef struct S {              \
        char cppCompat;             \
    } S

#define GBL_INLINE \
    static inline

#define GBL_UNUSED(a) \
    (void)(a)

#define GBL_DECL_VAR_TYPE(type, ...) \
    type
#define GBL_DECL_VAR_NAME(type, name) \
    name

#define GBL_DECL_VAR(type, name) \
  GBL_DECL_VAR_TYPE(type, name) GBL_DECL_VAR_NAME(type, name)

#define GBL_DECL_VAR_PAIR_TYPE(pair) \
    GBL_DECL_VAR_TYPE pair

#define GBL_DECL_VAR_PAIR_NAME(pair) \
    GBL_DECL_VAR_NAME pair

#define GBL_DECL_VAR_PAIR(pair) \
  GBL_DECL_VAR pair


#ifdef GBL_API_SHARED_LIB
#   ifdef _MSC_VER
#       ifdef GBL_API_EXPORTS
#           define GBL_EXPORT __declspec(dllexport)
#       else
#           define GBL_EXPORT __declspec(dllimport)
#       endif
#   elif defined(__clang__) || defined(__GNUC__)
#       define GBL_EXPORT __attribute__((visibility("default")))
#   else
#       define GBL_EXPORT
#   endif
#else
#   define GBL_EXPORT
#endif

#define GBL_API GBL_RESULT GBL_EXPORT

#define GBL_VA_ARGS(...) ,##__VA_ARGS__
#define _GBL_STRINGIFY(a) #a
#define GBL_STRINGIFY(a) _GBL_STRINGIFY(a)


#define GBL_CAT(A, B) A##B

#define GBL_MACRO_CALL(macro, ...)                  macro(__VA_ARGS__)
#define GBL_MACRO_CALL_NULL(...)                       

#define GBL_MACRO_CONDITIONAL_CALL_0(macro, ...) GBL_MACRO_CALL_NULL()
#define GBL_MACRO_CONDITIONAL_CALL_1(macro, ...) GBL_MACRO_CALL(macro, __VA_ARGS__)

#define GBL_MACRO_CONDITIONAL_CALL(enabled, ...) \
    GBL_CAT(GBL_MACRO_CONDITIONAL_CALL_,enabled)(__VA_ARGS__)

#define GBL_IDENTITY(A) A

#define GBL_APPEND_SUFFIX(NAME, NUM) GBL_CAT(NAME##_, NUM)
#define GBL_COMPOSE(NAME, ARGS) NAME ARGS

#define GBL_VA_SLIDING_ARG(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, COUNT, ...) COUNT
#define GBL_COMMA  ,
#define GBL_EXPAND() ,,,,,,,,,, // 10 commas (or 11 empty tokens)

#define GBL_VA_OVERLOAD_SUFFIXER(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, ...) \
    GBL_COMPOSE(GBL_VA_SLIDING_ARG, (EXPAND __VA_ARGS__ (), _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) )

#define GBL_VA_OVERLOAD_SUFFIXER_ARGC(...) \
    GBL_VA_OVERLOAD_SUFFIXER(0, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, __VA_ARGS__)

#define GBL_VA_OVERLOAD_SUFFIXER_1_N(...) \
    GBL_VA_OVERLOAD_SUFFIXER(0, N, N, N, N, N, N, N, N, N, 1, __VA_ARGS__)

#define GBL_VA_OVERLOAD_SUFFIXER_0_N(...) \
    GBL_VA_OVERLOAD_SUFFIXER(N, N, N, N, N, N, N, N, N, N, 0, __VA_ARGS__)

#define GBL_VA_OVERLOAD_SUFFIXER_2_N(...) \
    GBL_VA_OVERLOAD_SUFFIXER(0, N, N, N, N, N, N, N, N, 2, 1, __VA_ARGS__)

#define GBL_VA_OVERLOAD_SUFFIXER_5_N(...) \
    GBL_VA_OVERLOAD_SUFFIXER(0, N, N, N, N, N, 5, 4, 3, 2, 1, __VA_ARGS__)


#define GBL_VA_OVERLOAD_SELECT(BASE, SUFFIXER, ...) \
    GBL_APPEND_SUFFIX(BASE, SUFFIXER(__VA_ARGS__))

#define GBL_VA_OVERLOAD_CALL(BASE, SUFFIXER, ...) \
    GBL_VA_OVERLOAD_SELECT(BASE, SUFFIXER, __VA_ARGS__)(__VA_ARGS__)


#define GBL_VERSION_FIELD_MAJOR_BIT                 24u
#define GBL_VERSION_FIELD_MAJOR_MASK                0xf

#define GBL_VERSION_FIELD_MINOR_BIT                 8u
#define GBL_VERSION_FIELD_MINOR_MASK                0xff

#define GBL_VERSION_FIELD_PATCH_BIT                 0u
#define GBL_VERSION_FIELD_PATCH_MASK                0xf

#define GBL_VERSION_MAKE(major, minor, patch)                                               \
    (uint32_t)((major & GBL_VERSION_FIELD_MAJOR_MASK) << GBL_VERSION_FIELD_MAJOR_BIT)   |   \
    (uint32_t)((minor & GBL_VERSION_FIELD_MINOR_MASK) << GBL_VERSION_FIELD_MINOR_BIT)   |   \
    (uint32_t)((patch & GBL_VERSION_FIELD_PATCH_MASK) << GBL_VERSION_FIELD_PATCH_BIT)

#define GBL_VERSION_EXTRACT_MAJOR(version)                                                  \
    (uint8_t)((uint32_t)((version >> GBL_VERSION_FIELD_MAJOR_BIT) & GBL_VERSION_FIELD_MAJOR_MASK))

#define GBL_VERSION_EXTRACT_MINOR(version)                                                  \
    (uint16_t)((uint32_t)((version >> GBL_VERSION_FIELD_MINOR_BIT) & GBL_VERSION_FIELD_MINOR_MASK))

#define GBL_VERSION_EXTRACT_PATCH(version)                                                  \
    (uint8_t)((uint32_t)((version >> GBL_VERSION_FIELD_PATCH_BIT) & GBL_VERSION_FIELD_PATCH_MASK))

#define GBL_VERSION_STRINGIFY(major, minor, patch) \
    GBL_STRINGIFY(major) "."                       \
    GBL_STRINGIFY(minor) "."                       \
    GBL_STRINGIFY(patch)

#define GBL_SWITCH_CASE_STRINGIFY(s) \
    case s: return #s

#define GBL_VA_SNPRINTF(pFormat)                            \
    char buffer[GBL_VA_SNPRINTF_BUFFER_SIZE] = { '\0' };    \
    va_list argptr;                                         \
    va_start(argptr, pFormat);                              \
    const int vsnprintfBytes =                              \
        vsnprintf(buffer, sizeof(buffer), pFormat, argptr); \
    va_end(argptr)


/* A mask with x least-significant bits set, possibly 0 or >=32, PPC and endian-safe!
https://stackoverflow.com/questions/8774567/c-macro-to-create-a-bit-mask-possible-and-have-i-found-a-gcc-bug
*/
#define GBL_BIT_COUNT_MASK(x)               \
    (((x) >= sizeof(unsigned) * CHAR_BIT) ? \
        (unsigned) -1 : (1U << (x)) - 1)

#define GBL_BIT_FIELD_MASK(a, b)                            \
      (((unsigned) -1 >> (31 - (b))) & ~((1U << (a)) - 1))

//#define GBL_BIT_MASK(...) \
  //    GBL_VA_OVERLOAD_SUFFIXER_ARGC(GBL_BIT_COUNT_MASK, GBL_BIT_FIELD_MASK, __VA_ARGS__)

#define GBL_ASSERT(...) \
    GBL_VA_OVERLOAD_SELECT(GBL_ASSERT, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)





#ifdef __cplusplus
}
#endif

#endif // GIMBAL_MACROS_H
