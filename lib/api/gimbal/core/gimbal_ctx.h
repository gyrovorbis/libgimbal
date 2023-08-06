/*! \file
 *  \brief GblContext top-level debug and control-flow macros
 *  \ingroup core
 *
 *  \author Falco Girgis
 */
#ifndef GIMBAL_CTX_H
#define GIMBAL_CTX_H

#include "../algorithms/gimbal_numeric.h"
#include "../meta/ifaces/gimbal_ilogger.h"
#include "gimbal_thd.h"
#include <stdarg.h>
#include <string.h>
#include <errno.h>

GBL_DECLS_BEGIN

// ===== GBL API FRAME CONVENIENCE ACCESSORS =====

#define GBL_CTX_FRAME_DECLARE   GblStackFrame* GBL_CTX_FRAME_NAME
#define GBL_CTX_FRAME()         (GblThd_current()->pStackFrameTop)
#define GBL_CTX_CONTEXT()       GBL_CTX_FRAME()->pContext
#define GBL_CTX_OBJECT()        GBL_CTX_FRAME()->pObject
#define GBL_CTX_RECORD()        (GBL_CTX_FRAME_NAME)->record
#define GBL_CTX_RESULT()        (GBL_CTX_FRAME_NAME)->record.result
#define GBL_CTX_SOURCE()        (GBL_CTX_FRAME_NAME)->record.srcLocation
#define GBL_CTX_LAST_RECORD()   (GblThd_current()->callRecord)
#define GBL_CTX_LAST_RESULT()   (GBL_CTX_LAST_RECORD().result)

// ====== SOURCE LOCATION PROPAGATION UTILITIES =====

#define GBL_SRC_FILE    __FILE__
#define GBL_SRC_FN      __func__
#define GBL_SRC_LN      __LINE__
#define GBL_SRC_LOC     GBL_SOURCE_LOCATION
#define GblSrcLoc       GblSourceLocation

#define GBL_SOURCE_LOCATION(FILE, FUNCTION, LINE) GBL_STRUCT_INIT(GblSourceLocation, FILE, FUNCTION, LINE)

#define GBL_CTX_STACK_FRAME_SOURCE_PUSH(pStackFrame, current) \
    if(++pStackFrame->sourceCurrentCaptureDepth == 1) GBL_CTX_SOURCE() = current;

#define GBL_CTX_STACK_FRAME_SOURCE_POP(pStackFrame)                 \
    GBL_STMT_START {                                                \
        GBL_ASSERT(pStackFrame->sourceCurrentCaptureDepth);         \
        --pStackFrame->sourceCurrentCaptureDepth;                   \
    } GBL_STMT_END

#define GBL_CTX_SOURCE_LOC_PUSH(srcLoc) \
    GBL_CTX_STACK_FRAME_SOURCE_PUSH(GBL_CTX_FRAME_NAME, srcLoc)

#define GBL_CTX_SOURCE_PUSH(FILE, FUNCTION, LINE) \
    GBL_CTX_SOURCE_LOC_PUSH(GBL_SOURCE_LOCATION(FILE, FUNCTION, LINE))

#define GBL_CTX_SOURCE_POP()                    \
    GBL_CTX_STACK_FRAME_SOURCE_POP(GBL_CTX_FRAME_NAME)

#define GBL_CTX_SOURCE_SCOPED(CALL, loc, ...)   \
    GBL_STMT_START {                            \
        GBL_CTX_SOURCE_LOC_PUSH((loc));         \
        GBL_IDENTITY(CALL)(__VA_ARGS__);        \
        GBL_CTX_SOURCE_POP();                   \
    } GBL_STMT_END

// ====== BOTTOM-LEVEL ERROR HANDLING + CONTROL FLOW =====

#define GBL_CTX_RECORD_SET_JMP_CND_(expr, result, label, srcLoc, ...)          \
    GBL_STMT_START {                                                           \
        GBL_CTX_SOURCE_LOC_PUSH(srcLoc);                                       \
        if(!(expr)) GBL_UNLIKELY {                                             \
            GBL_CTX_RECORD_SET(result, __VA_ARGS__);                           \
            GBL_CTX_SOURCE_POP();                                              \
            label;                                                             \
        } else GBL_LIKELY {                                                    \
            GBL_CTX_SOURCE_POP();                                              \
        }                                                                      \
    } GBL_STMT_END

