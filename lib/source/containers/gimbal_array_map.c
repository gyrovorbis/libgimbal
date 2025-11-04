#include <gimbal/containers/gimbal_array_map.h>

static GblArrayMapEntry* GblArrayMap_entry_(GblArrayMap*const* ppSelf, size_t index) {
    return (*ppSelf && index < GblArrayMap_size(ppSelf))?
            (GblArrayMapEntry*)((uint8_t*)*ppSelf + sizeof(GblArrayMap) + sizeof(GblArrayMapEntry)*index) :
            GBL_NULL;
}

static GblArrayMapEntry* GblArrayMap_find_entry_(GblArrayMap*const* ppSelf, uintptr_t key) {
    const size_t  index = GblArrayMap_find(ppSelf, key);
    return (index != GBL_ARRAY_MAP_NPOS)?
                GblArrayMap_entry_(ppSelf, index) :
                GBL_NULL;
}

static uintptr_t GblArrayMap_valueFromVariant_(GblVariant* pVariant) {
    uintptr_t value = 0;
    if(pVariant) {
        if(pVariant->type == GBL_INVALID_TYPE)
            value = (uintptr_t)pVariant->pVoid;
        else
           GblVariant_valuePeek(pVariant, &value);
    }
    return value;
}

static ptrdiff_t GblArrayMap_diffEntry_(GblArrayMap*const* ppSelf, size_t index, uintptr_t key) {
    ptrdiff_t diff = GBL_ARRAY_MAP_NPOS;
    const GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    if(GBL_PRIV_REF(*ppSelf).pFnComparator) {
        diff = GBL_PRIV_REF(*ppSelf).pFnComparator(*ppSelf, pEntry->key, key);
    } else {
        diff = pEntry->key - key;
    }
    return diff;
}

