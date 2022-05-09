#include <gimbal/objects/gimbal_property.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/types/gimbal_quark.h>
#include <gimbal/objects/gimbal_object.h>
#include <gimbal/meta/gimbal_value_types.h>

/*
 * Really should add more data like info and shit
 */
typedef struct GblProperty_ {
    GblType                         objectType;
    GblQuark                        name;
    union {
        struct {
            GblType                 valueType;
            GBL_PROPERTY_FLAGS      flags;
            GblSize                 id;
        };
        struct {
            struct GblProperty_*    pLast;
            GblFlags                combinedFlags;
            GblSize                 propertyCount;
        };
    };
    struct GblProperty_*            pNext;
} GblProperty_;


static GblHashSet   propertyRegistry_;


// ========== PROPERTY SYSTEM PRIVATE ==========

static GblHash propertyHasher_(const GblHashSet* pSet, const GblProperty** pProperty) {
    GBL_UNUSED(pSet);
    return gblHashMurmur(*pProperty, sizeof(GblQuark) + sizeof(GblType));
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
    if(pFirstNode) pProperty = pFirstNode->pNext;
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

GBL_EXPORT const GblProperty* gblPropertyTableInsert          (GblType             objectType,
                                                               GblQuark            name,
                                                               GblSize             id,
                                                               GblType             valueType,
                                                               GblFlags            flags) GBL_NOEXCEPT
{
    GblProperty* pProp = NULL;
    GBL_API_BEGIN(GblHashSet_context(&propertyRegistry_));
    GBL_API_VERIFY_TYPE(objectType, GBL_OBJECT_TYPE);
    GBL_API_VERIFY_POINTER(name);
    GBL_API_PUSH_VERBOSE("[GblObject] Installing property: %s[%s]", GblType_name(objectType), GblQuark_toString(name));
    GBL_API_VERIFY_ARG(valueType != GBL_NIL_TYPE);
    GBL_API_VERIFY_ARG(flags); // make sure SOME flag is there

    GblProperty* pFirst = propertyRoot_(objectType);
    if(!pFirst) { // adding root node
        pFirst = GBL_API_MALLOC(sizeof(GblProperty_));
        pFirst->objectType      = objectType;
        pFirst->name            = GBL_QUARK_INVALID;
        pFirst->pLast           = NULL;
        pFirst->combinedFlags   = 0;
        pFirst->propertyCount   = 0;
        pFirst->pNext           = NULL;
        GblBool isFirst = GblHashSet_insert(&propertyRegistry_, &pFirst);
        GBL_API_VERIFY_EXPRESSION(isFirst, "Somehow our root node wasn't the first node for the type!");
    }

    pProp = GBL_API_MALLOC(sizeof(GblProperty_));
    pProp->objectType = objectType;
    pProp->name       = name;
    pProp->id         = id;
    pProp->valueType  = valueType;
    pProp->flags      = flags;
    pProp->pNext      = NULL;

    ++pFirst->propertyCount;
    pFirst->combinedFlags |= pProp->flags;
    if(pFirst->pLast) pFirst->pLast->pNext = pProp;
    pFirst->pLast = pProp;
    if(!pFirst->pNext) pFirst->pNext = pProp;

    GblBool isFirst = GblHashSet_insert(&propertyRegistry_, &pProp);
    if(!isFirst) GBL_API_WARN("Overwrote existing property!");

    GBL_API_POP(1);
    GBL_API_END_BLOCK();
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

    GBL_PROPERTY_FLAGS flagsAccum = 0;
    GblProperty* pIt    = pFirst;
    GblProperty* pPrev  = NULL;
    GblProperty* pTarget = NULL;

    // Traverse linked structure
     while(pIt->pNext) {
        pPrev = pIt;
        pIt = pIt->pNext;

        if(pIt->name == name) {
            // Remove node from list and store for later
            pTarget = pIt;
            pPrev->pNext = pIt->pNext;
        } else {
            // Accumulate flags for all other nodes
            flagsAccum |= pIt->flags;
        }
    }

    if(pTarget) {
        // Update root node
        pFirst->combinedFlags = flagsAccum;
        --pFirst->propertyCount;
        if(pFirst->pLast == pTarget) pFirst->pLast = pPrev;
        // Remove target from registry hash table
        success = GblHashSet_erase(&propertyRegistry_, &pTarget);

        // Check if we need to remove the root node too!
        if(!pFirst->propertyCount) {
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
        const unsigned propertyCount = pRoot->propertyCount;
        for(unsigned p = 0; p < propertyCount; ++p) { // last property removed will remove root!
            GBL_API_VERIFY_EXPRESSION(pRoot->pLast);
            const GblBool result = gblPropertyTableErase(objectType, pRoot->pLast->name);
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
        if(pIt->pNext) {
            pIt = gblPropertyTableFind(objectType, pIt->pNext->name);
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
            count += pProp->propertyCount;
        }
        objectType = GblType_parent(objectType);
    }
    return count;
}

GBL_EXPORT GblFlags gblPropertyTableFlags(GblType objectType) GBL_NOEXCEPT {
    const GblProperty* pProp = propertyRoot_(objectType);
    return pProp? pProp->combinedFlags : 0;
}

// ========== PROPERTY PUBLIC API ==========
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



