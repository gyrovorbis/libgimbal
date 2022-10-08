#include <gimbal/containers/gimbal_array_map.h>


GBL_EXPORT GblArrayMap* GblArrayMap_create(GblArrayMapCmpFn   pFnComparator,
                                           GblBool            binarySearches,
                                           GblContext*        pCtx) GBL_NOEXCEPT
{
    GblArrayMap* pMap = GBL_NULL;
    GBL_CTX_BEGIN(pCtx);
    pMap = GBL_CTX_MALLOC(sizeof(GblArrayMap));
    GBL_PRIV_REF(pMap).pCtx = pCtx;
    GBL_PRIV_REF(pMap).pFnComparator = pFnComparator;
    GBL_PRIV_REF(pMap).binarySearches = binarySearches;
    GBL_PRIV_REF(pMap).size = 0;
    GBL_CTX_END_BLOCK();
    return pMap;
}
/* THIS NEEDS MORE WORK. Either no, because userdata, or at least COPY variants gracefully!
GBL_EXPORT GblArrayMap* GblArrayMap_clone(GblArrayMap*const* ppSelf) GBL_NOEXCEPT {
    GblArrayMap* pMap = GBL_NULL;
    if(*ppSelf) {
        GBL_CTX_BEGIN((*ppSelf)->pCtx);
        const GblSize bytes = GBL_ARRAY_MAP_SIZE((*ppSelf)->size);
        pMap =GBL_CTX_MALLOC(bytes);
        memcpy(pMap, *ppSelf, bytes);
        GBL_CTX_END_BLOCK();
    }
    return pMap;
}
*/

GBL_EXPORT GBL_RESULT GblArrayMap_destroy(GblArrayMap** ppSelf) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL);
    GblArrayMap* pSelf = *ppSelf;
    if(pSelf) {
        for(GblSize e = 0; e < GBL_PRIV_REF(pSelf).size; ++e) {
            GBL_CTX_CALL(GblArrayMap_entryDestruct_(ppSelf, GblArrayMap_entry_(ppSelf, e)));
        }
        GBL_CTX_FREE(pSelf);
        *ppSelf = NULL;
    }
    GBL_CTX_END();
}

static GblBool GblArrayMap_erase_(GblArrayMap** ppSelf, uintptr_t key, GblBool free) GBL_NOEXCEPT {
    GblBool removed = GBL_FALSE;
    GBL_CTX_BEGIN(NULL);

    if(*ppSelf) {
        GblSize index = GblArrayMap_find(ppSelf, key);

        if(index != GBL_ARRAY_MAP_NPOS) {
            if(free)
                GBL_CTX_CALL(GblArrayMap_entryDestruct_(ppSelf, GblArrayMap_entry_(ppSelf, index)));

            if(GblArrayMap_size(ppSelf) == 1) {
                GblArrayMap_clear(ppSelf);
                removed = GBL_TRUE;

            } else {
                GblSize remainder = GblArrayMap_size(ppSelf)-1 - index;

                if(remainder)
                    memmove(GblArrayMap_entry_(ppSelf, index),
                            GblArrayMap_entry_(ppSelf, index+1), remainder * sizeof(GblArrayMapEntry));

                *ppSelf = GBL_CTX_REALLOC(*ppSelf, GBL_ARRAY_MAP_SIZE(GblArrayMap_size(ppSelf)-1));
                --GBL_PRIV_REF(*ppSelf).size;
                removed = GBL_TRUE;
            }
        }
    }
    GBL_CTX_END_BLOCK();
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
        GBL_CTX_BEGIN(NULL);
        const GblSize size = GblArrayMap_size(ppSelf);
        for(GblSize e = 0; e < size; ++e)
            GblArrayMap_entryDestruct_(ppSelf, GblArrayMap_entry_(ppSelf, e));

        if(GBL_PRIV_REF(*ppSelf).pCtx || GBL_PRIV_REF(*ppSelf).pFnComparator) {
            *ppSelf = GBL_CTX_REALLOC(*ppSelf, sizeof(GblArrayMap));
            GBL_PRIV_REF(*ppSelf).size = 0;
        } else {
            GBL_CTX_FREE(*ppSelf);
            *ppSelf = GBL_NULL;
        }
        GBL_CTX_END_BLOCK();
    }
}
