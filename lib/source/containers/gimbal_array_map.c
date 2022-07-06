#include <gimbal/containers/gimbal_array_map.h>


GBL_EXPORT GblArrayMap* GblArrayMap_create(GblArrayMapCompareFn   pFnComparator,
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
/* THIS NEEDS MORE WORK. Either no, because userdata, or at least COPY variants gracefully!
GBL_EXPORT GblArrayMap* GblArrayMap_clone(GblArrayMap*const* ppSelf) GBL_NOEXCEPT {
    GblArrayMap* pMap = GBL_NULL;
    if(*ppSelf) {
        GBL_API_BEGIN((*ppSelf)->pCtx);
        const GblSize bytes = GBL_ARRAY_MAP_SIZE((*ppSelf)->size);
        pMap =GBL_API_MALLOC(bytes);
        memcpy(pMap, *ppSelf, bytes);
        GBL_API_END_BLOCK();
    }
    return pMap;
}
*/
GBL_EXPORT GBL_RESULT GblArrayMap_destroy(GblArrayMap** ppSelf) GBL_NOEXCEPT {
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

static GblBool GblArrayMap_erase_(GblArrayMap** ppSelf, uintptr_t key, GblBool free) GBL_NOEXCEPT {
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
                removed = GBL_TRUE;
            }
        }
    }
    GBL_API_END_BLOCK();
    return removed;
}

GBL_EXPORT GblBool GblArrayMap_erase(GblArrayMap** ppSelf, uintptr_t key) GBL_NOEXCEPT {
    return GblArrayMap_erase_(ppSelf, key, GBL_TRUE);
}

GBL_EXPORT GblBool GblArrayMap_extractVariant(GblArrayMap** ppSelf, uintptr_t key, GblVariant* pVariant) GBL_NOEXCEPT {
    GblBool found = GBL_FALSE;
    const GblSize index = GblArrayMap_find(ppSelf, key);
    if(index != GBL_ARRAY_MAP_NPOS) {
        GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
        if(pEntry) {
            found = GBL_TRUE;
            memcpy(pVariant, &pEntry->value, sizeof(GblVariant));
            GblArrayMap_erase_(ppSelf, key, GBL_FALSE);
        }
    }
    return found;
}

GBL_EXPORT GblBool GblArrayMap_extractValue(GblArrayMap**ppSelf, uintptr_t key, uintptr_t* pValue) GBL_NOEXCEPT {
    GblBool found = GBL_FALSE;
    const GblSize index = GblArrayMap_find(ppSelf, key);
    if(index != GBL_ARRAY_MAP_NPOS) {
        GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
        if(pEntry) {
            found = GBL_TRUE;
            *pValue = GblArrayMap_valueFromVariant_(&pEntry->value);
            GblArrayMap_erase_(ppSelf, key, GBL_FALSE);
        }
    }
    return found;
}

GBL_EXPORT void GblArrayMap_clear(GblArrayMap** ppSelf) {
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


static GblArrayMapEntry* GblArrayMap_entryAdd_(GblArrayMap** ppSelf, uintptr_t key) {
    GblArrayMapEntry* pEntry = NULL;
    GBL_API_BEGIN(NULL);
    if(!*ppSelf) {
        *ppSelf = GBL_API_MALLOC(GBL_ARRAY_MAP_SIZE(1));
        memset(*ppSelf, 0, sizeof(GblArrayMap));
    }  else {
        *ppSelf = (GblArrayMap*)GBL_API_REALLOC(*ppSelf, GBL_ARRAY_MAP_SIZE(GblArrayMap_size(ppSelf) + 1));
    }
    ++(*ppSelf)->size;
    pEntry = GblArrayMap_entry_(ppSelf, GblArrayMap_size(ppSelf)-1);
    memset(pEntry, 0, sizeof(GblArrayMapEntry));
    pEntry->key = key;
    GBL_API_END_BLOCK();
    return pEntry;
}

static GBL_RESULT GblArrayMap_entrySetVariant_(GblArrayMapEntry* pEntry, GblVariant* pVariant) GBL_NOEXCEPT {
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

GBL_EXPORT GBL_RESULT GblArrayMap_setVariant(GblArrayMap** ppSelf, uintptr_t key, GblVariant* pVariant) GBL_NOEXCEPT {
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

static GBL_RESULT GblArrayMap_entrySetUserdata_(GblArrayMapEntry* pEntry, uintptr_t value, GblArrayMapDestructFn pDtor) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    if(pEntry->dtor) {
       GBL_API_CALL(pEntry->dtor(pEntry->value.pVoid));
   } else if(GblVariant_type(&pEntry->value) != GBL_INVALID_TYPE) {
       GBL_API_CALL(GblVariant_destruct(&pEntry->value));
   }
   pEntry->value.pVoid = (void*)value;
   pEntry->value.type = GBL_INVALID_TYPE;
   pEntry->dtor = pDtor;
   GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblArrayMap_setUserdata(GblArrayMap** ppSelf, uintptr_t key, uintptr_t value, GblArrayMapDestructFn pDtor) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL);
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(!pEntry) {
        pEntry = GblArrayMap_entryAdd_(ppSelf, key);
    }
    GBL_API_CALL(GblArrayMap_entrySetUserdata_(pEntry, value, pDtor));
    GBL_API_END();
}

GBL_EXPORT GblSize GblArrayMap_insertVariant(GblArrayMap** ppSelf, uintptr_t key, GblVariant* pVariant) GBL_NOEXCEPT {
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

GBL_EXPORT GblSize GblArrayMap_insertUserdata(GblArrayMap** ppSelf, uintptr_t key, uintptr_t value, GblArrayMapDestructFn pDtor) GBL_NOEXCEPT {
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
