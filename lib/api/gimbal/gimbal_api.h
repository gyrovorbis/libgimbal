#ifndef GIMBAL_API_H
#define GIMBAL_API_H

#include "gimbal_macros.h"
#include "gimbal_handle.h"
#include "gimbal_ext.h"
#include <stdarg.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SRC_FILE    GBL_SOURCE_FILE
#define SRC_FN      GBL_SOURCE_FUNCTION
#define SRC_LN      GBL_SOURCE_LINE
#define SRC_COL     GBL_SOURCE_COLUMN
#define SRC_LOC     GBL_SOURCE_LOCATION
#define SrcLoc      GblSourceLocation


typedef struct GblStackFrame {
    GblSourceLocation       sourceEntry;
    GblSourceLocation       sourceCurrent;
    GblApiResult            result;
    uint32_t                sourceCurrentCaptureDepth;
    GblHandle               hHandle;
    GblContext              hContext;
    void*                   pHandleUd;
    void*                   pContextUd;
    uint32_t                stackDepth;
} GblStackFrame;


static inline GBL_RESULT GBL_API_STACK_FRAME_CONSTRUCT(GblStackFrame* pFrame, GblHandle hHandle, GBL_RESULT initialResult, GblSourceLocation entryLoc) {
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    GblContext hContext = NULL;
    assert(GBL_RESULT_SUCCESS(gblHandleContext(hHandle, &hContext)));
    void* pHandleUserdata = NULL;
    assert(GBL_RESULT_SUCCESS(gblHandleUserdata(hHandle, &pHandleUserdata)));
    void* pContextUserdata = NULL;
    assert(GBL_RESULT_SUCCESS(gblHandleUserdata((GblHandle)hContext, &pContextUserdata)));

    memset(pFrame, 0, sizeof(GblStackFrame));
    GBL_API_RESULT_CONSTRUCT(&pFrame->result, hHandle, initialResult, entryLoc, "DefaultResult!");
    pFrame->sourceEntry     = entryLoc;
    pFrame->sourceCurrent   = entryLoc;
    pFrame->hHandle         = hHandle;
    pFrame->hContext        = hContext;
    pFrame->pContextUd      = pContextUserdata;
    pFrame->pHandleUd       = pHandleUserdata;
    return result;
}

static inline GblBool GBL_API_STACK_FRAME_SOURCE_PUSH(GblStackFrame* pStackFrame, GblSourceLocation current) {
    if(++pStackFrame->sourceCurrentCaptureDepth == 1) { //we care about the first entry point
        pStackFrame->sourceCurrent = current;
        return GBL_TRUE;
    }
    return GBL_FALSE;
}

static inline GblBool GBL_API_STACK_FRAME_SOURCE_POP(GblStackFrame* pStackFrame) {
    assert(pStackFrame->sourceCurrentCaptureDepth);
    if(--pStackFrame->sourceCurrentCaptureDepth == 0) {
        pStackFrame->sourceCurrent = pStackFrame->sourceEntry;
        return GBL_TRUE;
    }
    return GBL_FALSE;
}

 /* TODO:

 1) Transparently proxying FRAME to an inline function is slightly broken
    - anything that jumps to preset labels is wrong
    - need a GBL_API_END() label to return
2) GBL_API_FREE, PUSH, POP, ETC can all share most code!
3) Need to wrap log functions, fininsh commonality with main log function
4) High-level API_BEGIN, API_END, VERIFY, etc shit not done
    - use new API
    - make sure using proper source location shit
5) Make sure all of the flags determining error handling/propagation are all
    propagated as CMakeLists.txt properties correctly!
6) Make everything use the GblExt concept again
7) Exception-like Try blocks using longjump and returning GblResult 
   structures analogously to excepitons

   ** PRETTY MUCH DID ALL THAT SHIT**
8) Finish convenience overloads:
    a. VERIFY, VERIFY_POINTER, VERIFY_HANDLE, etc
    b. any top/bottom level "setResult" shit should optionally take error string too
    c. overload malloc/realloc to hide alignment arguments

*/


