#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/algorithms/gimbal_numeric.h>

struct GblHashSetBucket_ {
    uint32_t hash;
    uint16_t dib;
};

GBL_INLINE struct GblHashSetBucket_* GblHashSet_bucketAt_(const struct GblHashSet *map, size_t index) {
    return (struct GblHashSetBucket_*)(((char*)GBL_PRIV_REF(map).pBuckets)+(GBL_PRIV_REF(map).bucketSize*index));
}

GBL_INLINE void *GblHashSet_bucketItem_(struct GblHashSetBucket_ *entry) {
    return ((char*)entry) + sizeof(struct GblHashSetBucket_);
}

GBL_INLINE uint32_t GblHashSet_getHash_(const struct GblHashSet *map, const void *key) {
    return GBL_PRIV_REF(map).pFnHash(map, key);
}
/// \endcond


GBL_EXPORT GBL_RESULT GblHashSet_construct_8(GblHashSet*      pSet,
                                             size_t           elsize,
                                             GblHashSetHashFn pFnHash,
                                             GblHashSetCmpFn  pFnCompare,
                                             GblHashSetDtorFn pFnDestruct,
                                             size_t           capacity,
                                             GblContext*      pCtx,
                                             void*            pUserdata)
{

    GBL_CTX_BEGIN(pCtx);
    {
        GBL_CTX_VERIFY_POINTER(pSet);
        size_t  ncap = 16;
        if (capacity < ncap) {
            capacity = ncap;
        } else {
            while (ncap < capacity) {
                ncap *= 2;
            }
            capacity = ncap;
        }
        size_t bucketsz = sizeof(struct GblHashSetBucket_) + elsize;
        while (bucketsz & (sizeof(uintptr_t)-1)) {
            bucketsz++;
        }
        // hashmap + pSpare + edata
        memset(pSet, 0, sizeof(GblHashSet));
        GBL_PRIV_REF(pSet).pCtx           = pCtx;
        GBL_PRIV_REF(pSet).pUserdata      = pUserdata;
        GBL_PRIV_REF(pSet).entrySize      = elsize;
        GBL_PRIV_REF(pSet).bucketSize     = bucketsz;
        GBL_PRIV_REF(pSet).pFnHash        = pFnHash;
        GBL_PRIV_REF(pSet).pFnCompare     = pFnCompare;
        GBL_PRIV_REF(pSet).pFnDestruct    = pFnDestruct;
        GBL_PRIV_REF(pSet).pSpare         = GBL_CTX_MALLOC(GBL_PRIV_REF(pSet).bucketSize);
        GBL_PRIV_REF(pSet).capacity       = capacity;
        GBL_PRIV_REF(pSet).bucketCount    = capacity;
        GBL_PRIV_REF(pSet).mask           = GBL_PRIV_REF(pSet).bucketCount-1;
        GBL_PRIV_REF(pSet).pBuckets       = GBL_CTX_MALLOC(GBL_PRIV_REF(pSet).bucketSize*GBL_PRIV_REF(pSet).bucketCount);
        memset(GBL_PRIV_REF(pSet).pBuckets, 0, GBL_PRIV_REF(pSet).bucketSize*GBL_PRIV_REF(pSet).bucketCount);
    }
    GBL_CTX_END();

}
GBL_EXPORT GBL_RESULT             GblHashSet_construct_7(GblHashSet*                 pSet,
                                           size_t                      entrySize,
                                           GblHashSetHashFn       pFnHash,
                                           GblHashSetCmpFn    pFnCompare,
                                           GblHashSetDtorFn   pFnDestruct,
                                           size_t                      capacity,
                                           GblContext*                 pCtx)  {
    return GblHashSet_construct_8(pSet, entrySize, pFnHash, pFnCompare, pFnDestruct, capacity, pCtx, NULL);
}

GBL_EXPORT GBL_RESULT             GblHashSet_construct_6(GblHashSet*                 pSet,
                                           size_t                      entrySize,
                                           GblHashSetHashFn       pFnHash,
                                           GblHashSetCmpFn    pFnCompare,
                                           GblHashSetDtorFn   pFnDestruct,
                                           size_t                      capacity)
{
    return GblHashSet_construct_7(pSet, entrySize, pFnHash, pFnCompare, pFnDestruct, capacity, NULL);
}

