#include <gimbal/meta/properties/gimbal_property.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/strings/gimbal_quark.h>
#include <gimbal/meta/instances/gimbal_object.h>
#include <gimbal/meta/classes/gimbal_primitives.h>
#include "../types/gimbal_type_.h"

typedef struct GblPropertyRoot_ {
    GblProperty     base;
    size_t          count;
    GblProperty*    pLast;
} GblPropertyRoot_;

static GblHashSet propertyRegistry_;

// ========== PROPERTY SYSTEM PRIVATE ==========

static GblHash propertyHasher_(const GblHashSet* pSet, const GblProperty** pProperty) {
    GBL_UNUSED(pSet);
    return gblHash(((uint8_t*)*pProperty) + offsetof(GblProperty, private_.objectType), sizeof(GblQuark) + sizeof(GblType));
}

static GblBool propertyComparator_(const GblHashSet* pSet, const GblProperty** p1, const GblProperty** p2) {
    GBL_UNUSED(pSet);
    return GblProperty_objectType(*p1) == GblProperty_objectType(*p2) &&
           (*p1)->name                 == (*p2)->name;
}

static void propertyDestructor_(const GblHashSet* pSet, GblProperty** pProperty) {
    GBL_CTX_BEGIN(GblHashSet_context(pSet));
    GBL_CTX_FREE(*pProperty);
    GBL_CTX_END_BLOCK();
}

GBL_EXPORT size_t GblProperty_totalCount(void) {
    return GblHashSet_size(&propertyRegistry_);
}

GBL_INLINE GblPropertyRoot_* propertyRoot_(GblType objectType) {
    GblPropertyRoot_* pProperty = NULL;
    GBL_CTX_BEGIN(GblHashSet_context(&propertyRegistry_)); {
        GblProperty* pKey               = GBL_ALLOCA(sizeof(GblProperty));
        GblPropertyRoot_** ppRet        = NULL;
        pKey->name                      = GBL_QUARK_INVALID;
        GBL_PRIV_REF(pKey).objectType   = objectType;
        ppRet                           = (GblPropertyRoot_**)GblHashSet_get(&propertyRegistry_, &pKey);
        if(ppRet) pProperty             = *ppRet;
    } GBL_CTX_END_BLOCK();
    return pProperty;
}

GBL_INLINE GblProperty* propertyFirst_(GblType objectType) {
    GblProperty* pProperty = NULL;
    GBL_CTX_BEGIN(GblHashSet_context(&propertyRegistry_));
    const GblPropertyRoot_* pRoot = propertyRoot_(objectType);
    if(pRoot) pProperty = GBL_PRIV(pRoot->base).pNext;
    GBL_CTX_END_BLOCK();
    return pProperty;
}


extern GBL_RESULT GblProperty_init_(GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    GblHashSet_construct(&propertyRegistry_,
                         sizeof(GblProperty*),
                         (GblHashSetHashFn)propertyHasher_,
                         (GblHashSetCmpFn)propertyComparator_,
                         (GblHashSetDtorFn)propertyDestructor_,
                         32,
                         pCtx);
    GBL_CTX_END();
}

#if 0
static GblBool GblProperty_final_iterator_(const GblHashSet* pHash, void* pEntry, void* pClosure) {
    GblProperty* pProp = pEntry;

    GBL_CTX_WARN("%-20s: %40s", "Object",   GblType_name(GBL_PRIV_REF(pProp).objectType));
    GBL_CTX_WARN("%-20s: %40s", "Name",     GblProperty_name(pProp));
    //GBL_CTX_VERBOSE("%-20s: %40u", "RefCount", GblBox_refCount(GBL_BOX(pProp)));

    return GBL_FALSE;
}
#endif

extern GBL_RESULT GblProperty_final_(GblContext* pCtx) {
    GBL_CTX_BEGIN(pCtx);
    size_t  count = GblHashSet_size(&propertyRegistry_);
    if(count) {
#if 0
        GBL_CTX_RECORD_SET(GBL_RESULT_ERROR_MEM_FREE,
                           "[GblProperty]: Cannot finalize nonempty table! [%u remaining]",
                           count);
        GBL_CTX_PUSH();

        //GblHashSet_foreach(&propertyRegistry_,
        //                   GblProperty_final_iterator_,
        //                   NULL);
#endif
    } else GblHashSet_destruct(&propertyRegistry_);
    GBL_CTX_END();
}

