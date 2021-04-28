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

GBL_DECLARE_OPAQUE(GblContext);
GBL_DECLARE_OPAQUE(GblHandle);

typedef struct GblHandleCreateInfo {
    GblHandle   hParent;
    void*       pUserdata;
} GblHandleCreateInfo;


typedef struct GblHandle_ {
    GblObject_              baseObject;
    void*                   pUserdata;
    struct GblHandle_*      pParent;
    struct GblContext_*     pContext;
    GblApiResult            lastError;
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
GBL_API gblHandleLastErrorGet(GblHandle hHandle,
                            const GblApiResult** pError);


// this could theoretically construct or call into some userdata shit for more info
GBL_API gblHandleLastErrorSet(GblHandle hHandle,
                            const GblApiResult* pError);
















#ifdef __cplusplus
}
#endif


#endif // GIMBAL_HANDLE_H