GBL_EXPORT GBL_RESULT             GblHashSet_construct_5(GblHashSet*                 pSet,
                                           size_t                      entrySize,
                                           GblHashSetHashFn       pFnHash,
                                           GblHashSetCmpFn    pFnCompare,
                                           GblHashSetDtorFn   pFnDestruct)
{
    return GblHashSet_construct_6(pSet, entrySize, pFnHash, pFnCompare, pFnDestruct, 0);
}

GBL_EXPORT GBL_RESULT             GblHashSet_construct_4(GblHashSet*                 pSet,
                                           size_t                      entrySize,
                                           GblHashSetHashFn       pFnHash,
                                           GblHashSetCmpFn    pFnCompare)  {
    return GblHashSet_construct_5(pSet, entrySize, pFnHash, pFnCompare, NULL);
}


GBL_EXPORT GBL_RESULT  GblHashSet_clone(GblHashSet* pSelf, const GblHashSet* pRhs, GblContext* pCtx)  {
    if(!pCtx) pCtx = GBL_PRIV_REF(pRhs).pCtx;
    GBL_CTX_BEGIN(pCtx);
    GBL_CTX_CALL(GblHashSet_construct_8(pSelf,
                                       GBL_PRIV_REF(pRhs).entrySize,
                                       GBL_PRIV_REF(pRhs).pFnHash,
                                       GBL_PRIV_REF(pRhs).pFnCompare,
                                       GBL_PRIV_REF(pRhs).pFnDestruct,
                                       GBL_PRIV_REF(pRhs).capacity,
                                       pCtx,
                                       GBL_PRIV_REF(pRhs).pUserdata));

    for(size_t  s = 0; s < GBL_PRIV_REF(pRhs).bucketCount; ++s) {
        void* pEntry = GblHashSet_probe(pRhs, s);
        if(pEntry) {
            GblHashSet_insert(pSelf, pEntry);
        }
    }

    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblHashSet_constructClone(GblHashSet* pSelf, GblHashSet* pRhs) {
    GBL_CTX_BEGIN(GBL_PRIV_REF(pRhs).pCtx);
    GblHashSet_destruct(pSelf);
    GBL_CTX_CALL(GblHashSet_clone(pSelf, pRhs, NULL));
    GBL_CTX_END();
}

GBL_EXPORT GBL_RESULT GblHashSet_constructMove(GblHashSet* pSelf, GblHashSet* pRhs, GblContext* pCtx)  {
    if(pCtx != GBL_PRIV_REF(pRhs).pCtx && pCtx != NULL) {
        return GblHashSet_clone(pSelf, pRhs, pCtx);
    } else {
        memcpy(pSelf, pRhs, sizeof(GblHashSet));
        memset(pRhs, 0, sizeof(GblHashSet));
        return GBL_RESULT_SUCCESS;
    }
}

GBL_EXPORT GBL_RESULT GblHashSet_assignMove(GblHashSet* pLhs, GblHashSet* pRhs)  {
    GblHashSet_destruct(pLhs);
    return GblHashSet_constructMove(pLhs, pRhs, NULL);
}

static void free_elements(GblHashSet *map) {
    if (GBL_PRIV_REF(map).pFnDestruct) {
        for (size_t i = 0; i < GBL_PRIV_REF(map).bucketCount; i++) {
            struct GblHashSetBucket_ *bucket = GblHashSet_bucketAt_(map, i);
            if (bucket->dib) GBL_PRIV_REF(map).pFnDestruct(map, GblHashSet_bucketItem_(bucket));
        }
    }
}

// hashmap_clear quickly clears the map.
// Every item is called with the element-freeing function given in hashmap_new,
// if present, to free any data referenced in the elements of the hashmap.
// When the update_cap is provided, the map's capacity will be updated to match
// the currently number of allocated buckets. This is an optimization to ensure
// that this operation does not perform any allocations.
GBL_EXPORT void GblHashSet_clear(GblHashSet *map)  {
    if(!GBL_PRIV_REF(map).count) return;
    GblBool update_cap = GBL_TRUE;
    GBL_CTX_BEGIN(GBL_PRIV_REF(map).pCtx);
    GBL_PRIV_REF(map).count = 0;
    free_elements(map);
    if (update_cap) {
        GBL_PRIV_REF(map).capacity = GBL_PRIV_REF(map).bucketCount;
    } else if (GBL_PRIV_REF(map).bucketCount != GBL_PRIV_REF(map).capacity) {
        void *new_buckets = GBL_CTX_MALLOC(GBL_PRIV_REF(map).bucketSize*GBL_PRIV_REF(map).capacity);
        if (new_buckets) {
            GBL_CTX_FREE(GBL_PRIV_REF(map).pBuckets);
            GBL_PRIV_REF(map).pBuckets = new_buckets;
        }
        GBL_PRIV_REF(map).bucketCount = GBL_PRIV_REF(map).capacity;
    }
    memset(GBL_PRIV_REF(map).pBuckets, 0, GBL_PRIV_REF(map).bucketSize*GBL_PRIV_REF(map).bucketCount);
    GBL_PRIV_REF(map).mask = GBL_PRIV_REF(map).bucketCount-1;
    GBL_CTX_END_BLOCK();
}

static GblBool resize(struct GblHashSet *map, size_t new_cap) {
    GBL_CTX_BEGIN(GBL_PRIV_REF(map).pCtx); {
        GblHashSet _map2;
        GblHashSet* map2 = &_map2;
        GBL_CTX_CALL(GblHashSet_construct(map2,
                                          GBL_PRIV_REF(map).entrySize,
                                          GBL_PRIV_REF(map).pFnHash,
                                          GBL_PRIV_REF(map).pFnCompare,
                                          GBL_PRIV_REF(map).pFnDestruct,
                                          new_cap,
                                          GBL_PRIV_REF(map).pCtx,
                                          GBL_PRIV_REF(map).pUserdata));

        for (size_t i = 0; i < GBL_PRIV_REF(map).bucketCount; i++) {
            struct GblHashSetBucket_ *entry = GblHashSet_bucketAt_(map, i);
            if (!entry->dib) {
                continue;
            }
            entry->dib = 1;
            size_t j = entry->hash & GBL_PRIV_REF(map2).mask;
            for (;;) {
                struct GblHashSetBucket_ *bucket = GblHashSet_bucketAt_(map2, j);
                if (bucket->dib == 0) {
                    memcpy(bucket, entry, GBL_PRIV_REF(map).bucketSize);
                    break;
                }
                if (bucket->dib < entry->dib) {
                    memcpy(GBL_PRIV_REF(map2).pSpare, bucket, GBL_PRIV_REF(map).bucketSize);
                    memcpy(bucket, entry, GBL_PRIV_REF(map).bucketSize);
                    memcpy(entry, GBL_PRIV_REF(map2).pSpare, GBL_PRIV_REF(map).bucketSize);
                }
                j = (j + 1) & GBL_PRIV_REF(map2).mask;
                entry->dib += 1;
            }
        }
        GBL_CTX_FREE(GBL_PRIV_REF(map).pBuckets);
        GBL_PRIV_REF(map).pBuckets = GBL_PRIV_REF(map2).pBuckets;
        GBL_PRIV_REF(map).bucketCount = GBL_PRIV_REF(map2).bucketCount;
        GBL_PRIV_REF(map).mask = GBL_PRIV_REF(map2).mask;
        GBL_CTX_FREE(GBL_PRIV_REF(map2).pSpare);
    }
    GBL_CTX_END_BLOCK();
    return GBL_TRUE;
}

GBL_EXPORT GBL_RESULT GblHashSet_shrinkToFit(GblHashSet* pSelf)  {
    GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    if(GBL_PRIV_REF(pSelf).count < GBL_PRIV_REF(pSelf).bucketCount * 0.75) {
        GBL_CTX_VERIFY_EXPRESSION(resize(pSelf, GBL_PRIV_REF(pSelf).bucketCount*0.75));
    }
    GBL_CTX_END();
}


static void* GblHashSet_rawSet_(GblHashSet* map, const void* item, void** ppNewEntry)  {
    void* pPrevItem = NULL;

    void* edata = GBL_ALLOCA(GBL_PRIV_REF(map).bucketSize);

    GBL_ASSERT(item);

    if (GBL_PRIV_REF(map).count == GBL_PRIV_REF(map).bucketCount*0.75) {
        GblBool result = resize(map, GBL_PRIV_REF(map).bucketCount*2);
        if(!result) {
            GBL_CTX_BEGIN(GBL_PRIV_REF(map).pCtx);
            GBL_CTX_VERIFY(result, GBL_RESULT_ERROR_MEM_REALLOC);
            GBL_CTX_END_BLOCK();
            goto done;
        }
    }

    struct GblHashSetBucket_ *entry = edata;
    entry->hash = GblHashSet_getHash_(map, item);
    entry->dib = 1;
    memcpy(GblHashSet_bucketItem_(entry), item, GBL_PRIV_REF(map).entrySize);

    unsigned swaps = 0;
    size_t i = entry->hash & GBL_PRIV_REF(map).mask;
    for (;;) {
        struct GblHashSetBucket_ *bucket = GblHashSet_bucketAt_(map, i);
        if (bucket->dib == 0) {
            memcpy(bucket, entry, GBL_PRIV_REF(map).bucketSize);
            if(ppNewEntry && !swaps) {
                *ppNewEntry = GblHashSet_bucketItem_(bucket);
            }
            GBL_PRIV_REF(map).count++;
            break;
        }
        if (entry->hash == bucket->hash && GBL_PRIV_REF(map).pFnCompare(map, GblHashSet_bucketItem_(entry), GblHashSet_bucketItem_(bucket)))
        {
            memcpy(GBL_PRIV_REF(map).pSpare, GblHashSet_bucketItem_(bucket), GBL_PRIV_REF(map).entrySize);
            memcpy(GblHashSet_bucketItem_(bucket), GblHashSet_bucketItem_(entry), GBL_PRIV_REF(map).entrySize);
            if(ppNewEntry) *ppNewEntry = GblHashSet_bucketItem_(bucket);
            pPrevItem = GBL_PRIV_REF(map).pSpare;
            break;
        }
        if (bucket->dib < entry->dib) {
            memcpy(GBL_PRIV_REF(map).pSpare, bucket, GBL_PRIV_REF(map).bucketSize);
            memcpy(bucket, entry, GBL_PRIV_REF(map).bucketSize);
            memcpy(entry, GBL_PRIV_REF(map).pSpare, GBL_PRIV_REF(map).bucketSize);
            if(!swaps && ppNewEntry) {
                *ppNewEntry = GblHashSet_bucketItem_(bucket);
                ++swaps;
            }
        }
        i = (i + 1) & GBL_PRIV_REF(map).mask;
        entry->dib += 1;
    }
done:
    return pPrevItem;
}

GBL_EXPORT void* GblHashSet_emplace(GblHashSet* pSet, const void* pKey)  {
    void* pNewEntry = NULL;
    GblHashSet_rawSet_(pSet, pKey, &pNewEntry);
    if(!pNewEntry) {
        GBL_CTX_BEGIN(GBL_PRIV_REF(pSet).pCtx);
        GBL_CTX_VERIFY_EXPRESSION(pNewEntry, "Somehow we did a set and didn't get a new entry?");
        //GBL_CTX_VERIFY(!pOldEntry, GBL_RESULT_ERROR_OVERFLOW, "Could not emplace over existing hash map entry!");
        GBL_CTX_END_BLOCK();
    }
    return pNewEntry;
}


// hashmap_set inserts or replaces an item in the hash map. If an item is
// replaced then it is returned otherwise NULL is returned. This operation
// may allocate memory. If the system is unable to allocate additional
// memory then NULL is returned and hashmap_oom() returns true.
GBL_EXPORT void* GblHashSet_set(GblHashSet *map, const void *item)  {
    return GblHashSet_rawSet_(map, item, NULL);
}

// hashmap_probe returns the item in the bucket at position or NULL if an item
// is not set for that bucket. The position is 'moduloed' by the number of
// buckets in the hashmap.
GBL_EXPORT void* GblHashSet_probe(const GblHashSet *map, size_t  position)  {
    size_t i = position & GBL_PRIV_REF(map).mask;
    struct GblHashSetBucket_ *bucket = GblHashSet_bucketAt_(map, i);
    if (!bucket->dib) {
        return NULL;
    }
    return GblHashSet_bucketItem_(bucket);
}


GBL_EXPORT GblBool GblHashSet_erase(GblHashSet* pSet, const void* pKey) {
    GblBool removed = GBL_FALSE;
    void* pExtracted = NULL;
    pExtracted = GblHashSet_extract(pSet, pKey);
    if(pExtracted) {
        if(GBL_PRIV_REF(pSet).pFnDestruct) GBL_PRIV_REF(pSet).pFnDestruct(pSet, pExtracted);
        removed = GBL_TRUE;
    }
    return removed;
}

// hashmap_delete removes an item from the hash map and returns GBL_PRIV(it). If the
// item is not found then NULL is returned.
GBL_EXPORT void* GblHashSet_extract(GblHashSet *map, const void* pKey)  {
    void* pEntry = NULL;

    GBL_ASSERT(pKey);

    uint32_t hash = GblHashSet_getHash_(map, pKey);
    size_t i = hash & GBL_PRIV_REF(map).mask;
    for (;;) {
        struct GblHashSetBucket_ *bucket = GblHashSet_bucketAt_(map, i);
        if (!bucket->dib) {
            break;
        }
        if (bucket->hash == hash && GBL_PRIV_REF(map).pFnCompare(map, pKey, GblHashSet_bucketItem_(bucket)))
        {
            memcpy(GBL_PRIV_REF(map).pSpare, GblHashSet_bucketItem_(bucket), GBL_PRIV_REF(map).entrySize);
            bucket->dib = 0;
            for (;;) {
                struct GblHashSetBucket_ *prev = bucket;
                i = (i + 1) & GBL_PRIV_REF(map).mask;
                bucket = GblHashSet_bucketAt_(map, i);
                if (bucket->dib <= 1) {
                    prev->dib = 0;
                    break;
                }
                memcpy(prev, bucket, GBL_PRIV_REF(map).bucketSize);
                prev->dib--;
            }
            GBL_PRIV_REF(map).count--;
            if (GBL_PRIV_REF(map).bucketCount > GBL_PRIV_REF(map).capacity && GBL_PRIV_REF(map).count <= GBL_PRIV_REF(map).bucketCount*0.1) {
                // Ignore the return value. It's ok for the resize operation to
                // fail to allocate enough memory because a shrink operation
                // does not change the integrity of the data.
                resize(map, GBL_PRIV_REF(map).bucketCount/2);
            }
            pEntry = GBL_PRIV_REF(map).pSpare;
            break;
        }
        i = (i + 1) & GBL_PRIV_REF(map).mask;
    }
    return pEntry;
}

// hashmap_free frees the hash map
// Every item is called with the element-freeing function given in hashmap_new,
// if present, to free any data referenced in the elements of the hashmap.
GBL_EXPORT GBL_RESULT GblHashSet_destruct(GblHashSet *map)  {
    //if (!map) return
    GBL_CTX_BEGIN(GBL_PRIV_REF(map).pCtx);
    free_elements(map);
    GBL_CTX_FREE(GBL_PRIV_REF(map).pBuckets);
    GBL_CTX_FREE(GBL_PRIV_REF(map).pSpare);
    GBL_CTX_END();
}

// hashmap_scan iterates over all items in the hash map
// Param `iter` can return false to stop iteration early.
// Returns false if the iteration has been stopped early.
GBL_EXPORT GblBool GblHashSet_foreach(const GblHashSet *map,
                  GblHashSetIterFn iter, void* udata)
{
    for (size_t i = 0; i < GBL_PRIV_REF(map).bucketCount; i++) {
        struct GblHashSetBucket_ *bucket = GblHashSet_bucketAt_(map, i);
        if (bucket->dib) {
            if (!iter(map, GblHashSet_bucketItem_(bucket), udata)) {
                return GBL_FALSE;
            }
        }
    }
    return GBL_TRUE;
}

GBL_EXPORT GblHashSetIter GblHashSet_next(const GblHashSet* pSelf, const GblHashSetIter* pPrev)  {
    GblHashSetIter it = {
        (GblHashSet*)pSelf,
        0
    };
    GBL_CTX_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    if(pPrev && GBL_PRIV_REF(pPrev).bucketIdx < GblHashSet_bucketCount(pSelf)) {
        GBL_PRIV(it).bucketIdx = GBL_PRIV_REF(pPrev).bucketIdx + 1;
    }

    while(!GblHashSet_probe(pSelf, GBL_PRIV(it).bucketIdx) && GBL_PRIV(it).bucketIdx < GblHashSet_bucketCount(pSelf)) ++GBL_PRIV(it).bucketIdx;

    GBL_CTX_END_BLOCK();
    return it;
}


GBL_EXPORT const GblHashSet* GblHashSetIter_container(const GblHashSetIter* pSelf) {
    return GBL_PRIV_REF(pSelf).pSet;
}

GBL_EXPORT GblBool GblHashSetIter_valid(const GblHashSetIter* pSelf)  {
    return pSelf && GBL_PRIV_REF(pSelf).pSet && GBL_PRIV_REF(pSelf).bucketIdx < GBL_PRIV_REF(GBL_PRIV_REF(pSelf).pSet).bucketCount;
}

GBL_EXPORT void* GblHashSetIter_value(const GblHashSetIter* pSelf) {
    void* pKey = NULL;
    if(GblHashSetIter_valid(pSelf)) {
        GBL_CTX_BEGIN(GBL_PRIV_REF(GBL_PRIV_REF(pSelf).pSet).pCtx);
        pKey = GblHashSet_probe(GBL_PRIV_REF(pSelf).pSet, GBL_PRIV_REF(pSelf).bucketIdx);
        GBL_CTX_VERIFY_EXPRESSION(pKey, "No key for what should be a valid iterator!");
        GBL_CTX_END_BLOCK();
    }
    return pKey;
}


GBL_EXPORT void* GblHashSet_userdata(const GblHashSet* pSelf) {
    return GBL_PRIV_REF(pSelf).pUserdata;
}

GBL_EXPORT GblBool GblHashSet_empty(const GblHashSet* pSelf) {
    return GBL_PRIV_REF(pSelf).count? GBL_FALSE : GBL_TRUE;
}

GBL_EXPORT GblContext* GblHashSet_context(const GblHashSet* pSelf) {
    return GBL_PRIV_REF(pSelf).pCtx;
}

GBL_EXPORT size_t   GblHashSet_bucketSize(const GblHashSet* pSelf) {
    return GBL_PRIV_REF(pSelf).bucketSize;
}

GBL_EXPORT size_t  GblHashSet_size(const GblHashSet* pSelf) {
    return GBL_PRIV_REF(pSelf).count;
}

GBL_EXPORT size_t  GblHashSet_bucketCount(const GblHashSet* pSelf)  {
    return GBL_PRIV_REF(pSelf).bucketCount;
}

// hashpSelf_get returns the item based on the provided key. If the item is not
// found then NULL is returned.
GBL_EXPORT void* GblHashSet_get(const GblHashSet* pSelf, const void *pKey) {
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

GBL_EXPORT GblHashSetIter GblHashSet_find(const GblHashSet* map, const void* key) {
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

GBL_EXPORT void* GblHashSet_at(const GblHashSet* pSet, const void* pKey) {
    void* pEntry = NULL;
    pEntry = GblHashSet_get(pSet, pKey);
    if(!pEntry) {
        GBL_CTX_BEGIN(GBL_PRIV_REF(pSet).pCtx);
        GBL_CTX_VERIFY(pEntry, GBL_RESULT_ERROR_OUT_OF_RANGE, "The requested key was not found in the hash map!");
        GBL_CTX_END_BLOCK();
    }
    return pEntry;
}

GBL_EXPORT GblBool GblHashSet_contains(const GblHashSet* pSet, const void* pKey) {
    GblBool found = GBL_FALSE;
    if(GblHashSet_get(pSet, pKey)) found = GBL_TRUE;
    return found;
}

GBL_EXPORT size_t  GblHashSet_count(const GblHashSet* pSet, const void* pKey)  {
    size_t  count = 0;
    if(GblHashSet_get(pSet, pKey)) ++count;
    return count;
}


GBL_EXPORT GblBool GblHashSet_insert(GblHashSet* pSet, const void* pItem) {
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

GBL_EXPORT void GblHashSet_insertOrAssign(GblHashSet* pSet, const void* pEntry) {
    if(!pEntry) {
        GBL_CTX_BEGIN(GBL_PRIV_REF(pSet).pCtx);
        GBL_CTX_VERIFY_POINTER(pEntry);
        GBL_CTX_END_BLOCK();
    } else {
        void* pOldEntry = GblHashSet_set(pSet, pEntry);
        if(pOldEntry && GBL_PRIV_REF(pSet).pFnDestruct) GBL_PRIV_REF(pSet).pFnDestruct(pSet, pOldEntry);
    }
}

GBL_EXPORT void* GblHashSet_tryEmplace(GblHashSet* pSet, const void* pKey) {
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



