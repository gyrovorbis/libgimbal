#ifndef GIMBAL_ARRAY_MAP_H
#define GIMBAL_ARRAY_MAP_H

#include "../types/gimbal_typedefs.h"
#include "../types/gimbal_quark.h"
#include "../types/gimbal_variant.h"
#include "../types/gimbal_result.h"

#define GBL_ARRAY_MAP_NPOS             ((GblSize)-1)
#define GBL_ARRAY_MAP_SIZE(elements)   (sizeof(GblArrayMap) + (sizeof(GblArrayMapEntry)*(elements)))

#define SELF    GblArrayMap** ppSelf
#define CSELF   GblArrayMap*const* ppSelf

typedef GBL_RESULT  (*GblArrayMapDestructFn)(void* pValue);
typedef GblBool     (*GblArrayMapCompareFn) (uintptr_t key1, uintptr_t key2);

typedef struct GblArrayMapEntry {
    uintptr_t                   key;
    GblVariant                  value; // using value.type == GBL_INVALID_TYPE to signify userdata
    GblArrayMapDestructFn       dtor;
} GblArrayMapEntry;

typedef struct GblArrayMap {
    GblContext*             pCtx;
    GblArrayMapCompareFn    pFnComparator;
    GblSize                 size;
} GblArrayMap;

GBL_INLINE GblArrayMap* GblArrayMap_create          (GblArrayMapCompareFn pFnComp,
                                                     GblContext*          pCtx)      GBL_NOEXCEPT;
GBL_INLINE GblArrayMap* GblArrayMap_clone           (CSELF)                          GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblArrayMap_destroy         (SELF)                           GBL_NOEXCEPT;

GBL_INLINE GblSize      GblArrayMap_size            (CSELF)                          GBL_NOEXCEPT;
GBL_INLINE GblContext*  GblArrayMap_context         (CSELF)                          GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayMap_empty           (CSELF)                          GBL_NOEXCEPT;

GBL_INLINE GblBool      GblArrayMap_contains        (CSELF, uintptr_t key)           GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayMap_containsUserdata(CSELF, uintptr_t key)           GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayMap_containsVariant (CSELF, uintptr_t key)           GBL_NOEXCEPT;

GBL_INLINE uintptr_t    GblArrayMap_getValue        (CSELF, uintptr_t key)           GBL_NOEXCEPT;
GBL_INLINE GblVariant*  GblArrayMap_getVariant      (CSELF, uintptr_t key)           GBL_NOEXCEPT;
GBL_INLINE uintptr_t    GblArrayMap_atValue         (CSELF, uintptr_t key)           GBL_NOEXCEPT;
GBL_INLINE GblVariant*  GblArrayMap_atVariant       (CSELF, uintptr_t key)           GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblArrayMap_setUserdata     (SELF,
                                                     uintptr_t key,
                                                     uintptr_t value,
                                                     GblArrayMapDestructFn pDtor)    GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblArrayMap_setVariant      (SELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)           GBL_NOEXCEPT;

GBL_INLINE GblSize      GblArrayMap_insertUserdata  (SELF,
                                                     uintptr_t key,
                                                     uintptr_t value,
                                                     GblArrayMapDestructFn pDtor)    GBL_NOEXCEPT;

GBL_INLINE GblSize      GblArrayMap_insertVariant   (SELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)           GBL_NOEXCEPT;

GBL_INLINE GblBool      GblArrayMap_erase           (SELF, uintptr_t key)            GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayMap_extractVariant  (SELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)           GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayMap_extractValue    (SELF,
                                                     uintptr_t key,
                                                     uintptr_t* pValue)              GBL_NOEXCEPT;
GBL_INLINE void         GblArrayMap_clear           (SELF)                           GBL_NOEXCEPT;

