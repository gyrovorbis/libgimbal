/*! \file
 *  \brief GBL_EXPORT GBL_RESULT top-level debug and control-flow macros
 *  \ingroup debug
 */
#ifndef GIMBAL_API_FRAME_H
#define GIMBAL_API_FRAME_H

#include "gimbal_api_generators.h"
#include "../algorithms/gimbal_numeric.h"
#include "gimbal_ext.h"
#include "gimbal_call_stack.h"
#include "gimbal_thread.h"
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

// ===== GBL API FRAME CONVENIENCE ACCESSORS =====

#define GBL_API_FRAME_DECLARE   GblStackFrame* GBL_API_FRAME_NAME
#define GBL_API_FRAME()         (GBL_API_FRAME_NAME)
#define GBL_API_CONTEXT()       GBL_API_FRAME()->pContext
#define GBL_API_OBJECT()        GBL_API_FRAME()->pObject
#define GBL_API_RECORD()        GBL_API_FRAME()->record
#define GBL_API_RESULT()        GBL_API_RECORD().result
#define GBL_API_SOURCE()        GBL_API_FRAME()->sourceCurrent
#define GBL_API_LAST_RECORD()   GblThread_callRecord(NULL)
#define GBL_API_LAST_RESULT()   GblThread_callRecord(NULL)->result

// ====== SOURCE LOCATION PROPAGATION UTILITIES =====

#define GBL_API_SOURCE_LOC_PUSH(srcLoc)                     \
    GBL_API_STACK_FRAME_SOURCE_PUSH(GBL_API_FRAME(), srcLoc)
#define GBL_API_SOURCE_PUSH(FILE, FUNCTION, LINE, COLUMN)   \
    GBL_API_SOURCE_LOC_PUSH(GBL_SOURCE_LOCATION(FILE, FUNCTION, LINE, COLUMN))

#define GBL_API_SOURCE_POP()                    \
    GBL_API_STACK_FRAME_SOURCE_POP(GBL_API_FRAME())

#define GBL_API_SOURCE_SCOPED(CALL, loc, ...)   \
    GBL_STMT_START {                            \
        GBL_API_SOURCE_LOC_PUSH((loc));         \
        GBL_IDENTITY(CALL)(__VA_ARGS__);        \
        GBL_API_SOURCE_POP();                   \
    } GBL_STMT_END

// ====== BOTTOM-LEVEL ERROR HANDLING + CONTROL FLOW =====

#define GBL_API_RECORD_SET_JMP_CND_(expr, result, label, srcLoc, ...)          \
    GBL_STMT_START {                                                           \
        GBL_API_SOURCE_LOC_PUSH(srcLoc);                                       \
        if(!(expr)) GBL_UNLIKELY {                                             \
            GBL_API_RECORD_SET(result, __VA_ARGS__);                           \
            GBL_API_SOURCE_POP();                                              \
            label;                                                             \
        } else GBL_LIKELY {                                                    \
            GBL_API_SOURCE_POP();                                              \
        }                                                                      \
    } GBL_STMT_END

#define GBL_API_RECORD_SET_JMP_CND(expr, result, label, ...) \
    GBL_API_RECORD_SET_JMP_CND_(expr, result, goto label, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL), __VA_ARGS__);

//====== VERIFY =========

#define GBL_API_VERIFY_(expr, result, srcLoc, ...)              \
    GBL_API_RECORD_SET_JMP_CND_((expr),                         \
                               result,                          \
                               goto GBL_API_END_LABEL,          \
                               srcLoc, __VA_ARGS__)



#define GBL_API_VERIFY_N(srcLoc, expr, result, ...)  \
    GBL_API_VERIFY_(expr, result, srcLoc, __VA_ARGS__)

#define GBL_API_VERIFY_3(srcLoc, expr, result)                \
    GBL_API_VERIFY_N(srcLoc, expr, result, gblResultString(result))

