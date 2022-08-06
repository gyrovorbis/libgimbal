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

#define GBL_ARRAY_MAP_NPOS                      GBL_NPOS
#define GBL_ARRAY_MAP_SIZE(elements)            (sizeof(GblArrayMap) + (sizeof(GblArrayMapEntry)*(elements)))
#define GBL_ARRAY_MAP_BINARY_SEARCH_CUTOFF_SIZE 40

#define SELF    GblArrayMap** ppSelf
#define CSELF   GblArrayMap*const* ppSelf

GBL_FORWARD_DECLARE_STRUCT(GblArrayMapEntry);
GBL_FORWARD_DECLARE_STRUCT(GblArrayMap);

typedef GBL_RESULT  (*GblArrayMapDtorFn)(const GblArrayMap*, uintptr_t key, void* pEntry);
typedef int         (*GblArrayMapCmpFn) (const GblArrayMap*, uintptr_t key1, uintptr_t key2);

typedef struct GblArrayMapEntry {
    uintptr_t               key;
    GblVariant              value; // using value.type == GBL_INVALID_TYPE to signify userdata
    GblArrayMapDtorFn       dtor;
} GblArrayMapEntry;

/*! \brief Contiguous array-based associative container with [K,V] pairs
 *  \ingroup containers
 */
typedef struct GblArrayMap {
    GblContext*             pCtx;
    GblArrayMapCmpFn        pFnComparator;
    GblSize                 binarySearches  : 1;
#ifdef GBL_64BIT
    GblSize                 size            : 63;
#elif defined(GBL_32BIT)
    GblSize                 size            : 31;
#endif
} GblArrayMap;

GBL_EXPORT GblArrayMap* GblArrayMap_create          (GblArrayMapCmpFn pFnComp,
                                                     GblBool          binarySearch,
                                                     GblContext*      pCtx)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblArrayMap_destroy         (SELF)                    GBL_NOEXCEPT;

GBL_INLINE GblSize      GblArrayMap_size            (CSELF)                   GBL_NOEXCEPT;
GBL_INLINE GblContext*  GblArrayMap_context         (CSELF)                   GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayMap_empty           (CSELF)                   GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayMap_binarySearches  (CSELF)                   GBL_NOEXCEPT;

GBL_INLINE GblBool      GblArrayMap_contains        (CSELF, uintptr_t key)    GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayMap_containsUserdata(CSELF, uintptr_t key)    GBL_NOEXCEPT;
GBL_INLINE GblBool      GblArrayMap_containsVariant (CSELF, uintptr_t key)    GBL_NOEXCEPT;

GBL_INLINE uintptr_t    GblArrayMap_getValue        (CSELF, uintptr_t key)    GBL_NOEXCEPT;
GBL_INLINE GblVariant*  GblArrayMap_getVariant      (CSELF, uintptr_t key)    GBL_NOEXCEPT;
GBL_INLINE uintptr_t    GblArrayMap_atValue         (CSELF, uintptr_t key)    GBL_NOEXCEPT;
GBL_INLINE GblVariant*  GblArrayMap_atVariant       (CSELF, uintptr_t key)    GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblArrayMap_setUserdata     (SELF,
                                                     uintptr_t key,
                                                     uintptr_t value,
                                                     GblArrayMapDtorFn pDtor) GBL_NOEXCEPT;

GBL_INLINE GBL_RESULT   GblArrayMap_setVariant      (SELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)    GBL_NOEXCEPT;

GBL_INLINE GblSize      GblArrayMap_insertUserdata  (SELF,
                                                     uintptr_t key,
                                                     uintptr_t value,
                                                     GblArrayMapDtorFn pDtor) GBL_NOEXCEPT;

GBL_INLINE GblSize      GblArrayMap_insertVariant   (SELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)    GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblArrayMap_erase           (SELF, uintptr_t key)     GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblArrayMap_extractVariant  (SELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)    GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblArrayMap_extractValue    (SELF,
                                                     uintptr_t key,
                                                     uintptr_t* pValue)       GBL_NOEXCEPT;

GBL_EXPORT void         GblArrayMap_clear           (SELF)                    GBL_NOEXCEPT;

GBL_INLINE GblSize      GblArrayMap_find            (CSELF, uintptr_t key)    GBL_NOEXCEPT;
GBL_INLINE uintptr_t    GblArrayMap_probeKey        (CSELF, GblSize index)    GBL_NOEXCEPT;
GBL_INLINE uintptr_t    GblArrayMap_probeValue      (CSELF, GblSize index)    GBL_NOEXCEPT;
GBL_INLINE GblVariant*  GblArrayMap_probeVariant    (CSELF, GblSize index)    GBL_NOEXCEPT;

//========== IMPL ==========

