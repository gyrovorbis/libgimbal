#ifndef GIMBAL_OBJECT_H
#define GIMBAL_OBJECT_H

#include "gimbal_type.h"
#include "gimbal_ivariant.h"
#include "gimbal_itable.h"

#define GBL_OBJECT_TYPE                     (GblObject_type())
#define GBL_OBJECT(instance)                GBL_TYPE_INSTANCE_CAST(instance, GBL_TYPE_OBJECT, GblObject)
#define GBL_OBJECT_COMPATIBLE(instance)     GBL_TYPE_INSTANCE_IS_A(instance, GBL_TYPE_OBJECT)
#define GBL_OBJECT_CLASS(klass)             GBL_TYPE_CLASS_CAST(klass, GBL_TYPE_OBJECT, GblObjectClass)
#define GBL_OBJECT_CLASS_COMPATIBLE(klass)  GBL_TYPE_CLASS_IS_A(klass, GBL_TYPE_OBJECT)
#define GBL_OBJECT_GET_CLASS(instance)      GBL_TYPE_INSTANCE_CLASS_CAST(instance, GBL_TYPE_OBJECT, GblObjectClass)

#define SELF               GblObject* pSelf
#define CSELF               const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblObject);
GBL_FORWARD_DECLARE_STRUCT(GblEvent);

typedef enum GBL_PROPERTY_FLAGS {
    GBL_PROPERTY_FLAG_CONSTRUCT,
    GBL_PROPERTY_FLAG_READ,
    GBL_PROPERTY_FLAG_WRITE,
    GBL_PROPERTY_FLAG_SAVE,
    GBL_PROPERTY_FLAG_LOAD
} GBL_OBJECT_PROPERTY_FLAGS;

typedef struct GblProperty {
    struct GblProperty*                 pNext;
    GblSize                             slot;
    GBL_PROPERTY_FLAGS                  flags;
    GblType                             valueType;
    GblType                             objectType;
    const char                          name[1];
} GblProperty;


/*
GblIPropertyContainer (uses property System)
    - PropertyGet(slot, GblPropertyDescriptor)
    - PropertySet(slot, GblPropertyDescriptor)
    - PropertyNotify?
GblITable
    - GblIIterable
    - index()
    - newIndex
GblIEventHandler
GblIEventFilter
*/

typedef struct GblObjectClass {
    GblClass                base;
    GblIVariantIFace        iVariantIFace;
    GblITableIFace          iTableIFace;

    GBL_RESULT (*pFnConstructor)        (SELF);
    GBL_RESULT (*pFnDestructor)         (SELF);
    GBL_RESULT (*pFnConstructed)        (SELF);

    GBL_RESULT (*pFnPropertyGet)        (CSELF, GblUint slot, GblVariant* pValue, const GblProperty* pProp);
    GBL_RESULT (*pFnPropertySet)        (SELF,  GblUint slot, const GblVariant* pValue, const GblProperty* pProp);
    GBL_RESULT (*pFnPropertyNotify)     (SELF, GblProperty* pProperty);    // signal
    GBL_RESULT (*pFnPropertiesDynamic)  (SELF, GblITable** ppItable);

    GBL_RESULT (*pFnEventHandler)       (SELF, GblEvent* pEvent);   //handle all events
    GBL_RESULT (*pFnEventFilter)        (SELF, GblObject* pWatched, GblEvent* pEvent);  //installing into other objectsl
} GblObjectClass;

GBL_API GblObjectClass_init(GblObjectClass* pClass, void* pData, GblContext hCtx) GBL_NOEXCEPT;
GBL_API GblObjectClass_final(GblObjectClass* pClass, GblContext hCtx) GBL_NOEXCEPT;

typedef struct GblObject {
    union {
        GblObjectClass*     pClass;
        GblInstance         base;
    };
    GblRefCount             refCounter;
    struct GblObject*       pParent;
    struct GblObject*       pSiblingNext;
    struct GblObject*       pChildFirst;

    uint32_t                childEventsSend                        : 1;
    uint32_t                childEventsReceive                     : 1;
    uint32_t                parentITableIndexFallthrough           : 1;
    uint32_t                parentITableNewIndexFallthrough        : 1;
    uint32_t                hasName                                : 1;
    uint32_t                hasUserdata                            : 1;
    uint32_t                hasEventFilter                         : 1;

    //struct ExtraData {
    //    GblString*          pName;
    //    void*               pUserdata;
    //    struct GblObject*   pEventFilters;
    //}* pExtraData;
    // GblDataSet
} GblObject;


GBL_EXPORT GblObject*   GblObject_new(GblType type, const char* pFirstPropertyName, ...) GBL_NULL_TERMINATED GBL_NOEXCEPT;
GBL_EXPORT GblObject*   GblObject_newVaList(GblType type, const char* pFirstPropertyName, va_list list);
GBL_EXPORT GblObject*   GblObject_newv(GblType type, GblUint propertyCount, const char* pNames[], const GblVariant values[]);