#define GBL_CTX_RECORD_SET_JMP_CND(expr, result, label, ...) \
    GBL_CTX_RECORD_SET_JMP_CND_(expr, result, goto label, GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN), __VA_ARGS__);

//====== VERIFY =========

#define GBL_CTX_VERIFY_(expr, result, srcLoc, ...)              \
    GBL_CTX_RECORD_SET_JMP_CND_((expr),                         \
                               result,                          \
                               goto GBL_CTX_END_LABEL,          \
                               srcLoc, __VA_ARGS__)

#define GBL_CTX_VERIFY_N(srcLoc, expr, result, ...)  \
    GBL_CTX_VERIFY_(expr, result, srcLoc, __VA_ARGS__)

#define GBL_CTX_VERIFY_3(srcLoc, expr, result)                \
    GBL_CTX_VERIFY_N(srcLoc, expr, result, gblResultString(result))

#define GBL_CTX_VERIFY(...)                                                                                         \
    GBL_STMT_START {                                                                                                \
        const GblSrcLoc src_ = GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN);                                   \
        GBL_VA_OVERLOAD_SELECT(GBL_CTX_VERIFY, GBL_VA_OVERLOAD_SUFFIXER_3_N, src_, __VA_ARGS__)(src_, __VA_ARGS__); \
    } GBL_STMT_END

//====== VERIFY_EXPRESSION =========

#define GBL_CTX_VERIFY_EXPRESSION_N(srcLoc, expr, ...) \
    GBL_CTX_VERIFY_(expr, GBL_RESULT_ERROR_INVALID_EXPRESSION, srcLoc, __VA_ARGS__)

#define GBL_CTX_VERIFY_EXPRESSION_2(src, expr) \
    GBL_CTX_VERIFY_EXPRESSION_N(src, expr, "Invalid Expression: "#expr)

#define GBL_CTX_VERIFY_EXPRESSION(...)                                                                                          \
    GBL_STMT_START {                                                                                                            \
        const GblSrcLoc src_ = GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN);                                               \
        GBL_VA_OVERLOAD_SELECT(GBL_CTX_VERIFY_EXPRESSION, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);  \
    } GBL_STMT_END

//====== VERIFY_POINTER =========
#define GBL_CTX_VERIFY_POINTER_N(srcLoc, expr, ...) \
    GBL_CTX_VERIFY_(expr, GBL_RESULT_ERROR_INVALID_POINTER, srcLoc, __VA_ARGS__)

#define GBL_CTX_VERIFY_POINTER_2(src, expr) \
    GBL_CTX_VERIFY_POINTER_N(src, expr, "Invalid Pointer")

#define GBL_CTX_VERIFY_POINTER(...)                                                                                             \
    GBL_STMT_START {                                                                                                            \
        const GblSrcLoc src_ = GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN);                                               \
        GBL_VA_OVERLOAD_SELECT(GBL_CTX_VERIFY_POINTER, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);     \
    } GBL_STMT_END

//====== VERIFY_ARG =========
#define GBL_CTX_VERIFY_ARG_N(srcLoc, expr, ...) \
    GBL_CTX_VERIFY_(expr, GBL_RESULT_ERROR_INVALID_ARG, srcLoc, __VA_ARGS__)

#define GBL_CTX_VERIFY_ARG_2(src, expr) \
    GBL_CTX_VERIFY_ARG_N(src, expr, "Invalid Arg: "#expr);

#define GBL_CTX_VERIFY_ARG(...)                                                                                             \
    GBL_STMT_START {                                                                                                        \
        const GblSrcLoc src_ = GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN);                                           \
        GBL_VA_OVERLOAD_SELECT(GBL_CTX_VERIFY_ARG, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);     \
    } GBL_STMT_END

//===== VERIFY_TYPE =======
#define GBL_CTX_VERIFY_TYPE_N(srcLoc, actualType, expectedType, ...) \
    GBL_CTX_VERIFY_(GblType_check(actualType, expectedType), GBL_RESULT_ERROR_TYPE_MISMATCH, srcLoc, __VA_ARGS__)

