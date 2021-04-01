#ifndef GIMBAL_API_H
#define GIMBAL_API_H

#include "gimbal_config.h"
#include "gimbal_macros.h"

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

#define GBL_DECLARE_HANDLE(S)   \
    struct S##_;                \
    typedef struct S##_* S

//sizeof(emptyStruct) is 1 in Cpp, 0 in C!
#define GBL_DEFINE_EMPTY_STRUCT(S)  \
    typedef struct S {              \
        char cppCompat;             \
    } S
    
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

#define GBL_API_COOKIE() GBL_API_COOKIE_NAME_DEFAULT
#define GBL_API_CTX()    GBL_API_COOKIE().pContext
#define GBL_API_CTX_UD() GBL_API_CTX()->createInfo.userdata
#define GBL_API_RESULT() GBL_API_COOKIE().result


#define GBL_API_SOURCE_LOCATION_PUSH(FILE, FUNCTION, LINE, COLUMN)              \
    gblApiStackFrameSourceLocationCurrentPush(&GBL_API_COOKIE(),    \
                    GBL_SOURCE_LOCATION(GBL_SOURCE_FILE, GBL_SOURCE_FUNC, GBL_SOURCE_LINE, GBL_SOURCE_COLUMN))

#define GBL_API_SOURCE_LOCATION_POP() \
    gblApiStackFrameSourceLocationCurrentPop(&GBL_API_COOKIE())

#define GBL_API_SOURCE_TRACKED(FILE, FUNCTION, LINE, COLUMN, CALL) \
    do {    \
        GBL_API_SOURCE_LOCATION_PUSH(FILE, FUNCTION, LINE, COLUMN); \
        (CALL)  \
        GBL_API_SOURCE_LOCATION_POP()   \
    while(0)

#define GBL_API_ASSERT(expr) assert(expr)

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


    static inline EVMUBool evmuResultHandler(const EVMUContext*     pCtx,
                                             const EVMUDevice*      pDevice,
                                             EVMUBool               exp,
                                             EVMU_RESULT            result,
                                             const char*            pFile,
                                             const char*            pFunc,
                                             uint64_t               line,
                                             const char*            pFormat,
                                            ...)
    {
        if(!exp) {
            EVMU_VA_SNPRINTF(pFormat);
            EVMU_RESULT_LOG(pCtx,
                            pFile, pFunc, line,
                            EVMU_RESULT_ERROR_LOG_LEVEL, buffer);
            EVMU_RESULT_ASSERT(exp, buffer);
            EVMU_RESULT_SET_LAST_ERROR(pCtx, pDevice, result, EVMU_RESULT_ERROR_LOG_LEVEL, pFile, pFunc, line, buffer);
        } else {
            EVMU_RESULT_CLEAR_LAST_ERROR(pCtx);
        }

        return exp;

        #define GBL_ERROR_INIT(error, handle, result, src, ...) \

#define GBL_RESULT_ERROR(handle, result, src, ...) \
    do {
        GblError error;
        GBL_ERROR_INIT(&error, hHandle, result, src, __VA_ARGS__);
        GBL_RESULT_ERROR_LOG(&error);
        GBL_RESULT_ERROR_ASSERT(&error);
        GBL_RESULT_ERROR_UPDATE(&error);
    }   while(0)


// Differentiate between API entrypoints and just random internal function entrypoints!!!

#define GBL_API_BEGIN(pHandle, pFmt, ...)    \
    GblApiStackFrame GBL_API_COOKIE_NAME_DEFAULT = gblApiStackFrameCreate(pHandle, GBL_RESULT_SUCCESS,  \
                                                    GBL_SOURCE_LOCATION(GBL_SOURCE_FILE, GBL_SOURCE_FUNC, GBL_SOURCE_LINE, GBL_SOURCE_COLUMN)); \
    GBL_API_SOURCE_LOCATION_PUSH(GBL_SOURCE_FILE, GBL_SOURCE_FUNC, GBL_SOURCE_LINE, GBL_SOURCE_COLUMN); \
    GBL_API_VERBOSE(pFmt, __VA_ARGS__); \
    GBL_API_PUSH()  \
    GBL_API_SOURCE_LOCATION_POP()

#define GBL_API_DONE()  \
    goto GBL_API_END_LABEL;

#define GBL_API_END()                             \
    GBL_API_END_LABEL:                            \
        GBL_API_POP(GBL_API_COKIE().stackDepth);  \
        GblContextApiCookiePop(GBL_API_CTX());    \
        return GBL_API_RESULT()


#define GBL_API_RESULT_ACCUM(result) void


#define GBL_API_RESULT_SET(result) // if failure, log with all info


#define GBL_API_RESULT_SET_JMP_CND(expr, result, label) \
    do {                                                \
        if((exp)) {                                     \
            GBL_API_RESULT_SET(result);                 \
            goto label;                                 \
        }                                               \
    proceed:                                            \
    } while(0)



#define GBL_API_RESULT_SET_CND(exp, result)             \
    GBL_API_RESULT_SET_JMP_CND(exp, result, proceed)

#define GBL_API_RESULT_SET_JMP(result, label)           \
    GBL_API_RESULT_SET_JMP_CND(1, result, label)



#define GBL_API_VERIFY(expr, result)                \
    GBL_API_RESULT_SET_JMP_CND((expr),              \
                               result,              \
                               GBL_API_END_LABEL)

#define GBL_API_VERIFY_POINTER(pPtr)                    \
    GBL_API_VERIFY(hHandle != NULL,                     \
                   GBL_RESULT_ERROR_INVALID_POINTER)
#define GBL_API_SOURCE_TRACKED(FILE, FUNCTION, LINE, COLUMN, CALL) \

#define GBL_API_VERIFY_HANDLE(hHandle)                  \
    GBL_API_SOURCE_TRACKED(GBL_FILE, GBL_SOURCE_FILE, GBL_SOURCE_FUNCTION, GBL_SOURCE_LINE, GBL_SOURCE_COLUMN, \
            GBL_MACRO_CALL(GBL_API_VERIFY, hHandle != NULL, GBL_RESULT_ERROR_INVALID_HANDLE))


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