GBL_EXPORT GblObject*   GblObject_ref(GblObject* pObject) GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblObject_unref(GblObject* pObject) GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblObject_refCount(const GblObject* pObject) GBL_NOEXCEPT;

GBL_INLINE void         GblObject_userdataSet(SELF, void* pUserdata) GBL_NOEXCEPT;
GBL_INLINE void*        GblObject_userdataGet(CSELF) GBL_NOEXCEPT;

GBL_INLINE GblObject*   GblObject_parentGet(CSELF) GBL_NOEXCEPT;
GBL_INLINE void         GblObject_parentSet(SELF, GblObject* pParent) GBL_NOEXCEPT;

GBL_INLINE GblObject*   GblObject_ancestorFindByType(CSELF, GblType ancestorType) GBL_NOEXCEPT;

GBL_INLINE void         GblObject_childAdd(SELF, GblObject* pChild) GBL_NOEXCEPT;
GBL_INLINE GblBool      GblObject_childRemove(SELF, GblObject* pChild) GBL_NOEXCEPT;
GBL_INLINE GblObject*   GblObject_childFindByType(CSELF, GblType childType) GBL_NOEXCEPT;

GBL_INLINE GblObject*   GblObject_siblingFindByType(CSELF, GblType siblingType) GBL_NOEXCEPT;



// =========
GBL_API                 GblObject_propertyGet(CSELF, const char* pName, GblVariant* pValue) GBL_NOEXCEPT;
GBL_API                 GblObject_propertySet(SELF,  const char* pName, const GblVariant* pValue) GBL_NOEXCEPT;

GBL_API                 GblObject_getVaList(CSELF, const char* pFirstPropertyName, va_list list) GBL_NOEXCEPT;
GBL_API                 GblObject_setVaList(SELF, const char* pFirstPropertyName, va_list list) GBL_NOEXCEPT;

GBL_API                 GblObject_set(CSELF, const char* pFirstPropertyName, ...) GBL_NULL_TERMINATED GBL_NOEXCEPT;
GBL_API                 GblObject_get(SELF, const char* pFirstPropertyName, ...) GBL_NULL_TERMINATED GBL_NOEXCEPT;

GBL_API                 GblObject_setv(CSELF, GblUint propertyCount, const char* pNames[], const GblVariant values[]) GBL_NOEXCEPT;
GBL_API                 GblObject_getv(SELF, GblUint propertyCount, const char* pNames[], GblVariant values[]) GBL_NOEXCEPT;

// ========

//GBL_API                 GblObject_load(SELF, GblIStructuredReader* pReader) GBL_NOEXCEPT;
//GBL_API                 GblObject_save(SELF, GblIStructuredWrite* pWriter) GBL_NOEXCEPT;

// =======


// ============ INLINEZ ===============
/*
GBL_INLINE void GblObject_userdataSet(SELF, void* pUserdata) GBL_NOEXCEPT {
    pSelf->pUserdata = pUserdata;
}
GBL_INLINE void* GblObject_userdataGet(CSELF) GBL_NOEXCEPT {
    return pSelf->pUserdata;
}
*/
GBL_INLINE GblObject* GblObject_parentGet(CSELF) GBL_NOEXCEPT {
    return pSelf->pParent;
}

GBL_INLINE void GblObject_parentSet(SELF, GblObject* pParent) GBL_NOEXCEPT {
    if(pSelf->pParent) {
        GblObject_childRemove(pSelf->pParent, pSelf);
    }

    GblObject_childAdd(pParent, pSelf);
}

GBL_INLINE GblObject* GblObject_ancestorFindByType(CSELF, GblType ancestorType) GBL_NOEXCEPT {
    GblObject* pAncestor = NULL;
    GBL_API_BEGIN(NULL); {
        GblObject* pNode = GblObject_parentGet(pSelf);
        while(pNode) {
            if(GBL_TYPE_INSTANCE_IS_A(pNode, ancestorType)) {
                pAncestor = pNode;
                break;
            }
            pNode = GblObject_parentGet(pNode);
        }

    } GBL_API_END_BLOCK();
    return pAncestor;
}

GBL_INLINE GblContext GblObject_contextFind(const GblObject* pSelf) GBL_NOEXCEPT {
    return NULL;
    GBL_UNUSED(pSelf);
    //return GblObject_ancestorFindByType(pSelf, GBL_CONTEXT_TYPE);
}