#define GBL_CTX_VERIFY_TYPE_3(srcLoc, actualType, expectedType) \
    GBL_CTX_VERIFY_TYPE_N(srcLoc, actualType, expectedType, "Type mismatch [Actual: %s, Expected: %s]", GblType_name(actualType), GblType_name(expectedType))

#define GBL_CTX_VERIFY_TYPE_2(srcLoc, actualType) \
    GBL_CTX_VERIFY_(actualType != GBL_INVALID_TYPE, GBL_RESULT_ERROR_INVALID_TYPE, srcLoc, "Invalid Type");

#define GBL_CTX_VERIFY_TYPE(...)                                                                                            \
    GBL_STMT_START {                                                                                                        \
        const GblSrcLoc src_ = GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN);                                           \
        GBL_VA_OVERLOAD_SELECT(GBL_CTX_VERIFY_TYPE, GBL_VA_OVERLOAD_SUFFIXER_3_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);    \
    } GBL_STMT_END

//==== LAST RECORD =====
#define GBL_CTX_VERIFY_LAST_RECORD()                        \
    GBL_STMT_START {                                        \
        const GblCallRecord* pRecord =                      \
            GblThd_callRecord(NULL);                     \
        if(pRecord && GBL_RESULT_ERROR(pRecord->result)) {  \
            GBL_CTX_RESULT() = pRecord->result;             \
            GBL_CTX_DONE();                                 \
        }                                                   \
    } GBL_STMT_END

#define GBL_CTX_CLEAR_LAST_RECORD()                         \
    GblThd_setCallRecord(NULL, NULL)

//===== C STD ERRNO =====

#ifdef GBL_CONFIG_ERRNO_CHECKS
#define     GBL_CTX_ERRNO_CLEAR()   errno = 0
#else
#define     GBL_CTX_ERRNO_CLEAR()
#endif

#ifdef GBL_CONFIG_ERRNO_CHECKS
#   define GBL_CTX_PERROR(...)                      \
    GBL_STMT_START {                                \
        if(errno) GBL_UNLIKELY {                    \
            const GBL_RESULT code =                 \
                GBL_ERRNO_RESULT(errno);            \
            GBL_CTX_VERIFY(                         \
                GBL_RESULT_SUCCESS(code),           \
                code,                               \
                "ERRNO: %s", strerror(errno));      \
        }                                           \
    } GBL_STMT_END
#else
#   define GBL_CTX_PERROR(...)
#endif

GBL_MAYBE_UNUSED GBL_INLINE GBL_RESULT GBL_ERRNO_RESULT(int ernum) {
    switch(ernum) {
    case 0:     return GBL_RESULT_SUCCESS;
    default:    return GBL_RESULT_ERROR;
    }
}

// =======  API ======
#define GBL_CTX_INLINE_RETVAL() GBL_CTX_INLINE_RETURN_VALUE_NAME

#define GBL_CTX_INLINE(MethodPrefix, ReturnType, ...)                                                \
    GBL_INLINE ReturnType GBL_CTX_INLINE_##MethodPrefix##_(GBL_CTX_FRAME_DECLARE, ##__VA_ARGS__) {   \
        ReturnType GBL_CTX_INLINE_RETURN_VALUE_NAME;

#define GBL_CTX_INLINE_BEGIN(InitialRetValue)       \
        GBL_CTX_INLINE_RETVAL() = InitialRetValue;


#define GBL_CTX_INLINE_END()                                    \
        goto GBL_CTX_END_LABEL;                                 \
        GBL_CTX_END_LABEL: GBL_STMT_START {;} GBL_STMT_END;     \
    }

#define GBL_CTX_INLINE_RETURN()     \
    return GBL_CTX_INLINE_RETVAL()


#define GBL_CTX_INLINE_CALL_(MethodPrefix, srcLoc, ...)                                 \
        GBL_CTX_INLINE_##MethodPrefix##_(GBL_CTX_FRAME() GBL_VA_ARGS(__VA_ARGS__))

#define GBL_CTX_INLINE_CALL(MethodPrefix, ...) \
    GBL_CTX_INLINE_CALL_(MethodPrefix, GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN) GBL_VA_ARG)

// ============== GBL EXT USERMETHODS ==========

