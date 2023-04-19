/*! \file
 *  \brief ::GblArrayMap container and related functions
 *  \ingroup containers
 *  \copydoc GblArrayMap
 */
#ifndef GIMBAL_ARRAY_MAP_H
#define GIMBAL_ARRAY_MAP_H

#include "../core/gimbal_typedefs.h"
#include "../strings/gimbal_quark.h"
#include "../meta/types/gimbal_variant.h"
#include "../core/gimbal_result.h"

#define GBL_ARRAY_MAP_NPOS                      GBL_NPOS                                                        ///< Invalid index identifier returned when an entry couldn't be found
#define GBL_ARRAY_MAP_SIZE(elements)            (sizeof(GblArrayMap) + (sizeof(GblArrayMapEntry)*(elements)))   ///< Calculates total size of the GblArrayMap structure with elements
#define GBL_ARRAY_MAP_BINARY_SEARCH_CUTOFF_SIZE 40                                                              ///< Magical break-even point for when binary searches out-perform linear searches, based on profiling

#define GBL_PSELF    GblArrayMap** ppSelf
#define GBL_PCSELF   GblArrayMap*const* ppSelf

GBL_FORWARD_DECLARE_STRUCT(GblArrayMapEntry);
GBL_FORWARD_DECLARE_STRUCT(GblArrayMap);

typedef GBL_RESULT  (*GblArrayMapDtorFn)(const GblArrayMap*, uintptr_t key, void* pEntry);      ///< Custom destructor type for userdata values
typedef int         (*GblArrayMapCmpFn) (const GblArrayMap*, uintptr_t key1, uintptr_t key2);   ///< Custom comparator for nontrivial key types

/*! Represents a single K,V data entry within a GblArrayMap
 *  The destructor field is only used for userdata values, while
 *  GblVariant values have automatic lifetime management.
 *  \sa GblArrayMap
 */
typedef struct GblArrayMapEntry {
    uintptr_t               key;   ///< Opaque key value
    GblVariant              value; ///< Using value.type == GBL_INVALID_TYPE to signify userdata
    GblArrayMapDtorFn       dtor;  ///< Optional custom destructor for userdata types
} GblArrayMapEntry;

/*! Contiguous array-based associative container with [K,V] pairs.
 *  GblArrayMap is essentially a flat map structure with a few specific
 *  properties:
 *
 *  - Value types can either be uinptr_t userdata or typed GblVariants
 *  - Userdata types can have per-entry custom destructors
 *  - For non-trivial key types, a custom comparator can be specified
 *  - insertion and searching can either be done sorted in a binary search or unsorted linearly
 *
 *  \note
 *  GblArrayMap is a lazily-allocated structure, meaning it isn't actually allocated until it's
 *  needed. This is why the majority of the API takes a pointer to a pointer, so a NULL pointer
 *  value is a valid empty map. You can optionally preconstruct the structure with GblArrayMap_create.
 *  \ingroup containers
 *  \sa GblArrayMapEntry
 */
typedef struct GblArrayMap {
    GBL_PRIVATE()
        GblContext*             pCtx;                   ///< Optional custom context
        GblArrayMapCmpFn        pFnComparator;          ///< Optional custom comparator
        size_t                  binarySearches  : 1;    ///< Optionally sort values and use binary searches
#ifdef GBL_64BIT
        size_t                  size            : 63;   ///< Number of elements within the map
#elif defined(GBL_32BIT)
        size_t                  size            : 31;
#endif
    GBL_PRIVATE_END
} GblArrayMap;

/// Pre-creates a GblArrayMap with the given comparator, binary search config, and context.
GBL_EXPORT GblArrayMap* GblArrayMap_create          (GblArrayMapCmpFn pFnComp,
                                                     GblBool          binarySearch,
                                                     GblContext*      pCtx)        GBL_NOEXCEPT;
