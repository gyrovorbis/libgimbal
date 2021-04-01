#ifndef GIMBAL_HANDLE_H
#define GIMBAL_HANDLE_H

#include "gimbal_object.h"

#ifdef __cplusplus
extern "C" {
#endif


#if 0
GBL_DECLARE_ENUM(GBL_HANDLE_TYPE) {
    GBL_HANDLE_CONTEXT,
    GBL_HANDLE_COUNT
    GBL_HANDLE_USERTYPE=GBL_HANDLE_COUNT
};

typedef GBL_API_RESULT (*GblHandleConstructorFn)(GblHandleBase* pBase);
typedef GBL_API_RESULT (*GblHandleDestructorFn)(GblHandleBase* pBase);


typedef struct GblHandleMetaType {
    GBL_HANDLE_TYPE type;
    GblHandleConstructorFn  pFnVConstructor;
    GblHandleDestructorFn   pFnVDestructor;
} GblHandleMetaType;
#endif


GBL_DECLARE_HANDLE(GblHandle);
GBL_DECLARE_HANDLE(GblContext);

typedef struct GblHandleCreateInfo {
    GblHandle   hParent;
    void*       pUserdata;
} GblHandleCreateInfo;


typedef struct GblHandle_ {
    GblObject               baseObject;
    void*                   pUserdata;
    struct GblHandle_*      pParent;
    struct GblContext_*     pContext;
} GblHandle_;

GBL_API gblHandleConstruct(GblContext hContext,
                        GblHandle* phHandle,
                        const GblHandleCreateInfo* pInfo);

GBL_API gblHandleDestruct(GblHandle hHandle);

GBL_API gblHandleParent(GblHandle hHandle,
                        GblHandle* phParent);

GBL_API gblHandleContext(GblHandle hHandle,
                         GblContext* phCtx);

GBL_API gblHandleUserdata(GblHandle hHandle,
                          void** ppUserdata);

















#ifdef __cplusplus
}
#endif


#endif // GIMBAL_HANDLE_H
