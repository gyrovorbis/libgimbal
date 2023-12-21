/*! \file
 *  \brief ::GblArrayMap container and related functions
 *  \ingroup containers
 *  \copydoc GblArrayMap
 *
 *  \bug
 *      - Extracting the last item will still invoke its destructor!
 *
 *  \author Falco Girgis
 */
#ifndef GIMBAL_ARRAY_MAP_H
#define GIMBAL_ARRAY_MAP_H

#include "../core/gimbal_typedefs.h"
#include "../strings/gimbal_quark.h"
#include "../meta/types/gimbal_variant.h"
#include "../core/gimbal_result.h"

#define GBL_ARRAY_MAP_NPOS                      GBL_NPOS                                                        //!< Invalid index identifier returned when an entry couldn't be found
#define GBL_ARRAY_MAP_SIZE(elements)            (sizeof(GblArrayMap) + (sizeof(GblArrayMapEntry)*(elements)))   //!< Calculates total size of the GblArrayMap structure with elements
#define GBL_ARRAY_MAP_BINARY_SEARCH_CUTOFF_SIZE 40                                                              //!< Magical break-even point for when binary searches out-perform linear searches, based on profiling

#define GBL_PSELF    GblArrayMap** ppSelf
#define GBL_PCSELF   GblArrayMap*const* ppSelf

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblArrayMapEntry);
GBL_FORWARD_DECLARE_STRUCT(GblArrayMap);

typedef GBL_RESULT  (*GblArrayMapDtorFn)(const GblArrayMap*, uintptr_t key, void* pEntry);      //!< Custom destructor type for userdata values
typedef int         (*GblArrayMapCmpFn) (const GblArrayMap*, uintptr_t key1, uintptr_t key2);   //!< Custom comparator for nontrivial key types

/*! Represents a single K,V data entry within a GblArrayMap
 *
 *  \note
 *  The destructor field is only used for userdata values, while
 *  GblVariant values have automatic lifetime management.
 *
 *  \sa GblArrayMap
 */
typedef struct GblArrayMapEntry {
    uintptr_t               key;   //!< Opaque key value
    GblVariant              value; //!< Using value.type == GBL_INVALID_TYPE to signify userdata
    GblArrayMapDtorFn       dtor;  //!< Optional custom destructor for userdata types
} GblArrayMapEntry;

/*! Dynamic array-based [K,V] pair associative container
 *  \ingroup containers
 *
 *  Contiguous array-based associative container with [K,V] pairs.
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
 *
 *  \sa GblArrayMapEntry
 */
typedef struct GblArrayMap {
    GBL_PRIVATE_BEGIN
        GblContext*             pCtx;                   //!< Optional custom context
        GblArrayMapCmpFn        pFnComparator;          //!< Optional custom comparator
        size_t                  binarySearches  : 1;    //!< Optionally sort values and use binary searches
#ifdef GBL_64BIT
        size_t                  size            : 63;   //!< Number of elements within the map
#elif defined(GBL_32BIT)
        size_t                  size            : 31;
#endif
    GBL_PRIVATE_END
} GblArrayMap;

//! Pre-creates a GblArrayMap with the given comparator, binary search config, and context.
GBL_EXPORT GblArrayMap* GblArrayMap_create          (GblArrayMapCmpFn pFnComp,
                                                     GblBool          binarySearch,
                                                     GblContext*      pCtx)        GBL_NOEXCEPT;