#define GBL_CTX_EXT(prefix, ...)                                                \
    GBL_STMT_START {                                                            \
        const GBL_RESULT localResult = GblContext_##prefix(GBL_CTX_CONTEXT(),   \
                                                           GBL_CTX_FRAME_NAME,  \
                                                           ##__VA_ARGS__);      \
        GBL_ASSERT(!(GBL_CONFIG_ASSERT_ERROR_ENABLED &&                         \
                     GBL_RESULT_ERROR(localResult)),                            \
                   "Ext["#prefix"]: ERROR");                                    \
        GBL_ASSERT(!(GBL_CONFIG_ASSERT_PARTIAL_ENABLED &&                       \
                     GBL_RESULT_PARTIAL(localResult)),                          \
                   "Ext["#prefix"]: ERROR");                                    \
        GBL_UNUSED(localResult);                                                \
    } GBL_STMT_END


GBL_MAYBE_UNUSED GBL_CTX_INLINE(MALLOC, void*, size_t  size, size_t align, const char* pDebugStr) {
    GBL_CTX_INLINE_BEGIN(GBL_NULL);
    if(align == 0) {
        align = GBL_ALIGNOF(GBL_MAX_ALIGN_T);
        size = gblAlignedAllocSizeDefault(size);
    }
    GBL_ASSERT(size % align == 0);
    GBL_CTX_EXT(memAlloc_, size, align, pDebugStr, &GBL_CTX_INLINE_RETVAL());
    GBL_CTX_INLINE_END();
    // modify/set return value based on result
    GBL_CTX_INLINE_RETURN();
}

#define GBL_CTX_MALLOC_4(src, size, align, dbgStr) \
    GBL_CTX_INLINE_CALL_(MALLOC, src, size, align, dbgStr)

#define GBL_CTX_MALLOC_3(src, size, align) \
    GBL_CTX_MALLOC_4(src, size, align, GBL_NULL)

#define GBL_CTX_MALLOC_2(src, size) \
    GBL_CTX_MALLOC_3(src, gblAlignedAllocSizeDefault(size), GBL_ALIGNOF(GBL_MAX_ALIGN_T))

#define GBL_CTX_MALLOC(...)  \
    GBL_VA_OVERLOAD_SELECT(GBL_CTX_MALLOC, GBL_VA_OVERLOAD_SUFFIXER_ARGC, 1, __VA_ARGS__)(GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN), __VA_ARGS__)

#define GBL_CTX_NEW_4(src, type, count, dbgStr) \
    (type*)GBL_CTX_INLINE_CALL_(MALLOC, src, gblAlignedAllocSizeDefault(sizeof(type)*count), 0, dbgStr)

#define GBL_CTX_NEW_3(src, type, count) \
    GBL_CTX_NEW_4(src, type, count, GBL_NULL)

#define GBL_CTX_NEW_2(src, type) \
    GBL_CTX_NEW_3(src, type, 1)

#define GBL_CTX_NEW(...) \
    GBL_VA_OVERLOAD_SELECT(GBL_CTX_NEW, GBL_VA_OVERLOAD_SUFFIXER_ARGC, 1, __VA_ARGS__)(GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN), __VA_ARGS__)

GBL_MAYBE_UNUSED GBL_CTX_INLINE(REALLOC, void*, void* pData, size_t  newSize, size_t  newAlign) {
    GBL_CTX_INLINE_BEGIN(NULL);

    GBL_CTX_EXT(memRealloc_, pData, newSize, newAlign, &GBL_CTX_INLINE_RETVAL());
    GBL_CTX_INLINE_END();
    // modify/set return value based on result
    GBL_CTX_INLINE_RETURN();
}

#define GBL_CTX_REALLOC_4(src, pData, newSize, newAlign) \
    GBL_CTX_INLINE_CALL_(REALLOC, src, pData, newSize, newAlign)

#define GBL_CTX_REALLOC_3(src, pData, newSize) \
    GBL_CTX_REALLOC_4(src, pData, newSize, 1)

#define GBL_CTX_REALLOC(...)  \
     GBL_VA_OVERLOAD_SELECT(GBL_CTX_REALLOC, GBL_VA_OVERLOAD_SUFFIXER_ARGC, 1, __VA_ARGS__)(GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN), __VA_ARGS__)