static GBL_RESULT GblArrayMap_entryDestruct_(GblArrayMap*const* ppSelf, GblArrayMapEntry* pEntry) {
    if(pEntry->dtor) {
        return pEntry->dtor(*ppSelf, pEntry->key, pEntry->value.pVoid);
    } else if(GblVariant_typeOf(&pEntry->value) != GBL_INVALID_TYPE) {
        return GblVariant_destruct(&pEntry->value);
    } else return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblArrayMap_entrySetVariant_(GblArrayMap*const* ppSelf, GblArrayMapEntry* pEntry, GblVariant* pVariant) {
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    if(GblVariant_typeOf(&pEntry->value) != GblVariant_typeOf(pVariant)) {
        result = GblArrayMap_entryDestruct_(ppSelf, pEntry);
        pEntry->dtor = NULL;
        result = GblVariant_constructCopy(&pEntry->value, pVariant);
    } else {
        result = GblVariant_setCopy(&pEntry->value, pVariant);
    }
    return result;
}

static GblArrayMapEntry* GblArrayMap_entryAdd_(GblArrayMap** ppSelf, uintptr_t key) {
    GblArrayMapEntry* pEntry = NULL;
    GBL_CTX_BEGIN(NULL);
    const size_t size = GblArrayMap_size(ppSelf);

    // Have to insert in sorted order for Nth entry when binary searchable
    if(*ppSelf && size && GblArrayMap_binarySearches(ppSelf)) {
        size_t insertionIdx = size;

        // If we have a little array that's fast linearly
        if(size < GBL_ARRAY_MAP_BINARY_SEARCH_CUTOFF_SIZE) {
            // iterate (linearly) to find insertion point
            for(size_t  i = 0; i < size; ++i) {
                // insertion point is less than or equal to next
                if(GblArrayMap_diffEntry_(ppSelf, i, key) >= 0) {
                    insertionIdx = i;
                    break;
                }
            }
        // Time to bust out the big guns and search intelligently
        } else {

            ptrdiff_t upper = size - 1;
            ptrdiff_t lower = 0;
            GblArrayMap* pSelf = *ppSelf;
            GblBool found = GBL_FALSE;

            const GblArrayMapEntry* pEntryBegin = GblArrayMap_entry_(ppSelf, 0);

            do {
                const size_t  mid = lower + ((upper-lower) >> 1);
                const GblArrayMapEntry* pEntry = pEntryBegin + mid;
                const ptrdiff_t diff = GBL_PRIV_REF(pSelf).pFnComparator?
                                            GBL_PRIV_REF(pSelf).pFnComparator(pSelf, pEntry->key, key) :
                                            pEntry->key - key;
                if(diff > 0) {
                    upper = mid-1;
                } else if(diff < 0) {
                    lower = mid+1;
                } else {
                    insertionIdx = mid;
                    found = GBL_TRUE;
                    break;
                }
            } while(lower <= upper);

            if(!found) insertionIdx = lower;
        }

        // resize array and initialize entry to point to new position
        *ppSelf = (GblArrayMap*)GBL_CTX_REALLOC(*ppSelf, GBL_ARRAY_MAP_SIZE(size + 1));
        ++GBL_PRIV_REF(*ppSelf).size;
        pEntry = GblArrayMap_entry_(ppSelf, insertionIdx);

        // calculate slide adjustments for the rest of the items
        const size_t slideSize = (size - insertionIdx);
        if(slideSize) {
            // scooch the bitches over by one
            memmove(pEntry + 1,
                    pEntry,
                    sizeof(GblArrayMapEntry) * slideSize);
        }

    // regular-ass unordered insertion at the end for unsorted or initial entry
    } else {
        // initial allocation
        if GBL_UNLIKELY(!*ppSelf) {
            *ppSelf = (GblArrayMap*)GBL_CTX_MALLOC(GBL_ARRAY_MAP_SIZE(1));
            memset(*ppSelf, 0, sizeof(GblArrayMap));
        //appending to the end
        } else {
            *ppSelf = (GblArrayMap*)GBL_CTX_REALLOC(*ppSelf, GBL_ARRAY_MAP_SIZE(size + 1));
        }
        ++GBL_PRIV_REF(*ppSelf).size;
        pEntry = GblArrayMap_entry_(ppSelf, GblArrayMap_size(ppSelf)-1);
    }

    // initialize new entry
    assert(pEntry);
    if(pEntry) {
        memset(pEntry, 0, sizeof(GblArrayMapEntry));
        pEntry->key = key;
    }

    // return new entry
    GBL_CTX_END_BLOCK();
    return pEntry;
}

static GBL_RESULT GblArrayMap_entrySetUserdata_(GblArrayMap*const* ppSelf, GblArrayMapEntry* pEntry, uintptr_t value, GblArrayMapDtorFn pDtor) GBL_NOEXCEPT {
    GBL_RESULT result = GblArrayMap_entryDestruct_(ppSelf, pEntry);
    pEntry->value.pVoid = (void*)value;
    pEntry->value.type = GBL_INVALID_TYPE;
    pEntry->dtor = pDtor;
    return result;
}

GBL_EXPORT size_t  GblArrayMap_size(GblArrayMap*const* ppSelf) {
    return *ppSelf? GBL_PRIV_REF(*ppSelf).size : 0;
}

GBL_EXPORT GblContext* GblArrayMap_context(GblArrayMap*const* ppSelf){
    return *ppSelf? GBL_PRIV_REF(*ppSelf).pCtx : 0;
}

GBL_EXPORT GblBool GblArrayMap_empty(GblArrayMap*const* ppSelf) {
    return *ppSelf && GBL_PRIV_REF(*ppSelf).size? GBL_FALSE : GBL_TRUE;
}

GBL_EXPORT GblBool GblArrayMap_binarySearches(GblArrayMap*const* ppSelf) {
    return *ppSelf && GBL_PRIV_REF(*ppSelf).binarySearches? GBL_TRUE : GBL_FALSE;
}

GBL_EXPORT GblBool GblArrayMap_contains(GblArrayMap*const* ppSelf, uintptr_t key) {
    return GblArrayMap_find(ppSelf, key) != GBL_ARRAY_MAP_NPOS;
}

GBL_EXPORT GblBool GblArrayMap_containsUserdata(GblArrayMap*const* ppSelf, uintptr_t key){
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry) {
        if(GblVariant_typeOf(&pEntry->value) == GBL_INVALID_TYPE)
            return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_EXPORT GblBool GblArrayMap_containsVariant(GblArrayMap*const* ppSelf, uintptr_t key) {
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry) {
        if(GblVariant_typeOf(&pEntry->value) != GBL_INVALID_TYPE)
            return GBL_TRUE;
    }
    return GBL_FALSE;
}

GBL_EXPORT uintptr_t GblArrayMap_getValue(GblArrayMap*const* ppSelf, uintptr_t key) {
    uintptr_t value = 0;
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry) {
        value = GblArrayMap_valueFromVariant_(&pEntry->value);
    }
    return value;
}

GBL_EXPORT GblVariant* GblArrayMap_getVariant(GblArrayMap*const* ppSelf, uintptr_t key) {
    GblVariant* pVariant = GBL_NULL;
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry && pEntry->value.type != GBL_INVALID_TYPE) {
       pVariant = &pEntry->value;
    }
    return pVariant;
}

GBL_EXPORT uintptr_t GblArrayMap_atValue(GblArrayMap*const* ppSelf, uintptr_t key) {
    uintptr_t value = 0;
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);

    if GBL_UNLIKELY(!pEntry) {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);

        GBL_CTX_END_BLOCK();
    } else {
        value = GblArrayMap_valueFromVariant_(&pEntry->value);
    }

    return value;
}

