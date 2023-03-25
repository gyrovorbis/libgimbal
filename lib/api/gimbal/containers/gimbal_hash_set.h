/*! \file
 *  \brief GblHashSet structure and related functions
 *  \ingroup containers
 */

#ifndef GIMBAL_HASHSET_H
#define GIMBAL_HASHSET_H

#include "../core/gimbal_typedefs.h"
#include "../core/gimbal_ctx.h"

#define GBL_SELF_TYPE GblHashSet

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblHashSet);

typedef GblHash (*GblHashSetHashFn)(GBL_CSELF, const void*);              ///< User-defined hasher function, hashing two entries
typedef GblBool (*GblHashSetCmpFn) (GBL_CSELF, const void*, const void*); ///< User-defined comparator function, comparing two entries
typedef void    (*GblHashSetDtorFn)(GBL_CSELF, void*);                    ///< User-defined destructor function, destroying an entry
typedef GblBool (*GblHashSetIterFn)(GBL_CSELF, void*, void*);             ///< User-defined iterator function for traversal

/*! \brief Hash-table based abstract associative container with C++-style STL std::unoredered_set API
 *  \details
 *  GblHashSet uses open-addressing ans is implemented using a Robin Hood hashing algorithm.
 *  Using it requires providing a custom hasher function (which typically uses one of
 *  the libGimbal hashing algorithms such as gblHashMurmur()) as well as a custom comparator function.
 *
 *  \note
 *  Performance is pretty darn good. Read speed is faster than both C++'s std::unordered_pSelf and
 *  Qt's QHash (despite being runtime polymorphic with C function pointers) while write speed
 *  is slightly slower than both.
 *
 *  \ingroup containers
 */
typedef struct GblHashSet {
    GBL_PRIVATE()
        GblContext*        pCtx;
        size_t             entrySize;
        size_t             capacity;
        GblHashSetHashFn   pFnHash;
        GblHashSetCmpFn    pFnCompare;
        GblHashSetDtorFn   pFnDestruct;
        size_t             bucketSize;
        size_t             bucketCount;
        size_t             count;
        size_t             mask;
        void*              pBuckets;
        void*              pSpare;
        void*              pUserdata;
    GBL_PRIVATE_END
} GblHashSet;

/*! Iterator structure used for iterating over GblHashSet.
 * \sa GblHashSet
 */
typedef struct GblHashSetIter {
    GBL_PRIVATE()
        GblHashSet*        pSet;
        size_t             bucketIdx;
    GBL_PRIVATE_END
} GblHashSetIter;

GBL_EXPORT GBL_RESULT        GblHashSet_construct_8   (GBL_SELF,
                                                       size_t             entrySize,
                                                       GblHashSetHashFn   pFnHash,
                                                       GblHashSetCmpFn    pFnCompare,
                                                       GblHashSetDtorFn   pFnDestruct,
                                                       size_t             capacity,
                                                       GblContext*        pCtx,
                                                       void*              pUserdata)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblHashSet_construct_7   (GBL_SELF,
                                                       size_t             entrySize,
                                                       GblHashSetHashFn   pFnHash,
                                                       GblHashSetCmpFn    pFnCompare,
                                                       GblHashSetDtorFn   pFnDestruct,
                                                       size_t             capacity,
                                                       GblContext*        pCtx)         GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblHashSet_construct_6   (GBL_SELF,
                                                       size_t             entrySize,
                                                       GblHashSetHashFn   pFnHash,
                                                       GblHashSetCmpFn    pFnCompare,
                                                       GblHashSetDtorFn   pFnDestruct,
                                                       size_t             capacity)     GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblHashSet_construct_5   (GBL_SELF,
                                                       size_t             entrySize,
                                                       GblHashSetHashFn   pFnHash,
                                                       GblHashSetCmpFn    pFnCompare,
                                                       GblHashSetDtorFn   pFnDestruct)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblHashSet_construct_4   (GBL_SELF,
                                                       size_t             entrySize,
                                                       GblHashSetHashFn   pFnHash,
                                                       GblHashSetCmpFn    pFnCompare)   GBL_NOEXCEPT;