#define GBL_API_VERIFY(...)                                                                                         \
    GBL_STMT_START {                                                                                                \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);                                             \
        GBL_VA_OVERLOAD_SELECT(GBL_API_VERIFY, GBL_VA_OVERLOAD_SUFFIXER_3_N, src_, __VA_ARGS__)(src_, __VA_ARGS__); \
    } GBL_STMT_END

//====== VERIFY_EXPRESSION =========

#define GBL_API_VERIFY_EXPRESSION_N(srcLoc, expr, ...) \
    GBL_API_VERIFY_(expr, GBL_RESULT_ERROR_INVALID_EXPRESSION, srcLoc, __VA_ARGS__)

#define GBL_API_VERIFY_EXPRESSION_2(src, expr) \
    GBL_API_VERIFY_EXPRESSION_N(src, expr, "Invalid Expression: "#expr)

#define GBL_API_VERIFY_EXPRESSION(...)                                                                                          \
    GBL_STMT_START {                                                                                                            \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);                                                         \
        GBL_VA_OVERLOAD_SELECT(GBL_API_VERIFY_EXPRESSION, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);  \
    } GBL_STMT_END

//====== VERIFY_POINTER =========
#define GBL_API_VERIFY_POINTER_N(srcLoc, expr, ...) \
    GBL_API_VERIFY_(expr, GBL_RESULT_ERROR_INVALID_POINTER, srcLoc, __VA_ARGS__)

#define GBL_API_VERIFY_POINTER_2(src, expr) \
    GBL_API_VERIFY_POINTER_N(src, expr, "Invalid Pointer")

#define GBL_API_VERIFY_POINTER(...)                                                                                             \
    GBL_STMT_START {                                                                                                            \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);                                                         \
        GBL_VA_OVERLOAD_SELECT(GBL_API_VERIFY_POINTER, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);     \
    } GBL_STMT_END

//====== VERIFY_ARG =========
#define GBL_API_VERIFY_ARG_N(srcLoc, expr, ...) \
    GBL_API_VERIFY_(expr, GBL_RESULT_ERROR_INVALID_ARG, srcLoc, __VA_ARGS__)

#define GBL_API_VERIFY_ARG_2(src, expr) \
    GBL_API_VERIFY_ARG_N(src, expr, "Invalid Arg: "#expr);

#define GBL_API_VERIFY_ARG(...)                                                                                             \
    GBL_STMT_START {                                                                                                        \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);                                                     \
        GBL_VA_OVERLOAD_SELECT(GBL_API_VERIFY_ARG, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);     \
    } GBL_STMT_END

//===== VERIFY_TYPE =======
#define GBL_API_VERIFY_TYPE_N(srcLoc, actualType, expectedType, ...) \
    GBL_API_VERIFY_(GblType_check(actualType, expectedType), GBL_RESULT_ERROR_TYPE_MISMATCH, srcLoc, __VA_ARGS__)

#define GBL_API_VERIFY_TYPE_3(srcLoc, actualType, expectedType) \
    GBL_API_VERIFY_TYPE_N(srcLoc, actualType, expectedType, "Type mismatch [Actual: %s, Expected: %s]", GblType_name(actualType), GblType_name(expectedType))

#define GBL_API_VERIFY_TYPE_2(srcLoc, actualType) \
    GBL_API_VERIFY_(actualType != GBL_INVALID_TYPE, GBL_RESULT_ERROR_INVALID_TYPE, srcLoc, "Invalid Type");

#define GBL_API_VERIFY_TYPE(...)                                                                                            \
    GBL_STMT_START {                                                                                                        \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);                                                     \
        GBL_VA_OVERLOAD_SELECT(GBL_API_VERIFY_TYPE, GBL_VA_OVERLOAD_SUFFIXER_3_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);    \
    } GBL_STMT_END

//==== LAST RECORD =====
#define GBL_API_VERIFY_LAST_RECORD()                        \
    GBL_STMT_START {                                        \
        const GblCallRecord* pRecord =                      \
            GblThread_callRecord(NULL);                     \
        if(pRecord && GBL_RESULT_ERROR(pRecord->result)) {  \
            GBL_API_RESULT() = pRecord->result;             \
            GBL_API_DONE();                                 \
        }                                                   \
    } GBL_STMT_END

