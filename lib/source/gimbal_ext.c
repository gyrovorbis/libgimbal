#include <gimbal/core/gimbal_ext.h>
#include <gimbal/objects/gimbal_context.h>
#include <gimbal/core/gimbal_api_frame.h>

#define GBL_EXT_FN_DEFINE_(prefixName, ...) \
    GBL_API gblExt##prefixName (GBL_MAP_LIST(GBL_DECL_VAR_PAIR, __VA_ARGS__)) { \
        GBL_ASSERT(pFrame); \
        if(!pFrame->hContext) return GBL_RESULT_UNIMPLEMENTED; \
        return gblContext##prefixName (pFrame->hContext, GBL_MAP_LIST(GBL_DECL_VAR_PAIR_NAME, __VA_ARGS__)); \
    }

GBL_EXT_FN_DEFINE_(LogWrite,        (const GblStackFrame*, pFrame), (GBL_LOG_LEVEL, level), (const char*, pFmt), (va_list, varArgs))
GBL_EXT_FN_DEFINE_(LogPop,          (const GblStackFrame*, pFrame), (uint32_t, count))
GBL_EXT_FN_DEFINE_(LogPush,         (const GblStackFrame*, pFrame))

GBL_EXT_FN_DEFINE_(MemAlloc,        (const GblStackFrame*, pFrame), (GblSize, size), (GblSize, align), (const char*, pDebugStr), (void**, ppData))

GBL_EXT_FN_DEFINE_(MemRealloc,      (const GblStackFrame*, pFrame), (void*, pData), (GblSize, newSize), (GblSize, newAlign), (void**, ppNewData))
GBL_EXT_FN_DEFINE_(MemFree,         (const GblStackFrame*, pFrame), (void*, pData))
#if 0
GBL_API gblExtApiEvent(const GblStackFrame* pFrame, GBL_EVENT_TYPE type, const void* pData, GblSize size) {
    return gblObjectEvent(pFrame->hHandle, pFrame, type, pData, size);
}
#endif