#define                      GblHashSet_construct(...) \
        GBL_VA_OVERLOAD_CALL(GblHashSet_construct, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)

GBL_EXPORT GBL_RESULT        GblHashSet_clone         (GBL_SELF,
                                                       const GblHashSet* pRhs,
                                                       GblContext*       hCtx)          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT        GblHashSet_constructMove (GBL_SELF,
                                                       GblHashSet* pRhs,
                                                       GblContext* hCtx)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT        GblHashSet_constructClone(GBL_SELF, GblHashSet* pRhs)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblHashSet_assignMove    (GBL_SELF, GblHashSet* pRhs)      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT        GblHashSet_destruct      (GBL_SELF)                        GBL_NOEXCEPT;

GBL_INLINE size_t            GblHashSet_size          (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_INLINE size_t            GblHashSet_bucketCount   (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_INLINE size_t            GblHashSet_bucketSize    (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_INLINE GblContext*       GblHashSet_context       (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_INLINE GblBool           GblHashSet_empty         (GBL_CSELF)                       GBL_NOEXCEPT;
GBL_INLINE void*             GblHashSet_userdata      (GBL_CSELF)                       GBL_NOEXCEPT;

GBL_INLINE void*             GblHashSet_get           (GBL_CSELF, const void* pKey)     GBL_NOEXCEPT; //raw get, no error
GBL_INLINE void*             GblHashSet_at            (GBL_CSELF, const void* pKey)     GBL_NOEXCEPT; //throws range error
GBL_INLINE GblBool           GblHashSet_contains      (GBL_CSELF, const void* pKey)     GBL_NOEXCEPT; //true if entry exists
GBL_INLINE size_t            GblHashSet_count         (GBL_CSELF, const void* pKey)     GBL_NOEXCEPT; //# of matching entries (0 or 1)
GBL_INLINE GblHashSetIter    GblHashSet_find          (GBL_CSELF, const void* pKey)     GBL_NOEXCEPT;

GBL_EXPORT void*             GblHashSet_set           (GBL_SELF, const void* pEntry)    GBL_NOEXCEPT; //raw set, returns existing item w/o deleting
GBL_INLINE GblBool           GblHashSet_insert        (GBL_SELF, const void* pEntry)    GBL_NOEXCEPT; //throws duplicate error
GBL_INLINE void              GblHashSet_insertOrAssign(GBL_SELF, const void* pEntry)    GBL_NOEXCEPT; //deletes any overwritten value
GBL_EXPORT void*             GblHashSet_emplace       (GBL_SELF, const void* pKey)      GBL_NOEXCEPT; //throws duplicate error
GBL_INLINE void*             GblHashSet_tryEmplace    (GBL_SELF, const void* pKey)      GBL_NOEXCEPT; //gracefully returns NULL if already exists

GBL_EXPORT GblBool           GblHashSet_erase         (GBL_SELF, const void* pKey)      GBL_NOEXCEPT; //deletes entry, not found is fine
GBL_EXPORT void*             GblHashSet_extract       (GBL_SELF, const void* pKey)      GBL_NOEXCEPT; //removes entry, no deletion, not found is fine
GBL_EXPORT void              GblHashSet_clear         (GBL_SELF)                        GBL_NOEXCEPT; //deletes entries

GBL_EXPORT GBL_RESULT        GblHashSet_shrinkToFit   (GBL_SELF)                        GBL_NOEXCEPT;

GBL_EXPORT void*             GblHashSet_probe         (GBL_CSELF, size_t  position)     GBL_NOEXCEPT; //returns entry at slot or NULL, sparse

GBL_EXPORT GblBool           GblHashSet_foreach       (GBL_CSELF,
                                                       GblHashSetIterFn iter,
                                                       void*            pUdata)         GBL_NOEXCEPT; //iterates over every non-null position

GBL_EXPORT GblHashSetIter    GblHashSet_next          (GBL_CSELF,
                                                       const GblHashSetIter* pPrev)     GBL_NOEXCEPT; //[Lua-style] Returns iterator after given
GBL_EXPORT const GblHashSet* GblHashSetIter_container (const GblHashSetIter* pSelf)     GBL_NOEXCEPT;
GBL_EXPORT GblBool           GblHashSetIter_valid     (const GblHashSetIter* pSelf)     GBL_NOEXCEPT;
GBL_EXPORT void*             GblHashSetIter_value     (const GblHashSetIter* pSelf)     GBL_NOEXCEPT;

// ======== INLINE IMPL ======

/// \cond
struct GblHashSetBucket_ {
    uint32_t hash;
    uint16_t dib;
};

GBL_INLINE struct GblHashSetBucket_* GblHashSet_bucketAt_(const struct GblHashSet *map, size_t index) GBL_NOEXCEPT {
    return (struct GblHashSetBucket_*)(((char*)GBL_PRIV_REF(map).pBuckets)+(GBL_PRIV_REF(map).bucketSize*index));
}

GBL_INLINE void *GblHashSet_bucketItem_(struct GblHashSetBucket_ *entry) GBL_NOEXCEPT {
    return ((char*)entry) + sizeof(struct GblHashSetBucket_);
}

GBL_INLINE uint32_t GblHashSet_getHash_(const struct GblHashSet *map, const void *key) GBL_NOEXCEPT {
    return GBL_PRIV_REF(map).pFnHash(map, key);
}
/// \endcond

GBL_INLINE void* GblHashSet_userdata(const GblHashSet* pSelf) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).pUserdata;
}

GBL_INLINE GblBool GblHashSet_empty(const GblHashSet* pSelf) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).count? GBL_FALSE : GBL_TRUE;
}