#define GBL_CTX_RENEW_5(src, ptr, type, count, dbgStr) \
    GBL_CTX_INLINE_CALL(REALLOC, src, ptr, sizeof(type)*count, dbgStr)

#define GBL_CTX_RENEW_4(src, ptr, type, count) \
    GBL_CTX_RENEW_5(src, ptr, type, count, GBL_NULL)

#define GBL_CTX_RENEW_3(src, ptr, type) \
    GBL_CTX_RENEW_4(src, ptr, type, 1)

#define GBL_CTX_RENEW(...) \
     GBL_VA_OVERLOAD_SELECT(GBL_CTX_REALLOC, GBL_VA_OVERLOAD_SUFFIXER_ARGC, 1, __VA_ARGS__)(GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN), __VA_ARGS__)

#define GBL_CTX_FREE(pData) \
    GBL_CTX_SOURCE_SCOPED(GBL_CTX_EXT, (GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN)), memFree_, pData)

#define GBL_CTX_PUSH_(srcLoc, ...)                                  \
    GBL_STMT_START {                                                \
        GBL_CTX_SOURCE_LOC_PUSH(srcLoc);                            \
        GBL_RESULT_SUCCESS(GblThd_logPush(NULL));                \
        GBL_CTX_EXT(logPush_);                                      \
        GBL_CTX_SOURCE_POP();                                       \
        ++GBL_CTX_FRAME()->stackDepth;                              \
    } GBL_STMT_END

#define GBL_CTX_PUSH()                                                              \
    GBL_STMT_START {                                                                \
        const GblSrcLoc src_ = GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN);   \
        GBL_CTX_PUSH_(src_);                                                        \
    } GBL_STMT_END

#define GBL_CTX_PUSH_VERBOSE_N(srcLoc, pFmt, ...)                               \
    GBL_STMT_START {                                                            \
        GBL_CTX_SOURCE_SCOPED(GBL_CTX_VERBOSE, srcLoc, pFmt, ##__VA_ARGS__);    \
        GBL_CTX_PUSH_(srcLoc);                                                  \
    } GBL_STMT_END

#define GBL_CTX_PUSH_VERBOSE(...)                                                                                               \
    GBL_STMT_START {                                                                                                            \
        const GblSrcLoc src_ = GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN);                                               \
        GBL_VA_OVERLOAD_SELECT(GBL_CTX_PUSH_VERBOSE, GBL_VA_OVERLOAD_SUFFIXER_1_N, src_, ##__VA_ARGS__)(src_, ##__VA_ARGS__);   \
    } GBL_STMT_END

#define GBL_CTX_POP_2(srcLoc, count)                                    \
        GblThd_logPop(NULL, count);                                  \
        GBL_CTX_SOURCE_SCOPED(GBL_CTX_EXT, srcLoc, logPop_, count);     \
        GBL_CTX_FRAME()->stackDepth -= count;

#define GBL_CTX_POP_1(srcLoc) \
    GBL_CTX_POP_2(srcLoc, 1)

#define GBL_CTX_POP(...)                                                                        \
    GBL_STMT_START {                                                                            \
        const GblSrcLoc loc = GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN);                \
        GBL_VA_OVERLOAD_CALL(GBL_CTX_POP, GBL_VA_OVERLOAD_SUFFIXER_ARGC, loc, __VA_ARGS__);     \
    } GBL_STMT_END

GBL_MAYBE_UNUSED GBL_CTX_INLINE(LOG, GBL_RESULT, GblFlags level, const char* pFmt, ...) {
    GBL_CTX_INLINE_BEGIN(GBL_RESULT_SUCCESS);
    va_list varArgs;
    va_start(varArgs, pFmt);

    GBL_CTX_EXT(logWrite_, level, pFmt, varArgs);
    va_end(varArgs);
    GBL_CTX_INLINE_END();
    GBL_CTX_INLINE_RETURN();
}

