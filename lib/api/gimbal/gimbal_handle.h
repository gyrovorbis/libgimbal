#ifndef GIMBAL_HANDLE_H
#define GIMBAL_HANDLE_H

#include "gimbal_object.h"


#ifdef __cplusplus
extern "C" {
#endif

GBL_DECLARE_OPAQUE(GblContext);
GBL_DECLARE_OPAQUE(GblHandle);

// ADD EVENTHANDLER CALLBACK SHIT

typedef struct GblHandleCreateInfo {
    GblHandle   hParent;
    void*       pUserdata;
} GblHandleCreateInfo;


typedef struct GblHandle_ {
    GblObject_              baseObject;
    void*                   pUserdata;
    struct GblHandle_*      pParent;
    struct GblContext_*     pContext;
    GblCallRecord           lastRecord;
} GblHandle_;

GBL_API gblHandleConstruct(GblContext hContext,
                        GblHandle* phHandle,
                        const GblHandleCreateInfo* pInfo);

GBL_API gblHandleDestruct(GblHandle hHandle);

GBL_API gblHandleParentGet(GblHandle hHandle,
                        GblHandle* phParent);

GBL_API gblHandleParentSet(GblHandle hHandle,   //useful for run-time dependency injection
                           GblHandle hParent);

GBL_API gblHandleContext(GblHandle hHandle,
                         GblContext* phCtx);

GBL_API gblHandleUserdata(GblHandle hHandle,
                          void** ppUserdata);


// this could be returning just a the base of a custom subclass
GBL_API gblHandleLastCallRecordGet(GblHandle hHandle,
                            const GblCallRecord** ppRecord);


// this could theoretically construct or call into some userdata shit for more info
GBL_API gblHandleLastCallRecordSet(GblHandle hHandle,
                            const GblCallRecord* pRecord);
















#ifdef __cplusplus
}
#endif


#endif // GIMBAL_HANDLE_H