// ========== GblStackFrame ===========
    // always reference via pointer
    //initial allocation uses ALLOCA, can proxy to inline functions!!
#define GBL_API_FRAME_DECLARE   GblStackFrame* GBL_API_FRAME_NAME
#define GBL_API_FRAME()         (GBL_API_FRAME_NAME)
#define GBL_API_CONTEXT()       GBL_API_FRAME()->hContext
#define GBL_API_CONTEXT_UD()    GBL_API_FRAME()->pContextUd
#define GBL_API_HANDLE()        GBL_API_FRAME()->hHandle
#define GBL_API_HANDLE_UD()     GBL_API_FRAME()->pHandleUd
#define GBL_API_RESULT()        GBL_API_FRAME()->result
#define GBL_API_RESULT_CODE()   GBL_API_RESULT().resultCode
#define GBL_API_SOURCE()        GBL_API_FRAME()->sourceCurrent
#define GBL_API_SOURCE_LOC_PUSH(srcLoc) \
    GBL_API_STACK_FRAME_SOURCE_PUSH(GBL_API_FRAME(), srcLoc)
#define GBL_API_SOURCE_PUSH(FILE, FUNCTION, LINE, COLUMN)   \
    GBL_API_SOURCE_LOC_PUSH(GBL_SOURCE_LOCATION(FILE, FUNCTION, LINE, COLUMN))

#define GBL_API_SOURCE_POP() \
    GBL_API_STACK_FRAME_SOURCE_POP(GBL_API_FRAME())
#define GBL_API_SOURCE_SCOPED(CALL, loc, ...) \
    do {    \
        GBL_API_SOURCE_LOC_PUSH((loc)); \
        GBL_IDENTITY(CALL)(__VA_ARGS__);  \
        GBL_API_SOURCE_POP();   \
    } while(0)


// Convenience Accessors
#define GBL_API_RESULT_SUCCESS() \
        GBL_RESULT_SUCCESS(GBL_API_RESULT_CODE())
#define GBL_API_RESULT_PARTIAL_SUCCESS() \
        GBL_RESULT_PARTIAL_SUCCESS(GBL_API_RESULT_CODE())
#define GBL_API_RESULT_ERROR() \
        GBL_RESULT_ERROR(GBL_API_RESULT_CODE())
#define GBL_API_HANDLE_IS_CONTEXT() \
        (GBL_API_CONTEXT() == GBL_API_HANDLE())

#define GBL_API_RESULT_SET_JMP_CND_(expr, result, label, srcLoc) \
    do {                                                \
        GBL_API_SOURCE_LOC_PUSH(srcLoc); \
        if(!(expr)) {                                     \
            GBL_API_RESULT_SET(result, #expr);                 \
            GBL_API_SOURCE_POP();                       \
            label;                                 \
        } else {                                        \
            GBL_API_SOURCE_POP();                       \
        }                                               \
    } while(0)

// Verification/Control flow
#define GBL_API_RESULT_SET_JMP_CND(expr, result, label) \
    GBL_API_RESULT_SET_JMP_CND_(expr, result, goto label, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL))

#define GBL_API_RESULT_SET_CND(exp, result)             \
    GBL_API_RESULT_SET_JMP_CND_(exp, result, break, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL))

#define GBL_API_RESULT_SET_JMP(result, label)           \
    GBL_API_RESULT_SET_JMP_CND_(1, result, goto label, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL))


#define GBL_API_VERIFY_(expr, result, srcLoc)                \
    GBL_API_RESULT_SET_JMP_CND_((expr),              \
                               result,              \
                               goto GBL_API_END_LABEL, srcLoc)

#define GBL_API_VERIFY(expr, result)                \
    GBL_API_VERIFY_((expr),              \
                               result,              \
                               SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL))

#define GBL_API_VERIFY_POINTER(pPtr)                    \
    GBL_API_VERIFY_(pPtr != NULL,                     \
                   GBL_RESULT_ERROR_INVALID_POINTER, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL))


