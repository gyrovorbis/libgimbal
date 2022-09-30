#include <gimbal/containers/gimbal_hash_set.h>
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
GBL_MAYBE_UNUSED static void MM86128(const void *key, const int len, uint32_t seed, void *out) {
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

GBL_EXPORT GBL_RESULT GblHashSet_construct_8(GblHashSet*      pSet,
                                             GblSize          elsize,
                                             GblHashSetHashFn pFnHash,
                                             GblHashSetCmpFn  pFnCompare,
                                             GblHashSetDtorFn pFnDestruct,
                                             GblSize          capacity,
                                             GblContext*      pCtx,
                                             void*            pUserdata)
{

    GBL_API_BEGIN(pCtx);
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
        GBL_PRIV_REF(pSet).pSpare         = GBL_API_MALLOC(GBL_PRIV_REF(pSet).bucketSize);
        GBL_PRIV_REF(pSet).capacity       = capacity;
        GBL_PRIV_REF(pSet).bucketCount    = capacity;
        GBL_PRIV_REF(pSet).mask           = GBL_PRIV_REF(pSet).bucketCount-1;
        GBL_PRIV_REF(pSet).pBuckets       = GBL_API_MALLOC(GBL_PRIV_REF(pSet).bucketSize*GBL_PRIV_REF(pSet).bucketCount);
        memset(GBL_PRIV_REF(pSet).pBuckets, 0, GBL_PRIV_REF(pSet).bucketSize*GBL_PRIV_REF(pSet).bucketCount);
    }
    GBL_API_END();

}
GBL_EXPORT GBL_RESULT             GblHashSet_construct_7(GblHashSet*                 pSet,
                                           GblSize                     entrySize,
                                           GblHashSetHashFn       pFnHash,
                                           GblHashSetCmpFn    pFnCompare,
                                           GblHashSetDtorFn   pFnDestruct,
                                           GblSize                     capacity,
                                           GblContext*                 pCtx) GBL_NOEXCEPT {
    return GblHashSet_construct_8(pSet, entrySize, pFnHash, pFnCompare, pFnDestruct, capacity, pCtx, NULL);
}

GBL_EXPORT GBL_RESULT             GblHashSet_construct_6(GblHashSet*                 pSet,
                                           GblSize                     entrySize,
                                           GblHashSetHashFn       pFnHash,
                                           GblHashSetCmpFn    pFnCompare,
                                           GblHashSetDtorFn   pFnDestruct,
                                           GblSize                     capacity) GBL_NOEXCEPT
{
    return GblHashSet_construct_7(pSet, entrySize, pFnHash, pFnCompare, pFnDestruct, capacity, NULL);
}

GBL_EXPORT GBL_RESULT             GblHashSet_construct_5(GblHashSet*                 pSet,
                                           GblSize                     entrySize,
                                           GblHashSetHashFn       pFnHash,
                                           GblHashSetCmpFn    pFnCompare,
                                           GblHashSetDtorFn   pFnDestruct) GBL_NOEXCEPT
{
    return GblHashSet_construct_6(pSet, entrySize, pFnHash, pFnCompare, pFnDestruct, 0);
}

GBL_EXPORT GBL_RESULT             GblHashSet_construct_4(GblHashSet*                 pSet,
                                           GblSize                     entrySize,
                                           GblHashSetHashFn       pFnHash,
                                           GblHashSetCmpFn    pFnCompare) GBL_NOEXCEPT {
    return GblHashSet_construct_5(pSet, entrySize, pFnHash, pFnCompare, NULL);
}


