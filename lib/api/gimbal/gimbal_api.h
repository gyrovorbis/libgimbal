#ifndef GIMBAL_API_H
#define GIMBAL_API_H

#include "gimbal_macros.h"
#include "gimbal_context.h"
#include "gimbal_ext.h"
#include <stdarg.h>
#include <string.h>
#include <errno.h>

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
    GblCallRecord           record;
    uint32_t                sourceCurrentCaptureDepth;
    GblHandle               hHandle;
    GblContext              hContext;
    void*                   pHandleUd;
    void*                   pContextUd;
    uint32_t                stackDepth;
} GblStackFrame;


GBL_MAYBE_UNUSED GBL_INLINE GBL_RESULT GBL_API_STACK_FRAME_CONSTRUCT(GblStackFrame* pFrame, GblHandle hHandle, GBL_RESULT initialResult, GblSourceLocation entryLoc) {
    GBL_RESULT result       = GBL_RESULT_SUCCESS;
    GblContext hContext     = GBL_NULL;
    void* pHandleUserdata   = GBL_NULL;
    void* pContextUserdata  = GBL_NULL;

    if(hHandle) {
        result = gblHandleContext(hHandle, &hContext);
        GBL_ASSERT(GBL_RESULT_SUCCESS(result));
        result = gblHandleUserdata(hHandle, &pHandleUserdata);
        GBL_ASSERT(GBL_RESULT_SUCCESS(result));
        result = gblHandleUserdata((GblHandle)hContext, &pContextUserdata);
        GBL_ASSERT(GBL_RESULT_SUCCESS(result));
    }

    memset(pFrame, 0, sizeof(GblStackFrame));
    GBL_CALL_RECORD_CONSTRUCT(&pFrame->record, hHandle, initialResult, entryLoc, gblResultString(initialResult));
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
    GBL_ASSERT(pStackFrame->sourceCurrentCaptureDepth);
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
#define GBL_API_RECORD()        GBL_API_FRAME()->record
#define GBL_API_RESULT()        GBL_API_RECORD().result
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
        GBL_RESULT_SUCCESS(GBL_API_RESULT())
#define GBL_API_RESULT_PARTIAL_SUCCESS() \
        GBL_RESULT_PARTIAL_SUCCESS(GBL_API_RESULT())
#define GBL_API_RESULT_ERROR() \
        GBL_RESULT_ERROR(GBL_API_RESULT())
#define GBL_API_HANDLE_IS_CONTEXT() \
        (GBL_API_CONTEXT() == GBL_API_HANDLE())

#define GBL_API_RECORD_SET_JMP_CND_(expr, result, label, srcLoc, ...)          \
    do {                                                                       \
        GBL_API_SOURCE_LOC_PUSH(srcLoc);                                       \
        if(!(expr)) GBL_UNLIKELY {                                             \
            GBL_API_RECORD_SET(result GBL_VA_ARGS(__VA_ARGS__));               \
            GBL_API_SOURCE_POP();                                              \
            label;                                                             \
        } else GBL_LIKELY {                                                    \
            GBL_API_SOURCE_POP();                                              \
        }                                                                      \
    } while(0)

// Verification/Control flow
#define GBL_API_RECORD_SET_JMP_CND(expr, result, label, ...) \
    GBL_API_RECORD_SET_JMP_CND_(expr, result, goto label, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL) GBL_VA_ARGS(__VA_ARGS__));

#if 0
#define GBL_API_RESULT_SET_CND(exp, result)             \
    GBL_API_RESULT_SET_JMP_CND_(exp, result, break, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL))

#define GBL_API_RESULT_SET_JMP(result, label)           \
    GBL_API_RESULT_SET_JMP_CND_(1, result, goto label, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL))
#endif

//====== VERIFY =========