// Returns the very first starting property for a given type.
GBL_INLINE const GblPropertyRoot_* propertyRootBase_(GblType objectType) {
    const GblPropertyRoot_* pRoot = NULL;
    const uint8_t depth = GblType_depth(objectType);

    for(uint8_t d = 0; d <= depth; ++d) {
        GblType type = GblType_base(objectType, d);
        pRoot = propertyRoot_(type);
        if(pRoot) break;
    }

    return pRoot;
}

GBL_INLINE const GblProperty* propertyFirstNextBase_(GblType objectType, GblType prevType) {
    const GblProperty* pRoot = NULL;
    const uint8_t depth = GblType_depth(objectType);
    for(uint8_t d = 0; d <= depth; ++d) {
        GblType type = GblType_base(objectType, d);
        if(type == prevType) {
            if(d + 1 <= depth) {
                prevType = GblType_base(objectType, d+1);
                pRoot = propertyFirst_(prevType);
                if(pRoot) break;
            }
        }
    }
    return pRoot;
}

GBL_EXPORT const char* GblProperty_name(const GblProperty* pSelf) {
    return pSelf? GblQuark_toString(pSelf->name) : NULL;
}

GBL_EXPORT GblType GblProperty_objectType(const GblProperty* pSelf) {
    return pSelf? GBL_PRIV_REF(pSelf).objectType : GBL_INVALID_TYPE;
}

GBL_EXPORT GBL_RESULT GblProperty_createOrConstruct(GblProperty** ppSelf,
                                                    GblType       derivedType,
                                                    const char*   pName,
                                                    size_t        id,
                                                    GblFlags      flags,
                                                    size_t        optionalArgCount,
                                                    ...)
{
    va_list varArgs;
    va_start(varArgs, optionalArgCount);
    if(ppSelf && *ppSelf) {
        return GblProperty_constructVaList(*ppSelf,
                                           derivedType,
                                           pName,
                                           id,
                                           flags,
                                           optionalArgCount,
                                           &varArgs);
    } else {
        GblProperty* pProp = GblProperty_createVaList(derivedType,
                                                      pName,
                                                      id,
                                                      flags,
                                                      optionalArgCount,
                                                      &varArgs);
        if(ppSelf) *ppSelf = pProp;
        return pProp? GBL_RESULT_SUCCESS : GBL_RESULT_ERROR_INVALID_POINTER;
    }
}

GBL_EXPORT const GblProperty* GblProperty_findQuark(GblType objectType, GblQuark name) {
    const GblProperty* pProperty = NULL;
    GBL_CTX_BEGIN(GblHashSet_context(&propertyRegistry_)); {
        GBL_CTX_VERIFY(name != GBL_QUARK_INVALID,
                       GBL_RESULT_ERROR_INVALID_PROPERTY);
        GblProperty* pKey             = GBL_ALLOCA(sizeof(GblProperty));
        pKey->name                    = name;
        GBL_PRIV_REF(pKey).objectType = objectType;

        while(GBL_PRIV_REF(pKey).objectType != GBL_INVALID_TYPE) {
            const GblProperty** ppRet = (const GblProperty**)GblHashSet_get(&propertyRegistry_, &pKey);
            if(ppRet) {
                pProperty = *ppRet;
                break;
            }
            GBL_PRIV_REF(pKey).objectType = GblType_parent(GBL_PRIV_REF(pKey).objectType);
        }

    } GBL_CTX_END_BLOCK();
    return pProperty;
}

GBL_EXPORT const GblProperty* GblProperty_find(GblType objectType, const char* pName) {
    return GblProperty_findQuark(objectType, GblQuark_tryString(pName));
}

