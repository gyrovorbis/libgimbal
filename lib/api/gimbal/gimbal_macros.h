#ifndef GIMBAL_MACROS_H
#define GIMBAL_MACROS_H

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
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


GBL_MAYBE_UNUSED GBL_INLINE GBL_CONSTEXPR
uint8_t GBL_POW2_NEXT_UINT8(uint8_t n) GBL_NOEXCEPT {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4;
    return n + 1;
}

GBL_MAYBE_UNUSED GBL_INLINE GBL_CONSTEXPR
uint16_t GBL_POW2_NEXT_UINT16(uint16_t n) GBL_NOEXCEPT {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8;
    return n + 1;
}

GBL_MAYBE_UNUSED GBL_INLINE GBL_CONSTEXPR
uint32_t GBL_POW2_NEXT_UINT32(uint32_t n) GBL_NOEXCEPT {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8; n |= n >> 16;
    return n + 1;
}

GBL_MAYBE_UNUSED GBL_INLINE GBL_CONSTEXPR
uint64_t GBL_POW2_NEXT_UINT64(uint64_t n) GBL_NOEXCEPT {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8; n |= n >> 16; n |= n >> 32;
    return n + 1;
}

#if 0
#define GBL_POW2_NEXT(X)                    \
    _Generic((X),                           \
        uint8_t:    GBL_POW2_NEXT_UINT8,    \
        uint16_t:   GBL_POW2_NEXT_UINT16,   \
        uint32_t:   GBL_POW2_NEXT_UINT32,   \
        uint64_t:   GBL_POW2_NEXT_UINT64,   \
        default:    GBL_POW2_NEXT_UINT64    \
    )(X)
#endif


GBL_DEPRECATED("Dis shit's ancient!")
GBL_MAYBE_UNUSED GBL_NODISCARD GBL_NORETURN GBL_INLINE GBL_EXPORT_SHARED
int someFunc(void* GBL_RESTRICT pArray[GBL_STATIC_ARRAY(10)]) GBL_NOEXCEPT
{
    GBL_CONSTEXPR const void* pPtr = 0;
    GBL_STATIC_ASSERT(pPtr == GBL_NULL);

    void* pSomeShit = GBL_ALLOC_ALIGNED(sizeof(void*), GBL_ALIGNOF(void*));

    switch((unsigned long)pSomeShit) {
        default: break; GBL_FALLTHROUGH;
        case 0: break;
    };

    if(pArray) GBL_UNLIKELY {
        pSomeShit = GBL_ALLOCA(1337);
        if(pSomeShit) GBL_LIKELY {
            GBL_ALIGNAS(64) int goodVal;
            GBL_UNUSED(goodVal);
        }
    }
    GBL_QUICK_EXIT(0);
}



#ifdef __cplusplus
}
#endif

#endif // GIMBAL_MACROS_H