GBL_INLINE GblSize      GblArrayMap_find            (CSELF, uintptr_t key)           GBL_NOEXCEPT;
GBL_INLINE uintptr_t    GblArrayMap_probeKey        (CSELF, GblSize index)           GBL_NOEXCEPT;
GBL_INLINE uintptr_t    GblArrayMap_probeValue      (CSELF, GblSize index)           GBL_NOEXCEPT;
GBL_INLINE GblVariant*  GblArrayMap_probeVariant    (CSELF, GblSize index)           GBL_NOEXCEPT;

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

GBL_INLINE GBL_RESULT GblArrayMap_entryDestruct_(GblArrayMapEntry* pEntry) GBL_NOEXCEPT {
    if(pEntry->dtor) {
        return pEntry->dtor(pEntry->value.pVoid);
    } else if(GblVariant_type(&pEntry->value) != GBL_INVALID_TYPE) {
        return GblVariant_destruct(&pEntry->value);
    } else return GBL_RESULT_SUCCESS;
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

GBL_INLINE GblArrayMap* GblArrayMap_create(GblArrayMapCompareFn   pFnComparator,
                                           GblContext*            pCtx) GBL_NOEXCEPT
{
    GblArrayMap* pMap = GBL_NULL;
    GBL_API_BEGIN(pCtx);
    pMap = GBL_API_MALLOC(sizeof(GblArrayMap));
    pMap->pCtx = pCtx;
    pMap->pFnComparator = pFnComparator;
    pMap->size = 0;
    GBL_API_END_BLOCK();
    return pMap;
}

GBL_INLINE GblArrayMap* GblArrayMap_clone(CSELF) GBL_NOEXCEPT {
    GblArrayMap* pMap = GBL_NULL;
    if(*ppSelf) {
        GBL_API_BEGIN((*ppSelf)->pCtx);
        const GblSize bytes = GBL_ARRAY_MAP_SIZE((*ppSelf)->size);
        GBL_API_MALLOC(bytes);
        memcpy(pMap, *ppSelf, bytes);
        GBL_API_END_BLOCK();
    }
    return pMap;
}

GBL_INLINE GBL_RESULT GblArrayMap_destroy(SELF) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblArrayMap* pSelf = *ppSelf;
    if(pSelf) {
        for(GblSize e = 0; e < pSelf->size; ++e) {
            GBL_API_CALL(GblArrayMap_entryDestruct_(GblArrayMap_entry_(ppSelf, e)));
        }
        GBL_API_FREE(pSelf);
        *ppSelf = NULL;
    }
    GBL_API_END();
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
        if(GblVariant_type(&pEntry->value) == GBL_INVALID_TYPE)
            return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_INLINE GblBool GblArrayMap_containsVariant(CSELF, uintptr_t key) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry) {
        if(GblVariant_type(&pEntry->value) != GBL_INVALID_TYPE)
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
    if(pEntry) {
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
    pVariant = &pEntry->value;
    GBL_API_END_BLOCK();
    return pVariant;
}

GBL_INLINE GblArrayMapEntry* GblArrayMap_entryAdd_(SELF, uintptr_t key) {
    GblArrayMapEntry* pEntry = NULL;
    GBL_API_BEGIN(NULL);
    if(!*ppSelf) {
        *ppSelf = GBL_API_MALLOC(GBL_ARRAY_MAP_SIZE(1));
        (*ppSelf)->pCtx             = GBL_NULL;
        (*ppSelf)->pFnComparator    = GBL_NULL;
        (*ppSelf)->size             = 0;

    }  else {
        *ppSelf = (GblArrayMap*)GBL_API_REALLOC(*ppSelf, GBL_ARRAY_MAP_SIZE(GblArrayMap_size(ppSelf) + 1));
    }
    pEntry = GblArrayMap_entry_(ppSelf, GblArrayMap_size(ppSelf));
    memset(pEntry, 0, sizeof(GblArrayMapEntry));
    pEntry->key = key;
    ++(*ppSelf)->size;
    GBL_API_END_BLOCK();
    return pEntry;
}

GBL_INLINE GBL_RESULT GblArrayMap_entrySetVariant_(GblArrayMapEntry* pEntry, GblVariant* pVariant) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    if(pEntry->dtor && GblVariant_type(&pEntry->value) != GblVariant_type(pVariant)) {
        GBL_API_CALL(pEntry->dtor(pVariant->pVoid));
        pEntry->dtor = NULL;
        GBL_API_CALL(GblVariant_constructCopy(&pEntry->value, pVariant));
    } else {
        GBL_API_CALL(GblVariant_setCopy(&pEntry->value, pVariant));
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblArrayMap_setVariant(SELF, uintptr_t key, GblVariant* pVariant) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(!pEntry) {
        pEntry = GblArrayMap_entryAdd_(ppSelf, key);
        GBL_API_CALL(GblVariant_constructCopy(&pEntry->value, pVariant));
    } else {
        GBL_API_CALL(GblArrayMap_entrySetVariant_(pEntry, pVariant));
    }
    GBL_API_END();
}

GBL_INLINE GBL_RESULT GblArrayMap_entrySetUserdata_(GblArrayMapEntry* pEntry, uintptr_t value, GblArrayMapDestructFn pDtor) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    if(pEntry->dtor) {
       GBL_API_CALL(pEntry->dtor(pEntry));
   } else if(GblVariant_type(&pEntry->value) != GBL_INVALID_TYPE) {
       GBL_API_CALL(GblVariant_destruct(&pEntry->value));
   }
   pEntry->value.pVoid = (void*)value;
   pEntry->value.type = GBL_INVALID_TYPE;
   pEntry->dtor = pDtor;
   GBL_API_END();
}