/// Destroys a GblArrayMap.
GBL_EXPORT GBL_RESULT   GblArrayMap_destroy         (GBL_PSELF)                    GBL_NOEXCEPT;
/// Returns the number of entries within the given map.
GBL_INLINE size_t       GblArrayMap_size            (GBL_PCSELF)                   GBL_NOEXCEPT;
/// Returns the context associated with the given map.
GBL_INLINE GblContext*  GblArrayMap_context         (GBL_PCSELF)                   GBL_NOEXCEPT;
/// Returns whether the given map is empty.
GBL_INLINE GblBool      GblArrayMap_empty           (GBL_PCSELF)                   GBL_NOEXCEPT;
/// Returns whether the given map is sorted and uses binary searches.
GBL_INLINE GblBool      GblArrayMap_binarySearches  (GBL_PCSELF)                   GBL_NOEXCEPT;
/// Returns true if the given map contains the given key.
GBL_INLINE GblBool      GblArrayMap_contains        (GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
/// Returns true if the given map contains the given key, associated with a userdata value.
GBL_INLINE GblBool      GblArrayMap_containsUserdata(GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
/// Returns true if the given map contains the given key, associated with a GblVariant value.
GBL_INLINE GblBool      GblArrayMap_containsVariant (GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
/// Returns the GblVariant or userdata value associated with the given key as a uintptr_t.
GBL_INLINE uintptr_t    GblArrayMap_getValue        (GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
/// Returns the GblVariant associated with the given key.
GBL_INLINE GblVariant*  GblArrayMap_getVariant      (GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
/// Returns the GblVariant or userdata value associated with the given key, erroring if not found.
GBL_INLINE uintptr_t    GblArrayMap_atValue         (GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
/// Returns the GblVariant associated with the given key, erroring if not found.
GBL_INLINE GblVariant*  GblArrayMap_atVariant       (GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
/// Inserts or replaces the entry with the given key with a userdata value and optional destructor.
GBL_INLINE GBL_RESULT   GblArrayMap_setUserdata     (GBL_PSELF,
                                                     uintptr_t key,
                                                     uintptr_t value,
                                                     GblArrayMapDtorFn pDtor)     GBL_NOEXCEPT;
/// Inserts or replaces the entry with the given key with a GblVariant.
GBL_INLINE GBL_RESULT   GblArrayMap_setVariant      (GBL_PSELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)        GBL_NOEXCEPT;
/// Attempts to insert a new entry with the given key and userdata, returning the insertion index.
GBL_INLINE size_t       GblArrayMap_insertUserdata  (GBL_PSELF,
                                                     uintptr_t key,
                                                     uintptr_t value,
                                                     GblArrayMapDtorFn pDtor)     GBL_NOEXCEPT;
/// Attempts to insert a new entry with the given key and GblVariant value, returning insertion index.
GBL_INLINE size_t       GblArrayMap_insertVariant   (GBL_PSELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)        GBL_NOEXCEPT;
/// Attempts to erase the value with the given key, returning GBL_FALSE if not found.
GBL_EXPORT GblBool      GblArrayMap_erase           (GBL_PSELF, uintptr_t key)    GBL_NOEXCEPT;
/// Attempts to remove the GblVariant with the given key, moving it into the given pointer if found.
GBL_EXPORT GblBool      GblArrayMap_extractVariant  (GBL_PSELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)        GBL_NOEXCEPT;
/// Attempts to remove the value with the given key, moving it into the given pointer if found.
GBL_EXPORT GblBool      GblArrayMap_extractValue    (GBL_PSELF,
                                                     uintptr_t key,
                                                     uintptr_t* pValue)           GBL_NOEXCEPT;
/// Clears all entries within the given map.
GBL_EXPORT void         GblArrayMap_clear           (GBL_PSELF)                   GBL_NOEXCEPT;
/// Returns the index of the entry with the given key.
GBL_INLINE size_t       GblArrayMap_find            (GBL_PCSELF, uintptr_t key)   GBL_NOEXCEPT;
/// Returns the key for the entry at the given index.
GBL_INLINE uintptr_t    GblArrayMap_probeKey        (GBL_PCSELF, size_t  index)   GBL_NOEXCEPT;
/// Returns the value for the entry at the given index.
GBL_INLINE uintptr_t    GblArrayMap_probeValue      (GBL_PCSELF, size_t  index)   GBL_NOEXCEPT;
/// Returns the GblVariant for the entry at the given index.
GBL_INLINE GblVariant*  GblArrayMap_probeVariant    (GBL_PCSELF, size_t  index)   GBL_NOEXCEPT;

//========== IMPL ==========

///\cond
GBL_INLINE GblArrayMapEntry* GblArrayMap_entry_(GBL_PCSELF, size_t  index) GBL_NOEXCEPT {
    return (*ppSelf && index < GblArrayMap_size(ppSelf))?
            (GblArrayMapEntry*)((uint8_t*)*ppSelf + sizeof(GblArrayMap) + sizeof(GblArrayMapEntry)*index) :
            GBL_NULL;
}

GBL_INLINE GblArrayMapEntry* GblArrayMap_find_entry_(GBL_PCSELF, uintptr_t key) GBL_NOEXCEPT {
    const size_t  index = GblArrayMap_find(ppSelf, key);
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

GBL_INLINE ptrdiff_t GblArrayMap_diffEntry_(GBL_PCSELF, size_t  index, uintptr_t key) GBL_NOEXCEPT {
    ptrdiff_t diff = GBL_ARRAY_MAP_NPOS;
    const GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    if(GBL_PRIV_REF(*ppSelf).pFnComparator) {
        diff = GBL_PRIV_REF(*ppSelf).pFnComparator(*ppSelf, pEntry->key, key);
    } else {
        diff = pEntry->key - key;
    }
    return diff;
}

GBL_INLINE GBL_RESULT GblArrayMap_entryDestruct_(GBL_PCSELF, GblArrayMapEntry* pEntry) GBL_NOEXCEPT {
    if(pEntry->dtor) {
        return pEntry->dtor(*ppSelf, pEntry->key, pEntry->value.pVoid);
    } else if(GblVariant_typeOf(&pEntry->value) != GBL_INVALID_TYPE) {
        return GblVariant_destruct(&pEntry->value);
    } else return GBL_RESULT_SUCCESS;
}

GBL_INLINE GBL_RESULT GblArrayMap_entrySetVariant_(GBL_PCSELF, GblArrayMapEntry* pEntry, GblVariant* pVariant) GBL_NOEXCEPT {
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

GBL_INLINE GblArrayMapEntry* GblArrayMap_entryAdd_(GBL_PSELF, uintptr_t key) {
    GblArrayMapEntry* pEntry = NULL;
    GBL_CTX_BEGIN(NULL);
    const size_t  size = GblArrayMap_size(ppSelf);

    // Have to insert in sorted order for Nth entry when binary searchable
    if(*ppSelf && size && GblArrayMap_binarySearches(ppSelf)) {
        size_t  insertionIdx = size;

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

            ptrdiff_t upper = size-1;
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
        *ppSelf = (GblArrayMap*)GBL_CTX_REALLOC(*ppSelf, GBL_ARRAY_MAP_SIZE(size+1));
        ++GBL_PRIV_REF(*ppSelf).size;
        pEntry = GblArrayMap_entry_(ppSelf, insertionIdx);

        // calculate slide adjustments for the rest of the items
        const size_t  slideSize = (size - insertionIdx);
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
            *ppSelf = (GblArrayMap*)GBL_CTX_MALLOC(GBL_ARRAY_MAP_SIZE(1));
            memset(*ppSelf, 0, sizeof(GblArrayMap));
        //appending to the end
        } else GBL_LIKELY {
            *ppSelf = (GblArrayMap*)GBL_CTX_REALLOC(*ppSelf, GBL_ARRAY_MAP_SIZE(size + 1));
        }
        ++GBL_PRIV_REF(*ppSelf).size;
        pEntry = GblArrayMap_entry_(ppSelf, GblArrayMap_size(ppSelf)-1);
    }

    // initialize new entry
    memset(pEntry, 0, sizeof(GblArrayMapEntry));
    pEntry->key = key;

    // return new entry
    GBL_CTX_END_BLOCK();
    return pEntry;
}

GBL_INLINE GBL_RESULT GblArrayMap_entrySetUserdata_(GBL_PCSELF, GblArrayMapEntry* pEntry, uintptr_t value, GblArrayMapDtorFn pDtor) GBL_NOEXCEPT {
    GBL_RESULT result = GblArrayMap_entryDestruct_(ppSelf, pEntry);
    pEntry->value.pVoid = (void*)value;
    pEntry->value.type = GBL_INVALID_TYPE;
    pEntry->dtor = pDtor;
    return result;
}

///\endcond

/*!
 * \brief GblArrayMap_size
 * \param ppSelf
 * \return size of the map
 * \relatedalso GblArrayMap
 */
GBL_INLINE size_t  GblArrayMap_size(GBL_PCSELF) GBL_NOEXCEPT {
    return *ppSelf? GBL_PRIV_REF(*ppSelf).size : 0;
}
/*!
 * \brief GblArrayMap_context
 * \param ppSelf
 * \return associated context
 * \relatedalso GblArrayMap
 */
GBL_INLINE GblContext* GblArrayMap_context(GBL_PCSELF) GBL_NOEXCEPT {
    return *ppSelf? GBL_PRIV_REF(*ppSelf).pCtx : 0;
}
/*!
 * \brief GblArrayMap_empty
 * \param ppSelf
 * \return GBL_TRUE if the map is empty or NULL
 * \relatedalso GblArrayMap
 */
GBL_INLINE GblBool GblArrayMap_empty(GBL_PCSELF) GBL_NOEXCEPT {
    return *ppSelf && GBL_PRIV_REF(*ppSelf).size? GBL_FALSE : GBL_TRUE;
}
/*!
 * \brief GblArrayMap_binarySearches
 * \param ppSelf
 * \return GBL_TRUE if sorting and binary searches are enabled
 * \relatedalso GblArrayMap
 */
GBL_INLINE GblBool GblArrayMap_binarySearches(GBL_PCSELF) GBL_NOEXCEPT {
    return *ppSelf && GBL_PRIV_REF(*ppSelf).binarySearches? GBL_TRUE : GBL_FALSE;
}
/*!
 * \brief GblArrayMap_contains
 * \param ppSelf
 * \param key
 * \return GBL_TRUE if there exists an entry with the given key
 * \relatedalso GblArrayMap
 */
GBL_INLINE GblBool GblArrayMap_contains(GBL_PCSELF, uintptr_t key) GBL_NOEXCEPT {
    return GblArrayMap_find(ppSelf, key) != GBL_ARRAY_MAP_NPOS;
}
/*!
 * \brief GblArrayMap_containsUserdata
 * \param ppSelf
 * \param key
 * \return GBL_TRUE if there exists a userdata entry with the given key\
 * \relatedalso GblArrayMap
 */
GBL_INLINE GblBool GblArrayMap_containsUserdata(GBL_PCSELF, uintptr_t key) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry) {
        if(GblVariant_typeOf(&pEntry->value) == GBL_INVALID_TYPE)
            return GBL_TRUE;
    }
    return GBL_FALSE;
}
/*!
 * \brief GblArrayMap_containsVariant
 * \param ppSelf
 * \param key
 * \return GBL_TRUE if there exists a GblVariant with the associated key
 * \relatedalso GblArrayMap
 */
GBL_INLINE GblBool GblArrayMap_containsVariant(GBL_PCSELF, uintptr_t key) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry) {
        if(GblVariant_typeOf(&pEntry->value) != GBL_INVALID_TYPE)
            return GBL_TRUE;
    }
    return GBL_FALSE;
}
/*!
 * \brief GblArrayMap_getValue
 * \param ppSelf
 * \param key
 * \return value associated with the given key
 * \relatedalso GblArrayMap
 */
GBL_INLINE uintptr_t GblArrayMap_getValue(GBL_PCSELF, uintptr_t key) GBL_NOEXCEPT {
    uintptr_t value = 0;
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry) {
        value = GblArrayMap_valueFromVariant_(&pEntry->value);
    }
    return value;
}
/*!
 * \brief GblArrayMap_getVariant
 * \param ppSelf
 * \param key
 * \return GblVariant value associated with the given key
 * \relatedalso GblArrayMap
 */
GBL_INLINE GblVariant* GblArrayMap_getVariant(GBL_PCSELF, uintptr_t key) GBL_NOEXCEPT {
    GblVariant* pVariant = GBL_NULL;
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(pEntry && pEntry->value.type != GBL_INVALID_TYPE) {
       pVariant = &pEntry->value;
    }
    return pVariant;
}
/*!
 * \brief GblArrayMap_atValue
 * \param ppSelf
 * \param key
 * \return value associated with the given key or zero if it doesn't exist
 * \relatedalso GblArrayMap
 */
GBL_INLINE uintptr_t GblArrayMap_atValue(GBL_PCSELF, uintptr_t key) GBL_NOEXCEPT {
    uintptr_t value = 0;
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(!pEntry) GBL_UNLIKELY {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);

        GBL_CTX_END_BLOCK();
    } else GBL_LIKELY {
        value = GblArrayMap_valueFromVariant_(&pEntry->value);
    }
    return value;
}
/*!
 * \brief GblArrayMap_atVariant
 * \param ppSelf
 * \param key
 * \return GblVariant associated with the given key, or NULL if it doesn't exist
 * \relatedalso GblArrayMap
 */
GBL_INLINE GblVariant* GblArrayMap_atVariant(GBL_PCSELF, uintptr_t key) GBL_NOEXCEPT {
    GblVariant* pVariant = GBL_NULL;
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(!pEntry) GBL_UNLIKELY {
        GBL_CTX_BEGIN(NULL);
        GBL_CTX_VERIFY(GBL_FALSE,
                       GBL_RESULT_ERROR_OUT_OF_RANGE);

        GBL_CTX_END_BLOCK();
    } else GBL_LIKELY {
        if(pEntry->value.type != GBL_INVALID_TYPE)
            pVariant = &pEntry->value;
    }
    return pVariant;
}
/*!
 * \brief GblArrayMap_find
 * \param ppSelf
 * \param key
 * \return Index of the entry with the given key, or GBL_ARRAY_MAP_NPOS if not found
 * \relatedalso GblArrayMap
 */
GBL_INLINE size_t  GblArrayMap_find(GBL_PCSELF, uintptr_t key) GBL_NOEXCEPT {
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
/*!
 * \brief GblArrayMap_probeKey
 * \param ppSelf
 * \param index
 * \return value of the entry at the given index or 0 if there isn't one
 * \relatedalso GblArrayMap
 */
GBL_INLINE uintptr_t GblArrayMap_probeKey(GBL_PCSELF, size_t  index) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    return pEntry? pEntry->key : 0;
}
/*!
 * \brief GblArrayMap_probeVariant
 * \param ppSelf
 * \param index
 * \return GblVariant of the entry at the given index or NULL if there isn't one
 * \relatedalso GblArrayMap
 */
GBL_INLINE GblVariant* GblArrayMap_probeVariant(GBL_PCSELF, size_t  index) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    return pEntry && pEntry->value.type != GBL_INVALID_TYPE? &pEntry->value : GBL_NULL;
}
/*!
 * \brief GblArrayMap_probeValue
 * \param ppSelf
 * \param index
 * \return value of the entry at the given index, or 0 if there isn't one
 * \relatedalso GblArrayMap
 */
GBL_INLINE uintptr_t GblArrayMap_probeValue(GBL_PCSELF, size_t  index) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_entry_(ppSelf, index);
    return pEntry? GblArrayMap_valueFromVariant_(&pEntry->value) : 0;
}
/*!
 * \brief GblArrayMap_setVariant
 * \param ppSelf
 * \param key
 * \param pVariant
 * \return result of the GblVariant_set operation
 * \relatedalso GblArrayMap
 */