GBL_EXPORT GblVariant* GblArrayMap_atVariant(GblArrayMap* const* ppSelf, uintptr_t key) {
    GblVariant* pVariant = GBL_NULL;
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);

    if GBL_UNLIKELY(!pEntry) {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);

        GBL_CTX_END_BLOCK();
    } else if GBL_LIKELY(pEntry->value.type != GBL_INVALID_TYPE)
        pVariant = &pEntry->value;

    return pVariant;
}

GBL_EXPORT size_t  GblArrayMap_find(GblArrayMap*const* ppSelf, uintptr_t key) {
    size_t  index = GBL_ARRAY_MAP_NPOS;
    // Don't look for shit if this is a NIL array map
    if(*ppSelf) {
        GblArrayMap* pSelf = *ppSelf;
        const GblBool binarySearches = GblArrayMap_binarySearches(ppSelf);
        const size_t  size = GblArrayMap_size(ppSelf);

        /* Perform binary search if there are enough entries to warrant the complexity,
           and we have a sorted list. */
        if(binarySearches && size >= GBL_ARRAY_MAP_BINARY_SEARCH_CUTOFF_SIZE) {
            const GblArrayMapEntry* pEntryBegin = GblArrayMap_entry_(ppSelf, 0);
            ptrdiff_t upper = size-1;
            ptrdiff_t lower = 0;

            // Only run this loop if we're doing a binary search using a comparator
            if(GBL_PRIV_REF(pSelf).pFnComparator) {
                do {
                    const size_t  mid = lower + ((upper-lower) >> 1);
                    const GblArrayMapEntry* pEntry = pEntryBegin + mid;
                    const ptrdiff_t diff = GBL_PRIV_REF(pSelf).pFnComparator(pSelf, pEntry->key, key);
                    if(diff > 0) {
                        upper = mid-1;
                    } else if(diff < 0) {
                        lower = mid+1;
                    } else {
                        index = mid;
                        break;
                    }
                } while(lower <= upper);

            // only run this loop if we're doing a binary search without a comparator
            } else {

                do {
                    const size_t  mid = lower + ((upper-lower) >> 1);
                    const GblArrayMapEntry* pEntry = pEntryBegin + mid;
                    const ptrdiff_t diff = pEntry->key - key;
                    if(diff > 0) {
                        upper = mid-1;
                    } else if(diff < 0) {
                        lower = mid+1;
                    } else {
                        index = mid;
                        break;
                    }
                } while(lower <= upper);
            }
        } else { // do regular-ass linear searches

            // Only run this loop for linear searches with a comparator
            if(GBL_PRIV_REF(pSelf).pFnComparator) {
                const GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, 0);
                for(size_t  e = 0; e < size; ++e) {
                    if(GBL_PRIV_REF(pSelf).pFnComparator(pSelf, pEntry->key, key) == 0) {
                         index = e;
                         break;
                     }
                     pEntry++;
                 }
            // Only run this loop for linear searches without a comparator
            } else {
                const GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, 0);
                for(size_t  e = 0; e < size; ++e) {
                    if(pEntry->key == key) {
                         index = e;
                         break;
                     }
                     pEntry++;
                 }
            }
        }

    }
    return index;
}


GBL_EXPORT uintptr_t GblArrayMap_probeKey(GblArrayMap*const* ppSelf, size_t  index) {
    GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    return pEntry? pEntry->key : 0;
}

GBL_EXPORT GblVariant* GblArrayMap_probeVariant(GblArrayMap*const* ppSelf, size_t  index) {
    GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    return pEntry && pEntry->value.type != GBL_INVALID_TYPE? &pEntry->value : GBL_NULL;
}

GBL_EXPORT uintptr_t GblArrayMap_probeValue(GblArrayMap*const* ppSelf, size_t  index) {
    GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    return pEntry? GblArrayMap_valueFromVariant_(&pEntry->value) : 0;
}

GBL_EXPORT GBL_RESULT GblArrayMap_setVariant(GblArrayMap** ppSelf, uintptr_t key, GblVariant* pVariant) {
    GBL_RESULT result = GBL_RESULT_SUCCESS;
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(!pEntry) {
        pEntry = GblArrayMap_entryAdd_(ppSelf, key);
        result = GblVariant_constructCopy(&pEntry->value, pVariant);
    } else {
        result = GblArrayMap_entrySetVariant_(ppSelf, pEntry, pVariant);
    }
    return result;
}

