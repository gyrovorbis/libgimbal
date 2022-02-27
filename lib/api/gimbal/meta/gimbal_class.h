#ifndef GIMBAL_CLASS_H
#define GIMBAL_CLASS_H

#include "gimbal_type.h"


GBL_BEGIN_DECLS

#define SELF    GblClass*           pSelf
#define CSELF   const GblClass*     pSelf

typedef enum GBL_META_CALL {
    GBL_META_CALL_IFACE_INFO,
    GBL_META_CALL_COUNT
} GBL_META_CALL;



typedef GBL_RESULT (*GblTypeMetaCallFn)  (void*,        GBL_META_CALL call, GblUint id, void** ppArgs);
typedef GBL_RESULT (*GblTypeConstructFn) (void*);
typedef GBL_RESULT (*GblTypeDestructFn)  (void*);
typedef GBL_RESULT (*GblTypeCopyFn)      (void*,        const void*);
typedef GBL_RESULT (*GblTypeMoveFn)      (void*,        void*);
typedef GBL_RESULT (*GblTypeCompareFn)   (const void*,  const GblVariant*, GblCmpResult*);
typedef GBL_RESULT (*GblTypeConvertFn)   (const void*,  GblVariant*);
typedef GBL_RESULT (*GblTypeDebugFn)     (const void*,  GblString*);
typedef GBL_RESULT (*GblTypeLoadFn)      (void*,        const GblString*);
typedef GBL_RESULT (*GblTypeSaveFn)      (const void*,  GblString*);

typedef struct GblClass {
    GblType             typeId;
    GblTypeMetaCallFn   pFnMetaCall;
    GblTypeConstructFn  pFnConstruct;
    GblTypeDestructFn   pFnDestruct;
    GblTypeCopyFn       pFnCopy;
    GblTypeMoveFn       pFnMove;
    GblTypeCompareFn    pFnCompare;
    GblTypeConvertFn    pFnConvert;
    GblTypeDebugFn      pFnDebug;
    GblTypeLoadFn       pFnLoad;
    GblTypeSaveFn       pFnSave;
} GblClass;


GBL_INLINE GblType      GblClass_typeOf     (CSELF);
GBL_EXPORT GblSize      GblClass_sizeOf     (CSELF);
GBL_EXPORT GblSize      GblClass_alignOf    (CSELF);
GBL_EXPORT GblClass*    GblClass_parentOf   (CSELF);
GBL_EXPORT const char*  GblClass_name       (CSELF);
GBL_EXPORT GblRefCount  GblClass_refCount   (CSELF);
GBL_EXPORT GblClass*    GblClass_cast       (SELF,  GblType isAType);
GBL_EXPORT GblClass*    GblClass_ref        (SELF);
GBL_EXPORT GblRefCount  GblClass_unref      (SELF);

GBL_API                 GblClass_vMetaCall  (CSELF, void* pInstance, GBL_META_CALL call, GblUint id, void** ppArgs);
GBL_API                 GblClass_vConstruct (CSELF, void* pInstance);
GBL_API                 GblClass_vDestruct  (CSELF, void* pInstance);
GBL_API                 GblClass_vCopy      (CSELF, void* pInstance, const void* pSrc);
GBL_API                 GblClass_vMove      (CSELF, void* pInstance, const void* pSrc);
GBL_API                 GblClass_vCompare   (CSELF, const void* pInstance, const GblVariant* pVariant);
GBL_API                 GblClass_vConvert   (CSELF, const void* pInstance, GblVariant* pVariant);
GBL_API                 GblClass_vLoad      (CSELF, void* pInstance, const GblString* pString);
GBL_API                 GblClass_vSave      (CSELF, const void* pInstance, GblString* pString);



GBL_EXPORT GblType      GblClass_typeOf     (CSELF) {
    return pSelf? pSelf->typeId : GBL_TYPE_INVALID;
}
GBL_EXPORT GblClass*    GblClass_parentOf     (CSELF) {
    return pSelf? gblTypeClassPeek(gblTypeParent(pSelf->typeId)) : NULL;
}
GBL_EXPORT GblSize      GblClass_sizeOf     (CSELF) {
    return pSelf? gblTypeInfo(pSelf->typeId)->classSize : 0;
}

GBL_EXPORT GblSize      GblClass_alignOf    (CSELF) {
    return pSelf? gblTypeInfo(pSelf->typeId)->classAlign : 0;
}

GBL_EXPORT const char*  GblClass_name       (CSELF) {
    return pSelf? gblTypeName(pSelf->typeId) : "Invalid";
}

GBL_EXPORT GblRefCount  GblClass_refCount   (CSELF) {
    return pSelf? gblTypeClassRefCount(pSelf->typeId) : 0;
}
GBL_EXPORT GblClass*    GblClass_cast       (SELF,  GblType isAType) {
    if(pSelf && gblTypeIsA(pSelf->typeId, isAType)) {
        return pSelf;
    } else {
        return NULL;
    }
}
GBL_EXPORT GblClass*    GblClass_ref        (SELF) {
    return pSelf? gblTypeClassRef(pSelf->typeId) : NULL;
}
GBL_EXPORT GblRefCount    GblClass_unref      (SELF) {
    return pSelf? gblTypeClassUnref(pSelf) : 0;
}

#define GBL_DECLARE_VIRTUAL_METHOD(wrapper, vFunc, ClassType, ...)                                   \
   GBL_API wrapper (const ClassType* pSelf, GBL_EVAL(GBL_MAP_LIST(GBL_DECL_VAR_PAIR, __VA_ARGS__))) {                      \
        if(!pSelf)         return GBL_RESULT_ERROR_INVALID_ARG;                                     \
        if(!pSelf->##vFunc)  return GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL;                            \
        else               return pSelf->vFunc(GBL_MAP_LIST(GBL_DECL_VAR_PAIR_NAME, __VA_ARGS__))   \
    }

//GBL_DECARE_VIRTUAL_METHOD(GblClass_vMetaCall, pFnMetaCall, GblClass, (void*, pInstance), (GBL_META_CALL, call), (GblUint, id), (void**, ppArgs));


GBL_API                 GblClass_vConstruct (CSELF, void* pInstance) {

}
GBL_API                 GblClass_vDestruct  (CSELF, void* pInstance);
GBL_API                 GblClass_vCopy      (CSELF, void* pInstance, const void* pSrc);
GBL_API                 GblClass_vMove      (CSELF, void* pInstance, const void* pSrc);
GBL_API                 GblClass_vCompare   (CSELF, const void* pInstance, const GblVariant* pVariant);
GBL_API                 GblClass_vConvert   (CSELF, const void* pInstance, GblVariant* pVariant);
GBL_API                 GblClass_vLoad      (CSELF, void* pInstance, const GblString* pString);
GBL_API                 GblClass_vSave      (CSELF, const void* pInstance, GblString* pString);



#undef SELF
#undef CSELF

GBL_END_DECLS

#endif // GIMBAL_CLASS_H