#define GBL_CTX_LOG_(src, level, pFmt, ...) \
    GBL_CTX_INLINE_CALL_(LOG, src, level, pFmt, ##__VA_ARGS__)
#define GBL_CTX_LOG(level, pFmt, ...)       \
   GBL_CTX_LOG_(GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN), level, pFmt, ##__VA_ARGS__)

#define GBL_CTX_DEBUG(pFmt, ...)    \
    GBL_CTX_LOG_(GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN), GBL_LOG_LEVEL_DEBUG, pFmt, ##__VA_ARGS__)

#define GBL_CTX_VERBOSE(pFmt, ...)  \
    GBL_CTX_LOG_(GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN), GBL_LOG_LEVEL_VERBOSE, pFmt, ##__VA_ARGS__)

#define GBL_CTX_INFO(pFmt, ...)     \
    GBL_CTX_LOG_(GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN), GBL_LOG_LEVEL_INFO, pFmt, ##__VA_ARGS__)

#define GBL_CTX_WARN(pFmt, ...)     \
    GBL_CTX_LOG_(GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN), GBL_LOG_LEVEL_WARNING, pFmt, ##__VA_ARGS__)

#define GBL_CTX_ERROR(pFmt, ...)    \
    GBL_CTX_LOG_(GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN), GBL_LOG_LEVEL_ERROR, pFmt, ##__VA_ARGS__)

#define GBL_CTX_EVENT(event) \
    GBL_STMT_START { \
        const GblSrcLoc src_ = GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN);   \
        GBL_UNUSED(src_);                                                           \
       GblObject_sendEvent(GBL_CTX_OBJECT(), (GblEvent*)event);                     \
    } GBL_STMT_END


// ===================== RECORD = > ASSERT =========================

// Base Enabled Logic
#define GBL_CTX_RECORD_ASSERT_(record, test)                    \
    GBL_STMT_START {                                            \
        GBL_ASSERT(!test(record->result), record->message);     \
    } GBL_STMT_END

// Base Conditional Logic
#define GBL_CTX_RECORD_ASSERT_CONDITIONAL_(enabled, record, test)   \
    GBL_MACRO_CONDITIONAL_CALL(enabled, GBL_CTX_RECORD_ASSERT_, record, test)

// Partial Success
#define GBL_CTX_RECORD_ASSERT_PARTIAL(record)                               \
    GBL_CTX_RECORD_ASSERT_CONDITIONAL_(GBL_CONFIG_ASSERT_PARTIAL_ENABLED,   \
                                        record, GBL_RESULT_PARTIAL)
// Error
#define GBL_CTX_RECORD_ASSERT_ERROR(record)                                 \
    GBL_CTX_RECORD_ASSERT_CONDITIONAL_(GBL_CONFIG_ASSERT_ERROR_ENABLED,     \
                                       record, GBL_RESULT_ERROR)

// Unknown
#define GBL_CTX_RECORD_ASSERT_UNKNOWN(record)                                 \
    GBL_CTX_RECORD_ASSERT_CONDITIONAL_(GBL_CONFIG_ASSERT_UNKNOWN_ENABLED,     \
                                       record, GBL_RESULT_UNKNOWN)
// Both Together
#define GBL_CTX_RECORD_ASSERT(record)                       \
    GBL_STMT_START {                                        \
            GBL_CTX_RECORD_ASSERT_ERROR(record);            \
            GBL_CTX_RECORD_ASSERT_PARTIAL(record);          \
            GBL_CTX_RECORD_ASSERT_UNKNOWN(record);          \
    } GBL_STMT_END


// ==========================  RECORD = > LOG ========================

// Base Enabled Logic (uses a prefix prefix for all magic)
#define GBL_CTX_RECORD_LOG_(prefix, record)                                     \
        GBL_STMT_START {                                                        \
            if(GBL_RESULT_##prefix(record->result)) GBL_UNLIKELY {              \
                GBL_CTX_LOG(GBL_CONFIG_LOG_##prefix##_LEVEL,                    \
                            "%s: %s",                                           \
                             gblResultString(record->result), record->message); \
            }                                                                   \
        } GBL_STMT_END


// Base Conditional Logic
#define GBL_CTX_RECORD_LOG_CONDITIONAL_(prefix, record)                 \
        GBL_MACRO_CONDITIONAL_CALL(GBL_CONFIG_LOG_##prefix##_ENABLED,   \
                                    GBL_CTX_RECORD_LOG_, prefix, record)