GBL_INLINE void GblObject_childAdd(SELF, GblObject* pChild) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pChild);
    {
        GblObject* pNode = pSelf->pChildFirst;
        if(!pNode) {
            pSelf->pChildFirst = pChild;
        } else {
            while(pNode->pSiblingNext)
            {
                GBL_API_VERIFY_EXPRESSION(pNode != pChild);
                pNode = pNode->pSiblingNext;
            }
            pNode->pSiblingNext = pChild;
        }
        pChild->pParent = pSelf;
        pChild->pSiblingNext = NULL;
    }
    GBL_API_END_BLOCK();
}

GBL_INLINE GblBool GblObject_childRemove(SELF, GblObject* pChild) GBL_NOEXCEPT {
    GblBool success = GBL_FALSE;
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pChild);
    GBL_API_VERIFY_POINTER(pSelf->pChildFirst);
    {
        GblObject* pNode = pSelf->pChildFirst;
        if(pNode == pChild) {
            pSelf->pChildFirst = pChild->pSiblingNext;
            success = GBL_TRUE;
        } else {
            while(pNode->pSiblingNext) {
                if(pNode->pSiblingNext == pChild) {
                    pNode->pSiblingNext = pChild->pSiblingNext;
                    success = GBL_TRUE;
                    break;
                }
                pNode = pNode->pSiblingNext;
            }
        }
        if(success) {
            pChild->pSiblingNext = NULL;
            pChild->pParent = NULL;
        }
    }
    GBL_API_END_BLOCK();
    return success;
}

GBL_INLINE GblObject* GblObject_childFindByType(CSELF, GblType childType) GBL_NOEXCEPT {
    GblObject* pChild = NULL;
    GBL_API_BEGIN(NULL); {
        for(GblObject* pIt = pSelf->pChildFirst;
            pIt != NULL;
            pIt = pIt->pSiblingNext)
        {
            if(GBL_TYPE_INSTANCE_IS_A(pIt, childType)) {
                pChild = pIt;
                break;
            }
        }
    } GBL_API_END_BLOCK();
    return pChild;
}

GBL_INLINE GblObject* GblObject_siblingFindByType(CSELF, GblType siblingType) GBL_NOEXCEPT {
    GblObject* pObject = NULL;
    if(GblObject_parentGet(pSelf)) {
        GblObject_childFindByType(GblObject_parentGet(pSelf), siblingType);
    }
    return pObject;
}





#if 0
/*
void                    GblObject_nameSet(SELF, const char* pName);
const char*             GblObject_nameGet(CSELF);
*/
GblObject*              GblObject_ancestorFindByName(CSELF, const char* pName);




GblSize                 GblObject_childCount(CSELF);
GblObject*              GblObject_childAt(CSELF, GblSize index);
GblObject*              GblObject_childFindByName(CSELF, const char* pName);

GblObject*              GblObject_descendentFindByName(CSELF, const char* pName);
GblObject*              GblObject_descendentFindByType(CSELF, GblType childType);

GblSize                 GblObject_siblingCount(CSELF);
GblObject*              GblObject_siblingAt(CSELF, GblSize index);
GblObject*              GblObject_siblingFindByName(CSELF, const char* pName);




GBL_RESULT              GblObject_propertyNext(CSELF, const GblVariant* pPrevKey, GblVariant* pNextKey, GblVariant* pNextValue);

GBL_RESULT              GblObject_get(CSELF, const char* pFirstProperty, ...);
GBL_RESULT              GblObject_getVaList(CSELF, const char* pFirstProperty, va_list varArgs);
GBL_RESULT              GblObject_getVariants(CSELF, GblSize propertyCount, const char* pNames[], GblVariant pVariants[]);
GBL_RESULT              GblObject_toLson(CSELF, GblString* pString);
GBL_RESULT              GblObject_fromLson(SELF, const char* pString);

GBL_RESULT              GblObject_set(SELF, const char* pFirstProperty, ...);
GBL_RESULT              GblObject_setVaList(SELF, va_list varArgs);
GBL_RESULT              GblObject_setVariants(SELF, GblSize propertyCount, const char* pNames[], const GblVariant pVariants[]);

GBL_RESULT              GblObject_eventSend(SELF, GblEvent* pEvent);
void                    GblObject_eventFilterInstall(SELF, GblObject* pFilterObj);
void                    GblObject_eventFilterUninstall(SELF, const GblObject* pFilterObj);

GblContext              GblObject_findContext(CSELF);

#endif



/*


#define GBL_OBJECT_VCALL_BODY(VFUNC, ...)                                       \
    if(!pSelf)                  return GBL_RESULT_ERROR_INVALID_INSTANCE;       \
    if(!pSelf->pClass)          return GBL_RESULT_ERROR_INVALID_CLASS;          \
    if(!pSelf->pClass->VFUNC)   return GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL;   \
    else                        return pSelf->pClass->VFUNC(__VA_ARGS__)

GBL_DECLS_BEGIN

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