GBL_INLINE GBL_RESULT GblArrayMap_setUserdata(SELF, uintptr_t key, uintptr_t value, GblArrayMapDestructFn pDtor) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(!pEntry) {
        pEntry = GblArrayMap_entryAdd_(ppSelf, key);
    }
    GBL_API_CALL(GblArrayMap_entrySetUserdata_(pEntry, value, pDtor));
    GBL_API_END();
}

GBL_INLINE GblSize GblArrayMap_insertVariant(SELF, uintptr_t key, GblVariant* pVariant) GBL_NOEXCEPT {
    GblSize index = GblArrayMap_find(ppSelf, key);
    if(index == GBL_ARRAY_MAP_NPOS) {
        GBL_API_BEGIN(NULL);
        GblArrayMapEntry* pEntry = GblArrayMap_entryAdd_(ppSelf, key);
        GBL_API_CALL(GblVariant_constructCopy(&pEntry->value, pVariant));
        index = GblArrayMap_size(ppSelf)-1;
        GBL_API_END_BLOCK();
    }
    return index;
}

GBL_INLINE GblSize GblArrayMap_insertUserdata(SELF, uintptr_t key, uintptr_t value, GblArrayMapDestructFn pDtor) GBL_NOEXCEPT {
    GblSize index = GblArrayMap_find(ppSelf, key);
    if(index == GBL_ARRAY_MAP_NPOS) {
        GBL_API_BEGIN(NULL);
        GblArrayMapEntry* pEntry = GblArrayMap_entryAdd_(ppSelf, key);
        GBL_API_CALL(GblArrayMap_entrySetUserdata_(pEntry, value, pDtor));
        index = GblArrayMap_size(ppSelf)-1;
        GBL_API_END_BLOCK();
    }
    return index;
}