GBL_EXPORT GblBool GblProperty_foreach(GblType           objectType,
                                       GBL_PROPERTY_FLAG flags,
                                       GblPropertyIterFn pFnIt,
                                       void*             pClosure) {
    GBL_CTX_BEGIN(NULL);
    GblBool result = GBL_FALSE;

    GBL_CTX_VERIFY_TYPE(objectType);
    GBL_CTX_VERIFY_POINTER(pFnIt);

    const size_t  depth = GblType_depth(objectType);
    for(size_t  d = 0; d <= depth; ++d) {
        const GblType curType = GblType_base(objectType, d);
        const GblProperty* pProp = NULL;
        while((pProp = GblProperty_next(curType, pProp, flags))) {
            if(pFnIt(pProp, pClosure)) {
                result = GBL_TRUE;
                GBL_CTX_DONE();
            }
        }
    }

    GBL_CTX_END_BLOCK();
    return result;
}

GBL_EXPORT const GblProperty* GblProperty_next(GblType objectType, const GblProperty* pPrev, GblFlags mask) {
    const GblProperty* pNext = NULL;
    GBL_CTX_BEGIN(GblHashSet_context(&propertyRegistry_));
    const GblProperty* pIt = NULL;

    // We're first starting, grab the root node of the first type with a property entry
    if(!pPrev) {
        pIt = (GblProperty*)propertyRoot_(objectType);
        // Skip types which don't have the flag we're looking for
        if(!pIt || !(pIt->flags&mask))
            GBL_CTX_DONE();
    // We're continuing from an existing node
    } else {
        pIt = pPrev;
    }

    // while on unacceptable node
    while(pIt) {

        // regular next node
        if(GBL_PRIV_REF(pIt).pNext) {
            pIt = GblProperty_findQuark(objectType, GBL_PRIV_REF(pIt).pNext->name);
        //beginning of new type
        } else {
            break;
        }

        // Check if the mask matches and skip overrides!!
        if((pIt->flags & mask) && !(pIt->flags & GBL_PROPERTY_FLAG_OVERRIDE)) {
            pNext = pIt;
            break;
        }
    }

    GBL_CTX_END_BLOCK();
    return pNext;
}

