#include <gimbal/containers/gimbal_hashset.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <gimbal/algorithms/gimbal_numeric.h>


//-----------------------------------------------------------------------------
// SipHash reference C implementation
//
// Copyright (c) 2012-2016 Jean-Philippe Aumasson
// <jeanphilippe.aumasson@gmail.com>
// Copyright (c) 2012-2014 Daniel J. Bernstein <djb@cr.yp.to>
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.
//
// default: SipHash-2-4
//-----------------------------------------------------------------------------
static uint64_t SIP64(const uint8_t *in, const size_t inlen,
                      uint64_t seed0, uint64_t seed1)
{
#define U8TO64_LE(p) \
    {  (((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) | \
        ((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) | \
        ((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) | \
        ((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56)) }
#define U64TO8_LE(p, v) \
    { U32TO8_LE((p), (uint32_t)((v))); \
      U32TO8_LE((p) + 4, (uint32_t)((v) >> 32)); }
#define U32TO8_LE(p, v) \
    { (p)[0] = (uint8_t)((v)); \
      (p)[1] = (uint8_t)((v) >> 8); \
      (p)[2] = (uint8_t)((v) >> 16); \
      (p)[3] = (uint8_t)((v) >> 24); }
#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))
#define SIPROUND \
    { v0 += v1; v1 = ROTL(v1, 13); \
      v1 ^= v0; v0 = ROTL(v0, 32); \
      v2 += v3; v3 = ROTL(v3, 16); \
      v3 ^= v2; \
      v0 += v3; v3 = ROTL(v3, 21); \
      v3 ^= v0; \
      v2 += v1; v1 = ROTL(v1, 17); \
      v1 ^= v2; v2 = ROTL(v2, 32); }
    uint64_t k0 = U8TO64_LE((uint8_t*)&seed0);
    uint64_t k1 = U8TO64_LE((uint8_t*)&seed1);
    uint64_t v3 = UINT64_C(0x7465646279746573) ^ k1;
    uint64_t v2 = UINT64_C(0x6c7967656e657261) ^ k0;
    uint64_t v1 = UINT64_C(0x646f72616e646f6d) ^ k1;
    uint64_t v0 = UINT64_C(0x736f6d6570736575) ^ k0;
    const uint8_t *end = in + inlen - (inlen % sizeof(uint64_t));
    for (; in != end; in += 8) {
        uint64_t m = U8TO64_LE(in);
        v3 ^= m;
        SIPROUND; SIPROUND;
        v0 ^= m;
    }
    const int left = inlen & 7;
    uint64_t b = ((uint64_t)inlen) << 56;
    switch (left) {
    case 7: b |= ((uint64_t)in[6]) << 48;
    case 6: b |= ((uint64_t)in[5]) << 40;
    case 5: b |= ((uint64_t)in[4]) << 32;
    case 4: b |= ((uint64_t)in[3]) << 24;
    case 3: b |= ((uint64_t)in[2]) << 16;
    case 2: b |= ((uint64_t)in[1]) << 8;
    case 1: b |= ((uint64_t)in[0]); break;
    case 0: break;
    }
    v3 ^= b;
    SIPROUND; SIPROUND;
    v0 ^= b;
    v2 ^= 0xff;
    SIPROUND; SIPROUND; SIPROUND; SIPROUND;
    b = v0 ^ v1 ^ v2 ^ v3;
    uint64_t out = 0;
    U64TO8_LE((uint8_t*)&out, b);
    return out;
}

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
//
// Murmur3_86_128
//-----------------------------------------------------------------------------
static void MM86128(const void *key, const int len, uint32_t seed, void *out) {
#define	ROTL32(x, r) ((x << r) | (x >> (32 - r)))
#define FMIX32(h) h^=h>>16; h*=0x85ebca6b; h^=h>>13; h*=0xc2b2ae35; h^=h>>16;
    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 16;
    uint32_t h1 = seed;
    uint32_t h2 = seed;
    uint32_t h3 = seed;
    uint32_t h4 = seed;
    uint32_t c1 = 0x239b961b;
    uint32_t c2 = 0xab0e9789;
    uint32_t c3 = 0x38b34ae5;
    uint32_t c4 = 0xa1e38b93;
    const uint32_t * blocks = (const uint32_t *)(data + nblocks*16);
    for (int i = -nblocks; i; i++) {
        uint32_t k1 = blocks[i*4+0];
        uint32_t k2 = blocks[i*4+1];
        uint32_t k3 = blocks[i*4+2];
        uint32_t k4 = blocks[i*4+3];
        k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
        h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;
        k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;
        h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;
        k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;
        h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;
        k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;
        h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
    }
    const uint8_t * tail = (const uint8_t*)(data + nblocks*16);
    uint32_t k1 = 0;
    uint32_t k2 = 0;
    uint32_t k3 = 0;
    uint32_t k4 = 0;
    switch(len & 15) {
    case 15: k4 ^= tail[14] << 16;
    case 14: k4 ^= tail[13] << 8;
    case 13: k4 ^= tail[12] << 0;
             k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;
    case 12: k3 ^= tail[11] << 24;
    case 11: k3 ^= tail[10] << 16;
    case 10: k3 ^= tail[ 9] << 8;
    case  9: k3 ^= tail[ 8] << 0;
             k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;
    case  8: k2 ^= tail[ 7] << 24;
    case  7: k2 ^= tail[ 6] << 16;
    case  6: k2 ^= tail[ 5] << 8;
    case  5: k2 ^= tail[ 4] << 0;
             k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;
    case  4: k1 ^= tail[ 3] << 24;
    case  3: k1 ^= tail[ 2] << 16;
    case  2: k1 ^= tail[ 1] << 8;
    case  1: k1 ^= tail[ 0] << 0;
             k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
    };
    h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;
    h1 += h2; h1 += h3; h1 += h4;
    h2 += h1; h3 += h1; h4 += h1;
    FMIX32(h1); FMIX32(h2); FMIX32(h3); FMIX32(h4);
    h1 += h2; h1 += h3; h1 += h4;
    h2 += h1; h3 += h1; h4 += h1;
    ((uint32_t*)out)[0] = h1;
    ((uint32_t*)out)[1] = h2;
    ((uint32_t*)out)[2] = h3;
    ((uint32_t*)out)[3] = h4;
}


GBL_EXPORT GblHash gblHashSip(const void* pData, GblSize size) GBL_NOEXCEPT {
    return SIP64((uint8_t*)pData, size, gblSeed(0), gblSeed(1));
}

GBL_EXPORT GblHash gblHashMurmur(const void* pData, GblSize size) GBL_NOEXCEPT {
    char out[16];
    MM86128(pData, size, gblSeed(0), &out);
    return *(uint64_t*)out;
}

struct bucket {
    uint64_t hash:48;
    uint64_t dib:16;
};


static inline struct bucket *bucket_at(const struct GblHashSet *map, size_t index) {
    return (struct bucket*)(((char*)map->pBuckets)+(map->bucketSize*index));
}

static inline void *bucket_item(struct bucket *entry) {
    return ((char*)entry)+sizeof(struct bucket);
}

static inline uint64_t get_hash(const struct GblHashSet *map, const void *key) {
    return map->pFnHash(map, key) << 16 >> 16;
}



GBL_API             GblHashSet_construct_8(GblHashSet*                 pSet,
                                           GblSize                     elsize,
                                           GblHashSetEntryHashFn       pFnHash,
                                           GblHashSetEntryCompareFn    pFnCompare,
                                           GblHashSetEntryDestructFn   pFnDestruct,
                                           GblSize                     capacity,
                                           GblContext                  hCtx,
                                           void*                       pUserdata) GBL_NOEXCEPT
{

    GBL_API_BEGIN(hCtx);
    {
        GBL_API_VERIFY_POINTER(pSet);
        GblSize ncap = 16;
        if (capacity < ncap) {
            capacity = ncap;
        } else {
            while (ncap < capacity) {
                ncap *= 2;
            }
            capacity = ncap;
        }
        size_t bucketsz = sizeof(struct bucket) + elsize;
        while (bucketsz & (sizeof(uintptr_t)-1)) {
            bucketsz++;
        }
        // hashmap + pSpare + edata
        memset(pSet, 0, sizeof(GblHashSet));
        pSet->hCtx           = hCtx;
        pSet->pUserdata      = pUserdata;
        pSet->entrySize      = elsize;
        pSet->bucketSize     = bucketsz;
        pSet->pFnHash        = pFnHash;
        pSet->pFnCompare     = pFnCompare;
        pSet->pFnDestruct    = pFnDestruct;
        pSet->pSpare         = GBL_API_MALLOC(pSet->bucketSize);
        pSet->capacity       = capacity;
        pSet->bucketCount    = capacity;
        pSet->mask           = pSet->bucketCount-1;
        pSet->pBuckets        = GBL_API_MALLOC(pSet->bucketSize*pSet->bucketCount);
        memset(pSet->pBuckets, 0, pSet->bucketSize*pSet->bucketCount);
    }
    GBL_API_END();

}
GBL_API             GblHashSet_construct_7(GblHashSet*                 pSet,
                                           GblSize                     elsize,
                                           GblHashSetEntryHashFn       pFnHash,
                                           GblHashSetEntryCompareFn    pFnCompare,
                                           GblHashSetEntryDestructFn   pFnDestruct,
                                           GblSize                     capacity,
                                           GblContext                  hCtx) GBL_NOEXCEPT {
    return GblHashSet_construct_8(pSet, elsize, pFnHash, pFnCompare, pFnDestruct, capacity, hCtx, NULL);
}

GBL_API             GblHashSet_construct_6(GblHashSet*                 pSet,
                                           GblSize                     elsize,
                                           GblHashSetEntryHashFn       pFnHash,
                                           GblHashSetEntryCompareFn    pFnCompare,
                                           GblHashSetEntryDestructFn   pFnDestruct,
                                           GblSize                     capacity) GBL_NOEXCEPT
{
    return GblHashSet_construct_7(pSet, elsize, pFnHash, pFnCompare, pFnDestruct, capacity, NULL);
}

GBL_API             GblHashSet_construct_5(GblHashSet*                 pSet,
                                           GblSize                     elsize,
                                           GblHashSetEntryHashFn       pFnHash,
                                           GblHashSetEntryCompareFn    pFnCompare,
                                           GblHashSetEntryDestructFn   pFnDestruct) GBL_NOEXCEPT
{
    return GblHashSet_construct_6(pSet, elsize, pFnHash, pFnCompare, pFnDestruct, 0);
}

GBL_API             GblHashSet_construct_4(GblHashSet*                 pSet,
                                           GblSize                     elsize,
                                           GblHashSetEntryHashFn       pFnHash,
                                           GblHashSetEntryCompareFn    pFnCompare) GBL_NOEXCEPT {
    return GblHashSet_construct_5(pSet, elsize, pFnHash, pFnCompare, NULL);
}


GBL_API  GblHashSet_constructCopy(GblHashSet* pLhs, const GblHashSet* pRhs) GBL_NOEXCEPT {
    GBL_UNUSED(pRhs);
    return GBL_RESULT_UNIMPLEMENTED;

}

GBL_API GblHashSet_constructMove(GblHashSet* pLhs, GblHashSet* pRhs) GBL_NOEXCEPT {
    memcpy(pLhs, pRhs, sizeof(GblHashSet));
    memset(pRhs, 0, sizeof(GblHashSet));
    return GBL_RESULT_SUCCESS;
}

GBL_API GblHashSet_assignMove(GblHashSet* pLhs, GblHashSet* pRhs) GBL_NOEXCEPT {
    GblHashSet_destruct(pLhs);
    return GblHashSet_constructMove(pLhs, pRhs);
}

GBL_EXPORT void* GblHashSet_at(const GblHashSet* pSet, const void* pKey) GBL_NOEXCEPT {
    void* pEntry = NULL;
    GBL_API_BEGIN(pSet->hCtx);
    pEntry = GblHashSet_get(pSet, pKey);
    GBL_API_VERIFY(pEntry, GBL_RESULT_ERROR_OUT_OF_RANGE, "The requested key was not found in the hash map!");
    GBL_API_END_BLOCK();
    return pEntry;
}

GBL_EXPORT GblBool GblHashSet_contains(const GblHashSet* pSet, const void* pKey) GBL_NOEXCEPT {
    GblBool found = GBL_FALSE;
    GBL_API_BEGIN(pSet->hCtx);
    if(GblHashSet_get(pSet, pKey)) found = GBL_TRUE;
    GBL_API_END_BLOCK();
    return found;
}

GBL_EXPORT GblSize GblHashSet_count(const GblHashSet* pSet, const void* pKey) GBL_NOEXCEPT {
    GblSize count = 0;
    GBL_API_BEGIN(pSet->hCtx);
    if(GblHashSet_get(pSet, pKey)) ++count;
    GBL_API_END_BLOCK();
    return count;
}

static void free_elements(GblHashSet *map) {
    if (map->pFnDestruct) {
        for (size_t i = 0; i < map->bucketCount; i++) {
            struct bucket *bucket = bucket_at(map, i);
            if (bucket->dib) map->pFnDestruct(map, bucket_item(bucket));
        }
    }
}

// hashmap_clear quickly clears the map.
// Every item is called with the element-freeing function given in hashmap_new,
// if present, to free any data referenced in the elements of the hashmap.
// When the update_cap is provided, the map's capacity will be updated to match
// the currently number of allocated buckets. This is an optimization to ensure
// that this operation does not perform any allocations.
GBL_EXPORT void GblHashSet_clear(GblHashSet *map) GBL_NOEXCEPT {
    GblBool update_cap = GBL_TRUE;
    GBL_API_BEGIN(map->hCtx);
    map->count = 0;
    free_elements(map);
    if (update_cap) {
        map->capacity = map->bucketCount;
    } else if (map->bucketCount != map->capacity) {
        void *new_buckets = GBL_API_MALLOC(map->bucketSize*map->capacity);
        if (new_buckets) {
            GBL_API_FREE(map->pBuckets);
            map->pBuckets = new_buckets;
        }
        map->bucketCount = map->capacity;
    }
    memset(map->pBuckets, 0, map->bucketSize*map->bucketCount);
    map->mask = map->bucketCount-1;
    GBL_API_END_BLOCK();
}

static GblBool resize(struct GblHashSet *map, size_t new_cap) {
    GBL_API_BEGIN(map->hCtx); {
        GblHashSet _map2;
        GblHashSet* map2 = &_map2;
        GBL_API_CALL(GblHashSet_construct(map2,
                                          map->entrySize,
                                          map->pFnHash,
                                          map->pFnCompare,
                                          map->pFnDestruct,
                                          new_cap,
                                          map->hCtx,
                                          map->pUserdata));

        for (size_t i = 0; i < map->bucketCount; i++) {
            struct bucket *entry = bucket_at(map, i);
            if (!entry->dib) {
                continue;
            }
            entry->dib = 1;
            size_t j = entry->hash & map2->mask;
            for (;;) {
                struct bucket *bucket = bucket_at(map2, j);
                if (bucket->dib == 0) {
                    memcpy(bucket, entry, map->bucketSize);
                    break;
                }
                if (bucket->dib < entry->dib) {
                    memcpy(map2->pSpare, bucket, map->bucketSize);
                    memcpy(bucket, entry, map->bucketSize);
                    memcpy(entry, map2->pSpare, map->bucketSize);
                }
                j = (j + 1) & map2->mask;
                entry->dib += 1;
            }
        }
        GBL_API_FREE(map->pBuckets);
        map->pBuckets = map2->pBuckets;
        map->bucketCount = map2->bucketCount;
        map->mask = map2->mask;
        GBL_API_FREE(map2->pSpare);
    }
    GBL_API_END_BLOCK();
    return GBL_TRUE;
}

GBL_EXPORT GblBool GblHashSet_insert(GblHashSet* pSet, const void* pItem) GBL_NOEXCEPT {
    GblBool inserted = GBL_FALSE;
    GBL_API_BEGIN(pSet->hCtx);
    {
        void* pValue = GblHashSet_get(pSet, pItem);
        if(pValue) GBL_API_DONE();

        pValue = GblHashSet_set(pSet, pItem);
        GBL_API_VERIFY_EXPRESSION(!pValue, "Somehow the item wsn't found in the map before inserting it, but it was there!");
        inserted = GBL_TRUE;
    }
    GBL_API_END_BLOCK();
    return inserted;
}

GBL_EXPORT void GblHashSet_insertOrAssign(GblHashSet* pSet, const void* pEntry) GBL_NOEXCEPT {
    GBL_API_BEGIN(pSet->hCtx);
    GBL_API_VERIFY_POINTER(pEntry);
    {
        void* pOldEntry = GblHashSet_set(pSet, pEntry);
        if(pOldEntry && pSet->pFnDestruct) pSet->pFnDestruct(pSet, pOldEntry);
    }
    GBL_API_END_BLOCK();
}


static GBL_EXPORT void* GblHashSet_rawSet_(GblHashSet* map, const void* item, void** ppNewEntry) GBL_NOEXCEPT {
    void* pPrevItem = NULL;
    GBL_API_BEGIN(map->hCtx); {
        void* edata = GBL_ALLOCA(map->bucketSize);
        GBL_API_VERIFY_POINTER(item);
        if (map->count == map->bucketCount*0.75f) {
            GBL_API_VERIFY_EXPRESSION(resize(map, map->bucketCount*2));
        }

        struct bucket *entry = edata;
        entry->hash = get_hash(map, item);
        entry->dib = 1;
        memcpy(bucket_item(entry), item, map->entrySize);

        size_t i = entry->hash & map->mask;
        for (;;) {
            struct bucket *bucket = bucket_at(map, i);
            if (bucket->dib == 0) {
                memcpy(bucket, entry, map->bucketSize);
                map->count++;
                if(ppNewEntry) *ppNewEntry = bucket_item(bucket);
                break;
            }
            if (entry->hash == bucket->hash && map->pFnCompare(map, bucket_item(entry), bucket_item(bucket)))
            {
                memcpy(map->pSpare, bucket_item(bucket), map->entrySize);
                memcpy(bucket_item(bucket), bucket_item(entry), map->entrySize);
                if(ppNewEntry) *ppNewEntry = bucket_item(bucket);
                pPrevItem = map->pSpare;
                break;
            }
            if (bucket->dib < entry->dib) {
                memcpy(map->pSpare, bucket, map->bucketSize);
                memcpy(bucket, entry, map->bucketSize);
                memcpy(entry, map->pSpare, map->bucketSize);
            }
            i = (i + 1) & map->mask;
            entry->dib += 1;
        }
    } GBL_API_END_BLOCK();
    return pPrevItem;
}

GBL_EXPORT void* GblHashSet_emplace(GblHashSet* pSet, const void* pKey) GBL_NOEXCEPT {
    void* pNewEntry = NULL;
    GBL_API_BEGIN(pSet->hCtx);
    {
        GblHashSet_rawSet_(pSet, pKey, &pNewEntry);
        GBL_API_VERIFY_EXPRESSION(pNewEntry, "Somehow we did a set and didn't get a new entry?");
        //GBL_API_VERIFY(!pOldEntry, GBL_RESULT_ERROR_OVERFLOW, "Could not emplace over existing hash map entry!");
    }
    GBL_API_END_BLOCK();
    return pNewEntry;
}

GBL_EXPORT void* GblHashSet_tryEmplace(GblHashSet* pSet, const void* pKey) GBL_NOEXCEPT {
    void* pPrevEntry = NULL;
    GBL_API_BEGIN(pSet->hCtx);
    if(GblHashSet_contains(pSet, pKey)) {
        GBL_API_DONE();
    } else {
        pPrevEntry = GblHashSet_emplace(pSet, pKey);
        GBL_API_VERIFY_EXPRESSION(pPrevEntry, "Fetching the previous entry should've succeeded after 'contains(key)' returned true!");
    }
    GBL_API_END_BLOCK();
    return pPrevEntry;
}



// hashmap_set inserts or replaces an item in the hash map. If an item is
// replaced then it is returned otherwise NULL is returned. This operation
// may allocate memory. If the system is unable to allocate additional
// memory then NULL is returned and hashmap_oom() returns true.
GBL_EXPORT void* GblHashSet_set(GblHashSet *map, const void *item) GBL_NOEXCEPT {
    return GblHashSet_rawSet_(map, item, NULL);
}

// hashmap_get returns the item based on the provided key. If the item is not
// found then NULL is returned.
GBL_EXPORT void* GblHashSet_get(const GblHashSet *map, const void *key) GBL_NOEXCEPT {
    void* pEntry = NULL;
    GBL_API_BEGIN(map->hCtx); {
        GBL_API_VERIFY_POINTER(key);
        uint64_t hash = get_hash(map, key);
        size_t i = hash & map->mask;
        for (;;) {
            struct bucket *bucket = bucket_at(map, i);
            if (!bucket->dib) {
                break;
            }
            if (bucket->hash == hash &&
                map->pFnCompare(map, key, bucket_item(bucket)))
            {
                pEntry = bucket_item(bucket);
                break;
            }
            i = (i + 1) & map->mask;
        }
    } GBL_API_END_BLOCK();
    return pEntry;
}

// hashmap_probe returns the item in the bucket at position or NULL if an item
// is not set for that bucket. The position is 'moduloed' by the number of
// buckets in the hashmap.
GBL_EXPORT void* GblHashSet_probe(const GblHashSet *map, GblSize position) GBL_NOEXCEPT {
    size_t i = position & map->mask;
    struct bucket *bucket = bucket_at(map, i);
    if (!bucket->dib) {
        return NULL;
    }
    return bucket_item(bucket);
}


GBL_EXPORT GblBool GblHashSet_erase(GblHashSet* pSet, const void* pKey) {
    GblBool removed = GBL_FALSE;
    void* pExtracted = NULL;
    GBL_API_BEGIN(pSet->hCtx);
    pExtracted = GblHashSet_extract(pSet, pKey);
    if(pExtracted) {
        if(pSet->pFnDestruct) pSet->pFnDestruct(pSet, pExtracted);
        removed = GBL_TRUE;
    }
    GBL_API_END_BLOCK();
    return removed;
}

// hashmap_delete removes an item from the hash map and returns it. If the
// item is not found then NULL is returned.
GBL_EXPORT void* GblHashSet_extract(GblHashSet *map, const void* pKey) GBL_NOEXCEPT {
    void* pEntry = NULL;
    GBL_API_BEGIN(map->hCtx); {
        GBL_API_VERIFY_POINTER(pKey);
        uint64_t hash = get_hash(map, pKey);
        size_t i = hash & map->mask;
        for (;;) {
            struct bucket *bucket = bucket_at(map, i);
            if (!bucket->dib) {
                break;
            }
            if (bucket->hash == hash && map->pFnCompare(map, pKey, bucket_item(bucket)))
            {
                memcpy(map->pSpare, bucket_item(bucket), map->entrySize);
                bucket->dib = 0;
                for (;;) {
                    struct bucket *prev = bucket;
                    i = (i + 1) & map->mask;
                    bucket = bucket_at(map, i);
                    if (bucket->dib <= 1) {
                        prev->dib = 0;
                        break;
                    }
                    memcpy(prev, bucket, map->bucketSize);
                    prev->dib--;
                }
                map->count--;
                if (map->bucketCount > map->capacity && map->count <= map->bucketCount*0.1) {
                    // Ignore the return value. It's ok for the resize operation to
                    // fail to allocate enough memory because a shrink operation
                    // does not change the integrity of the data.
                    resize(map, map->bucketCount/2);
                }
                pEntry = map->pSpare;
                break;
            }
            i = (i + 1) & map->mask;
        }
    } GBL_API_END_BLOCK();
    return pEntry;
}

// hashmap_free frees the hash map
// Every item is called with the element-freeing function given in hashmap_new,
// if present, to free any data referenced in the elements of the hashmap.
GBL_EXPORT void GblHashSet_destruct(GblHashSet *map) GBL_NOEXCEPT {
    if (!map) return;
    GBL_API_BEGIN(map->hCtx);
    free_elements(map);
    GBL_API_FREE(map->pBuckets);
    GBL_API_FREE(map->pSpare);
    GBL_API_END_BLOCK();
}

// hashmap_scan iterates over all items in the hash map
// Param `iter` can return false to stop iteration early.
// Returns false if the iteration has been stopped early.
GBL_EXPORT GblBool GblHashSet_foreach(const GblHashSet *map,
                  GblHashSetIterateFn iter, void* udata) GBL_NOEXCEPT
{
    for (size_t i = 0; i < map->bucketCount; i++) {
        struct bucket *bucket = bucket_at(map, i);
        if (bucket->dib) {
            if (!iter(map, bucket_item(bucket), udata)) {
                return GBL_FALSE;
            }
        }
    }
    return GBL_TRUE;
}


GBL_EXPORT GblHashSetIterator GblHashSet_next(const GblHashSet* pSelf, const GblHashSetIterator* pPrev) GBL_NOEXCEPT {
    GblHashSetIterator it = {
        pSelf,
        0
    };
    GBL_API_BEGIN(pSelf->hCtx);
    if(pPrev && pPrev->bucketIdx < GblHashSet_bucketCount(pSelf)) {
        it.bucketIdx = pPrev->bucketIdx + 1;
    }

    while(!GblHashSet_probe(pSelf, it.bucketIdx) && it.bucketIdx < GblHashSet_bucketCount(pSelf)) ++it.bucketIdx;

    GBL_API_END_BLOCK();
    return it;
}


GBL_EXPORT const GblHashSet* GblHashSetIterator_container(const GblHashSetIterator* pSelf) {
    return pSelf->pSet;
}

GBL_EXPORT GblBool GblHashSetIterator_valid(const GblHashSetIterator* pSelf) GBL_NOEXCEPT {
    return pSelf && pSelf->pSet && pSelf->bucketIdx < pSelf->pSet->bucketCount;
}

GBL_EXPORT void* GblHashSetIterator_value(const GblHashSetIterator* pSelf) {
    void* pKey = NULL;
    if(GblHashSetIterator_valid(pSelf)) {
        GBL_API_BEGIN(pSelf->pSet);
        pKey = GblHashSet_probe(pSelf->pSet, pSelf->bucketIdx);
        GBL_API_VERIFY_EXPRESSION(pKey, "No key for what should be a valid iterator!");
        GBL_API_END_BLOCK();
    }
    return pKey;
}