#define GBL_API_CLEAR_LAST_RECORD()                         \
    GblThread_setCallRecord(NULL, NULL)

//===== C STD ERRNO =====

#ifdef GBL_CONFIG_ERRNO_CHECKS
#define     GBL_API_ERRNO_CLEAR()   errno = 0
#else
#define     GBL_API_ERRNO_CLEAR()
#endif

#ifdef GBL_CONFIG_ERRNO_CHECKS
#   define GBL_API_PERROR(...)                      \
    GBL_STMT_START {                                \
        if(errno) GBL_UNLIKELY {                    \
            const GBL_RESULT code =                 \
                GBL_ERRNO_RESULT(errno);            \
            GBL_API_VERIFY(                         \
                GBL_RESULT_SUCCESS(code),           \
                code,                               \
                "ERRNO: %x", errno);                \
        }                                           \
    } GBL_STMT_END
#else
#   define GBL_API_PERROR(...)
#endif

GBL_MAYBE_UNUSED GBL_INLINE GBL_RESULT GBL_ERRNO_RESULT(int ernum) {
    switch(ernum) {
    case 0:     return GBL_RESULT_SUCCESS;
    default:    return GBL_RESULT_ERROR;
    }
}

// =======  API ======
#define GBL_API_INLINE_RETVAL() GBL_API_INLINE_RETURN_VALUE_NAME

#define GBL_API_INLINE(MethodPrefix, ReturnType, ...)                                                               \
    GBL_INLINE ReturnType GBL_API_INLINE_##MethodPrefix##_(GBL_API_FRAME_DECLARE, SrcLoc srcLoc, ##__VA_ARGS__) {   \
        ReturnType GBL_API_INLINE_RETURN_VALUE_NAME;

#define GBL_API_INLINE_BEGIN(InitialRetValue)       \
        GBL_API_INLINE_RETVAL() = InitialRetValue;  \
        GBL_API_SOURCE_LOC_PUSH(srcLoc);


#define GBL_API_INLINE_END()                                    \
        goto GBL_API_END_LABEL;                                 \
        GBL_API_END_LABEL: GBL_STMT_START {;} GBL_STMT_END;     \
    }

#define GBL_API_INLINE_RETURN()     \
    GBL_API_SOURCE_POP();           \
    return GBL_API_INLINE_RETVAL()


#define GBL_API_INLINE_CALL_(MethodPrefix, srcLoc, ...) \
    GBL_API_INLINE_##MethodPrefix##_(GBL_API_FRAME(), srcLoc GBL_VA_ARGS(__VA_ARGS__))

#define GBL_API_INLINE_CALL(MethodPrefix, ...) \
    GBL_API_INLINE_CALL_(MethodPrefix, SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL) GBL_VA_ARG)

// ============== GBL EXT USERMETHODS ==========