GBL_INLINE GBL_RESULT GblArrayMap_setVariant(GBL_PSELF, uintptr_t key, GblVariant* pVariant) GBL_NOEXCEPT {
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
/*!
 * \brief GblArrayMap_setUserdata
 * \param ppSelf
 * \param key
 * \param value
 * \param pDtor
 * \return GBL_RESULT_SUCCESS or associated error code upon failure
 * \relatedalso GblArrayMap
 */
GBL_INLINE GBL_RESULT GblArrayMap_setUserdata(GBL_PSELF, uintptr_t key, uintptr_t value, GblArrayMapDtorFn pDtor) GBL_NOEXCEPT {
    GblArrayMapEntry* pEntry = GblArrayMap_find_entry_(ppSelf, key);
    if(!pEntry) {
        pEntry = GblArrayMap_entryAdd_(ppSelf, key);
    }
    return GblArrayMap_entrySetUserdata_(ppSelf, pEntry, value, pDtor);
}
/*!
 * \brief GblArrayMap_insertVariant
 * \param ppSelf
 * \param key
 * \param pVariant
 * \return Index the given variant was inserted at, or GBL_ARRAY_MAP_NPOS if it wasn't
 * \relatedalso GblArrayMap
 */
GBL_INLINE size_t  GblArrayMap_insertVariant(GBL_PSELF, uintptr_t key, GblVariant* pVariant) GBL_NOEXCEPT {
    size_t  index = GblArrayMap_find(ppSelf, key);
    if(index == GBL_ARRAY_MAP_NPOS) {
        GblArrayMapEntry* pEntry = GblArrayMap_entryAdd_(ppSelf, key);
        GblVariant_constructCopy(&pEntry->value, pVariant);
        index = GblArrayMap_size(ppSelf)-1;
    }
    return index;
}
/*!
 * \brief GblArrayMap_insertUserdata
 * \param ppSelf
 * \param key
 * \param value
 * \param pDtor
 * \return Index the given userdata value was inserted at, or GBL_ARRAY_MAP_NPOS if it wasn't
 * \relatedalso GblArrayMap
 */
GBL_INLINE size_t  GblArrayMap_insertUserdata(GBL_PSELF, uintptr_t key, uintptr_t value, GblArrayMapDtorFn pDtor) GBL_NOEXCEPT {
    size_t  index = GblArrayMap_find(ppSelf, key);
    if(index == GBL_ARRAY_MAP_NPOS) {
        GblArrayMapEntry* pEntry = GblArrayMap_entryAdd_(ppSelf, key);
        GblArrayMap_entrySetUserdata_(ppSelf, pEntry, value, pDtor);
        index = GblArrayMap_size(ppSelf)-1;
    }
    return index;
}

/*!
 * \fn GblArrayMap* GblArrayMap_create(GblArrayMapCmpFn pFnComparator, GblBool binarySearches, GblContext* pCtx)
 * \param pFnComparator
 * \param binarySearches
 * \param pCtx
 * \return pointer to a new GblArrayMap
 * \relatedalso GblArrayMap
 */
/*!
 * \fn GBL_RESULT GblArrayMap_destroy(GblArrayMap** ppSelf)
 * \param ppSelf
 * \return GBL_RESULT_SUCCESS or error code upon failure
 * \relatedalso GblArrayMap
 */
/*!
 * \fn GblBool GblArrayMap_erase(GblArrayMap** ppSelf, uintptr_t key)
 * \param ppSelf
 * \param key
 * \return GBL_TRUE if the entry with the given key was removed.
 * \relatedalso GblArrayMap
 */
/*!
 * \fn GblBool GblArrayMap_extractVariant(GblArrayMap** ppSelf, uintptr_t key, GblVariant* pVariant)
 * \param ppSelf
 * \param key
 * \param pVariant
 * \return GBL_TRUE if the given variant was extracted
 * \relatesalso GblArrayMap
 */
/*!
 * \fn GblBool GblArrayMap_extractValue(GblArrayMap** ppSelf, uintptr_t key, uintptr_t* pValue)
 * \param ppSelf
 * \param key
 * \param pValue
 * \return GBL_TRUE if the given value was successfully extracted
 * \relatedalso GblArrayMap
 */
/*!
 * \fn void GblArrayMap_clear(GblArrayMap** ppSelf)
 * \param ppSelf
 * \relatedalso GblArrayMap
 */


#undef GBL_PCSELF
#undef GBL_PSELF

#endif // GIMBAL_ARRAY_MAP_HPP