GBL_INLINE GblArrayMapEntry* GblArrayMap_entry_(CSELF, GblSize index) GBL_NOEXCEPT {
    return (*ppSelf && index < GblArrayMap_size(ppSelf))?
            (GblArrayMapEntry*)((uint8_t*)*ppSelf + sizeof(GblArrayMap) + sizeof(GblArrayMapEntry)*index) :
            GBL_NULL;
}

GBL_INLINE GblArrayMapEntry* GblArrayMap_find_entry_(CSELF, uintptr_t key) GBL_NOEXCEPT {
    const GblSize index = GblArrayMap_find(ppSelf, key);
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

GBL_INLINE GblBool GblArrayMap_binarySearches(CSELF) GBL_NOEXCEPT {
    return *ppSelf && (*ppSelf)->binarySearches? GBL_TRUE : GBL_FALSE;
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
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(!pEntry) GBL_UNLIKELY {
        GBL_API_BEGIN(NULL);
        GBL_API_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);

        GBL_API_END_BLOCK();
    } else GBL_LIKELY {
        value = GblArrayMap_valueFromVariant_(&pEntry->value);
    }
    return value;
}

GBL_INLINE GblVariant* GblArrayMap_atVariant(CSELF, uintptr_t key) GBL_NOEXCEPT {
    GblVariant* pVariant = 0;
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(!pEntry) GBL_UNLIKELY {
        GBL_API_BEGIN(NULL);
        GBL_API_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);

        GBL_API_END_BLOCK();
    } else GBL_LIKELY {
        if(pEntry->value.type != GBL_INVALID_TYPE)
            pVariant = &pEntry->value;
    }
    return pVariant;
}

GBL_INLINE ptrdiff_t GblArrayMap_diffEntry_(CSELF, GblSize index, uintptr_t key) GBL_NOEXCEPT {
    ptrdiff_t diff = GBL_ARRAY_MAP_NPOS;
    const GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    if((*ppSelf)->pFnComparator) {
        diff = (*ppSelf)->pFnComparator(*ppSelf, pEntry->key, key);
    } else {
        diff = pEntry->key - key;
    }
    return diff;
}