GBL_INLINE GblContext* GblHashSet_context(const GblHashSet* pSelf) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).pCtx;
}

GBL_INLINE size_t   GblHashSet_bucketSize(const GblHashSet* pSelf) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).bucketSize;
}

GBL_INLINE size_t  GblHashSet_size(const GblHashSet* pSelf) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).count;
}

GBL_INLINE size_t  GblHashSet_bucketCount(const GblHashSet* pSelf) GBL_NOEXCEPT {
    return GBL_PRIV_REF(pSelf).bucketCount;
}

// hashpSelf_get returns the item based on the provided key. If the item is not
// found then NULL is returned.
GBL_INLINE void* GblHashSet_get(const GblHashSet* pSelf, const void *pKey) GBL_NOEXCEPT {
    void* pEntry = NULL;
    if(!pKey) {
        GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
        GBL_CTX_VERIFY_POINTER(pKey);
        GBL_CTX_END_BLOCK();
    } else {
        const uint32_t hash = GblHashSet_getHash_(pSelf, pKey);
        size_t i = hash & GBL_PRIV_REF(pSelf).mask;
        for (;;) {
            struct GblHashSetBucket_ *bucket = GblHashSet_bucketAt_(pSelf, i);
            if (!bucket->dib) {
                break;
            }
            if (bucket->hash == hash &&
                GBL_PRIV_REF(pSelf).pFnCompare(pSelf, pKey, GblHashSet_bucketItem_(bucket)))
            {
                pEntry = GblHashSet_bucketItem_(bucket);
                break;
            }
            i = (i + 1) & GBL_PRIV_REF(pSelf).mask;
        }
    }
    return pEntry;
}

