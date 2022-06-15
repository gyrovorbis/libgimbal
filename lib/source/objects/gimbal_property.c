#include <gimbal/objects/gimbal_property.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/strings/gimbal_quark.h>
#include <gimbal/objects/gimbal_object.h>
#include <gimbal/meta/gimbal_primitives.h>


static GblHashSet   propertyRegistry_;


// ========== PROPERTY SYSTEM PRIVATE ==========

static GblHash propertyHasher_(const GblHashSet* pSet, const GblProperty** pProperty) {
    GBL_UNUSED(pSet);
    return gblHashMurmur(((uint8_t*)*pProperty) + offsetof(GblProperty, objectType), sizeof(GblQuark) + sizeof(GblType));
}

static GblBool propertyComparator_(const GblHashSet* pSet, const GblProperty** p1, const GblProperty** p2) {
    GBL_UNUSED(pSet);
    return (*p1)->objectType    == (*p2)->objectType &&
           (*p1)->name          == (*p2)->name;
}

static void propertyDestructor_(const GblHashSet* pSet, GblProperty** pProperty) {
    GBL_API_BEGIN(pSet->pCtx);
    GBL_API_FREE(*pProperty);
    GBL_API_END_BLOCK();
}

GBL_INLINE GblProperty* propertyRoot_(GblType objectType) {
    GblProperty* pProperty = NULL;
    GBL_API_BEGIN(GblHashSet_context(&propertyRegistry_)); {
        GblProperty* pKey           = GBL_ALLOCA(sizeof(GblProperty));
        GblProperty** ppRet         = NULL;
        pKey->name                  = GBL_QUARK_INVALID;
        pKey->objectType            = objectType;
        ppRet                       = (GblProperty**)GblHashSet_get(&propertyRegistry_, &pKey);
        if(ppRet) pProperty         = *ppRet;
    } GBL_API_END_BLOCK();
    return pProperty;
}

GBL_INLINE  GblProperty* propertyFirst_(GblType objectType) {
    GblProperty* pProperty = NULL;
    GBL_API_BEGIN(GblHashSet_context(&propertyRegistry_));
    const GblProperty* pFirstNode = propertyRoot_(objectType);
    if(pFirstNode) pProperty = pFirstNode->pNext_;
    GBL_API_END_BLOCK();
    return pProperty;
}


GBL_API gblPropertyTableInitialize(GblContext* pCtx, GblSize initialCapacity) {
    GBL_API_BEGIN(pCtx);
    GblHashSet_construct(&propertyRegistry_,
                         sizeof(GblProperty*),
                         (GblHashSetEntryHashFn)propertyHasher_,
                         (GblHashSetEntryCompareFn)propertyComparator_,
                         (GblHashSetEntryDestructFn)propertyDestructor_,
                         initialCapacity,
                         pCtx);
    GBL_API_END();
}

GBL_API gblPropertyTableFinalize(void) {
    GBL_API_BEGIN(GblHashSet_context(&propertyRegistry_));
    GblHashSet_destruct(&propertyRegistry_);
    GBL_API_END();
}


GBL_EXPORT GblSize gblpropertyTableSize(void) {
    return GblHashSet_size(&propertyRegistry_);
}