#define GBL_API_VERIFY_HANDLE(hHandle)                  \
    GBL_API_VERIFY_(hHandle != GBL_HANDLE_INVALID,                     \
                   GBL_RESULT_ERROR_INVALID_HANDLE, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL))

#define GBL_API_VERIFY_ARG(expr) \
    GBL_API_VERIFY_((expr),             \
                   GBL_RESULT_ERROR_INVALID_ARG, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL))

// ======= GBL_API_INLINE API ======
#define GBL_API_INLINE_RETVAL() GBL_API_INLINE_RETURN_VALUE_NAME

#define GBL_API_INLINE(MethodPrefix, ReturnType, ...) \
    static inline ReturnType GBL_API_INLINE_##MethodPrefix##_(GBL_API_FRAME_DECLARE, SrcLoc srcLoc, ##__VA_ARGS__) { \
        ReturnType GBL_API_INLINE_RETURN_VALUE_NAME;

#define GBL_API_INLINE_BEGIN(InitialRetValue) \
        GBL_API_INLINE_RETVAL() = InitialRetValue; \
        GBL_API_SOURCE_LOC_PUSH(srcLoc);


#define GBL_API_INLINE_END()                \
        goto GBL_API_END_LABEL;             \
        GBL_API_END_LABEL: do ; while(0);   \
    }

#define GBL_API_INLINE_RETURN() \
    GBL_API_SOURCE_POP();  \
    return GBL_API_INLINE_RETVAL()

#define GBL_API_INLINE_CALL(MethodPrefix, srcLoc, ...) \
    GBL_API_INLINE_##MethodPrefix##_(GBL_API_FRAME(), srcLoc, __VA_ARGS__)

// ============== GBL EXT USERMETHODS ==========

#define GBL_API_EXT(prefix, ...) \
    do { \
        GBL_API_SOURCE_PUSH(SRC_FILE, SRC_FN, SRC_LN, SRC_COL); \
        const GBL_RESULT localResult = GBL_EXT_##prefix(GBL_API_FRAME(), ##__VA_ARGS__);    \
        GBL_ASSERT(!(GBL_CONFIG_ASSERT_ERROR_ENABLED && GBL_RESULT_ERROR(localResult)), "Ext["#prefix"]: ERROR"); \
        GBL_ASSERT(!(GBL_CONFIG_ASSERT_PARTIAL_ENABLED && GBL_RESULT_PARTIAL(localResult)), "Ext["#prefix"]: PARTIAL"); \
        GBL_API_SOURCE_POP(); \
    } while(0)


GBL_API_INLINE(MALLOC, void*, GblSize size, GblSize align) {
    GBL_API_INLINE_BEGIN(NULL);

    GBL_API_EXT(MALLOC, size, align, &GBL_API_INLINE_RETVAL());
    GBL_API_INLINE_END();
    // modify/set return value based on result
    GBL_API_INLINE_RETURN();
}

#define GBL_API_MALLOC_3(src, size, align) \
    GBL_API_INLINE_CALL(MALLOC, src, size, align)

#define GBL_API_MALLOC_2(src, size) \
    GBL_API_MALLOC_3(src, size, 1)


#define GBL_API_MALLOC(...)  \
        GBL_VA_OVERLOAD_SELECT(GBL_API_MALLOC, GBL_VA_OVERLOAD_SUFFIXER_ARGC, 1, __VA_ARGS__)(SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL), __VA_ARGS__)


GBL_API_INLINE(REALLOC, void*, void* pData, GblSize newSize, GblSize newAlign) {
    GBL_API_INLINE_BEGIN(NULL);

    GBL_API_EXT(REALLOC, pData, newSize, newAlign, &GBL_API_INLINE_RETVAL());
    GBL_API_INLINE_END();
    // modify/set return value based on result
    GBL_API_INLINE_RETURN();
}

#define GBL_API_REALLOC_4(src, pData, newSize, newAlign) \
    GBL_API_INLINE_CALL(REALLOC, src, pData, newSize, newAlign)

#define GBL_API_REALLOC_3(src, pData, newSize) \
    GBL_API_REALLOC_4(src, pData, newSize, 1)


#define GBL_API_REALLOC(...)  \
     GBL_VA_OVERLOAD_SELECT(GBL_API_REALLOC, GBL_VA_OVERLOAD_SUFFIXER_ARGC, 1, __VA_ARGS__)(SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL), __VA_ARGS__)


