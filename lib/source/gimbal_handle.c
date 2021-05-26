#include <gimbal/gimbal_handle.h>
#include <gimbal/gimbal_api.h>
#include <string.h>

GBL_API gblHandleDestruct(GblHandle hHandle) {
    if(hHandle == GBL_HANDLE_INVALID) {
        return GBL_RESULT_ERROR_INVALID_HANDLE;
    }
    return GBL_RESULT_SUCCESS;
}

GBL_API gblHandleParentGet(GblHandle hHandle,
                        GblHandle* phParent) {
    if(hHandle == GBL_HANDLE_INVALID) return GBL_RESULT_ERROR_INVALID_HANDLE;
    if(!phParent) return GBL_RESULT_ERROR_INVALID_POINTER;
    *phParent = hHandle->pParent;
    return GBL_RESULT_SUCCESS;
}

GBL_API gblHandleContext(GblHandle hHandle,
                         GblContext* phCtx) {
    if(hHandle == GBL_HANDLE_INVALID) return GBL_RESULT_ERROR_INVALID_HANDLE;
    if(!phCtx) return GBL_RESULT_ERROR_INVALID_POINTER;
    *phCtx = hHandle->pContext;
    return GBL_RESULT_SUCCESS;
}

GBL_API gblHandleUserdata(GblHandle hHandle,
                          void** ppUserdata) {
   if(hHandle == GBL_HANDLE_INVALID) return GBL_RESULT_ERROR_INVALID_HANDLE;
   if(!ppUserdata) return GBL_RESULT_ERROR_INVALID_POINTER;
   *ppUserdata = hHandle->pUserdata;
   return GBL_RESULT_SUCCESS;
}

// this could be returning just a the base of a custom subclass
GBL_API gblHandleLastCallRecordGet(GblHandle hHandle,
                              const GblCallRecord** ppRecord) {
    if(hHandle == GBL_HANDLE_INVALID) return GBL_RESULT_ERROR_INVALID_HANDLE;
    if(!ppRecord) return GBL_RESULT_ERROR_INVALID_POINTER;

    *ppRecord = &hHandle->lastRecord;

    return GBL_RESULT_SUCCESS;
}


// this could theoretically construct or call into some userdata shit for more info
GBL_API gblHandleLastCallRecordSet(GblHandle hHandle,
                              const GblCallRecord* pRecord) {
    if(hHandle == GBL_HANDLE_INVALID) return GBL_RESULT_ERROR_INVALID_HANDLE;

    if(pRecord) memcpy(&hHandle->lastRecord, pRecord, sizeof(GblCallRecord));
    else memset(&hHandle->lastRecord, 0, sizeof(GblCallRecord));

    return GBL_RESULT_SUCCESS;
}
