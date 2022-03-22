#ifndef GIMBAL_OBJECT_H
#define GIMBAL_OBJECT_H

#include "gimbal_instance.h"

#define GBL_OBJECT_TYPE     (GblObject_type())

#define SELF                GblObject* pSelf
#define CSELF               const GblObject* pSelf

#define GBL_OBJECT_VCALL_BODY(VFUNC, ...)                                       \
    if(!pSelf)                  return GBL_RESULT_ERROR_INVALID_INSTANCE;       \
    if(!pSelf->pClass)          return GBL_RESULT_ERROR_INVALID_CLASS;          \
    if(!pSelf->pClass->VFUNC)   return GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL;   \
    else                        return pSelf->pClass->VFUNC(__VA_ARGS__)

GBL_DECLS_BEGIN


typedef GBL_RESULT (*GblObjectConstructFn) (SELF);
typedef GBL_RESULT (*GblObjectDestructFn)  (SELF);
typedef GBL_RESULT (*GblObjectCopyFn)      (SELF, const void*);
typedef GBL_RESULT (*GblObjectMoveFn)      (SELF, void*);
typedef GBL_RESULT (*GblObjectCompareFn)   (CSELF, const GblVariant*, GblCmpResult*);
typedef GBL_RESULT (*GblObjectConvertFn)   (CSELF, GblVariant*);
typedef GBL_RESULT (*GblObjectDebugFn)     (CSELF, GblString*);
typedef GBL_RESULT (*GblObjectLoadFn)      (SELF, const GblString*);
typedef GBL_RESULT (*GblObjectSaveFn)      (SELF, GblString*);

typedef struct GblObjectClass {
    GblClass                base;
    GblObjectConstructFn    pFnConstruct;
    GblObjectDestructFn     pFnDestruct;
    GblObjectCopyFn         pFnCopy;        //copy reference for object types
    GblObjectMoveFn         pFnMove;
    GblObjectCompareFn      pFnCompare;
    GblObjectConvertFn      pFnConvert;
    GblObjectDebugFn        pFnDebug;
    GblObjectLoadFn         pFnLoad;
    GblObjectSaveFn         pFnSave;
} GblObjectClass;

typedef struct GblObject {
    union {
        GblObjectClass* pClass;
        GblInstance     base;
    };
} GblObject;


GblType                 GblObject_type(void);
GblObject*              GblObject_new(GblType type, GblSize size, ...);
GblObject*              GblObject_ref(GblObject* pObject);
GblObject*              GblObject_unref(GblObject* pObject);


GBL_INLINE GBL_RESULT   GblObject_vConstruct (SELF);
GBL_INLINE GBL_RESULT   GblObject_vDestruct  (SELF);
GBL_INLINE GBL_RESULT   GblObject_vCopy      (SELF, const void* pSrc);
GBL_INLINE GBL_RESULT   GblObject_vMove      (SELF, void* pSrc);
GBL_INLINE GBL_RESULT   GblObject_vCompare   (CSELF, const GblVariant* pVariant, GblCmpResult* pResult);
GBL_INLINE GBL_RESULT   GblObject_vConvert   (CSELF, GblVariant* pVariant);
GBL_INLINE GBL_RESULT   GblObject_vLoad      (SELF, const GblString* pString);
GBL_INLINE GBL_RESULT   GblObject_vSave      (CSELF, GblString* pString);


GBL_INLINE GBL_RESULT GblObject_vConstruct(SELF) {
    GBL_OBJECT_VCALL_BODY(pFnConstruct, pSelf);
}
GBL_INLINE GBL_RESULT GblObject_vDestruct(SELF) {
    GBL_OBJECT_VCALL_BODY(pFnDestruct, pSelf);
}
GBL_INLINE GBL_RESULT GblObject_vCopy(SELF, const void* pSrc) {
    GBL_OBJECT_VCALL_BODY(pFnCopy, pSelf, pSrc);
}
GBL_INLINE GBL_RESULT GblObject_vMove(SELF, void* pSrc) {
    GBL_OBJECT_VCALL_BODY(pFnMove, pSelf, pSrc);
}
GBL_INLINE GBL_RESULT GblObject_vCompare(CSELF, const GblVariant* pVariant, GblCmpResult* pResult) {
    GBL_OBJECT_VCALL_BODY(pFnCompare, pSelf, pVariant, pResult);
}
GBL_INLINE GBL_RESULT GblObject_vConvert(CSELF, GblVariant* pVariant) {
    GBL_OBJECT_VCALL_BODY(pFnConvert, pSelf, pVariant);
}
GBL_INLINE GBL_RESULT GblObject_vLoad(SELF, const GblString* pString) {
    GBL_OBJECT_VCALL_BODY(pFnLoad, pSelf, pString);
}
GBL_INLINE GBL_RESULT GblObject_vSave(CSELF, GblString* pString) {
    GBL_OBJECT_VCALL_BODY(pFnSave, pSelf, pString);
}


GBL_DECLS_END


#undef CSELF
#undef SELF

#endif // GIMBAL_OBJECT_H