#define GBL_API_FREE(pData) \
    GBL_API_SOURCE_SCOPED(GBL_API_EXT, (SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL)), FREE, pData)


#define GBL_API_PUSH_1(srcLoc, ...) \
    do {    \
        GBL_API_SOURCE_LOC_PUSH(srcLoc);   \
        GBL_API_EXT(LOG_PUSH);  \
        GBL_API_SOURCE_POP();  \
        ++GBL_API_FRAME()->stackDepth;   \
    } while(0)

//        GBL_API_SOURCE_SCOPED(GBL_API_EXT, srcLoc, LOG_PUSH); \


#define GBL_API_PUSH_N(srcLoc, pFmt, ...) \
    do { \
        GBL_API_SOURCE_SCOPED(GBL_API_VERBOSE, srcLoc, pFmt, ##__VA_ARGS__); \
        GBL_API_PUSH_1(srcLoc); \
    } while(0)

#define GBL_API_PUSH(...) \
    do { \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_VA_OVERLOAD_SELECT(GBL_API_PUSH, GBL_VA_OVERLOAD_SUFFIXER_1_N, src_, ##__VA_ARGS__)(src_, ##__VA_ARGS__); \
    } while(0)

#define GBL_API_POP_2(srcLoc, count) \
        GBL_API_SOURCE_SCOPED(GBL_API_EXT, srcLoc, LOG_POP, count); \
        GBL_API_FRAME()->stackDepth -= count;


#define GBL_API_POP_1(srcLoc) \
    GBL_API_POP_2(srcLoc, 1)

#define GBL_API_POP(...) \
    do { \
        const SrcLoc loc = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_VA_OVERLOAD_CALL(GBL_API_POP, GBL_VA_OVERLOAD_SUFFIXER_ARGC, loc, __VA_ARGS__); \
    } while (0)


GBL_API_INLINE(LOG, GBL_RESULT, GBL_LOG_LEVEL level, const char* pFmt, ...) {
    GBL_API_INLINE_BEGIN(GBL_RESULT_SUCCESS);
    va_list varArgs;
    va_start(varArgs, pFmt);

    GBL_API_EXT(LOG_WRITE, level, pFmt, varArgs);
    GBL_API_INLINE_END();
    GBL_API_INLINE_RETURN();
}

#define GBL_API_LOG_(src, level, pFmt, ...) \
    GBL_API_INLINE_CALL(LOG, src, level, pFmt, ##__VA_ARGS__);
#define GBL_API_LOG(level, pFmt, ...) \
   GBL_API_LOG_(SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL), level, pFmt, ##__VA_ARGS__)
#define GBL_API_DEBUG(...)      \
    GBL_API_SOURCE_SCOPED(GBL_API_LOG, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL), GBL_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define GBL_API_VERBOSE(pFmt, ...)    \
    do {    \
        const SrcLoc src = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_API_LOG_(src, GBL_LOG_LEVEL_VERBOSE, pFmt, ##__VA_ARGS__);  \
    } while(0)
#define GBL_API_INFO(...)       \
    GBL_API_SOURCE_SCOPED(GBL_API_LOG, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL), GBL_LOG_LEVEL_INFO, __VA_ARGS__)
#define GBL_API_WARN(...)       \
    GBL_API_SOURCE_SCOPED(GBL_API_LOG, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL), GBL_LOG_LEVEL_WARN, __VA_ARGS__)
#define GBL_API_ERROR(...)      \
    GBL_API_SOURCE_SCOPED(GBL_API_LOG, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL), GBL_LOG_LEVEL_ERROR, __VA_ARGS__)



// ===================== RESULT = > ASSERT =========================

// Base Enabled Logic
#define GBL_API_RESULT_ASSERT_(result, test)                \
    do {                                                    \
        GBL_ASSERT(!test(result->resultCode), result->message);    \
    } while(0)

// Base Conditional Logic
#define GBL_API_RESULT_ASSERT_CONDITIONAL_(enabled, result, test)                   \
    GBL_MACRO_CONDITIONAL_CALL(enabled, GBL_API_RESULT_ASSERT_, result, test)

// Partial Success
#define GBL_API_RESULT_ASSERT_PARTIAL(result)                                   \
    GBL_API_RESULT_ASSERT_CONDITIONAL_(GBL_CONFIG_ASSERT_PARTIAL_ENABLED,   \
                                        result, GBL_RESULT_PARTIAL)
// Error
#define GBL_API_RESULT_ASSERT_ERROR(result)                                         \
    GBL_API_RESULT_ASSERT_CONDITIONAL_(GBL_CONFIG_ASSERT_ERROR_ENABLED,         \
                                       result, GBL_RESULT_ERROR)
// Both Together
#define GBL_API_RESULT_ASSERT(result)                       \
    do {                                                    \
            GBL_API_RESULT_ASSERT_ERROR(result);            \
            GBL_API_RESULT_ASSERT_PARTIAL(result);  \
    } while(0)


// ==========================  RESULT = > LOG ========================

// Base Enabled Logic (udes a prefix prefix for all magic)
#define GBL_API_RESULT_LOG_(prefix, result)                         \
        do {                                                        \
            if(GBL_RESULT_##prefix(result->resultCode)) {                 \
                GBL_API_LOG(GBL_CONFIG_LOG_##prefix##_LEVEL,    \
                            "Result: %d, Message: %s",              \
                             result->resultCode, result->message);        \
            }                                                       \
        } while(0)


// Base Conditional Logic
#define GBL_API_RESULT_LOG_CONDITIONAL_(prefix, result)                     \
        GBL_MACRO_CONDITIONAL_CALL(GBL_CONFIG_LOG_##prefix##_ENABLED,   \
                                    GBL_API_RESULT_LOG_, prefix, result)
// Success
#define GBL_API_RESULT_LOG_SUCCESS(result) \
        GBL_API_RESULT_LOG_CONDITIONAL_(SUCCESS, result)

// Partial Success
#define GBL_API_RESULT_LOG_PARTIAL(result) \
        GBL_API_RESULT_LOG_CONDITIONAL_(PARTIAL, result)

// Error
#define GBL_API_RESULT_LOG_ERROR(result) \
        GBL_API_RESULT_LOG_CONDITIONAL_(ERROR, result)

// Combined
#define GBL_API_RESULT_LOG(result)                      \
    do {                                                \
        GBL_API_RESULT_LOG_ERROR(result);               \
        GBL_API_RESULT_LOG_PARTIAL(result);     \
        GBL_API_RESULT_LOG_SUCCESS(result);             \
    } while(0)

       
// ================= RESULT => HANDLE::LAST_ERROR ==============
#define GBL_API_RESULT_LAST_ERROR_(prefix, result)                              \
    do {                                                                        \
        if(GBL_RESULT_##prefix(result->resultCode)) {                                 \
            GBL_ASSERT(gblHandleLastErrorSet(GBL_API_HANDLE(), result), "Last error failed!");   \
        }                                                                       \
    } while(0)


#define GBL_API_RESULT_LAST_ERROR_CONDITIONAL_(prefix, result) \
    GBL_MACRO_CONDITIONAL_CALL(GBL_CONFIG_LAST_ERROR_##prefix##_ENABLED,    \
                            GBL_API_RESULT_LAST_ERROR_, prefix, result)

#define GBL_API_RESULT_LAST_ERROR_SUCCESS(result) \
    GBL_API_RESULT_LAST_ERROR_CONDITIONAL_(SUCCESS, result)

#define GBL_API_RESULT_LAST_ERROR_PARTIAL(result) \
    GBL_API_RESULT_LAST_ERROR_CONDITIONAL_(PARTIAL, result)

#define GBL_API_RESULT_LAST_ERROR_ERROR(result) \
    GBL_API_RESULT_LAST_ERROR_CONDITIONAL_(ERROR, result)

#define GBL_API_RESULT_LAST_ERROR(result) \
    do { \
        GBL_API_RESULT_LAST_ERROR_ERROR(result);    \
        GBL_API_RESULT_LAST_ERROR_PARTIAL(result);  \
        GBL_API_RESULT_LAST_ERROR_SUCCESS(result);  \
    } while(0)


// ================= RESULT => TOP-LEVEL DISPATCH ==============
#define GBL_API_RESULT_HANDLER(result)                                          \
    do {                                                                        \
        GBL_API_RESULT_LOG((result));                                            \
        GBL_API_RESULT_ASSERT((result));                                         \
        GBL_API_RESULT_LAST_ERROR((result));                                         \
    } while(0)

#define GBL_API_RESULT_SET(resultCode, ...) \
    do {    \
        GBL_API_SOURCE_LOC_PUSH(SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL));  \
        GBL_API_RESULT_CONSTRUCT(&GBL_API_RESULT(), GBL_API_HANDLE(), resultCode, GBL_API_SOURCE() GBL_VA_ARGS(__VA_ARGS__)); \
        GBL_API_RESULT_HANDLER(&GBL_API_RESULT());                                                      \
        GBL_API_SOURCE_POP(); \
    } while(0)