GBL_INLINE GblHashSetIter GblHashSet_find(const GblHashSet* map, const void* key) GBL_NOEXCEPT {
    GblHashSetIter it = {
        (GblHashSet*)map,
        GBL_PRIV_REF(map).bucketCount
    };

    GBL_ASSERT(key);
    uint32_t hash = GblHashSet_getHash_(map, key);
    size_t i = hash & GBL_PRIV_REF(map).mask;
    for (;;) {
        struct GblHashSetBucket_ *bucket = GblHashSet_bucketAt_(map, i);
        if (!bucket->dib) {
            break;
        }
        if (bucket->hash == hash &&
            GBL_PRIV_REF(map).pFnCompare(map, key, GblHashSet_bucketItem_(bucket)))
        {
            GBL_PRIV(it).bucketIdx = i;
            break;
        }
        i = (i + 1) & GBL_PRIV_REF(map).mask;
    }
    return it;

}

GBL_INLINE void* GblHashSet_at(const GblHashSet* pSet, const void* pKey) GBL_NOEXCEPT {
    void* pEntry = NULL;
    pEntry = GblHashSet_get(pSet, pKey);
    if(!pEntry) {
        GBL_CTX_BEGIN(GBL_PRIV_REF(pSet).pCtx);
        GBL_CTX_VERIFY(pEntry, GBL_RESULT_ERROR_OUT_OF_RANGE, "The requested key was not found in the hash map!");
        GBL_CTX_END_BLOCK();
    }
    return pEntry;
}

GBL_INLINE GblBool GblHashSet_contains(const GblHashSet* pSet, const void* pKey) GBL_NOEXCEPT {
    GblBool found = GBL_FALSE;
    if(GblHashSet_get(pSet, pKey)) found = GBL_TRUE;
    return found;
}

GBL_INLINE size_t  GblHashSet_count(const GblHashSet* pSet, const void* pKey) GBL_NOEXCEPT {
    size_t  count = 0;
    if(GblHashSet_get(pSet, pKey)) ++count;
    return count;
}


GBL_INLINE GblBool GblHashSet_insert(GblHashSet* pSet, const void* pItem) GBL_NOEXCEPT {
    GblBool inserted = GBL_FALSE;
    {
        void* pValue = GblHashSet_get(pSet, pItem);
        if(!pValue) {
            pValue = GblHashSet_set(pSet, pItem);
            if(!pValue) {
                GBL_CTX_BEGIN(GBL_PRIV_REF(pSet).pCtx);
                GBL_CTX_VERIFY_EXPRESSION(!pValue, "Somehow the item wasn't found in the map before inserting it, but it was there!");
                GBL_CTX_END();
            }
            inserted = GBL_TRUE;
        }
    }
    return inserted;
}

GBL_INLINE void GblHashSet_insertOrAssign(GblHashSet* pSet, const void* pEntry) GBL_NOEXCEPT {
    if(!pEntry) {
        GBL_CTX_BEGIN(GBL_PRIV_REF(pSet).pCtx);
        GBL_CTX_VERIFY_POINTER(pEntry);
        GBL_CTX_END_BLOCK();
    } else {
        void* pOldEntry = GblHashSet_set(pSet, pEntry);
        if(pOldEntry && GBL_PRIV_REF(pSet).pFnDestruct) GBL_PRIV_REF(pSet).pFnDestruct(pSet, pOldEntry);
    }
}

GBL_INLINE void* GblHashSet_tryEmplace(GblHashSet* pSet, const void* pKey) GBL_NOEXCEPT {
    void* pPrevEntry = NULL;
    if(!GblHashSet_contains(pSet, pKey)) {
        pPrevEntry = GblHashSet_emplace(pSet, pKey);
        if(!pPrevEntry) {
            GBL_CTX_BEGIN(GBL_PRIV_REF(pSet).pCtx);
            GBL_CTX_VERIFY_EXPRESSION(pPrevEntry, "Fetching the previous entry should've succeeded after 'contains(key)' returned true!");
            GBL_CTX_END_BLOCK();
        }
    }
    return pPrevEntry;
}


GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_HASHSET_H