static GBL_RESULT GblProperty_validate_(const GblProperty* pProp) {
    GBL_CTX_BEGIN(GblHashSet_context(&propertyRegistry_));

    GBL_CTX_VERIFY_TYPE(GblProperty_objectType(pProp), GBL_OBJECT_TYPE);

    GBL_CTX_VERIFY(pProp->name != GBL_QUARK_INVALID,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "Attempted to add property without a valid name: [%s]",
                   GblType_name(GblProperty_objectType(pProp)));

    GBL_CTX_VERIFY(pProp->valueType != GBL_INVALID_TYPE &&
                   pProp->valueType != GBL_NIL_TYPE,
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "Attempted to add property with invalid or nil type: [%s::%s]",
                   GblType_name(GblProperty_objectType(pProp)),
                   GblProperty_name(pProp));

    GBL_CTX_VERIFY(pProp->flags,
                  GBL_RESULT_ERROR_INVALID_PROPERTY,
                  "Attempted to add property with no flags: [%s::%s]",
                  GblType_name(GblProperty_objectType(pProp)),
                  GblProperty_name(pProp));

    GBL_CTX_VERIFY(!(!(pProp->flags & GBL_PROPERTY_FLAG_READ) && (pProp->flags & GBL_PROPERTY_FLAG_OUT)),
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "Attempted to add OUT flag without READ flag: [%s::%s]",
                   GblType_name(GblProperty_objectType(pProp)),
                   GblProperty_name(pProp));

    GBL_CTX_VERIFY(!(!(pProp->flags & (GBL_PROPERTY_FLAG_WRITE | GBL_PROPERTY_FLAG_CONSTRUCT)) &&
                    (pProp->flags & GBL_PROPERTY_FLAG_OUT)),
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "Attempted to add IN flag without WRITE or CONSTRUCT flag: [%s::%s]",
                   GblType_name(GblProperty_objectType(pProp)),
                   GblProperty_name(pProp));

    GBL_CTX_VERIFY(!((pProp->flags & (GBL_PROPERTY_FLAG_ABSTRACT | GBL_PROPERTY_FLAG_OVERRIDE)) ==
                    (GBL_PROPERTY_FLAG_ABSTRACT | GBL_PROPERTY_FLAG_OVERRIDE)),
                   GBL_RESULT_ERROR_INVALID_PROPERTY,
                   "Attempted to add property marked both ABSTRACT and OVERRIDE: [%s::%s]",
                   GblType_name(GblProperty_objectType(pProp)),
                   GblProperty_name(pProp));

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblProperty_install(GblType objectType, GblProperty* pProperty) {
    GBL_CTX_BEGIN(GblHashSet_context(&propertyRegistry_));

    GBL_PRIV_REF(pProperty).objectType = objectType;

    GBL_CTX_PUSH_VERBOSE("[GblObject] Installing property: [%s::%s]",
                         GblType_name(GBL_PRIV_REF(pProperty).objectType),
                         GblQuark_toString(pProperty->name));

    GBL_CTX_VERIFY_CALL(GblProperty_validate_(pProperty));

    GblPropertyRoot_* pRoot = propertyRoot_(GBL_PRIV_REF(pProperty).objectType);
    if GBL_UNLIKELY(!pRoot) { // adding root node
        pRoot = GBL_CTX_MALLOC(sizeof(GblPropertyRoot_));
        memset(pRoot, 0, sizeof(GblPropertyRoot_));
        GBL_PRIV(pRoot->base).objectType = GBL_PRIV_REF(pProperty).objectType;

        GblBool isFirst = GblHashSet_insert(&propertyRegistry_, &pRoot);
        GBL_CTX_VERIFY_EXPRESSION(isFirst, "Somehow our root node wasn't the first node for the type!");
    }

    ++pRoot->count;
    pRoot->base.flags |= pProperty->flags;

    if(pRoot->pLast)
        GBL_PRIV_REF(pRoot->pLast).pNext = pProperty;

    pRoot->pLast = pProperty;

    if(!GBL_PRIV(pRoot->base).pNext)
        GBL_PRIV(pRoot->base).pNext = pProperty;

    GblBool isFirst = GblHashSet_insert(&propertyRegistry_, &pProperty);
    if(!isFirst) GBL_CTX_WARN("Overwrote existing property!");

    GBL_CTX_END();
}


GBL_EXPORT GblBool GblProperty_uninstallQuark(GblType objectType, GblQuark name) {
    GblBool success = GBL_FALSE;
    GBL_CTX_BEGIN(GblHashSet_context(&propertyRegistry_));
    GBL_CTX_VERIFY(name != GBL_QUARK_INVALID,
                   GBL_RESULT_ERROR_INVALID_PROPERTY);
    GBL_CTX_VERIFY_ARG(objectType != GBL_INVALID_TYPE);
    GBL_CTX_PUSH_VERBOSE("[GblObject] Uninstalling property: %s[%s]",
                         GblType_name(objectType), GblQuark_toString(name));

    // Find root node
    GblPropertyRoot_* pRoot = propertyRoot_(objectType);
    GBL_CTX_VERIFY_POINTER(pRoot);

    GblFlags flagsAccum     = 0;
    GblProperty* pIt        = &pRoot->base;
    GblProperty* pPrev      = NULL;
    GblProperty* pTarget    = NULL;

    // Traverse linked structure
     while(GBL_PRIV_REF(pIt).pNext) {
        pPrev = pIt;
        pIt = GBL_PRIV_REF(pIt).pNext;

        if(pIt->name == name) {
            // Remove node from list and store for later
            pTarget = pIt;
            GBL_PRIV_REF(pPrev).pNext = GBL_PRIV_REF(pIt).pNext;
        } else {
            // Accumulate flags for all other nodes
            flagsAccum |= pIt->flags;
        }
    }

    if(pTarget) {
        // Update root node
        pRoot->base.flags = flagsAccum;
        --pRoot->count;
        if(pRoot->pLast == pTarget) pRoot->pLast = pPrev;
        // Remove target from registry hash table
        success = GblHashSet_erase(&propertyRegistry_, &pTarget);

        // Check if we need to remove the root node too!
        if(!pRoot->count) {
            success = GblHashSet_erase(&propertyRegistry_, &pRoot);
        }
    }
    GBL_CTX_END_BLOCK();
    return success;
}

GBL_EXPORT GblBool GblProperty_uninstall(GblType objectType, const char* pName) {
    return GblProperty_uninstallQuark(objectType, GblQuark_tryString(pName));
}

GBL_EXPORT GblBool GblProperty_uninstallAll(GblType objectType) {
    GblBool success = GBL_FALSE;
    GBL_CTX_BEGIN(GblHashSet_context(&propertyRegistry_));
    const GblPropertyRoot_* pRoot = propertyRoot_(objectType);
    if(pRoot) {
        const unsigned propertyCount = pRoot->count;
        for(unsigned p = 0; p < propertyCount; ++p) { // last property removed will remove root!
            GBL_CTX_VERIFY_EXPRESSION(pRoot->pLast);
            const GblBool result = GblProperty_uninstallQuark(objectType, pRoot->pLast->name);
            GBL_CTX_VERIFY_EXPRESSION(result);
        }
    }
    success = GBL_TRUE;
    GBL_CTX_END_BLOCK();
    return success;
}


GBL_EXPORT GBL_RESULT GblProperty_defaultValue(const GblProperty* pSelf, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_VCALL(GblProperty, pFnDefaultValue, pSelf, pValue);
    GBL_CTX_END();
}

GBL_EXPORT GblBool GblProperty_checkValue(const GblProperty* pSelf, const GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_VCALL(GblProperty, pFnCheckValue, pSelf, pValue);
    GBL_CTX_END_BLOCK();
    return GBL_RESULT_SUCCESS(GBL_CTX_RESULT());
}

GBL_EXPORT GBL_RESULT GblProperty_validateValue(const GblProperty* pSelf, GblVariant* pValue) {
    GBL_CTX_BEGIN(NULL);
    GBL_VCALL(GblProperty, pFnValidateValue, pSelf, pValue);
    GBL_CTX_END();
}

GBL_EXPORT int GblProperty_compareValues(const GblProperty* pSelf, const GblVariant* pV1, const GblVariant* pV2) {
    int result = 0;
    GBL_CTX_BEGIN(NULL);
    GBL_VCALL(GblProperty, pFnCompareValues, pSelf, pV1, pV2, &result);
    GBL_CTX_END_BLOCK();
    return result;
}

GBL_EXPORT size_t  GblProperty_count(GblType objectType) {
    size_t  count = 0;
    while(objectType != GBL_INVALID_TYPE) {
        const GblPropertyRoot_* pRoot = propertyRoot_(objectType);
        if(pRoot) {
            count += pRoot->count;
        }
        objectType = GblType_parent(objectType);
    }
    return count;
}

GBL_EXPORT GblFlags GblProperty_combinedFlags(GblType objectType) {
    const GblPropertyRoot_* pRoot = propertyRoot_(objectType);
    return pRoot? pRoot->base.flags : 0;
}

static GBL_RESULT GblProperty_initVaList_(GblProperty* pSelf,
                                          GblType      derivedType,
                                          const char*  pName,
                                          size_t       id,
                                          GblFlags     flags,
                                          size_t       optionalArgCount,
                                          va_list*     pList)
{
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_TYPE(derivedType, GBL_PROPERTY_TYPE);

    pSelf->name                    = GblQuark_fromString(pName);
    pSelf->id                      = id;
    pSelf->flags                   = flags;

    if(optionalArgCount) {
        GBL_VCALL(GblProperty, pFnInitOptionalArgs, pSelf, optionalArgCount, pList);
    }

    GBL_CTX_END();
}

GBL_EXPORT GblProperty* GblProperty_createVaList(GblType     derivedType,
                                                 const char* pName,
                                                 size_t      id,
                                                 GblFlags    flags,
                                                 size_t      optionalArgCount,
                                                 va_list*    pList)
{
    GblProperty* pProp = GBL_PROPERTY(GblBox_create(derivedType));
    GblProperty_initVaList_(pProp,
                            derivedType,
                            pName,
                            id,
                            flags,
                            optionalArgCount,
                            pList);
    return pProp;
}

GBL_EXPORT GBL_RESULT GblProperty_constructVaList(GblProperty* pSelf,
                                                  GblType     derivedType,
                                                  const char* pName,
                                                  size_t      id,
                                                  GblFlags    flags,
                                                  size_t      optionalArgCount,
                                                  va_list*    pList)
{
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblBox_construct(GBL_BOX(pSelf), derivedType));
    GBL_CTX_VERIFY_CALL(GblProperty_initVaList_(pSelf,
                                                derivedType,
                                                pName,
                                                id,
                                                flags,
                                                optionalArgCount,
                                                pList));
    GBL_CTX_END();
}

