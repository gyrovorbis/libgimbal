#ifndef GIMBAL_API_H
#define GIMBAL_API_H

#include "gimbal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GBL_FORWARD_DECLARE_STRUCT(S)  \
    struct S;                          \
    typedef struct S S

#define GBL_DECLARE_ENUM(E)     \
    typedef GBL_ENUM E;         \
    enum E

#define GBL_FORWARD_DECLARE_ENUM(E) \
    typedef GBL_ENUM E

#define GBL_DECLARE_HANDLE(S)   \
    struct S##_;                \
    typedef struct S##_* S

//sizeof(emptyStruct) is 1 in Cpp, 0 in C!
#define GBL_DEFINE_EMPTY_STRUCT(S)  \
    typedef struct S {              \
        char cppCompat;             \
    } S
    

#ifdef _MSC_VER
#    ifdef GBL_API_EXPORTS
#        define GBL_EXPORT __declspec(dllexport)
#    else
#        define GBL_EXPORT __declspec(dllimport)
#    endif
#elif defined(__clang__) || defined(__GNUC__)
#    define GBL_EXPORT __attribute__((visibility("default")))
#else
#    define GBL_EXPORT
#endif

#define GBL_API GBL_RESULT GBL_EXPORT

#define GBL_API_COOKIE() GBL_API_COOKIE_NAME_DEFAULT
#define GBL_API_CTX()    GBL_API_COOKIE().pContext
#define GBL_API_CTX_UD() GBL_API_CTX()->createInfo.userdata
#define GBL_API_RESULT() GBL_API_COOKIE().result

#define GBL_API_MALLOC(size, alignment) void
#define GBL_API_REALLOC(pPtr, size, alignment) void
#define GBL_API_FREE(pPtr) void

#define GBL_API_PUSH(pFmt, ...) void
#define GBL_API_POP(count) void
#define GBL_API_DEBUG(pFmt, ...) void
#define GBL_API_VERBOSE(pFmt, ...) void
#define GBL_API_INFO(pFmt, ...) void
#define GBL_API_WARN(pFmt, ...)
#define GBL_API_ERROR(pFmt, ...) void

// Differentiate between API entrypoints and just random internal function entrypoints!!!

#define GBL_API_BEGIN(pCtx, pFmt, ...)    \
    GblApiCookie GBL_API_COOKIE_NAME_DEFAULT = { pCtx, NULL, initialResult, 0 };   \
    GblContextApiCookiePush(GBL_API_CTX(), &GBL_API_COOKIE());      \
    GBL_API_VERBOSE(pFmt, __VA_ARGS__);                                \
    GBL_API_PUSH()

#define GBL_API_DONE()  \
    goto GBL_API_END_LABEL_DEFAULT

#define GBL_API_END()                             \
    GBL_API_END_LABEL_DEFAULT:                    \
        GBL_API_POP(GBL_API_COKIE().stackDepth);  \
        GblContextApiCookiePop(GBL_API_CTX());    \
        return GBL_API_RESULT()




#define GBL_API_RESULT_ACCUM(result) void

#define GBL_API_RESULT_SET(result, pFmt, ...) void
#define GBL_API_RESULT_SET_CND(exp, result, pFmt, ...) void
#define GBL_API_RESULT_SET_JMP(result, label, pFmt, ...) void
#define GBL_API_RESULT_SET_JMP_CND(expr, result, label, pFmt, ...) void

#define GBL_API_VERIFY(expr, label, pFmt, ...) void
#define GBL_API_VERIFY_POINTER(pPtr, label, pFmt, ...) void
#define GBL_API_VERIFY_HANDLE(pHandle, label, pFmt, ...) void

#define GBL_API_ASSERT(expr) assert(expr)
//#define GBL_API_TRY()










#if 0 // WORK ON ME


//overload to fuck
#define GIM_API_BEGIN(pCtx, pDev, initialResult, pFmt, ...)            \
    GIMApiCookie GIM_API_COOKIE_NAME_DEFAULT = { pCtx, pDev, NULL, initialResult, 0 };   \
    GIMContextApiCookiePush_(GIM_API_CTX(), &GIM_API_COOKIE());      \
    GIM_API_VERBOSE(pFmt, __VA_ARGS__);                                \
    GIM_API_PUSH()

#define GBL_API_BEGIN(pCtx, initialResult, pFmt, ...)            \
    GblApiCookie GIM_API_COOKIE_NAME_DEFAULT = { pCtx, NULL, initialResult, 0 };   \
    GblContextApiCookiePush_(GIM_API_CTX(), &GIM_API_COOKIE());      \
    Gbl_API_VERBOSE(pFmt, __VA_ARGS__);                                \
    Gbl_API_PUSH()

#define GIM_API_COOKIE() GIM_API_COOKIE_NAME_DEFAULT
#define GIM_API_CTX()    GIM_API_COOKIE().pContext
#define GIM_API_CTX_UD()
#define GIM_API_RESULT() GIM_API_COOKIE().result

#define GIM_API_END()                              \
    GIM_API_END_LABEL_DEFAULT:                         \
        GIM_API_POP(GIM_API_COKIE().stackDepth);  \
        GIMContextApiCookiePop_(GIM_API_CTX());   \
        return GIM_API_RESULT()

#define GIM_API_ERROR(result, ...) //log/assert/cache error, store in cookie, but then continue


#define GIM_API_FAIL(result, ...) // Error PLUS goto

#define GIM_API_ASSERT(exp, ...) // fail with fixed result type

//#define GIM_API_EXIT(code) exit(code)

#define GIM_API_VALIDATE(exp, result, label, ...) \
    do {                                                                    \
        if(!GIMResultHandler(pCtx, pDev, exp, result,     \
                              GIM_FILE, GIM_FUNC, GIM_LINE,              \
                              __VA_ARGS__)) {                     \
            GIM_API_RESULT() == result;    \
            goto label;         \
        }           \
    } while(0)

#define GIM_API_VALIDATE_ARG(argName, exp, ...) \
    GIM_API_VALIDATE(exp, GIM_RESULT_ERROR_INVALID_ARG, GIM_API_END_LABEL_DEFAULT, __VA_ARGS__)

#define GIM_API_VALIDATE_HANDLE(pHandle, pFmt, ...)

#define GIM_API_TRY()

#define GIM_API_PUSH(...) \
    do {   \
        if(GIM_RESULT_SUCCESS(GIM_LOG_PUSH(GIM_API_CTX(), __VA_ARGS__))) {   \
            ++GIM_API_COOKIE().stackDepth;     \
        }   \
    } while(0)


#define GIM_API_POP(count) \
    do {   \
        if(GIM_RESULT_SUCCESS(GIM_LOG_POP(GIM_API_CTX(), count))) {   \
            --GIM_API_COOKIE().stackDepth;     \
        }   \
    } while(0)

#define GIM_API_LOG()


#define GIM_API_MALLOC()
#define GIM_API_REALLOC()
#define GIM_API_FREE()

// All top-level utilities to be used by any client/user of the gimbal API

// validation/control flow/logging macros

#endif


#ifdef __cplusplus
}
#endif

#endif // GIM_API_H
