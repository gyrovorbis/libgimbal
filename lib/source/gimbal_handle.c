#include <gimbal/gimbal_handle.h>
#include <string.h>

GBL_API gblHandleDestruct(GblHandle hHandle) {
    return GBL_RESULT_SUCCESS;
}

GBL_API gblHandleParentGet(GblHandle hHandle,
                        GblHandle* phParent) {
    *phParent = hHandle->pParent;
    return GBL_RESULT_SUCCESS;
}

GBL_API gblHandleContext(GblHandle hHandle,
                         GblContext* phCtx) {
    GBL_ASSERT(hHandle);
    GBL_ASSERT(phCtx);
    *phCtx = hHandle->pContext;
    return GBL_RESULT_SUCCESS;
}

GBL_API gblHandleUserdata(GblHandle hHandle,
                          void** ppUserdata) {
    *ppUserdata = hHandle->pUserdata;
    return GBL_RESULT_SUCCESS;
}

// this could be returning just a the base of a custom subclass
GBL_API gblHandleLastErrorGet(GblHandle hHandle,
                              const GblApiResult** pError) {
    *pError = &hHandle->lastError;
    return GBL_RESULT_SUCCESS;
}


// this could theoretically construct or call into some userdata shit for more info
GBL_API gblHandleLastErrorSet(GblHandle hHandle,
                              const GblApiResult* pError) {
    memcpy(&hHandle->lastError, pError, sizeof(GblApiResult));
    return GBL_RESULT_SUCCESS;
}