GBL_INLINE GblSize GblArrayMap_find(CSELF, uintptr_t key) GBL_NOEXCEPT {
    GblSize index = GBL_ARRAY_MAP_NPOS;
    // Don't look for shit if this is a NIL array map
    if(*ppSelf) {
        GblArrayMap* pSelf = *ppSelf;
        const GblBool binarySearches = GblArrayMap_binarySearches(ppSelf);
        const GblSize size = GblArrayMap_size(ppSelf);

        /* Perform binary search if there are enough entries to warrant the complexity,
           and we have a sorted list. */
        if(binarySearches && size >= GBL_ARRAY_MAP_BINARY_SEARCH_CUTOFF_SIZE) {
            const GblArrayMapEntry* pEntryBegin = GblArrayMap_entry_(ppSelf, 0);
            ptrdiff_t upper = size-1;
            ptrdiff_t lower = 0;

            // Only run this loop if we're doing a binary search using a comparator
            if(pSelf->pFnComparator) {
                do {
                    const GblSize mid = lower + ((upper-lower) >> 1);
                    const GblArrayMapEntry* pEntry = pEntryBegin + mid;
                    const ptrdiff_t diff = pSelf->pFnComparator(pSelf, pEntry->key, key);
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
                    const GblSize mid = lower + ((upper-lower) >> 1);
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
            if(pSelf->pFnComparator) {
                const GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, 0);
                for(GblSize e = 0; e < size; ++e) {
                    if(pSelf->pFnComparator(pSelf, pEntry->key, key) == 0) {
                         index = e;
                         break;
                     }
                     pEntry++;
                 }
            // Only run this loop for linear searches without a comparator
            } else {
                const GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, 0);
                for(GblSize e = 0; e < size; ++e) {
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

GBL_INLINE GBL_RESULT GblArrayMap_entryDestruct_(CSELF, GblArrayMapEntry* pEntry) GBL_NOEXCEPT {
    if(pEntry->dtor) {
        return pEntry->dtor(*ppSelf, pEntry->key, pEntry->value.pVoid);
    } else if(GblVariant_typeOf(&pEntry->value) != GBL_INVALID_TYPE) {
        return GblVariant_destruct(&pEntry->value);
    } else return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_RESULT GblArrayMap_entrySetVariant_(CSELF, GblArrayMapEntry* pEntry, GblVariant* pVariant) GBL_NOEXCEPT {
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

GBL_INLINE GblArrayMapEntry* GblArrayMap_entryAdd_(SELF, uintptr_t key) {
    GblArrayMapEntry* pEntry = NULL;
    GBL_API_BEGIN(NULL);
    const GblSize size = GblArrayMap_size(ppSelf);

    // Have to insert in sorted order for Nth entry when binary searchable
    if(*ppSelf && size && GblArrayMap_binarySearches(ppSelf)) {
        GblSize insertionIdx = size;

        // If we have a little bitch array that's fast linearly
        if(size < GBL_ARRAY_MAP_BINARY_SEARCH_CUTOFF_SIZE) {
            // iterate (linearly, fuck you) to find insertion point
            for(GblSize i = 0; i < size; ++i) {
                // insertion point is less than or equal to next
                if(GblArrayMap_diffEntry_(ppSelf, i, key) >= 0) {
                    insertionIdx = i;
                    break;
                }
            }
        // Time to bust out the big guns and search intelligently
        } else {

            ptrdiff_t upper = size-1;
            ptrdiff_t lower = 0;
            GblArrayMap* pSelf = *ppSelf;
            GblBool found = GBL_FALSE;

            const GblArrayMapEntry* pEntryBegin = GblArrayMap_entry_(ppSelf, 0);

            do {
                const GblSize mid = lower + ((upper-lower) >> 1);
                const GblArrayMapEntry* pEntry = pEntryBegin + mid;
                const ptrdiff_t diff = pSelf->pFnComparator?
                                            pSelf->pFnComparator(pSelf, pEntry->key, key) :
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
        *ppSelf = (GblArrayMap*)GBL_API_REALLOC(*ppSelf, GBL_ARRAY_MAP_SIZE(size+1));
        ++(*ppSelf)->size;
        pEntry = GblArrayMap_entry_(ppSelf, insertionIdx);

        // calculate slide adjustments for the rest of the items
        const GblSize slideSize = (size - insertionIdx);
        if(slideSize) {
            // scooch the bitches over by one
            memmove(pEntry + 1,
                    pEntry,
                    sizeof(GblArrayMapEntry) * slideSize);
        }

    // regular-ass unordered insertion at the end for unsorted or initial entry
    } else {
        // initial allocation
        if(!*ppSelf) GBL_UNLIKELY {
            *ppSelf = (GblArrayMap*)GBL_API_MALLOC(GBL_ARRAY_MAP_SIZE(1));
            memset(*ppSelf, 0, sizeof(GblArrayMap));
        //appending to the end
        } else GBL_LIKELY {
            *ppSelf = (GblArrayMap*)GBL_API_REALLOC(*ppSelf, GBL_ARRAY_MAP_SIZE(size + 1));
        }
        ++(*ppSelf)->size;
        pEntry = GblArrayMap_entry_(ppSelf, GblArrayMap_size(ppSelf)-1);
    }

    // initialize new entry
    memset(pEntry, 0, sizeof(GblArrayMapEntry));
    pEntry->key = key;

    // return new entry
    GBL_API_END_BLOCK();
    return pEntry;
}

GBL_INLINE GBL_RESULT GblArrayMap_setVariant(SELF, uintptr_t key, GblVariant* pVariant) GBL_NOEXCEPT {
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

GBL_INLINE GBL_RESULT GblArrayMap_entrySetUserdata_(CSELF, GblArrayMapEntry* pEntry, uintptr_t value, GblArrayMapDtorFn pDtor) GBL_NOEXCEPT {
    GBL_RESULT result = GblArrayMap_entryDestruct_(ppSelf, pEntry);
    pEntry->value.pVoid = (void*)value;
    pEntry->value.type = GBL_INVALID_TYPE;
    pEntry->dtor = pDtor;
    return result;
}

GBL_INLINE GBL_RESULT GblArrayMap_setUserdata(SELF, uintptr_t key, uintptr_t value, GblArrayMapDtorFn pDtor) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(!pEntry) {
        pEntry = GblArrayMap_entryAdd_(ppSelf, key);
    }
    return GblArrayMap_entrySetUserdata_(ppSelf, pEntry, value, pDtor);
}

GBL_INLINE GblSize GblArrayMap_insertVariant(SELF, uintptr_t key, GblVariant* pVariant) GBL_NOEXCEPT {
    GblSize index = GblArrayMap_find(ppSelf, key);
    if(index == GBL_ARRAY_MAP_NPOS) {
        GblArrayMapEntry* pEntry = GblArrayMap_entryAdd_(ppSelf, key);
        GblVariant_constructCopy(&pEntry->value, pVariant);
        index = GblArrayMap_size(ppSelf)-1;
    }
    return index;
}

GBL_INLINE GblSize GblArrayMap_insertUserdata(SELF, uintptr_t key, uintptr_t value, GblArrayMapDtorFn pDtor) GBL_NOEXCEPT {
    GblSize index = GblArrayMap_find(ppSelf, key);
    if(index == GBL_ARRAY_MAP_NPOS) {
        GblArrayMapEntry* pEntry = GblArrayMap_entryAdd_(ppSelf, key);
        GblArrayMap_entrySetUserdata_(ppSelf, pEntry, value, pDtor);
        index = GblArrayMap_size(ppSelf)-1;
    }
    return index;
}


#undef CSELF
#undef SELF

#endif // GIMBAL_ARRAY_MAP_HPP