#define GBL_API_VERIFY_(expr, result, srcLoc, ...)              \
    GBL_API_RECORD_SET_JMP_CND_((expr),                         \
                               result,                          \
                               goto GBL_API_END_LABEL,          \
                               srcLoc GBL_VA_ARGS(__VA_ARGS__))

#define GBL_API_VERIFY(expr, result, ...)                \
    GBL_API_VERIFY_((expr), result, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL) GBL_VA_ARGS(__VA_ARGS__))

//====== VERIFY_EXPRESSION =========

#define GBL_API_VERIFY_EXPRESSION_N(srcLoc, expr, ...) \
    GBL_API_VERIFY_(expr, GBL_RESULT_ERROR_INVALID_EXPRESSION, srcLoc GBL_VA_ARGS(__VA_ARGS__))

#define GBL_API_VERIFY_EXPRESSION_2(src, expr) \
    GBL_API_VERIFY_EXPRESSION_N(src, expr, "Invalid Expression: "#expr)

#define GBL_API_VERIFY_EXPRESSION(...) \
    do { \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_VA_OVERLOAD_SELECT(GBL_API_VERIFY_EXPRESSION, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);   \
    } while(0)

//====== VERIFY_POINTER=========
#define GBL_API_VERIFY_POINTER_N(srcLoc, expr, ...) \
    GBL_API_VERIFY_(expr, GBL_RESULT_ERROR_INVALID_POINTER, srcLoc GBL_VA_ARGS(__VA_ARGS__))

#define GBL_API_VERIFY_POINTER_2(src, expr) \
    GBL_API_VERIFY_POINTER_N(src, expr, "Invalid Pointer")

#define GBL_API_VERIFY_POINTER(...) \
    do { \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_VA_OVERLOAD_SELECT(GBL_API_VERIFY_POINTER, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);   \
    } while(0)

//====== VERIFY_HANDLE=========
#define GBL_API_VERIFY_HANDLE_N(srcLoc, expr, ...) \
    GBL_API_VERIFY_(expr, GBL_RESULT_ERROR_INVALID_HANDLE, srcLoc GBL_VA_ARGS(__VA_ARGS__))

#define GBL_API_VERIFY_HANDLE_2(src, expr) \
    GBL_API_VERIFY_HANDLE_N(src, expr, "Invalid Handle")

#define GBL_API_VERIFY_HANDLE(...) \
    do { \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_VA_OVERLOAD_SELECT(GBL_API_VERIFY_HANDLE, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);   \
    } while(0)


//====== VERIFY_ARG=========
#define GBL_API_VERIFY_ARG_N(srcLoc, expr, ...) \
    GBL_API_VERIFY_(expr, GBL_RESULT_ERROR_INVALID_ARG, srcLoc GBL_VA_ARGS(__VA_ARGS__))

#define GBL_API_VERIFY_ARG_2(src, expr) \
    GBL_API_VERIFY_ARG_N(src, expr, "Invalid Arg: "#expr);

#define GBL_API_VERIFY_ARG(...) \
    do { \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_VA_OVERLOAD_SELECT(GBL_API_VERIFY_ARG, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);   \
    } while(0)


#ifdef GBL_CONFIG_ERRNO_CHECKS
#define     GBL_API_ERRNO_CLEAR()   errno = 0
#else
#define     GBL_API_ERRNO_CLEAR()
#endif

#ifdef GBL_CONFIG_ERRNO_CHECKS
#   define GBL_API_PERROR(...)                      \
    do {                                            \
        if(errno) GBL_UNLIKELY {                    \
            const GBL_RESULT code =                 \
                GBL_ERRNO_RESULT(errno);            \
            GBL_API_VERIFY(                         \
                GBL_RESULT_SUCCESS(code),           \
                code);                              \
        }                                           \
    } while(0)
#else
#   define GBL_API_PERROR(...)
#endif

GBL_MAYBE_UNUSED GBL_INLINE GBL_API GBL_ERRNO_RESULT(int ernum) {
    switch(ernum) {
    case 0:     return GBL_RESULT_SUCCESS;
    default:    return GBL_RESULT_ERROR;
    }
}

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
    GBL_API_INLINE_##MethodPrefix##_(GBL_API_FRAME(), srcLoc GBL_VA_ARGS(__VA_ARGS__))

// ============== GBL EXT USERMETHODS ==========

#define GBL_API_EXT(prefix, ...) \
    do { \
        GBL_API_SOURCE_PUSH(SRC_FILE, SRC_FN, SRC_LN, SRC_COL); \
        GBL_MAYBE_UNUSED const GBL_RESULT localResult = GBL_EXT_##prefix(GBL_API_FRAME(), ##__VA_ARGS__);    \
        GBL_ASSERT(!(GBL_CONFIG_ASSERT_ERROR_ENABLED && GBL_RESULT_ERROR(localResult)), "Ext["#prefix"]: ERROR"); \
        GBL_ASSERT(!(GBL_CONFIG_ASSERT_PARTIAL_ENABLED && GBL_RESULT_PARTIAL(localResult)), "Ext["#prefix"]: ERROR"); \
        GBL_API_SOURCE_POP(); \
    } while(0)


GBL_MAYBE_UNUSED GBL_API_INLINE(MALLOC, void*, GblSize size, GblSize align, const char* pDebugStr) {
    GBL_API_INLINE_BEGIN(GBL_NULL);
    GBL_ASSERT(size % align == 0);
    GBL_API_EXT(MALLOC, size, align, pDebugStr, &GBL_API_INLINE_RETVAL());
    GBL_API_INLINE_END();
    // modify/set return value based on result
    GBL_API_INLINE_RETURN();
}

#define GBL_API_MALLOC_4(src, size, align, dbgStr) \
    GBL_API_INLINE_CALL(MALLOC, src, size, align, dbgStr)

#define GBL_API_MALLOC_3(src, size, align) \
    GBL_API_MALLOC_4(src, size, align, NULL)

#define GBL_API_MALLOC_2(src, size) \
    GBL_API_MALLOC_3(src, size, GBL_ALIGNOF(max_align_t))


#define GBL_API_MALLOC(...)  \
    GBL_VA_OVERLOAD_SELECT(GBL_API_MALLOC, GBL_VA_OVERLOAD_SUFFIXER_ARGC, 1, __VA_ARGS__)(SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL), __VA_ARGS__)


GBL_MAYBE_UNUSED GBL_API_INLINE(REALLOC, void*, void* pData, GblSize newSize, GblSize newAlign) {
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


#define GBL_API_PUSH_(srcLoc, ...) \
    do {    \
        GBL_API_SOURCE_LOC_PUSH(srcLoc);   \
        GBL_API_EXT(LOG_PUSH);  \
        GBL_API_SOURCE_POP();  \
        ++GBL_API_FRAME()->stackDepth;   \
    } while(0)

#define GBL_API_PUSH()                                                  \
    do {                                                                \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL); \
        GBL_API_PUSH_(src_);                                            \
    } while(0)

#define GBL_API_PUSH_VERBOSE_N(srcLoc, pFmt, ...) \
    do { \
        GBL_API_SOURCE_SCOPED(GBL_API_VERBOSE, srcLoc, pFmt, ##__VA_ARGS__); \
        GBL_API_PUSH_(srcLoc); \
    } while(0)

#define GBL_API_PUSH_VERBOSE(...) \
    do { \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_VA_OVERLOAD_SELECT(GBL_API_PUSH_VERBOSE, GBL_VA_OVERLOAD_SUFFIXER_1_N, src_, ##__VA_ARGS__)(src_, ##__VA_ARGS__); \
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

GBL_MAYBE_UNUSED GBL_API_INLINE(LOG, GBL_RESULT, GBL_LOG_LEVEL level, const char* pFmt, ...) {
    GBL_API_INLINE_BEGIN(GBL_RESULT_SUCCESS);
    va_list varArgs;
    va_start(varArgs, pFmt);

    GBL_API_EXT(LOG_WRITE, level, pFmt, varArgs);
    va_end(varArgs);
    GBL_API_INLINE_END();
    GBL_API_INLINE_RETURN();
}

#define GBL_API_LOG_(src, level, pFmt, ...) \
    GBL_API_INLINE_CALL(LOG, src, level, pFmt, ##__VA_ARGS__);
#define GBL_API_LOG(level, pFmt, ...) \
   GBL_API_LOG_(SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL), level, pFmt, ##__VA_ARGS__)

#define GBL_API_DEBUG(pFmt, ...)      \
    do {    \
        const SrcLoc src = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_API_LOG_(src, GBL_LOG_LEVEL_DEBUG, pFmt, ##__VA_ARGS__);  \
    } while(0)

#define GBL_API_VERBOSE(pFmt, ...)    \
    do {    \
        const SrcLoc src = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_API_LOG_(src, GBL_LOG_LEVEL_VERBOSE, pFmt, ##__VA_ARGS__);  \
    } while(0)

#define GBL_API_INFO(pFmt, ...)       \
    do {    \
        const SrcLoc src = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_API_LOG_(src, GBL_LOG_LEVEL_INFO, pFmt, ##__VA_ARGS__);  \
    } while(0)

#define GBL_API_WARN(pFmt, ...)       \
    do {    \
        const SrcLoc src = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_API_LOG_(src, GBL_LOG_LEVEL_WARNING, pFmt, ##__VA_ARGS__);  \
    } while(0)

#define GBL_API_ERROR(pFmt, ...)      \
    do {    \
        const SrcLoc src = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_API_LOG_(src, GBL_LOG_LEVEL_ERROR, pFmt, ##__VA_ARGS__);  \
    } while(0)



// ===================== RECORD = > ASSERT =========================

// Base Enabled Logic
#define GBL_API_RECORD_ASSERT_(record, test)                    \
    do {                                                        \
        GBL_ASSERT(!test(record->result), record->message);     \
    } while(0)

// Base Conditional Logic
#define GBL_API_RECORD_ASSERT_CONDITIONAL_(enabled, record, test)              \
    GBL_MACRO_CONDITIONAL_CALL(enabled, GBL_API_RECORD_ASSERT_, record, test)

// Partial Success
#define GBL_API_RECORD_ASSERT_PARTIAL(record)                               \
    GBL_API_RECORD_ASSERT_CONDITIONAL_(GBL_CONFIG_ASSERT_PARTIAL_ENABLED,   \
                                        record, GBL_RESULT_PARTIAL)
// Error
#define GBL_API_RECORD_ASSERT_ERROR(record)                                 \
    GBL_API_RECORD_ASSERT_CONDITIONAL_(GBL_CONFIG_ASSERT_ERROR_ENABLED,     \
                                       record, GBL_RESULT_ERROR)

// Unknown
#define GBL_API_RECORD_ASSERT_UNKNOWN(record)                                 \
    GBL_API_RECORD_ASSERT_CONDITIONAL_(GBL_CONFIG_ASSERT_UNKNOWN_ENABLED,     \
                                       record, GBL_RESULT_UNKNOWN)
// Both Together
#define GBL_API_RECORD_ASSERT(record)                       \
    do {                                                    \
            GBL_API_RECORD_ASSERT_ERROR(record);            \
            GBL_API_RECORD_ASSERT_PARTIAL(record);          \
            GBL_API_RECORD_ASSERT_UNKNOWN(record);          \
    } while(0)


// ==========================  RECORD = > LOG ========================

// Base Enabled Logic (uses a prefix prefix for all magic)
#define GBL_API_RECORD_LOG_(prefix, record)                         \
        do {                                                        \
            if(GBL_RESULT_##prefix(record->result)) GBL_UNLIKELY {  \
                GBL_API_LOG(GBL_CONFIG_LOG_##prefix##_LEVEL,        \
                            "Result: %d, Message: %s",              \
                             record->result, record->message);      \
            }                                                       \
        } while(0)


// Base Conditional Logic
#define GBL_API_RECORD_LOG_CONDITIONAL_(prefix, record)                 \
        GBL_MACRO_CONDITIONAL_CALL(GBL_CONFIG_LOG_##prefix##_ENABLED,   \
                                    GBL_API_RECORD_LOG_, prefix, record)
// Success
#define GBL_API_RECORD_LOG_SUCCESS(record) \
        GBL_API_RECORD_LOG_CONDITIONAL_(SUCCESS, record)

// Partial Success
#define GBL_API_RECORD_LOG_PARTIAL(record) \
        GBL_API_RECORD_LOG_CONDITIONAL_(PARTIAL, record)

// Error
#define GBL_API_RECORD_LOG_ERROR(record) \
        GBL_API_RECORD_LOG_CONDITIONAL_(ERROR, record)

// Unknown
#define GBL_API_RECORD_LOG_UNKNOWN(record) \
        GBL_API_RECORD_LOG_CONDITIONAL_(UNKNOWN, record)

// Combined
#define GBL_API_RECORD_LOG(record)                      \
    do {                                                \
        GBL_API_RECORD_LOG_ERROR(record);               \
        GBL_API_RECORD_LOG_PARTIAL(record);             \
        GBL_API_RECORD_LOG_UNKNOWN(record);             \
        GBL_API_RECORD_LOG_SUCCESS(record);             \
    } while(0)


// ================= RECORD => HANDLE::LAST_ERROR ==============
#define GBL_API_RECORD_LAST_RECORD_(prefix, record)                             \
    do {                                                                        \
        if(GBL_RESULT_##prefix(record->result)) {                               \
            GBL_ASSERT(gblHandleLastCallRecordSet(GBL_API_HANDLE(), record),    \
                        "Last error failed!");                                  \
        }                                                                       \
    } while(0)


#define GBL_API_RECORD_LAST_RECORD_CONDITIONAL_(prefix, record)                     \
    GBL_MACRO_CONDITIONAL_CALL(GBL_CONFIG_LAST_CALL_RECORD_##prefix##_ENABLED,      \
                            GBL_API_RECORD_LAST_RECORD_, prefix, record)

#define GBL_API_RECORD_LAST_RECORD_SUCCESS(record) \
    GBL_API_RECORD_LAST_RECORD_CONDITIONAL_(SUCCESS, record)

#define GBL_API_RECORD_LAST_RECORD_PARTIAL(record) \
    GBL_API_RECORD_LAST_RECORD_CONDITIONAL_(PARTIAL, record)

#define GBL_API_RECORD_LAST_RECORD_ERROR(record) \
    GBL_API_RECORD_LAST_RECORD_CONDITIONAL_(ERROR, record)

#define GBL_API_RECORD_LAST_RECORD_UNKNOWN(record) \
    GBL_API_RECORD_LAST_RECORD_CONDITIONAL_(UNKNOWN, record)

#define GBL_API_RECORD_LAST_RECORD(record)          \
    do {                                            \
        GBL_API_RECORD_LAST_RECORD_ERROR(record);   \
        GBL_API_RECORD_LAST_RECORD_PARTIAL(record); \
        GBL_API_RECORD_LAST_RECORD_UNKNOWN(record); \
        GBL_API_RECORD_LAST_RECORD_SUCCESS(record); \
    } while(0)


// ================= RECORD => TOP-LEVEL DISPATCH ==============
#define GBL_API_RECORD_HANDLER(record)          \
    do {                                        \
        if(GBL_API_HANDLE()) {  \
            GBL_API_RECORD_LOG((record));           \
            GBL_API_RECORD_LAST_RECORD((record));   \
        }   \
        GBL_API_RECORD_ASSERT((record));        \
    } while(0)

#define GBL_API_RECORD_SET_N(file, func, line, col, result, pFmt, ...)                                                                  \
    do {                                                                                                                                \
        GBL_API_SOURCE_LOC_PUSH(SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL));                                                            \
        GBL_CALL_RECORD_CONSTRUCT(&GBL_API_RECORD(), GBL_API_HANDLE(), result, GBL_API_SOURCE(), pFmt GBL_VA_ARGS(__VA_ARGS__));        \
        GBL_API_RECORD_HANDLER(&GBL_API_RECORD());                                                                                      \
        GBL_API_SOURCE_POP();                                                                                                           \
    } while(0)

#define GBL_API_RECORD_SET_5(file, func, line, col, result) \
    GBL_API_RECORD_SET_N(file, func, line, col, result, gblResultString(result))

#define GBL_API_RECORD_SET(...)  \
    GBL_VA_OVERLOAD_CALL(GBL_API_RECORD_SET, GBL_VA_OVERLOAD_SUFFIXER_5_N, SRC_FILE, SRC_FN, SRC_LN, SRC_COL, __VA_ARGS__)



#define GBL_API_VERIFY_HANDLE_N(srcLoc, expr, ...) \
    GBL_API_VERIFY_(expr, GBL_RESULT_ERROR_INVALID_HANDLE, srcLoc GBL_VA_ARGS(__VA_ARGS__))

#define GBL_API_VERIFY_HANDLE_2(src, expr) \
    GBL_API_VERIFY_HANDLE_N(src, expr, "Invalid Handle")

#define GBL_API_VERIFY_HANDLE(...) \
    do { \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_VA_OVERLOAD_SELECT(GBL_API_VERIFY_HANDLE, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);   \
    } while(0)

#define GBL_API_CALL_N(src, funcCall, ...)                                      \
    do {                                                                        \
        GBL_API_SOURCE_LOC_PUSH(src);                                           \
        GBL_MAYBE_UNUSED const GBL_RESULT localResult = (funcCall);             \
        if(!GBL_RESULT_SUCCESS(localResult)) GBL_UNLIKELY {                     \
            GBL_API_RECORD_SET(localResult GBL_VA_ARGS(__VA_ARGS__));           \
        }                                                                       \
        GBL_API_SOURCE_POP();                                                   \
    } while(0)

#define GBL_API_CALL_2(src, funcCall) \
    GBL_API_CALL_N(src, funcCall, "Call[%s] -> Result[%s]", #funcCall, gblResultString(localResult));

#define GBL_API_CALL(...) \
    do {    \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_VA_OVERLOAD_SELECT(GBL_API_CALL, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);   \
    } while(0)

// ================= TOP-LEVEL API UTILITIES ==============


#define GBL_API_BEGIN_FRAME(file, func, line, col, hHandle, frame) \
    const SrcLoc gblApiEntrySrcLoc_ = SRC_LOC(file, func, line, col); \
    GBL_API_FRAME_DECLARE = frame;   \
    GBL_API_STACK_FRAME_CONSTRUCT(GBL_API_FRAME(), (GblHandle)hHandle, GBL_RESULT_SUCCESS, gblApiEntrySrcLoc_)

#define GBL_API_BEGIN_LOG_5(file, func, line, col, hHandle)  \
    GBL_API_BEGIN_FRAME(file, func, line, col, hHandle, ((GblStackFrame*)alloca(sizeof(GblStackFrame))))

#define GBL_API_BEGIN_LOG_N(file, func, line, col, hHandle, ...)    \
    GBL_API_BEGIN_LOG_5(file, func, line, col, hHandle); \
    GBL_API_PUSH_VERBOSE(__VA_ARGS__);

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
        return GBL_API_RESULT()






#ifdef __cplusplus
}
#endif

#endif // GIMBAL_API_H
