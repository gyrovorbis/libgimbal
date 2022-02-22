#ifndef GIMBAL_HASHMAP_H
#define GIMBAL_HASHMAP_H

#include <hashmap.c/hashmap.h>
#include <time.h>
#include "../gimbal_container.h"
#include "../gimbal_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * UNIFIED ITERATION SHIT WITH VECTOR
 * FOREACH MACROS
 *
 * MAYBE the active seed should really be in the GblCOntext if it should be global for the lifetime of the app?
 */


typedef struct GblHashMap {
    GblContext hCtx;
    struct hashmap* pImpl_;
} GblHashMap;

GBL_INLINE void* gblHashAlloc_(size_t bytes) {
    GBL_API_BEGIN(NULL);
    return GBL_API_MALLOC(bytes);
}

GBL_INLINE void* gblHashRealloc_(void* pData, size_t bytes) {
    GBL_API_BEGIN(NULL);
    return GBL_API_REALLOC(pData, bytes);
}

GBL_INLINE void gblHashFree_(void* pData) {
    GBL_API_BEGIN(NULL);
    GBL_API_FREE(pData);
}

//Oom => Out-of-Memory => Don't bother, use Context
GBL_INLINE GBL_RESULT gblHashMapOom(GblHashMap* pMap, GblBool* pValue) {
    GBL_API_ASSERT(pMap);
    GBL_API_BEGIN(pMap->hCtx);
    GBL_API_VERIFY_POINTER(pValue);
    *pValue = hashmap_oom(pMap->pImpl_);
    GBL_API_END();
}

#define GBL_API_VERIFY_HASH_MAP(hashMap)                            \
    GBL_STMT_START {                                                \
        GblBool oom = GBL_FALSE;                                    \
        GBL_API_CALL(gblHashMapOom(hashMap, &oom));                 \
        GBL_API_VERIFY(!oom, GBL_RESULT_ERROR_MEM_ALLOC,            \
                       "Hash map just returned out of memory!!");   \
    } GBL_STMT_END

GBL_INLINE GBL_RESULT gblHashMapSip(const void* pData, GblSize len, uint64_t seed0, uint64_t seed1, uint64_t* pResult) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_POINTER(pResult);
    *pResult = hashmap_sip(pData, len, seed0, seed1);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblHashMapMurmur(const void* pData, GblSize len, uint64_t seed0, uint64_t seed1, uint64_t* pResult) {
    GBL_API_BEGIN(NULL);
    GBL_API_VERIFY_POINTER(pData);
    GBL_API_VERIFY_POINTER(pResult);
    *pResult = hashmap_murmur(pData, len, seed0, seed1);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblHashMapInit(GblHashMap* pHashMap,
                       GblContext hCtx,
                       GblSize elemSize,
                       GblSize capacity,
                       uint64_t (*pFnHash)(const void *item,
                                        uint64_t seed0, uint64_t seed1),
                       int (*pFnCompare)(const void *a, const void *b,
                                      void *udata),
                       void (*pFnElfree)(void *item)) {
    int seed = time(NULL);
    GBL_API_BEGIN(hCtx);
    GBL_API_VERIFY_POINTER(pHashMap);

    srand(seed);
    memset(pHashMap, 0, sizeof(GblHashMap));
    pHashMap->hCtx = hCtx;

    pHashMap->pImpl_ = hashmap_new_with_allocator(
                                &gblHashAlloc_, &gblHashRealloc_, &gblHashFree_,
                                elemSize, capacity,
                                seed, seed,
                                pFnHash,
                                pFnCompare,
                                pFnElfree,
                                pHashMap);

    GBL_API_VERIFY(pHashMap->pImpl_, GBL_RESULT_ERROR_INTERNAL, "Failed to construct the given hash map!");
    GBL_API_VERIFY_HASH_MAP(pHashMap);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblHashMapFinalize(GblHashMap* pHashMap) {
    GBL_API_ASSERT(pHashMap);
    GBL_API_BEGIN(pHashMap->hCtx);
    hashmap_free(pHashMap->pImpl_);
    GBL_API_END();
};

GBL_INLINE GBL_RESULT gblHashMapClear(GblHashMap* pHashMap, GblBool updateCapacity) {
    GBL_API_ASSERT(pHashMap);
    GBL_API_BEGIN(pHashMap->hCtx);
    hashmap_clear(pHashMap->pImpl_, updateCapacity);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblHashMapCount(const GblHashMap* pHashMap, GblSize* pSize) {
    GBL_API_ASSERT(pHashMap);
    GBL_API_BEGIN(pHashMap->hCtx);
    GBL_API_VERIFY_POINTER(pSize);
    *pSize = hashmap_count(pHashMap->pImpl_);
    GBL_API_END();
}


GBL_INLINE GBL_RESULT gblHashMapGet(GblHashMap* pMap, const void* pItem, void** ppData) {
    GBL_API_ASSERT(pMap);
    GBL_API_BEGIN(pMap->hCtx);
    GBL_API_VERIFY_POINTER(pItem);
    GBL_API_VERIFY_POINTER(ppData);
    *ppData = hashmap_get(pMap->pImpl_, pItem);
    GBL_API_VERIFY_POINTER(*ppData, GBL_RESULT_ERROR_INTERNAL, "Invalid data pointer returned!");
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblHashMapSet(GblHashMap* pMap, void* pItem, void** ppData) {
    GBL_API_ASSERT(pMap);
    GBL_API_BEGIN(pMap->hCtx);
    GBL_API_VERIFY_POINTER(pItem);
    GBL_API_VERIFY_POINTER(ppData);
    *ppData = hashmap_set(pMap->pImpl_, pItem);
    GBL_API_VERIFY_HASH_MAP(pMap);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblHashMapDelete(GblHashMap* pMap, void* pItem, void** ppData) {
    GBL_API_ASSERT(pMap);
    GBL_API_BEGIN(pMap->hCtx);
    GBL_API_VERIFY_POINTER(pItem);
    GBL_API_VERIFY_POINTER(ppData);
    *ppData = hashmap_delete(pMap->pImpl_, pItem);
    GBL_API_VERIFY_POINTER(*ppData, GBL_RESULT_ERROR_INTERNAL, "Invalid data pointer returned!");
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblHashMapProbe(GblHashMap* pMap, uint64_t pos, void** ppData) {
    GBL_API_ASSERT(pMap);
    GBL_API_BEGIN(pMap->hCtx);
    GBL_API_VERIFY_POINTER(ppData);
    *ppData = hashmap_probe(pMap->pImpl_, pos);
    GBL_API_END();
}

GBL_INLINE GBL_RESULT gblHashMapScan(GblHashMap* pMap, bool (*pFnIter)(const void *item, void *udata), void *pUdata, GblBool* pResult) {
    GBL_API_ASSERT(pMap);
    GBL_API_BEGIN(pMap->hCtx);
    GBL_API_VERIFY_POINTER(pResult);
    GBL_API_VERIFY_POINTER(pFnIter);
    GBL_API_VERIFY_POINTER(pUdata);
    *pResult = hashmap_scan(pMap->pImpl_, pFnIter, pUdata);
    GBL_API_END();
}

// FOREACH ITERATORS AND CONVENIENCE FUNCTIONS


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_HASHMAP_H