//! Destroys a GblArrayMap.
GBL_EXPORT GBL_RESULT   GblArrayMap_destroy         (GBL_PSELF)                    GBL_NOEXCEPT;
//! Returns the number of entries within the given map.
GBL_EXPORT size_t       GblArrayMap_size            (GBL_PCSELF)                   GBL_NOEXCEPT;
//! Returns the context associated with the given map.
GBL_EXPORT GblContext*  GblArrayMap_context         (GBL_PCSELF)                   GBL_NOEXCEPT;
//! Returns whether the given map is empty.
GBL_EXPORT GblBool      GblArrayMap_empty           (GBL_PCSELF)                   GBL_NOEXCEPT;
//! Returns whether the given map is sorted and uses binary searches.
GBL_EXPORT GblBool      GblArrayMap_binarySearches  (GBL_PCSELF)                   GBL_NOEXCEPT;
//! Returns true if the given map contains the given key.
GBL_EXPORT GblBool      GblArrayMap_contains        (GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
//! Returns true if the given map contains the given key, associated with a userdata value.
GBL_EXPORT GblBool      GblArrayMap_containsUserdata(GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
//! Returns true if the given map contains the given key, associated with a GblVariant value.
GBL_EXPORT GblBool      GblArrayMap_containsVariant (GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
//! Returns the GblVariant or userdata value associated with the given key as a uintptr_t.
GBL_EXPORT uintptr_t    GblArrayMap_getValue        (GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
//! Returns the GblVariant associated with the given key.
GBL_EXPORT GblVariant*  GblArrayMap_getVariant      (GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
//! Returns the GblVariant or userdata value associated with the given key, erroring if not found.
GBL_EXPORT uintptr_t    GblArrayMap_atValue         (GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
//! Returns the GblVariant associated with the given key, erroring if not found.
GBL_EXPORT GblVariant*  GblArrayMap_atVariant       (GBL_PCSELF, uintptr_t key)    GBL_NOEXCEPT;
//! Inserts or replaces the entry with the given key with a userdata value and optional destructor.
GBL_EXPORT GBL_RESULT   GblArrayMap_setUserdata     (GBL_PSELF,
                                                     uintptr_t key,
                                                     uintptr_t value,
                                                     GblArrayMapDtorFn pDtor)     GBL_NOEXCEPT;
//! Inserts or replaces the entry with the given key with a GblVariant.
GBL_EXPORT GBL_RESULT   GblArrayMap_setVariant      (GBL_PSELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)        GBL_NOEXCEPT;
//! Attempts to insert a new entry with the given key and userdata, returning the insertion index.
GBL_EXPORT size_t       GblArrayMap_insertUserdata  (GBL_PSELF,
                                                     uintptr_t key,
                                                     uintptr_t value,
                                                     GblArrayMapDtorFn pDtor)     GBL_NOEXCEPT;
//! Attempts to insert a new entry with the given key and GblVariant value, returning insertion index.
GBL_EXPORT size_t       GblArrayMap_insertVariant   (GBL_PSELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)        GBL_NOEXCEPT;
//! Attempts to erase the value with the given key, returning GBL_FALSE if not found.
GBL_EXPORT GblBool      GblArrayMap_erase           (GBL_PSELF, uintptr_t key)    GBL_NOEXCEPT;
//! Attempts to remove the GblVariant with the given key, moving it into the given pointer if found.
GBL_EXPORT GblBool      GblArrayMap_extractVariant  (GBL_PSELF,
                                                     uintptr_t key,
                                                     GblVariant* pVariant)        GBL_NOEXCEPT;
//! Attempts to remove the value with the given key, moving it into the given pointer if found.
GBL_EXPORT GblBool      GblArrayMap_extractValue    (GBL_PSELF,
                                                     uintptr_t key,
                                                     uintptr_t* pValue)           GBL_NOEXCEPT;
//! Clears all entries within the given map.
GBL_EXPORT void         GblArrayMap_clear           (GBL_PSELF)                   GBL_NOEXCEPT;
//! Returns the index of the entry with the given key.
GBL_EXPORT size_t       GblArrayMap_find            (GBL_PCSELF, uintptr_t key)   GBL_NOEXCEPT;
//! Returns the key for the entry at the given index.
GBL_EXPORT uintptr_t    GblArrayMap_probeKey        (GBL_PCSELF, size_t  index)   GBL_NOEXCEPT;
//! Returns the value for the entry at the given index.
GBL_EXPORT uintptr_t    GblArrayMap_probeValue      (GBL_PCSELF, size_t  index)   GBL_NOEXCEPT;
//! Returns the GblVariant for the entry at the given index.
GBL_EXPORT GblVariant*  GblArrayMap_probeVariant    (GBL_PCSELF, size_t  index)   GBL_NOEXCEPT;

GBL_DECLS_END

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
