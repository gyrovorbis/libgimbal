#ifndef GIMBAL_OBJECT_H
#define GIMBAL_OBJECT_H

#include "gimbal_instance.h"

#define GBL_OBJECT_TYPE     (GblObject_type())

#define SELF                struct GblObject* pSelf
#define CSELF               const SELF

#define GBL_OBJECT_VCALL_BODY(VFUNC, ...)                                       \
    if(!pSelf)                  return GBL_RESULT_ERROR_INVALID_INSTANCE;       \
    if(!pSelf->pClass)          return GBL_RESULT_ERROR_INVALID_CLASS;          \
    if(!pSelf->pClass->VFUNC)   return GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL;   \
    else                        return pSelf->pClass->VFUNC(__VA_ARGS__)

GBL_DECLS_BEGIN

typedef enum GblPropertyFlags {} GblPropertyFlags;

typedef struct GblPropertyInfo {
    struct GblParamSpec*    pNext;
    const char*             pName;
    GblPropertyFlags        flags;
    GblType                 valueType;
    GblType                 objectType;
} GblPropertyInfo;


struct GblParamSpec;
typedef struct GblParamSpec GblParamSpec;

typedef struct GblObjectConstructParam {
      GblParamSpec*   pPSpec;
      GblVariant*     pPValue;
} GblObjectConstructParam;


struct GblEvent;

typedef struct GblObjectClass {
    GblClass                base;
    GblPropertyInfo*        pPropertyList;

    GBL_RESULT (*pFnConstructor)        (SELF, GblType type, GblUint propertyCount, GblObjectConstructParam* pProperties);
    GBL_RESULT (*pFnDestructor)         (SELF);

    GBL_RESULT (*pFnPropertyGet)        (CSELF, GblUint slot, GblVariant* pValue, const GblParamSpec* pSpec);
    GBL_RESULT (*pFnPropertySet)        (SELF, GblUint slot, const GblVariant* pValue, const GblParamSpec* pSpec);

    GBL_RESULT (*pFnEventHandler)       (SELF, GblEvent* pEvent);   //handle all events
    GBL_RESULT (*pFnEventFilter)        (SELF, GblObject* pWatched, GblEvent* pEvent);  //installing into other objects

    GBL_RESULT (*pFnNotify)             (SELF, GblParamSpec* pSpec);    // signal

    GBL_RESULT (*pFnConstructed)        (SELF);
} GblObjectClass;

typedef struct GblObject {
    union {
        GblObjectClass* pClass;
        GblInstance     base;
    };
    GblRefCount         refCounter;
    struct GblObject*   pParent;
    struct GblObject*   pSiblingNext;
    struct GblObject*   pChildFirst;
    void*               pPrivateData;

    uint32_t    childEventsSend     : 1;
    uint32_t    childEventsReceive  : 1;

    struct ExtraData {
        GblString*          pName;
        void*               pUserdata;
        struct GblObject*   pEventFilters;
    }* pExtraData;
} GblObject;


GblObject*              GblObject_new(GblType type, GblSize size, ...);
GblObject*              GblObject_ref(GblObject* pObject);
GblObject*              GblObject_unref(GblObject* pObject);
GblRefCount             GblObject_refCount(const GblObject* pObject);

void                    GblObject_nameSet(SELF, const char* pName);
const char*             GblObject_nameGet(CSELF);

void                    GblObject_userdataSet(SELF, void* pUserdata);
void*                   GblObject_userdataGet(CSELF);

GblObject*              GblObject_parentGet(CSELF) GBL_NOEXCEPT;
void                    GblObject_parentSet(SELF, GblObject* pParent);
GblObject*              GblObject_ancestorFindByName(CSELF, const char* pName);
GblObject*              GblObject_ancestorFindByType(CSELF, GblType ancestorType);

void                    GblObject_childAdd(SELF, GblObject* pChild);
void                    GblObject_childRemove(SELF, const GblObject* pChild);
GblSize                 GblObject_childCount(CSELF);
GblObject*              GblObject_childAt(CSELF, GblSize index);
GblObject*              GblObject_childFindByName(CSELF, const char* pName);
GblObject*              GblObject_childFindByType(CSELF, GblType childType);
GblObject*              GblObject_descendentFindByName(CSELF, const char* pName);
GblObject*              GblObject_descendentFindByType(CSELF, GblType childType);

GblSize                 GblObject_siblingCount(CSELF);
GblObject*              GblObject_siblingAt(CSELF, GblSize index);
GblObject*              GblObject_siblingFindByName(CSELF, const char* pName);
GblObject*              GblObject_siblingFindByType(CSELF, GblType siblingType);

GBL_RESULT              GblObject_propertyGet(CSELF, const char* pName, GblVariant* pVariant);
GBL_RESULT              GblObject_propertySet(SELF, const char* pName, const GblVariant* pVariant);
GBL_RESULT              GblObject_propertyNext(CSELF, const GblVariant* pPrevKey, GblVariant* pNextKey, GblVariant* pNextValue);

GBL_RESULT              GblObject_get(CSELF, const char* pFirstProperty, ...);
GBL_RESULT              GblObject_getVaList(CSELF, const char* pFirstProperty, va_list varArgs);
GBL_RESULT              GblObject_getVariants(CSELF, GblSize propertyCount, const char* pNames[], GblVariant pVariants[]);
GBL_RESULT              GblObject_toLson(CSELF, GblString* pString);
GBL_RESULT              GblObject_fromLson(SELF, const char* pString);

GBL_RESULT              GblObject_set(SELF, const char* pFirstProperty, ...);
GBL_RESULT              GblObject_setVaList(SELF, const char* pFirstProperty, va_list varArgs);
GBL_RESULT              GblObject_setVariants(SELF, GblSize propertyCount, const char* pNames[], const GblVariant pVariants[]);

GBL_RESULT              GblObject_eventSend(SELF, GblEvent* pEvent);
void                    GblObject_eventFilterInstall(SELF, GblObject* pFilterObj);
void                    GblObject_eventFilterUninstall(SELF, const GblObject* pFilterObj);

GblContext              GblObject_findContext(CSELF);


/*

GBL_INLINE GBL_RESULT   GblObject_vConstruct (SELF);
GBL_INLINE GBL_RESULT   GblObject_vDestruct  (SELF);
GBL_INLINE GBL_RESULT   GblObject_vCopy      (SELF, const void* pSrc);
GBL_INLINE GBL_RESULT   GblObject_vMove      (SELF, void* pSrc);
GBL_INLINE GBL_RESULT   GblObject_vCompare   (CSELF, const GblVariant* pVariant, GblCmpResult* pResult);
GBL_INLINE GBL_RESULT   GblObject_vConvert   (CSELF, GblVariant* pVariant);
GBL_INLINE GBL_RESULT   GblObject_vLoad      (SELF, const GblString* pString);
GBL_INLINE GBL_RESULT   GblObject_vSave      (CSELF, GblString* pString);
*/
#if 0
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

#endif
GBL_DECLS_END


#undef CSELF
#undef SELF

#endif // GIMBAL_OBJECT_H