// Partial Success
#define GBL_CTX_RECORD_LOG_PARTIAL(record) \
        GBL_CTX_RECORD_LOG_CONDITIONAL_(PARTIAL, record)

// Error
#define GBL_CTX_RECORD_LOG_ERROR(record) \
        GBL_CTX_RECORD_LOG_CONDITIONAL_(ERROR, record)

// Unknown
#define GBL_CTX_RECORD_LOG_UNKNOWN(record) \
        GBL_CTX_RECORD_LOG_CONDITIONAL_(UNKNOWN, record)

// Combined
#define GBL_CTX_RECORD_LOG(record)                      \
    GBL_STMT_START {                                    \
        GBL_CTX_RECORD_LOG_ERROR(record);               \
        GBL_CTX_RECORD_LOG_PARTIAL(record);             \
        GBL_CTX_RECORD_LOG_UNKNOWN(record);             \
    } GBL_STMT_END


// ================= RECORD => HANDLE::LAST_ERROR ==============
#define GBL_CTX_RECORD_LAST_RECORD_(prefix, record)                                     \
    GBL_STMT_START {                                                                    \
        if(GBL_RESULT_##prefix(record->result)) {                                       \
            GblContext_callRecordSet_(GBL_CTX_CONTEXT(), GBL_CTX_FRAME_NAME, record);   \
            GblThd_setCallRecord(NULL, record);                                      \
        }                                                                               \
    } GBL_STMT_END

#define GBL_CTX_RECORD_LAST_RECORD_PARTIAL(record) \
    GBL_CTX_RECORD_LAST_RECORD_(PARTIAL, record)

#define GBL_CTX_RECORD_LAST_RECORD_ERROR(record)   \
    GBL_CTX_RECORD_LAST_RECORD_(ERROR, record)

#define GBL_CTX_RECORD_LAST_RECORD_UNKNOWN(record) \
    GBL_CTX_RECORD_LAST_RECORD_(UNKNOWN, record)

#define GBL_CTX_RECORD_LAST_RECORD(record)          \
    GBL_STMT_START {                                \
        GBL_CTX_RECORD_LAST_RECORD_ERROR(record);   \
        GBL_CTX_RECORD_LAST_RECORD_PARTIAL(record); \
        GBL_CTX_RECORD_LAST_RECORD_UNKNOWN(record); \
    } GBL_STMT_END

// ================= RECORD => TOP-LEVEL DISPATCH ==============
#define GBL_CTX_RECORD_HANDLER(record)            \
    GBL_STMT_START {                              \
        GBL_CTX_RECORD_LOG((record));             \
        GBL_CTX_RECORD_LAST_RECORD((record));     \
        GBL_CTX_RECORD_ASSERT((record));          \
    } GBL_STMT_END

#define GBL_CTX_RECORD_SET_N(file, func, line,  result, ...)                                \
    GBL_STMT_START {                                                                        \
        GBL_CTX_SOURCE_LOC_PUSH(GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN));         \
        GblCallRecord_construct(&GBL_CTX_RECORD(), result, GBL_CTX_SOURCE(), __VA_ARGS__);  \
        GBL_CTX_RECORD_HANDLER(&GBL_CTX_RECORD());                                          \
        GBL_CTX_SOURCE_POP();                                                               \
    } GBL_STMT_END

#define GBL_CTX_RECORD_SET_5(file, func, line, result, pFmt) \
    GBL_CTX_RECORD_SET_N(file, func, line, result, "%s", pFmt)

#define GBL_CTX_RECORD_SET_4(file, func, line, result) \
    GBL_CTX_RECORD_SET_5(file, func, line, result, gblResultString(result))

#define GBL_CTX_RECORD_SET(...)  \
    GBL_VA_OVERLOAD_CALL(GBL_CTX_RECORD_SET, GBL_VA_OVERLOAD_SUFFIXER_5_N, GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN, __VA_ARGS__)

#define GBL_CTX_CALL_N(src, funcCall, ...)                                      \
    GBL_STMT_START {                                                            \
        GBL_CTX_SOURCE_LOC_PUSH(src);                                           \
        GBL_MAYBE_UNUSED const GBL_RESULT localResult = (funcCall);             \
        if(!GBL_RESULT_SUCCESS(localResult)) GBL_UNLIKELY {                     \
            GBL_CTX_RESULT() = localResult;                                     \
        }                                                                       \
        GBL_CTX_SOURCE_POP();                                                   \
    } GBL_STMT_END

