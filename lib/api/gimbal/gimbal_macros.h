#ifndef GIMBAL_MACROS_H
#define GIMBAL_MACROS_H

#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <gimbal/gimbal_config.h>
#include "gimbal_compiler.h"
#include "gimbal_macros_utils.h"

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


#ifdef GBL_API_SHARED_LIB
#   ifdef GBL_API_EXPORTS
#       define GBL_EXPORT GBL_EXPORT_SHARED
#   else
#       define GBL_EXPORT GBL_IMPORT_SHARED
#   endif
#else
#   define GBL_EXPORT
#endif


#define GBL_API GBL_RESULT GBL_EXPORT


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



#define GBL_ASSERT(...) \
    GBL_VA_OVERLOAD_SELECT(GBL_ASSERT, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)





#ifdef __cplusplus
}
#endif

#endif // GIMBAL_MACROS_H