#define GBL_API_EXT(prefix, ...)                                                                                        \
    GBL_STMT_START {                                                                                                    \
        GBL_API_SOURCE_PUSH(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);                                                         \
        GBL_MAYBE_UNUSED const GBL_RESULT localResult = GBL_EXT_##prefix(GBL_API_FRAME(), ##__VA_ARGS__);               \
        GBL_ASSERT(!(GBL_CONFIG_ASSERT_ERROR_ENABLED && GBL_RESULT_ERROR(localResult)), "Ext["#prefix"]: ERROR");       \
        GBL_ASSERT(!(GBL_CONFIG_ASSERT_PARTIAL_ENABLED && GBL_RESULT_PARTIAL(localResult)), "Ext["#prefix"]: ERROR");   \
        GBL_UNUSED(localResult);                                                                                        \
        GBL_API_SOURCE_POP();                                                                                           \
    } GBL_STMT_END


GBL_MAYBE_UNUSED GBL_API_INLINE(MALLOC, void*, GblSize size, GblSize align, const char* pDebugStr) {
    GBL_API_INLINE_BEGIN(GBL_NULL);
    if(align == 0) {
        align = GBL_ALIGNOF(GBL_MAX_ALIGN_T);
        size = gblAlignedAllocSize(size);
    }
    GBL_ASSERT(size % align == 0);
    GBL_API_EXT(MALLOC, size, align, pDebugStr, &GBL_API_INLINE_RETVAL());
    GBL_API_INLINE_END();
    // modify/set return value based on result
    GBL_API_INLINE_RETURN();
}

#define GBL_API_MALLOC_4(src, size, align, dbgStr) \
    GBL_API_INLINE_CALL_(MALLOC, src, size, align, dbgStr)

#define GBL_API_MALLOC_3(src, size, align) \
    GBL_API_MALLOC_4(src, size, align, NULL)

#define GBL_API_MALLOC_2(src, size) \
    GBL_API_MALLOC_3(src, gblAlignedAllocSize(size), GBL_ALIGNOF(GBL_MAX_ALIGN_T))

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
    GBL_API_INLINE_CALL_(REALLOC, src, pData, newSize, newAlign)

#define GBL_API_REALLOC_3(src, pData, newSize) \
    GBL_API_REALLOC_4(src, pData, newSize, 1)

#define GBL_API_REALLOC(...)  \
     GBL_VA_OVERLOAD_SELECT(GBL_API_REALLOC, GBL_VA_OVERLOAD_SUFFIXER_ARGC, 1, __VA_ARGS__)(SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL), __VA_ARGS__)

#define GBL_API_FREE(pData) \
    GBL_API_SOURCE_SCOPED(GBL_API_EXT, (SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL)), FREE, pData)

#define GBL_API_PUSH_(srcLoc, ...)                                  \
    GBL_STMT_START {                                                \
        GBL_API_SOURCE_LOC_PUSH(srcLoc);                            \
        GBL_RESULT_SUCCESS(GblThread_logPush(NULL));                \
        GBL_API_EXT(LOG_PUSH);                                      \
        GBL_API_SOURCE_POP();                                       \
        ++GBL_API_FRAME()->stackDepth;                              \
    } GBL_STMT_END

#define GBL_API_PUSH()                                                  \
    GBL_STMT_START {                                                    \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL); \
        GBL_API_PUSH_(src_);                                            \
    } GBL_STMT_END

