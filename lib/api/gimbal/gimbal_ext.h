#ifndef GIMBAL_EXT_H
#define GIMBAL_EXT_H

#include "gimbal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

GBL_FORWARD_DECLARE_STRUCT(GblStackFrame);

typedef GBL_RESULT (*GblExtLogWriteFn)      (const GblStackFrame* pFrame, GBL_LOG_LEVEL, const char*, va_list);
typedef GBL_RESULT (*GblExtLogPushFn)       (const GblStackFrame* pFrame);
typedef GBL_RESULT (*GblExtLogPopFn)        (const GblStackFrame* pFrame, uint32_t);

//Custom allocators
typedef GBL_RESULT (*GblExtMemAllocFn)     (const GblStackFrame* pFrame, GblSize, GblSize, const char*, void**);
typedef GBL_RESULT (*GblExtMemReallocFn)    (const GblStackFrame* pFrame, void*, GblSize, GblSize, void**);
typedef GBL_RESULT (*GblExtMemFreeFn)       (const GblStackFrame* pFrame, void*);

typedef GBL_RESULT (*GblExtApiBeginFn)      (void*, void*);
typedef GBL_RESULT (*GblExtApiEndFn)        (void*, void*);
typedef GBL_RESULT (*GblExtApiLastErrorFn)  (void*, void*);

//define all extension points and shit in one centralized location



/*Call everything through these top-level macros so that user
  is able to override them at compile-time + use inline functions



GBL_EXT_EVENT_HANDLER
    a. user-defined macro (compile-time polymorphic)
    b. gblExtEventHandler
        1. user-defined implementation (link-time polymorphic)
        2. dynamic implementation   (run-time polymorphic)
            a. calls context function pointer
            b. calls event handler
            c. falls back to default */

#ifndef GBL_EXT_EVENT_HANDLER
#   define GBL_EXT_EVENT_HANDLER    gblExtEventHandler
#endif

/* ==== SERIOUSLY CONSIDER MAKING THIS SHIT TAKE SOURCE CONTEXT INFO
    actually give context a pointer to latest
*/
#ifndef GBL_EXT_MALLOC
#   define GBL_EXT_MALLOC           gblExtMemAlloc
#endif

#ifndef GBL_EXT_REALLOC
#   define GBL_EXT_REALLOC          gblExtMemRealloc
#endif

#ifndef GBL_EXT_FREE
#   define GBL_EXT_FREE             gblExtMemFree
#endif

#ifndef GBL_EXT_LOG_WRITE
#   define GBL_EXT_LOG_WRITE            gblExtLogWrite
#endif

#ifndef GBL_EXT_LOG_PUSH
#   define GBL_EXT_LOG_PUSH         gblExtLogPush
#endif

#ifndef GBL_EXT_LOG_POP
#   define GBL_EXT_LOG_POP          gblExtLogPop
#endif

#ifndef GBL_EXT_RESULT_STRING
#   define GBL_EXT_RESULT_STRING    gblExtResultString
#endif


GBL_API gblExtLogWrite       (const GblStackFrame* pFrame, GBL_LOG_LEVEL level, const char* pFmt, va_list varArgs);
GBL_API gblExtLogPush        (const GblStackFrame* pFrame);
GBL_API gblExtLogPop         (const GblStackFrame* pFrame, uint32_t count);

GBL_API gblExtMemAlloc      (const GblStackFrame* pFrame, GblSize size, GblSize alignment, const char* pDebugInfoString, void** ppData);
GBL_API gblExtMemRealloc     (const GblStackFrame* pFrame, void* pData, GblSize newSize, GblSize newAlign, void** ppNewData);
GBL_API gblExtMemFree        (const GblStackFrame* pFrame, void* pData);

GBL_API gblExtApiBegin       (GblHandle hHandle);
GBL_API gblExtApiEnd         (GblHandle hHandle);


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_EXT_H