GBL_EXPORT GblProperty* GblProperty_create(GblType     derivedType,
                                           const char* pName,
                                           size_t      id,
                                           GblFlags    flags,
                                           size_t      optionalArgCount,
                                           ...)
{

    va_list varArgs;
    va_start(varArgs, optionalArgCount);
    GblProperty* pProp = GblProperty_createVaList(derivedType,
                                                  pName,
                                                  id,
                                                  flags,
                                                  optionalArgCount,
                                                  &varArgs);
    va_end(varArgs);
    return pProp;
}

GBL_EXPORT GBL_RESULT GblProperty_construct(GblProperty* pSelf,
                                            GblType     derivedType,
                                            const char* pName,
                                            size_t      id,
                                            GblFlags    flags,
                                            size_t      optionalArgCount,
                                            ...)
{
    va_list varArgs;
    va_start(varArgs, optionalArgCount);
    GBL_RESULT result = GblProperty_constructVaList(pSelf,
                                                    derivedType,
                                                    pName,
                                                    id,
                                                    flags,
                                                    optionalArgCount,
                                                    &varArgs);
    va_end(varArgs);
    return result;
}

static GBL_RESULT GblProperty_initOptionalArgs_(GblProperty* pProp, size_t  argCount, va_list* pList) {
    GBL_UNUSED(pProp, argCount, pList);
    if(!argCount)
        return GBL_RESULT_SUCCESS;
    else if(argCount == 1) {
        pProp->valueType = va_arg(*pList, GblType);
        return GBL_RESULT_SUCCESS;
    }
    else return GBL_RESULT_INCOMPLETE;
}

