/*! \file
 *  \brief ::GblArrayMap structure and related functions
 *  \ingroup containers
 */

#ifndef GIMBAL_ARRAY_MAP_H
#define GIMBAL_ARRAY_MAP_H

#include "../types/gimbal_typedefs.h"
#include "../strings/gimbal_quark.h"
#include "../types/gimbal_variant.h"
#include "../types/gimbal_result.h"

#define GBL_ARRAY_MAP_NPOS             GBL_NPOS
#define GBL_ARRAY_MAP_SIZE(elements)   (sizeof(GblArrayMap) + (sizeof(GblArrayMapEntry)*(elements)))

#define SELF    GblArrayMap** ppSelf
#define CSELF   GblArrayMap*const* ppSelf

GBL_FORWARD_DECLARE_STRUCT(GblArrayMapEntry);

typedef GBL_RESULT  (*GblArrayMapDestructFn)(uintptr_t key, void* pEntry);
typedef int         (*GblArrayMapCompareFn) (uintptr_t key1, uintptr_t key2);

typedef struct GblArrayMapEntry {
    uintptr_t                   key;
    GblVariant                  value; // using value.type == GBL_INVALID_TYPE to signify userdata
    GblArrayMapDestructFn       dtor;
} GblArrayMapEntry;

/*! \brief Contiguous array-based associative container with [K,V] pairs
 *  \ingroup containers
 */
typedef struct GblArrayMap {
    GblContext*             pCtx;
    GblArrayMapCompareFn    pFnComparator;
    GblSize                 binarySearch    : 1;    //todo
#ifdef GBL_64BIT
    GblSize                 size            : 63;
#elif defined(GBL_32BIT)
    GblSize                 size            : 31;
#endif
} GblArrayMap;

GBL_EXPORT GblArrayMap* GblArrayMap_create          (GblArrayMapCompareFn pFnComp,
                                                     GblContext*          pCtx)     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblArrayMap_destroy         (SELF)                          GBL_NOEXCEPT;

GBL_INLINE GblSize      GblArrayMap_size            (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblContext*  GblArrayMap_context         (CSELF)                         GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayMap_empty           (CSELF)                         GBL_NOEXCEPT;

GBL_INLINE GblBool      GblArrayMap_contains        (CSELF, uintptr_t key)          GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayMap_containsUserdata(CSELF, uintptr_t key)          GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayMap_containsVariant (CSELF, uintptr_t key)          GBL_NOEXCEPT;

GBL_INLINE uintptr_t    GblArrayMap_getValue        (CSELF, uintptr_t key)          GBL_NOEXCEPT;
GBL_INLINE GblVariant*  GblArrayMap_getVariant      (CSELF, uintptr_t key)          GBL_NOEXCEPT;
GBL_INLINE uintptr_t    GblArrayMap_atValue         (CSELF, uintptr_t key)          GBL_NOEXCEPT;
GBL_INLINE GblVariant*  GblArrayMap_atVariant       (CSELF, uintptr_t key)          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblArrayMap_setUserdata     (SELF,
                                                     uintptr_t key,
                                                     uintptr_t value,
                                                     GblArrayMapDestructFn pDtor)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblArrayMap_setVariant      (SELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)          GBL_NOEXCEPT;

GBL_EXPORT GblSize      GblArrayMap_insertUserdata  (SELF,
                                                     uintptr_t key,
                                                     uintptr_t value,
                                                     GblArrayMapDestructFn pDtor)   GBL_NOEXCEPT;

GBL_EXPORT GblSize      GblArrayMap_insertVariant   (SELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)          GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblArrayMap_erase           (SELF, uintptr_t key)           GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblArrayMap_extractVariant  (SELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)          GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblArrayMap_extractValue    (SELF,
                                                     uintptr_t key,
                                                     uintptr_t* pValue)             GBL_NOEXCEPT;

GBL_EXPORT void         GblArrayMap_clear           (SELF)                          GBL_NOEXCEPT;

GBL_INLINE GblSize      GblArrayMap_find            (CSELF, uintptr_t key)          GBL_NOEXCEPT;
GBL_INLINE uintptr_t    GblArrayMap_probeKey        (CSELF, GblSize index)          GBL_NOEXCEPT;
GBL_INLINE uintptr_t    GblArrayMap_probeValue      (CSELF, GblSize index)          GBL_NOEXCEPT;
GBL_INLINE GblVariant*  GblArrayMap_probeVariant    (CSELF, GblSize index)          GBL_NOEXCEPT;

//========== IMPL ==========

GBL_INLINE GblArrayMapEntry* GblArrayMap_entry_(CSELF, GblSize index) GBL_NOEXCEPT {
    return (*ppSelf && index < GblArrayMap_size(ppSelf))?
            (GblArrayMapEntry*)((uint8_t*)*ppSelf + sizeof(GblArrayMap) + sizeof(GblArrayMapEntry)*index) :
            GBL_NULL;
}

GBL_INLINE GblArrayMapEntry* GblArrayMap_find_entry_(CSELF, uintptr_t key) GBL_NOEXCEPT {
    GblSize index = GblArrayMap_find(ppSelf, key);
    return (index != GBL_ARRAY_MAP_NPOS)?
                GblArrayMap_entry_(ppSelf, index) :
                GBL_NULL;
}

GBL_INLINE uintptr_t GblArrayMap_valueFromVariant_(GblVariant* pVariant) GBL_NOEXCEPT {
    uintptr_t value = 0;
    if(pVariant) {
        if(pVariant->type == GBL_INVALID_TYPE)
            value = (uintptr_t)pVariant->pVoid;
        else
           GblVariant_getValuePeek(pVariant, &value);
    }
    return value;
}

GBL_INLINE GblSize GblArrayMap_size(CSELF) GBL_NOEXCEPT {
    return *ppSelf? (*ppSelf)->size : 0;
}

GBL_INLINE GblContext* GblArrayMap_context(CSELF) GBL_NOEXCEPT {
    return *ppSelf? (*ppSelf)->pCtx : 0;
}

GBL_INLINE GblBool GblArrayMap_empty(CSELF) GBL_NOEXCEPT {
    return *ppSelf && (*ppSelf)->size? GBL_FALSE : GBL_TRUE;
}

GBL_INLINE GblBool GblArrayMap_contains(CSELF, uintptr_t key) GBL_NOEXCEPT {
    return GblArrayMap_find(ppSelf, key) != GBL_ARRAY_MAP_NPOS;
}

GBL_INLINE GblBool GblArrayMap_containsUserdata(CSELF, uintptr_t key) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry) {
        if(GblVariant_typeOf(&pEntry->value) == GBL_INVALID_TYPE)
            return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_INLINE GblBool GblArrayMap_containsVariant(CSELF, uintptr_t key) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry) {
        if(GblVariant_typeOf(&pEntry->value) != GBL_INVALID_TYPE)
            return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_INLINE uintptr_t GblArrayMap_getValue(CSELF, uintptr_t key) GBL_NOEXCEPT {
    uintptr_t value = 0;
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry) {
        value = GblArrayMap_valueFromVariant_(&pEntry->value);
    }
    return value;
}