#define GBL_API_CALL(funcCall) \
    do {    \
        GBL_API_SOURCE_PUSH(SRC_FILE, SRC_FN, SRC_LN, SRC_COL); \
        const GBL_RESULT localResult = (funcCall);  \
        if(!GBL_RESULT_SUCCESS(localResult)) {  \
            GBL_API_RESULT_SET(localResult, "Call failed!");    \
        }   \
        GBL_API_SOURCE_POP();   \
    } while(0)



// ================= TOP-LEVEL API UTILITIES ==============


#define GBL_API_BEGIN_LOG_5(file, func, line, col, hHandle)  \
    const SrcLoc gblApiEntrySrcLoc_ = SRC_LOC(file, func, line, col); \
    GBL_API_FRAME_DECLARE = (GblStackFrame*)GBL_ALLOCA(sizeof(GblStackFrame));   \
    GBL_API_STACK_FRAME_CONSTRUCT(GBL_API_FRAME(), (GblHandle)hHandle, GBL_RESULT_SUCCESS, gblApiEntrySrcLoc_)

#define GBL_API_BEGIN_LOG_N(file, func, line, col, hHandle, ...)    \
    GBL_API_BEGIN_LOG_5(file, func, line, col, hHandle); \
    GBL_API_PUSH(__VA_ARGS__);

// FIGURE ME THE FUCK OUT, PASS SRC_CONTEXT SHIT
#define GBL_API_BEGIN(...) \
        GBL_VA_OVERLOAD_CALL(GBL_API_BEGIN_LOG, GBL_VA_OVERLOAD_SUFFIXER_5_N, SRC_FILE, SRC_FN, SRC_LN, SRC_COL, __VA_ARGS__)

#define GBL_API_DONE()  \
    goto GBL_API_END_LABEL


#define GBL_API_END_BLOCK() \
    goto GBL_API_END_LABEL; \
    GBL_API_END_LABEL:                            \
        if(GBL_API_FRAME()->stackDepth) GBL_API_POP(GBL_API_FRAME()->stackDepth);

#define GBL_API_END()                             \
        GBL_API_END_BLOCK(); \
        return GBL_API_RESULT_CODE()


  

#ifdef __cplusplus
}
#endif

#endif // GIMBAL_API_H
