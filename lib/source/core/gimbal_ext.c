#include <gimbal/core/gimbal_ext.h>
#include <gimbal/meta/instances/gimbal_context.h>
#include <gimbal/core/gimbal_api_frame.h>

#define GBL_EXT_FN_DEFINE_(extName, contextName, ...) \
    GBL_EXPORT GBL_RESULT gblExt##extName (GBL_MAP_LIST(GBL_DECL_VAR_PAIR, __VA_ARGS__)) { \
        GBL_ASSERT(pFrame); \
        if(!pFrame->pContext) return GBL_RESULT_UNIMPLEMENTED; \
        return GblContext_##contextName##_ (pFrame->pContext, GBL_MAP_LIST(GBL_DECL_VAR_PAIR_NAME, __VA_ARGS__)); \
    }

GBL_EXT_FN_DEFINE_(LogWrite, logWrite,       (const GblStackFrame*, pFrame), (GBL_LOG_LEVEL, level), (const char*, pFmt), (va_list, varArgs))
GBL_EXT_FN_DEFINE_(LogPop,   logPop,       (const GblStackFrame*, pFrame), (uint32_t, count))
GBL_EXT_FN_DEFINE_(LogPush,  logPush,       (const GblStackFrame*, pFrame))

GBL_EXT_FN_DEFINE_(MemAlloc, memAlloc,       (const GblStackFrame*, pFrame), (GblSize, size), (GblSize, align), (const char*, pDebugStr), (void**, ppData))

GBL_EXT_FN_DEFINE_(MemRealloc, memRealloc,     (const GblStackFrame*, pFrame), (void*, pData), (GblSize, newSize), (GblSize, newAlign), (void**, ppNewData))
GBL_EXT_FN_DEFINE_(MemFree, memFree,        (const GblStackFrame*, pFrame), (void*, pData))

GBL_EXT_FN_DEFINE_(CallRecordSet, callRecordSet,  (const GblStackFrame*, pFrame), (const GblCallRecord*, pRecord));

#if 0
GBL_EXPORT GBL_RESULT gblExtApiEvent(const GblStackFrame* pFrame, GBL_EVENT_TYPE type, const void* pData, GblSize size) {
    return gblObjectEvent(pFrame->hHandle, pFrame, type, pData, size);
}
#endif