GBL_INLINE GblVariant* GblArrayMap_getVariant(CSELF, uintptr_t key) GBL_NOEXCEPT {
    GblVariant* pVariant = GBL_NULL;
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry && pEntry->value.type != GBL_INVALID_TYPE) {
       pVariant = &pEntry->value;
    }
    return pVariant;
}


GBL_INLINE uintptr_t GblArrayMap_atValue(CSELF, uintptr_t key) GBL_NOEXCEPT {
    uintptr_t value = 0;
    GBL_API_BEGIN(NULL);
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    GBL_API_VERIFY(pEntry,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    value = GblArrayMap_valueFromVariant_(&pEntry->value);
    GBL_API_END_BLOCK();
    return value;
}

GBL_INLINE GblVariant* GblArrayMap_atVariant(CSELF, uintptr_t key) GBL_NOEXCEPT {
    GblVariant* pVariant = 0;
    GBL_API_BEGIN(NULL);
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    GBL_API_VERIFY(pEntry,
                   GBL_RESULT_ERROR_OUT_OF_RANGE);
    if(pEntry->value.type != GBL_INVALID_TYPE)
        pVariant = &pEntry->value;
    GBL_API_END_BLOCK();
    return pVariant;
}

GBL_INLINE GblSize GblArrayMap_find(CSELF, uintptr_t key) GBL_NOEXCEPT {
    GblSize index = GBL_ARRAY_MAP_NPOS;
    if(*ppSelf) {
        const GblSize size = GblArrayMap_size(ppSelf);
        for(GblSize e = 0; e < size; ++e) {
            const GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, e);
            if((*ppSelf)->pFnComparator) {
                if((*ppSelf)->pFnComparator(key, pEntry->key) == 0) {
                    index = e;
                    break;
                }
            } else if(key == pEntry->key) {
                index = e;
                break;
            }
        }
    }
    return index;
}

GBL_INLINE uintptr_t GblArrayMap_probeKey(CSELF, GblSize index) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    return pEntry? pEntry->key : 0;
}

GBL_INLINE GblVariant* GblArrayMap_probeVariant(CSELF, GblSize index) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    return pEntry && pEntry->value.type != GBL_INVALID_TYPE? &pEntry->value : GBL_NULL;
}


GBL_INLINE uintptr_t GblArrayMap_probeValue(CSELF, GblSize index) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    return pEntry? GblArrayMap_valueFromVariant_(&pEntry->value) : 0;
}




#undef CSELF
#undef SELF

#endif // GIMBAL_ARRAY_MAP_HPP