GBL_EXPORT GBL_RESULT GblArrayMap_setUserdata(GblArrayMap** ppSelf, uintptr_t key, uintptr_t value, GblArrayMapDtorFn pDtor) {
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(!pEntry) {
        pEntry = GblArrayMap_entryAdd_(ppSelf, key);
    }
    return GblArrayMap_entrySetUserdata_(ppSelf, pEntry, value, pDtor);
}

GBL_EXPORT size_t  GblArrayMap_insertVariant(GblArrayMap** ppSelf, uintptr_t key, GblVariant* pVariant) {
    size_t  index = GblArrayMap_find(ppSelf, key);
    if(index == GBL_ARRAY_MAP_NPOS) {
        GblArrayMapEntry* pEntry = GblArrayMap_entryAdd_(ppSelf, key);
        GblVariant_constructCopy(&pEntry->value, pVariant);
        index = GblArrayMap_size(ppSelf)-1;
    }
    return index;
}

GBL_EXPORT size_t  GblArrayMap_insertUserdata(GblArrayMap** ppSelf, uintptr_t key, uintptr_t value, GblArrayMapDtorFn pDtor) {
    size_t  index = GblArrayMap_find(ppSelf, key);
    if(index == GBL_ARRAY_MAP_NPOS) {
        GblArrayMapEntry* pEntry = GblArrayMap_entryAdd_(ppSelf, key);
        GblArrayMap_entrySetUserdata_(ppSelf, pEntry, value, pDtor);
        index = GblArrayMap_size(ppSelf)-1;
    }
    return index;
}

GBL_EXPORT GblArrayMap* GblArrayMap_create(GblArrayMapCmpFn   pFnComparator,
                                           GblBool            binarySearches,
                                           GblContext*        pCtx)
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
        const size_t  bytes = GBL_ARRAY_MAP_SIZE((*ppSelf)->size);
        pMap =GBL_CTX_MALLOC(bytes);
        memcpy(pMap, *ppSelf, bytes);
        GBL_CTX_END_BLOCK();
    }
    return pMap;
}
*/

GBL_EXPORT GBL_RESULT GblArrayMap_destroy(GblArrayMap** ppSelf) {
    GBL_CTX_BEGIN(NULL);
    GblArrayMap* pSelf = *ppSelf;
    if(pSelf) {
        for(size_t  e = 0; e < GBL_PRIV_REF(pSelf).size; ++e) {
            GBL_CTX_CALL(GblArrayMap_entryDestruct_(ppSelf, GblArrayMap_entry_(ppSelf, e)));
        }
        GBL_CTX_FREE(pSelf);
        *ppSelf = NULL;
    }
    GBL_CTX_END();
}

static GblBool GblArrayMap_erase_(GblArrayMap** ppSelf, uintptr_t key, GblBool free) {
    GblBool removed = GBL_FALSE;
    GBL_CTX_BEGIN(NULL);

    if(*ppSelf) {
        const size_t  index = GblArrayMap_find(ppSelf, key);

        if(index != GBL_ARRAY_MAP_NPOS) {
            if GBL_UNLIKELY(GblArrayMap_size(ppSelf) == 1) {
                GblArrayMap_clear(ppSelf);
            } else {
                if(free)
                    GBL_CTX_CALL(GblArrayMap_entryDestruct_(ppSelf, GblArrayMap_entry_(ppSelf, index)));

                const size_t remainder = GblArrayMap_size(ppSelf) - 1 - index;

                if(remainder)
                    memmove(GblArrayMap_entry_(ppSelf, index),
                            GblArrayMap_entry_(ppSelf, index+1), remainder * sizeof(GblArrayMapEntry));

                *ppSelf = GBL_CTX_REALLOC(*ppSelf, GBL_ARRAY_MAP_SIZE(GblArrayMap_size(ppSelf)-1));
                --GBL_PRIV_REF(*ppSelf).size;
                removed = GBL_TRUE;
            }
            removed = GBL_TRUE;
        }
    }
    GBL_CTX_END_BLOCK();
    return removed;
}

GBL_EXPORT GblBool GblArrayMap_erase(GblArrayMap** ppSelf, uintptr_t key) {
    return GblArrayMap_erase_(ppSelf, key, GBL_TRUE);
}

GBL_EXPORT GblBool GblArrayMap_extractVariant(GblArrayMap** ppSelf, uintptr_t key, GblVariant* pVariant) {
    GblBool found = GBL_FALSE;
    const size_t  index = GblArrayMap_find(ppSelf, key);
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


GBL_EXPORT GblBool GblArrayMap_extractValue(GblArrayMap**ppSelf, uintptr_t key, uintptr_t* pValue) {
    GblBool found = GBL_FALSE;
    const size_t  index = GblArrayMap_find(ppSelf, key);
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
        const size_t  size = GblArrayMap_size(ppSelf);
        for(size_t  e = 0; e < size; ++e)
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