#define GBL_CTX_CALL_2(src, funcCall) \
    GBL_CTX_CALL_N(src, funcCall, #funcCall);

#define GBL_CTX_CALL(...)                                                                                           \
    GBL_STMT_START {                                                                                                \
        const GblSrcLoc src_ = GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN);                                   \
        GBL_VA_OVERLOAD_SELECT(GBL_CTX_CALL, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);   \
    } GBL_STMT_END

#define GBL_CTX_VERIFY_CALL(...) \
    GBL_STMT_START {    \
        const GblSrcLoc src_ = GBL_SRC_LOC(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN);                                   \
        GBL_VA_OVERLOAD_SELECT(GBL_CTX_CALL, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);   \
        if(!GBL_RESULT_SUCCESS(GBL_CTX_RESULT())) goto GBL_CTX_END_LABEL;                                           \
    } GBL_STMT_END


// ================= TOP-LEVEL API UTILITIES ==============

#define GBL_CTX_BEGIN_FRAME(file, func, line, pObject, frame)                               \
    GBL_CTX_FRAME_DECLARE = frame;                                                          \
    GblStackFrame_construct(GBL_CTX_FRAME_NAME, (GblObject*)pObject, GBL_RESULT_SUCCESS);   \
    GBL_RESULT_SUCCESS(GblThd_stackFramePush(NULL, GBL_CTX_FRAME_NAME))

#define GBL_CTX_BEGIN_LOG_4(file, func, line, hHandle)  \
    GBL_CTX_BEGIN_FRAME(file, func, line, hHandle, ((GblStackFrame*)GBL_ALLOCA(sizeof(GblStackFrame))))

#define GBL_CTX_BEGIN_LOG_N(file, func, line, hHandle, ...)    \
    GBL_CTX_BEGIN_LOG_5(file, func, line, hHandle);            \
    GBL_CTX_PUSH_VERBOSE(__VA_ARGS__);

#define GBL_CTX_BEGIN(...) \
        GBL_VA_OVERLOAD_CALL(GBL_CTX_BEGIN_LOG, GBL_VA_OVERLOAD_SUFFIXER_4_N, GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN, __VA_ARGS__)

#define GBL_CTX_DONE()  \
    goto GBL_CTX_END_LABEL

#define GBL_CTX_END_BLOCK()                                 \
    goto GBL_CTX_END_LABEL;                                 \
    GBL_LABEL_EMPTY(GBL_CTX_END_LABEL);                     \
        if(GBL_CTX_FRAME_NAME->stackDepth)                  \
            GBL_CTX_POP(GBL_CTX_FRAME_NAME->stackDepth);    \
            GblThd_stackFramePop(NULL)

#define GBL_CTX_END()               \
        GBL_CTX_END_BLOCK();        \
        return GBL_CTX_FRAME_NAME->record.result

#define GBL_CTX_END_EMPTY()         \
        GBL_LABEL_EMPTY(GBL_CTX_END_LABEL)

#define GBL_CTX_BLOCK_6(file, func, line, hHandle, frame, block)       \
    GBL_CTX_BEGIN_FRAME(file, func, line, hHandle, frame);             \
    block;                                                             \
    GBL_CTX_END_BLOCK()

#define GBL_CTX_BLOCK_5(file, func, line, hHandle, block) \
    GBL_CTX_BLOCK_7(file, func, line, hHandle, ((GblStackFrame*)GBL_ALLOCA(sizeof(GblStackFrame))), block)

#define GBL_CTX_BLOCK_4(file, func, line, block) \
    GBL_CTX_BLOCK_6(file, func, line, NULL, block)

#define GBL_CTX_BLOCK(...) \
    GBL_VA_OVERLOAD_SELECT(GBL_CTX_BLOCK, GBL_VA_OVERLOAD_SUFFIXER_ARGC, GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN, __VA_ARGS__)(GBL_SRC_FILE, GBL_SRC_FN, GBL_SRC_LN, __VA_ARGS__)

GBL_DECLS_END

#endif // GIMBAL_API_H