#define GBL_API_PUSH_VERBOSE_N(srcLoc, pFmt, ...)                               \
    GBL_STMT_START {                                                            \
        GBL_API_SOURCE_SCOPED(GBL_API_VERBOSE, srcLoc, pFmt, ##__VA_ARGS__);    \
        GBL_API_PUSH_(srcLoc);                                                  \
    } GBL_STMT_END

#define GBL_API_PUSH_VERBOSE(...)                                                                                               \
    GBL_STMT_START {                                                                                                            \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);                                                         \
        GBL_VA_OVERLOAD_SELECT(GBL_API_PUSH_VERBOSE, GBL_VA_OVERLOAD_SUFFIXER_1_N, src_, ##__VA_ARGS__)(src_, ##__VA_ARGS__);   \
    } GBL_STMT_END

#define GBL_API_POP_2(srcLoc, count)                                    \
        GblThread_logPop(NULL, count);                                  \
        GBL_API_SOURCE_SCOPED(GBL_API_EXT, srcLoc, LOG_POP, count);     \
        GBL_API_FRAME()->stackDepth -= count;

#define GBL_API_POP_1(srcLoc) \
    GBL_API_POP_2(srcLoc, 1)

#define GBL_API_POP(...)                                                                    \
    GBL_STMT_START {                                                                        \
        const SrcLoc loc = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);                      \
        GBL_VA_OVERLOAD_CALL(GBL_API_POP, GBL_VA_OVERLOAD_SUFFIXER_ARGC, loc, __VA_ARGS__); \
    } GBL_STMT_END

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
    GBL_API_INLINE_CALL_(LOG, src, level, pFmt, ##__VA_ARGS__);
#define GBL_API_LOG(level, pFmt, ...)       \
   GBL_API_LOG_(SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL), level, pFmt, ##__VA_ARGS__)

#define GBL_API_DEBUG(pFmt, ...)                                        \
    GBL_STMT_START {                                                    \
        const SrcLoc src = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_API_LOG_(src, GBL_LOG_LEVEL_DEBUG, pFmt, ##__VA_ARGS__);    \
    } GBL_STMT_END

#define GBL_API_VERBOSE(pFmt, ...)                                      \
    GBL_STMT_START {                                                    \
        const SrcLoc src = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_API_LOG_(src, GBL_LOG_LEVEL_VERBOSE, pFmt, ##__VA_ARGS__);  \
    } GBL_STMT_END

#define GBL_API_INFO(pFmt, ...)                                         \
    GBL_STMT_START {                                                    \
        const SrcLoc src = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_API_LOG_(src, GBL_LOG_LEVEL_INFO, pFmt, ##__VA_ARGS__);     \
    } GBL_STMT_END

#define GBL_API_WARN(pFmt, ...)                                         \
    GBL_STMT_START {                                                    \
        const SrcLoc src = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_API_LOG_(src, GBL_LOG_LEVEL_WARNING, pFmt, ##__VA_ARGS__);  \
    } GBL_STMT_END

#define GBL_API_ERROR(pFmt, ...)                                        \
    GBL_STMT_START {                                                    \
        const SrcLoc src = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_API_LOG_(src, GBL_LOG_LEVEL_ERROR, pFmt, ##__VA_ARGS__);    \
    } GBL_STMT_END


#define GBL_API_EVENT_2(srcLoc, event) \
    GBL_API_SOURCE_SCOPED(GBL_API_EXT, srcLoc, EVENT, type, NULL, 0)

#define GBL_API_EVENT(event) \
    GBL_STMT_START { \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);  \
        GBL_UNUSED(src_);                                                \
       GblObject_sendEvent(GBL_API_OBJECT(), (GblEvent*)event);          \
    } GBL_STMT_END


// ===================== RECORD = > ASSERT =========================

// Base Enabled Logic
#define GBL_API_RECORD_ASSERT_(record, test)                    \
    GBL_STMT_START {                                            \
        GBL_ASSERT(!test(record->result), record->message);     \
    } GBL_STMT_END

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
    GBL_STMT_START {                                        \
            GBL_API_RECORD_ASSERT_ERROR(record);            \
            GBL_API_RECORD_ASSERT_PARTIAL(record);          \
            GBL_API_RECORD_ASSERT_UNKNOWN(record);          \
    } GBL_STMT_END


// ==========================  RECORD = > LOG ========================

// Base Enabled Logic (uses a prefix prefix for all magic)
#define GBL_API_RECORD_LOG_(prefix, record)                                     \
        GBL_STMT_START {                                                        \
            if(GBL_RESULT_##prefix(record->result)) GBL_UNLIKELY {              \
                GBL_API_LOG(GBL_CONFIG_LOG_##prefix##_LEVEL,                    \
                            "%s: %s",                                           \
                             gblResultString(record->result), record->message); \
            }                                                                   \
        } GBL_STMT_END


// Base Conditional Logic
#define GBL_API_RECORD_LOG_CONDITIONAL_(prefix, record)                 \
        GBL_MACRO_CONDITIONAL_CALL(GBL_CONFIG_LOG_##prefix##_ENABLED,   \
                                    GBL_API_RECORD_LOG_, prefix, record)

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
    GBL_STMT_START {                                    \
        GBL_API_RECORD_LOG_ERROR(record);               \
        GBL_API_RECORD_LOG_PARTIAL(record);             \
        GBL_API_RECORD_LOG_UNKNOWN(record);             \
    } GBL_STMT_END


// ================= RECORD => HANDLE::LAST_ERROR ==============
#define GBL_API_RECORD_LAST_RECORD_(prefix, record)                             \
    GBL_STMT_START {                                                            \
        if(GBL_RESULT_##prefix(record->result)) {                               \
            gblExtCallRecordSet(GBL_API_FRAME(), record);                       \
            GblThread_setCallRecord(NULL, record);                              \
        }                                                                       \
    } GBL_STMT_END


#define GBL_API_RECORD_LAST_RECORD_CONDITIONAL_(prefix, record)                     \
    GBL_MACRO_CONDITIONAL_CALL(GBL_CONFIG_LAST_CALL_RECORD_##prefix##_ENABLED,      \
                            GBL_API_RECORD_LAST_RECORD_, prefix, record)

#define GBL_API_RECORD_LAST_RECORD_PARTIAL(record) \
    GBL_API_RECORD_LAST_RECORD_CONDITIONAL_(PARTIAL, record)

#define GBL_API_RECORD_LAST_RECORD_ERROR(record) \
    GBL_API_RECORD_LAST_RECORD_CONDITIONAL_(ERROR, record)

#define GBL_API_RECORD_LAST_RECORD_UNKNOWN(record) \
    GBL_API_RECORD_LAST_RECORD_CONDITIONAL_(UNKNOWN, record)

#define GBL_API_RECORD_LAST_RECORD(record)          \
    GBL_STMT_START {                                \
        GBL_API_RECORD_LAST_RECORD_ERROR(record);   \
        GBL_API_RECORD_LAST_RECORD_PARTIAL(record); \
        GBL_API_RECORD_LAST_RECORD_UNKNOWN(record); \
    } GBL_STMT_END


// ================= RECORD => TOP-LEVEL DISPATCH ==============
#define GBL_API_RECORD_HANDLER(record)                \
    GBL_STMT_START {                                  \
            GBL_API_RECORD_LOG((record));             \
            GBL_API_RECORD_LAST_RECORD((record));     \
            GBL_API_RECORD_ASSERT((record));          \
    } GBL_STMT_END

#define GBL_API_RECORD_SET_N(file, func, line, col, result, ...)                                                    \
    GBL_STMT_START {                                                                                                \
        GBL_API_SOURCE_LOC_PUSH(SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL));                                        \
        GBL_CALL_RECORD_CONSTRUCT(&GBL_API_RECORD(), GBL_API_OBJECT(), result, GBL_API_SOURCE(), __VA_ARGS__);      \
        GBL_API_RECORD_HANDLER(&GBL_API_RECORD());                                                                  \
        GBL_API_SOURCE_POP();                                                                                       \
    } GBL_STMT_END

#define GBL_API_RECORD_SET_6(file, func, line, col, result, pFmt) \
    GBL_API_RECORD_SET_N(file, func, line, col, result, "%s", pFmt)

#define GBL_API_RECORD_SET_5(file, func, line, col, result) \
    GBL_API_RECORD_SET_6(file, func, line, col, result, gblResultString(result))

#define GBL_API_RECORD_SET(...)  \
    GBL_VA_OVERLOAD_CALL(GBL_API_RECORD_SET, GBL_VA_OVERLOAD_SUFFIXER_6_N, SRC_FILE, SRC_FN, SRC_LN, SRC_COL, __VA_ARGS__)


#define GBL_API_CALL_N(src, funcCall, ...)                                      \
    GBL_STMT_START {                                                            \
        GBL_API_SOURCE_LOC_PUSH(src);                                           \
        GBL_MAYBE_UNUSED const GBL_RESULT localResult = (funcCall);             \
        if(!GBL_RESULT_SUCCESS(localResult)) GBL_UNLIKELY {                     \
            GBL_API_RESULT() = localResult;                                     \
        }                                                                       \
        GBL_API_SOURCE_POP();                                                   \
    } GBL_STMT_END

#define GBL_API_CALL_2(src, funcCall) \
    GBL_API_CALL_N(src, funcCall, #funcCall);

#define GBL_API_CALL(...)                                                                                           \
    GBL_STMT_START {                                                                                                \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);                                             \
        GBL_VA_OVERLOAD_SELECT(GBL_API_CALL, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);   \
    } GBL_STMT_END

#define GBL_API_VERIFY_CALL(...) \
    GBL_STMT_START {    \
        const SrcLoc src_ = SRC_LOC(SRC_FILE, SRC_FN, SRC_LN, SRC_COL);                                             \
        GBL_VA_OVERLOAD_SELECT(GBL_API_CALL, GBL_VA_OVERLOAD_SUFFIXER_2_N, src_, __VA_ARGS__)(src_, __VA_ARGS__);   \
        if(!GBL_RESULT_SUCCESS(GBL_API_RESULT())) goto GBL_API_END_LABEL;                                           \
    } GBL_STMT_END


// ================= TOP-LEVEL API UTILITIES ==============

#define GBL_API_BEGIN_FRAME(file, func, line, col, pObject, frame)                                                  \
    const SrcLoc gblApiEntrySrcLoc_ = SRC_LOC(file, func, line, col);                                               \
    GBL_API_FRAME_DECLARE = frame;                                                                                  \
    GBL_API_STACK_FRAME_CONSTRUCT(GBL_API_FRAME(), (GblObject*)pObject, GBL_RESULT_SUCCESS, gblApiEntrySrcLoc_);    \
    GBL_RESULT_SUCCESS(GblThread_stackFramePush(NULL, GBL_API_FRAME()))

#define GBL_API_BEGIN_LOG_5(file, func, line, col, hHandle)  \
    GBL_API_BEGIN_FRAME(file, func, line, col, hHandle, ((GblStackFrame*)GBL_ALLOCA(sizeof(GblStackFrame))))

#define GBL_API_BEGIN_LOG_N(file, func, line, col, hHandle, ...)    \
    GBL_API_BEGIN_LOG_5(file, func, line, col, hHandle);            \
    GBL_API_PUSH_VERBOSE(__VA_ARGS__);

#define GBL_API_BEGIN(...) \
        GBL_VA_OVERLOAD_CALL(GBL_API_BEGIN_LOG, GBL_VA_OVERLOAD_SUFFIXER_5_N, SRC_FILE, SRC_FN, SRC_LN, SRC_COL, __VA_ARGS__)

#define GBL_API_DONE()  \
    goto GBL_API_END_LABEL

#define GBL_API_END_BLOCK()                             \
    goto GBL_API_END_LABEL;                             \
    GBL_LABEL_EMPTY(GBL_API_END_LABEL);                 \
        if(GBL_API_FRAME()->stackDepth)                 \
            GBL_API_POP(GBL_API_FRAME()->stackDepth);   \
    GBL_RESULT_SUCCESS(GblThread_stackFramePop(NULL))

#define GBL_API_END()               \
        GBL_API_END_BLOCK();        \
        return GBL_API_RESULT()

#define GBL_API_END_EMPTY()         \
        GBL_LABEL_EMPTY(GBL_API_END_LABEL)

#define GBL_API_BLOCK_7(file, func, line, col, hHandle, frame, block)       \
    GBL_API_BEGIN_FRAME(file, func, line, col, hHandle, frame);             \
    block;                                                                  \
    GBL_API_END_BLOCK()

#define GBL_API_BLOCK_6(file, func, line, col, hHandle, block) \
    GBL_API_BLOCK_7(file, func, line, col, hHandle, ((GblStackFrame*)GBL_ALLOCA(sizeof(GblStackFrame))), block)

#define GBL_API_BLOCK_5(file, func, line, col, block) \
    GBL_API_BLOCK_6(file, func, line, col, NULL, block)

#define GBL_API_BLOCK(...) \
    GBL_VA_OVERLOAD_SELECT(GBL_API_BLOCK, GBL_VA_OVERLOAD_SUFFIXER_ARGC, SRC_FILE, SRC_FN, SRC_LN, SRC_COL, __VA_ARGS__)(SRC_FILE, SRC_FN, SRC_LN, SRC_COL, __VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_API_H