static GBL_RESULT GblProperty_defaultValue_(const GblProperty* pProp, GblVariant* pVariant) {
    GBL_CTX_BEGIN(NULL);
    GBL_CTX_VERIFY_CALL(GblVariant_destruct(pVariant));
    GBL_CTX_VERIFY_CALL(GblVariant_constructDefault(pVariant, pProp->valueType));
    GBL_CTX_END();
}

static GBL_RESULT GblProperty_checkValue_(const GblProperty* pProp, const GblVariant* pVariant) {
    if(GblType_check(GblVariant_typeOf(pVariant), pProp->valueType))
        return GBL_RESULT_SUCCESS;
    else
        return GBL_RESULT_ERROR_INVALID_TYPE;
}

static GBL_RESULT GblProperty_validateValue_(const GblProperty* pProp, GblVariant* pVariant) {
    GBL_UNUSED(pProp, pVariant);
    return GBL_RESULT_UNIMPLEMENTED;
}

static GBL_RESULT GblProperty_compareValues_(const GblProperty* pProp, const GblVariant* pV1, const GblVariant* pV2, int* pResult) {
    GBL_UNUSED(pProp);
    *pResult = GblVariant_compare(pV1, pV2);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblPropertyClass_init_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);
    GBL_CTX_BEGIN(NULL);

    GblPropertyClass* pSelf     = (GblPropertyClass*)pClass;
    pSelf->pFnInitOptionalArgs  = GblProperty_initOptionalArgs_;
    pSelf->pFnDefaultValue      = GblProperty_defaultValue_;
    pSelf->pFnCheckValue        = GblProperty_checkValue_;
    pSelf->pFnValidateValue     = GblProperty_validateValue_;
    pSelf->pFnCompareValues     = GblProperty_compareValues_;
    GBL_CTX_END();
}

GBL_EXPORT GblType GblProperty_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize    = sizeof(GblPropertyClass),
        .pFnClassInit = GblPropertyClass_init_,
        .instanceSize = sizeof(GblProperty)
    };

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GblProperty"),
                                GBL_BOX_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}



