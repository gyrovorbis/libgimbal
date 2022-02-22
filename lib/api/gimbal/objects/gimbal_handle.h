#ifndef GIMBAL_HANDLE_H
#define GIMBAL_HANDLE_H

#include "gimbal_object.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GBL_HANDLE_INVALID GBL_OBJECT_INVALID

typedef struct GblHandleCreateInfo {
    GblObjectCreateInfo objectInfo;
    GblHandle           hParent;
    void*               pUserdata;
} GblHandleCreateInfo;


typedef struct GblHandle_ {
    GblObject_              baseObject;
    void*                   pUserdata;
    GblHandle               hParent;
    GblContext              hContext;
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


#ifdef __cplusplus
}
#endif


#endif // GIMBAL_HANDLE_H