static GBL_RESULT gblPropertyTableInsert_(GblProperty* pProperty) {
    GBL_API_BEGIN(GblHashSet_context(&propertyRegistry_));
    GBL_API_VERIFY_TYPE(pProperty->objectType, GBL_OBJECT_TYPE);
    GBL_API_VERIFY_POINTER(pProperty->name);
    GBL_API_PUSH_VERBOSE("[GblObject] Installing property: %s[%s]",
                         GblType_name(pProperty->objectType),
                         GblQuark_toString(pProperty->name));
    GBL_API_VERIFY_ARG(pProperty->valueType != GBL_NIL_TYPE);
    GBL_API_VERIFY_ARG(pProperty->flags); // make sure SOME flag is there

    GblProperty* pFirst = propertyRoot_(pProperty->objectType);
    if(!pFirst) { // adding root node
        pFirst = GBL_API_MALLOC(sizeof(GblProperty));
        pFirst->objectType      = pProperty->objectType;
        pFirst->name            = GBL_QUARK_INVALID;
        pFirst->pLast_          = NULL;
        pFirst->flags           = 0;
        pFirst->propertyCount_  = 0;
        pFirst->pNext_          = NULL;
        GblBool isFirst = GblHashSet_insert(&propertyRegistry_, &pFirst);
        GBL_API_VERIFY_EXPRESSION(isFirst, "Somehow our root node wasn't the first node for the type!");
    }

    ++pFirst->propertyCount_;
    pFirst->flags |= pProperty->flags;
    if(pFirst->pLast_) pFirst->pLast_->pNext_ = pProperty;
    pFirst->pLast_ = pProperty;
    if(!pFirst->pNext_) pFirst->pNext_ = pProperty;

    GblBool isFirst = GblHashSet_insert(&propertyRegistry_, &pProperty);
    if(!isFirst) GBL_API_WARN("Overwrote existing property!");

    GBL_API_POP(1);
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT gblPropertyTableInsertExisting(GblProperty* pProperty) {
    return gblPropertyTableInsert_(pProperty);
}

GBL_EXPORT const GblProperty* gblPropertyTableInsert(GblType             objectType,
                                                     GblQuark            name,
                                                     GblSize             id,
                                                     GblType             valueType,
                                                     GblFlags            flags) GBL_NOEXCEPT
{
    GBL_API_BEGIN(GblHashSet_context(&propertyRegistry_));
    GblProperty* pProp = GBL_API_MALLOC(sizeof(GblProperty));
    pProp->objectType = objectType;
    pProp->name       = name;
    pProp->id         = id;
    pProp->valueType  = valueType;
    pProp->flags      = flags;
    pProp->pNext_      = NULL;

    GBL_API_VERIFY_CALL(gblPropertyTableInsert_(pProp));

    GBL_API_END_BLOCK();
    if(!GBL_RESULT_SUCCESS(GBL_API_RESULT())) {
        GBL_API_FREE(pProp);
        pProp = NULL;
    }
    return pProp;
}

GBL_EXPORT GblBool gblPropertyTableErase(GblType objectType, GblQuark name) GBL_NOEXCEPT {
    GblBool success = GBL_FALSE;
    GBL_API_BEGIN(GblHashSet_context(&propertyRegistry_));
    GBL_API_VERIFY_ARG(objectType != GBL_INVALID_TYPE);
    GBL_API_PUSH_VERBOSE("[GblObject] Uninstalling property: %s[%s]",
                         GblType_name(objectType), GblQuark_toString(name));

    // Find root node
    GblProperty* pFirst = propertyRoot_(objectType);
    GBL_API_VERIFY_POINTER(pFirst);

    GblFlags flagsAccum = 0;
    GblProperty* pIt    = pFirst;
    GblProperty* pPrev  = NULL;
    GblProperty* pTarget = NULL;

    // Traverse linked structure
     while(pIt->pNext_) {
        pPrev = pIt;
        pIt = pIt->pNext_;

        if(pIt->name == name) {
            // Remove node from list and store for later
            pTarget = pIt;
            pPrev->pNext_ = pIt->pNext_;
        } else {
            // Accumulate flags for all other nodes
            flagsAccum |= pIt->flags;
        }
    }

    if(pTarget) {
        // Update root node
        pFirst->flags = flagsAccum;
        --pFirst->propertyCount_;
        if(pFirst->pLast_ == pTarget) pFirst->pLast_ = pPrev;
        // Remove target from registry hash table
        success = GblHashSet_erase(&propertyRegistry_, &pTarget);

        // Check if we need to remove the root node too!
        if(!pFirst->propertyCount_) {
            success = GblHashSet_erase(&propertyRegistry_, &pFirst);
        }
    }
    GBL_API_END_BLOCK();
    return success;
}

GBL_EXPORT GblBool gblPropertyTableEraseAll(GblType objectType) GBL_NOEXCEPT {
    GblBool success = GBL_FALSE;
    GBL_API_BEGIN(GblHashSet_context(&propertyRegistry_));
    const GblProperty* pRoot = propertyRoot_(objectType);
    if(pRoot) {
        const unsigned propertyCount = pRoot->propertyCount_;
        for(unsigned p = 0; p < propertyCount; ++p) { // last property removed will remove root!
            GBL_API_VERIFY_EXPRESSION(pRoot->pLast_);
            const GblBool result = gblPropertyTableErase(objectType, pRoot->pLast_->name);
            GBL_API_VERIFY_EXPRESSION(result);
        }
    }
    success = GBL_TRUE;
    GBL_API_END_BLOCK();
    return success;
}

GBL_EXPORT const GblProperty* gblPropertyTableFind(GblType objectType, GblQuark name) GBL_NOEXCEPT {
    const GblProperty* pProperty = NULL;
    GBL_API_BEGIN(GblHashSet_context(&propertyRegistry_)); {
        GblProperty* pKey           = GBL_ALLOCA(sizeof(GblProperty));
        pKey->name                  = name;
        pKey->objectType            = objectType;

        while(pKey->objectType != GBL_INVALID_TYPE) {
            const GblProperty** ppRet = (const GblProperty**)GblHashSet_get(&propertyRegistry_, &pKey);
            if(ppRet) {
                pProperty = *ppRet;
                break;
            }
            pKey->objectType = GblType_parent(pKey->objectType);
        }

    } GBL_API_END_BLOCK();
    return pProperty;
}

GBL_INLINE const GblProperty* propertyRootBase_(GblType objectType) {
    const GblProperty* pRoot = NULL;
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

GBL_EXPORT const GblProperty* gblPropertyTableNext(GblType objectType, const GblProperty* pPrev, GblFlags mask) GBL_NOEXCEPT {
    const GblProperty* pNext = NULL;
    GBL_API_BEGIN(GblHashSet_context(&propertyRegistry_));
    const GblProperty* pIt = NULL;

    // We're first starting, grab the root node of the first type with a property entry
    if(!pPrev) {
        pIt = propertyRootBase_(objectType);
    // We're continuing from an existing node
    } else {
        pIt = pPrev;
    }

    // while on unacceptable node
    while(pIt) {

        // regular next node
        if(pIt->pNext_) {
            pIt = gblPropertyTableFind(objectType, pIt->pNext_->name);
        //beginning of new type
        } else {
            pIt = propertyFirstNextBase_(objectType, pIt->objectType);
            if(!pIt) break;
        }

        // Check if the mask matches and skip overrides!!
        if((pIt->flags & mask) && !(pIt->flags & GBL_PROPERTY_FLAG_OVERRIDE)) {
            pNext = pIt;
            break;
        }
    }

    GBL_API_END_BLOCK();
    return pNext;
}

GBL_EXPORT GblSize gblPropertyTableCount(GblType objectType) GBL_NOEXCEPT {
    GblSize count = 0;
    while(objectType != GBL_INVALID_TYPE) {
        const GblProperty* pProp = propertyRoot_(objectType);
        if(pProp) {
            count += pProp->propertyCount_;
        }
        objectType = GblType_parent(objectType);
    }
    return count;
}

GBL_EXPORT GblFlags gblPropertyTableFlags(GblType objectType) GBL_NOEXCEPT {
    const GblProperty* pProp = propertyRoot_(objectType);
    return pProp? pProp->flags : 0;
}

// ========== PROPERTY PUBLIC API ==========

typedef struct GblPropertyPrivate_ {
    GBL_ATOMIC_INT16    refCount;
   #if 0
        GblLinkedListNode   listNode;
    union {
        struct {
            const char*             pNick;
            const char*             pDesc;
        };
        struct { //private
            struct GblProperty*     pLast;
            GblSize                 propertyCount;
        };
    };
#endif
} GblPropertyPrivate_;

GBL_EXPORT GblQuark GblProperty_nameQuark(const GblProperty* pSelf) GBL_NOEXCEPT {
    return pSelf? pSelf->name : GBL_QUARK_INVALID;
}
GBL_EXPORT const char* GblProperty_nameString(const GblProperty* pSelf) GBL_NOEXCEPT {
    return pSelf? GblQuark_toString(pSelf->name) : NULL;
}
GBL_EXPORT GblType GblProperty_valueType(const GblProperty* pSelf) GBL_NOEXCEPT {
    return pSelf? pSelf->valueType : GBL_INVALID_TYPE;
}
GBL_EXPORT GblType GblProperty_objectType(const GblProperty* pSelf) GBL_NOEXCEPT {
    return pSelf? pSelf->objectType : GBL_INVALID_TYPE;
}
GBL_EXPORT GblSize GblProperty_id(const GblProperty* pSelf) GBL_NOEXCEPT {
    return pSelf? pSelf->id : 0;
}
GBL_EXPORT GblFlags GblProperty_flags(const GblProperty* pSelf)  GBL_NOEXCEPT {
    return pSelf? pSelf->flags : 0;
}

GBL_EXPORT GblType GblProperty_type(void) GBL_NOEXCEPT {
    static GblType type = GBL_INVALID_TYPE;
    if(type == GBL_INVALID_TYPE) {
        type = GblType_registerStatic(GBL_INVALID_TYPE,
                                      GblQuark_internStringStatic("Property"),
                                      &(const GblTypeInfo) {
                                          .classSize            = sizeof(GblPropertyClass),
                                          .instanceSize         = sizeof(GblProperty),
                                          .instancePrivateSize  = sizeof(GblPropertyPrivate_)
                                      },
                                      GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE |
                                      GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE);
    }
    return type;
}

GBL_EXPORT GblProperty* GblProperty_create(GblType type, const GblPropertyInfo* pInfo) {
    GblProperty* pProperty = GBL_PROPERTY(GblInstance_create(type));
    pProperty->objectType   = pInfo->objectType;
    pProperty->name         = GblQuark_fromString(pInfo->pName);
    pProperty->pDesc        = pInfo->pDesc;
    pProperty->pNick        = pInfo->pNick;
    pProperty->flags        = pInfo->flags;
    pProperty->id           = pInfo->id;
    pProperty->valueType    = pInfo->valueType;
    return pProperty;
}

GBL_EXPORT GblProperty* GblProperty_ref(GblProperty* pProperty) {
    GblPropertyPrivate_* pPrivate = GblInstance_private(GBL_INSTANCE(pProperty), GBL_PROPERTY_TYPE);
    GBL_ATOMIC_INT16_INC(pPrivate->refCount);
    return pProperty;
}

GBL_EXPORT GblRefCount GblProperty_unref(GblProperty* pProperty) {
    GblRefCount count = 0;
    GblPropertyPrivate_* pPrivate = GblInstance_private(GBL_INSTANCE(pProperty), GBL_PROPERTY_TYPE);
    if((count = GBL_ATOMIC_INT16_DEC(pPrivate->refCount)-1) == 0) {
        GblPropertyClass* pClass = pProperty->pClass;
        if(pClass->pFnDestruct) pClass->pFnDestruct(pProperty);
        GblInstance_destroy(GBL_INSTANCE(pProperty));
    }
    return count;
}

GBL_EXPORT GblRefCount GblProperty_refCount(const GblProperty* pProperty) {
    GblRefCount count = 0;
    GblPropertyPrivate_* pPrivate = GblInstance_private(GBL_INSTANCE(pProperty), GBL_PROPERTY_TYPE);
    count = GBL_ATOMIC_INT16_LOAD(pPrivate->refCount);
    return count;
}

GBL_EXPORT GBL_RESULT GblProperty_variantDefault(const GblProperty* pProperty, GblVariant* pVariant)  {
    GBL_API_BEGIN(NULL);
    GBL_INSTANCE_VCALL_PREFIX(GBL_PROPERTY, pFnVariantDefault, pProperty, pVariant);
    GBL_API_END();
}
GBL_EXPORT GBL_RESULT GblProperty_variantValidate(const GblProperty* pProperty, GblVariant* pVariant) {
    GBL_API_BEGIN(NULL);
    GBL_INSTANCE_VCALL_PREFIX(GBL_PROPERTY, pFnVariantValidate, pProperty, pVariant);
    GBL_API_END();
}
GBL_EXPORT GblInt  GblProperty_variantCompare(const GblProperty* pProperty,
                                              const GblVariant* pVariant1,
                                              const GblVariant* pVariant2)
{
    GblInt result = INT_MAX;
    GBL_API_BEGIN(NULL);
    GBL_INSTANCE_VCALL_PREFIX(GBL_PROPERTY, pFnVariantCompare, pProperty, pVariant1, pVariant2, &result);
    GBL_API_END_BLOCK();
    return result;
}
GBL_EXPORT GblBool GblProperty_variantCheck(const GblProperty* pProperty,
                                            const GblVariant* pVariant)
{
    GBL_API_BEGIN(NULL);
    GBL_INSTANCE_VCALL_PREFIX(GBL_PROPERTY, pFnVariantCheck, pProperty, pVariant);
    GBL_API_VERIFY_LAST_RECORD();
    GBL_API_END_BLOCK();
    return GBL_RESULT_SUCCESS(GBL_API_RESULT());
}