GBL_INLINE GblBool GblArrayMap_erase_(SELF, uintptr_t key, GblBool free) GBL_NOEXCEPT {
    GblBool removed = GBL_FALSE;
    GBL_API_BEGIN(NULL);
    if(*ppSelf) {
        GblSize index = GblArrayMap_find(ppSelf, key);
        if(index != GBL_ARRAY_MAP_NPOS) {
            if(free)
                GBL_API_CALL(GblArrayMap_entryDestruct_(GblArrayMap_entry_(ppSelf, index)));
            if(GblArrayMap_size(ppSelf) == 1) {
                GblArrayMap_clear(ppSelf);
                removed = GBL_TRUE;
            } else {
                GblSize remainder = GblArrayMap_size(ppSelf)-1 - index;
                if(remainder)
                    memmove(GblArrayMap_entry_(ppSelf, index),
                            GblArrayMap_entry_(ppSelf, index+1), remainder * sizeof(GblArrayMapEntry));
                *ppSelf = GBL_API_REALLOC(*ppSelf, GBL_ARRAY_MAP_SIZE(GblArrayMap_size(ppSelf)-1));
                --(*ppSelf)->size;
            }
        }
    }
    GBL_API_END_BLOCK();
    return removed;
}

GBL_INLINE GblBool GblArrayMap_erase(SELF, uintptr_t key) GBL_NOEXCEPT {
    return GblArrayMap_erase_(ppSelf, key, GBL_TRUE);
}

GBL_INLINE GblBool GblArrayMap_extractVariant(SELF, uintptr_t key, GblVariant* pVariant) GBL_NOEXCEPT {
    GblBool found = GBL_TRUE;
    const GblSize index = GblArrayMap_find(ppSelf, key);
    if(index != GBL_ARRAY_MAP_NPOS) {
        GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
        if(pEntry) {
            found = GBL_TRUE;
            memcpy(pVariant, &pEntry->value, sizeof(GblVariant));
            GblArrayMap_erase_(ppSelf, index, GBL_FALSE);
        }
    }
    return found;
}

GBL_INLINE GblBool GblArrayMap_extractValue(SELF, uintptr_t key, uintptr_t* pValue) GBL_NOEXCEPT {
    GblBool found = GBL_TRUE;
    const GblSize index = GblArrayMap_find(ppSelf, key);
    if(index != GBL_ARRAY_MAP_NPOS) {
        GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
        if(pEntry) {
            found = GBL_TRUE;
            *pValue = GblArrayMap_valueFromVariant_(&pEntry->value);
            GblArrayMap_erase_(ppSelf, index, GBL_FALSE);
        }
    }
    return found;
}

GBL_INLINE void GblArrayMap_clear(SELF) {
    if(*ppSelf) {
        GBL_API_BEGIN(NULL);
        const GblSize size = GblArrayMap_size(ppSelf);
        for(GblSize e = 0; e < size; ++e)
            GblArrayMap_entryDestruct_(GblArrayMap_entry_(ppSelf, e));

        if((*ppSelf)->pCtx || (*ppSelf)->pFnComparator) {
            *ppSelf = GBL_API_REALLOC(*ppSelf, sizeof(GblArrayMap));
            (*ppSelf)->size = 0;
        } else {
            GBL_API_FREE(*ppSelf);
            *ppSelf = GBL_NULL;
        }
        GBL_API_END_BLOCK();
    }
}

GBL_INLINE GblSize GblArrayMap_find(CSELF, uintptr_t key) GBL_NOEXCEPT {
    GblSize index = GBL_ARRAY_MAP_NPOS;
    if(*ppSelf) {
        const GblSize size = GblArrayMap_size(ppSelf);
        for(GblSize e = 0; e < size; ++e) {
            const GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
            if((*ppSelf)->pFnComparator) {
                if((*ppSelf)->pFnComparator(key, pEntry->key)) {
                    index = e;
                    break;
                } else if(key == pEntry->key) {
                    index = e;
                    break;
                }
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
    return pEntry? &pEntry->value : GBL_NULL;
}


GBL_INLINE uintptr_t GblArrayMap_probeValue(CSELF, GblSize index) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    return pEntry? GblArrayMap_valueFromVariant_(&pEntry->value) : 0;
}




#undef CSELF
#undef SELF

#endif // GIMBAL_ARRAY_MAP_HPP