GBL_EXPORT GBL_RESULT  GblHashSet_clone(GblHashSet* pSelf, const GblHashSet* pRhs, GblContext* pCtx) GBL_NOEXCEPT {
    if(!pCtx) pCtx = GBL_PRIV_REF(pRhs).pCtx;
    GBL_API_BEGIN(pCtx);
    GBL_API_CALL(GblHashSet_construct_8(pSelf,
                                       GBL_PRIV_REF(pRhs).entrySize,
                                       GBL_PRIV_REF(pRhs).pFnHash,
                                       GBL_PRIV_REF(pRhs).pFnCompare,
                                       GBL_PRIV_REF(pRhs).pFnDestruct,
                                       GBL_PRIV_REF(pRhs).capacity,
                                       pCtx,
                                       GBL_PRIV_REF(pRhs).pUserdata));

    for(GblSize s = 0; s < GBL_PRIV_REF(pRhs).bucketCount; ++s) {
        void* pEntry = GblHashSet_probe(pRhs, s);
        if(pEntry) {
            GblHashSet_insert(pSelf, pEntry);
        }
    }

    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblHashSet_constructClone(GblHashSet* pSelf, GblHashSet* pRhs) {
    GBL_API_BEGIN(GBL_PRIV_REF(pRhs).pCtx);
    GblHashSet_destruct(pSelf);
    GBL_API_CALL(GblHashSet_clone(pSelf, pRhs, NULL));
    GBL_API_END();
}

GBL_EXPORT GBL_RESULT GblHashSet_constructMove(GblHashSet* pSelf, GblHashSet* pRhs, GblContext* pCtx) GBL_NOEXCEPT {
    if(pCtx != GBL_PRIV_REF(pRhs).pCtx && pCtx != NULL) {
        return GblHashSet_clone(pSelf, pRhs, pCtx);
    } else {
        memcpy(pSelf, pRhs, sizeof(GblHashSet));
        memset(pRhs, 0, sizeof(GblHashSet));
        return GBL_RESULT_SUCCESS;
    }
}

GBL_EXPORT GBL_RESULT GblHashSet_assignMove(GblHashSet* pLhs, GblHashSet* pRhs) GBL_NOEXCEPT {
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
GBL_EXPORT void GblHashSet_clear(GblHashSet *map) GBL_NOEXCEPT {
    if(!GBL_PRIV_REF(map).count) return;
    GblBool update_cap = GBL_TRUE;
    GBL_API_BEGIN(GBL_PRIV_REF(map).pCtx);
    GBL_PRIV_REF(map).count = 0;
    free_elements(map);
    if (update_cap) {
        GBL_PRIV_REF(map).capacity = GBL_PRIV_REF(map).bucketCount;
    } else if (GBL_PRIV_REF(map).bucketCount != GBL_PRIV_REF(map).capacity) {
        void *new_buckets = GBL_API_MALLOC(GBL_PRIV_REF(map).bucketSize*GBL_PRIV_REF(map).capacity);
        if (new_buckets) {
            GBL_API_FREE(GBL_PRIV_REF(map).pBuckets);
            GBL_PRIV_REF(map).pBuckets = new_buckets;
        }
        GBL_PRIV_REF(map).bucketCount = GBL_PRIV_REF(map).capacity;
    }
    memset(GBL_PRIV_REF(map).pBuckets, 0, GBL_PRIV_REF(map).bucketSize*GBL_PRIV_REF(map).bucketCount);
    GBL_PRIV_REF(map).mask = GBL_PRIV_REF(map).bucketCount-1;
    GBL_API_END_BLOCK();
}

static GblBool resize(struct GblHashSet *map, size_t new_cap) {
    GBL_API_BEGIN(GBL_PRIV_REF(map).pCtx); {
        GblHashSet _map2;
        GblHashSet* map2 = &_map2;
        GBL_API_CALL(GblHashSet_construct(map2,
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
        GBL_API_FREE(GBL_PRIV_REF(map).pBuckets);
        GBL_PRIV_REF(map).pBuckets = GBL_PRIV_REF(map2).pBuckets;
        GBL_PRIV_REF(map).bucketCount = GBL_PRIV_REF(map2).bucketCount;
        GBL_PRIV_REF(map).mask = GBL_PRIV_REF(map2).mask;
        GBL_API_FREE(GBL_PRIV_REF(map2).pSpare);
    }
    GBL_API_END_BLOCK();
    return GBL_TRUE;
}

GBL_EXPORT GBL_RESULT GblHashSet_shrinkToFit(GblHashSet* pSelf) GBL_NOEXCEPT {
    GBL_API_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    if(GBL_PRIV_REF(pSelf).count < GBL_PRIV_REF(pSelf).bucketCount * 0.75) {
        GBL_API_VERIFY_EXPRESSION(resize(pSelf, GBL_PRIV_REF(pSelf).bucketCount*0.75));
    }
    GBL_API_END();
}


GBL_EXPORT void* GblHashSet_rawSet_(GblHashSet* map, const void* item, void** ppNewEntry) GBL_NOEXCEPT {
    void* pPrevItem = NULL;

    void* edata = GBL_ALLOCA(GBL_PRIV_REF(map).bucketSize);

    GBL_ASSERT(item);

    if (GBL_PRIV_REF(map).count == GBL_PRIV_REF(map).bucketCount*0.75) {
        GblBool result = resize(map, GBL_PRIV_REF(map).bucketCount*2);
        if(!result) {
            GBL_API_BEGIN(GBL_PRIV_REF(map).pCtx);
            GBL_API_VERIFY(result, GBL_RESULT_ERROR_MEM_REALLOC);
            GBL_API_END_BLOCK();
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

GBL_EXPORT void* GblHashSet_emplace(GblHashSet* pSet, const void* pKey) GBL_NOEXCEPT {
    void* pNewEntry = NULL;
    GblHashSet_rawSet_(pSet, pKey, &pNewEntry);
    if(!pNewEntry) {
        GBL_API_BEGIN(GBL_PRIV_REF(pSet).pCtx);
        GBL_API_VERIFY_EXPRESSION(pNewEntry, "Somehow we did a set and didn't get a new entry?");
        //GBL_API_VERIFY(!pOldEntry, GBL_RESULT_ERROR_OVERFLOW, "Could not emplace over existing hash map entry!");
        GBL_API_END_BLOCK();
    }
    return pNewEntry;
}


// hashmap_set inserts or replaces an item in the hash map. If an item is
// replaced then it is returned otherwise NULL is returned. This operation
// may allocate memory. If the system is unable to allocate additional
// memory then NULL is returned and hashmap_oom() returns true.
GBL_EXPORT void* GblHashSet_set(GblHashSet *map, const void *item) GBL_NOEXCEPT {
    return GblHashSet_rawSet_(map, item, NULL);
}

// hashmap_probe returns the item in the bucket at position or NULL if an item
// is not set for that bucket. The position is 'moduloed' by the number of
// buckets in the hashmap.
GBL_EXPORT void* GblHashSet_probe(const GblHashSet *map, GblSize position) GBL_NOEXCEPT {
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
GBL_EXPORT void* GblHashSet_extract(GblHashSet *map, const void* pKey) GBL_NOEXCEPT {
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
GBL_EXPORT GBL_RESULT GblHashSet_destruct(GblHashSet *map) GBL_NOEXCEPT {
    //if (!map) return
    GBL_API_BEGIN(GBL_PRIV_REF(map).pCtx);
    free_elements(map);
    GBL_API_FREE(GBL_PRIV_REF(map).pBuckets);
    GBL_API_FREE(GBL_PRIV_REF(map).pSpare);
    GBL_API_END();
}

// hashmap_scan iterates over all items in the hash map
// Param `iter` can return false to stop iteration early.
// Returns false if the iteration has been stopped early.
GBL_EXPORT GblBool GblHashSet_foreach(const GblHashSet *map,
                  GblHashSetIterFn iter, void* udata) GBL_NOEXCEPT
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

GBL_EXPORT GblHashSetIter GblHashSet_next(const GblHashSet* pSelf, const GblHashSetIter* pPrev) GBL_NOEXCEPT {
    GblHashSetIter it = {
        (GblHashSet*)pSelf,
        0
    };
    GBL_API_BEGIN(GBL_PRIV_REF(pSelf).pCtx);
    if(pPrev && GBL_PRIV_REF(pPrev).bucketIdx < GblHashSet_bucketCount(pSelf)) {
        GBL_PRIV(it).bucketIdx = GBL_PRIV_REF(pPrev).bucketIdx + 1;
    }

    while(!GblHashSet_probe(pSelf, GBL_PRIV(it).bucketIdx) && GBL_PRIV(it).bucketIdx < GblHashSet_bucketCount(pSelf)) ++GBL_PRIV(it).bucketIdx;

    GBL_API_END_BLOCK();
    return it;
}


GBL_EXPORT const GblHashSet* GblHashSetIter_container(const GblHashSetIter* pSelf) {
    return GBL_PRIV_REF(pSelf).pSet;
}

GBL_EXPORT GblBool GblHashSetIter_valid(const GblHashSetIter* pSelf) GBL_NOEXCEPT {
    return pSelf && GBL_PRIV_REF(pSelf).pSet && GBL_PRIV_REF(pSelf).bucketIdx < GBL_PRIV_REF(GBL_PRIV_REF(pSelf).pSet).bucketCount;
}

GBL_EXPORT void* GblHashSetIter_value(const GblHashSetIter* pSelf) {
    void* pKey = NULL;
    if(GblHashSetIter_valid(pSelf)) {
        GBL_API_BEGIN(GBL_PRIV_REF(GBL_PRIV_REF(pSelf).pSet).pCtx);
        pKey = GblHashSet_probe(GBL_PRIV_REF(pSelf).pSet, GBL_PRIV_REF(pSelf).bucketIdx);
        GBL_API_VERIFY_EXPRESSION(pKey, "No key for what should be a valid iterator!");
        GBL_API_END_BLOCK();
    }
    return pKey;
}





