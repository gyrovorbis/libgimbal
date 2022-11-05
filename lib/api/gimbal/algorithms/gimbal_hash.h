/*! \addtogroup algorithms
 * @{
 */
/*! \file
 *  \brief Random number generation, hashing, and CRC algorithms
 */

#ifndef GIMBAL_HASH_H
#define GIMBAL_HASH_H

#include "../core/gimbal_typedefs.h"
#include "../core/gimbal_ctx.h"
#include <time.h>

// default hashing algorithm
#ifndef gblHash
#   define gblHash  superFastHash
#endif

#define GBL_SEED_COUNT                  2

GBL_DECLS_BEGIN

GBL_INLINE int      gblRand          (void)                            GBL_NOEXCEPT;
GBL_INLINE int      gblRandRange     (int min, int max)                GBL_NOEXCEPT;

GBL_INLINE void     gblRandBuffer    (void*   pData,
                                      GblSize size)                    GBL_NOEXCEPT;

GBL_INLINE int      gblRandString    (char*       pBuffer,
                                      int         minSize,
                                      int         maxSize,
                                      const char* pCharList)           GBL_NOEXCEPT;

GBL_INLINE uint64_t gblSeed          (uint8_t index)                   GBL_NOEXCEPT;

GBL_EXPORT GblHash  gblHashSip       (const void* pData, GblSize size) GBL_NOEXCEPT;
GBL_INLINE GblHash  gblHashMurmur    (const void* pData, GblSize size) GBL_NOEXCEPT;
GBL_INLINE GblHash  gblHashFnv1      (const void* pData, GblSize size) GBL_NOEXCEPT;
GBL_INLINE GblHash  gblHashSuperFast (const void* pData, GblSize size) GBL_NOEXCEPT;
GBL_INLINE GblHash  gblHashCrc       (const void* pData, GblSize size) GBL_NOEXCEPT;

/// @}

// ===== INLINE IMPLEMENTATIONS =====

GBL_INLINE uint64_t gblSeed(uint8_t index) GBL_NOEXCEPT {
    static uint64_t seed[GBL_SEED_COUNT] = {0};
    static GblBool  init = GBL_FALSE;
    if(!init) GBL_UNLIKELY {
        seed[0] = (uint64_t)time(NULL);
        for(unsigned i = 1; i < GBL_SEED_COUNT; ++i) {
            seed[i] = gblHashCrc(&seed[i-1], sizeof(uint64_t));
        }
        init = GBL_TRUE;
    }
    return seed[index];
}

GBL_INLINE int gblRand(void) GBL_NOEXCEPT {
    return gblRandRange(0, RAND_MAX);
}


GBL_INLINE int gblRandRange(int min, int max) GBL_NOEXCEPT {
    GBL_ASSERT(max <= RAND_MAX);
    static GblBool seeded = GBL_FALSE;
    if(!seeded) GBL_UNLIKELY {
        srand((unsigned)gblSeed(0));
        seeded = GBL_TRUE;
    }
    return (rand() % (max - min + 1)) + min;
}

GBL_INLINE int gblRandString(char* pBuffer, int minSize, int maxSize, const char* pCharList) GBL_NOEXCEPT {
    static const char* pWordChars = "abcdefghijklmnopqrstuvwxyz "
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "1234567890";

    if(!pCharList) pCharList = pWordChars;
    const int size = gblRandRange(minSize, maxSize);
    const int listLength = strlen(pCharList);

    for(GblSize c = 0; c < (GblSize)size; ++c) {
        pBuffer[c] = pCharList[gblRandRange(0, listLength-1)];
    }

    pBuffer[size] = '\0';
    return size;
}

GBL_INLINE void gblRandBuffer(void* pData, GblSize size) GBL_NOEXCEPT {
    for(GblSize i = 0; i < size; ++i) {
        (((uint8_t*)pData)[i]) = (uint8_t)gblRandRange(0, 255);
    }
}

GBL_MAYBE_UNUSED GBL_INLINE GblHash gblCrc32LittleEndian(const void* pData, GblSize size) GBL_NOEXCEPT {
    GblSize i;
    uint32_t rv = 0xFFFFFFFF;

    for(i = 0; i < size; ++i) {
        rv ^= *((uint8_t*)pData + i);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
        rv = (0xEDB88320 & (-(rv & 1))) ^(rv >> 1);
    }

     return ~rv;
}

/* This one isn't quite as nice as the one above for little-endian... */
GBL_MAYBE_UNUSED GBL_INLINE GblHash gblCrc32BigEndian(const void *pData, int size) GBL_NOEXCEPT {
    int i, j;
    uint32_t rv = 0xFFFFFFFF, b, c;

    for(i = 0; i < size; ++i) {
        b = *(uint8_t*)pData+i;

        for(j = 0; j < 8; ++j) {
            c = ((rv & 0x80000000) ? 1 : 0) ^(b & 1);
            b >>= 1;

            if(c)   rv = ((rv << 1) ^ 0x04C11DB6) | c;
            else    rv <<= 1;
        }
    }

    return rv;
}

GBL_INLINE GblHash  gblHashCrc(const void* pData, GblSize size) GBL_NOEXCEPT {
#if GBL_BIG_ENDIAN == 0
    return gblCrc32LittleEndian(pData, size);
#else
    return gblCrc32BigEndian(pData, size);
#endif
}

GBL_INLINE void gblMurmurHash3_x86_32_(const void* key,
                                       int         len,
                                       uint32_t    seed,
                                       void*       out)
GBL_NOEXCEPT
{
#define	GBL_MURMUR_HASH_ROTL32_(x, r) ((x << r) | (x >> (32 - r)))
#define GBL_MURMUR_HASH_FMIX32_(h) h^=h>>16; h*=0x85ebca6b; h^=h>>13; h*=0xc2b2ae35; h^=h>>16;
// DO NOT DO UNALIGNED WORD ACCESSES ON THE RISC/EMBEDDED CPUS
#if !defined(__DREAMCAST__) && !defined(__VITA__)
#   define GBL_MURMUR_HASH_GET_BLOCK_(k, p, i) (k = p[i])
#else
#   define GBL_MURMUR_HASH_GET_BLOCK_(k, p, i) memcpy(&k, p + i, sizeof(uint32_t))
#endif

    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 4;
    int i;

    uint32_t h1 = seed;

    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;

    //----------
    // body

    const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

    for(i = -nblocks; i; i++)
    {
      uint32_t k1;
      GBL_MURMUR_HASH_GET_BLOCK_(k1, blocks, i);

      k1 *= c1;
      k1 = GBL_MURMUR_HASH_ROTL32_(k1,15);
      k1 *= c2;

      h1 ^= k1;
      h1 = GBL_MURMUR_HASH_ROTL32_(h1,13);
      h1 = h1*5+0xe6546b64;
    }

    //----------
    // tail

    const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

    uint32_t k1 = 0;

    switch(len & 3)
    {
    case 3: k1 ^= tail[2] << 16;
    case 2: k1 ^= tail[1] << 8;
    case 1: k1 ^= tail[0];
            k1 *= c1; k1 = GBL_MURMUR_HASH_ROTL32_(k1,15); k1 *= c2; h1 ^= k1;
    };

    //----------
    // finalization

    h1 ^= len;

    GBL_MURMUR_HASH_FMIX32_(h1);

    *(uint32_t*)out = h1;

#undef GBL_MURMUR_HASH_ROTL32_
#undef GBL_MURMUR_HASH_FMIX32_
#undef GBL_MURMUR_HASH_GET_BLOCK_
}

GBL_FORCE_INLINE GblHash gblHashMurmur(const void* pData, GblSize size) GBL_NOEXCEPT {
    uint32_t out;
    gblMurmurHash3_x86_32_(pData, size, gblSeed(0), &out);
    return out;
}

GBL_FORCE_INLINE GblHash gblHashFnv1(const void* pData, GblSize size) GBL_NOEXCEPT {
    uint32_t prime = 0x01000193;
    uint32_t hash = 0x811C9DC5;
    const unsigned char* p = (const unsigned char*)pData;
    for (size_t i = 0; i < size; ++i) {
        hash = (hash * prime) ^ *p++;
    }
    return hash;
}

//
// super hash function by Paul Hsieh
//
GBL_INLINE GblHash gblHashSuperFast(const void* pData, GblSize size) GBL_NOEXCEPT {
#define GBL_HASH_SUPER_FAST_GET_16_BITS_(d) \
    ((((uint32_t)(((const uint8_t *)(d))[1])) << 8) + (uint32_t)(((const uint8_t *)(d))[0]))

   uint32_t hash = size, tmp;
   const char* data = (const char*)pData;
   int rem;

    if (size <= 0 || pData == NULL) return 0;

    rem = size & 3;
    size >>= 2;

    /* Main loop */
    for (;size > 0; size--) {
        hash  += GBL_HASH_SUPER_FAST_GET_16_BITS_ (data);
        tmp    = (GBL_HASH_SUPER_FAST_GET_16_BITS_ (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += GBL_HASH_SUPER_FAST_GET_16_BITS_ (data);
            hash ^= hash << 16;
            hash ^= data[sizeof (uint16_t)] << 18;
            hash += hash >> 11;
            break;
        case 2: hash += GBL_HASH_SUPER_FAST_GET_16_BITS_ (data);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        case 1: hash += *data;
            hash ^= hash << 10;
            hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
#undef GBL_HASH_SUPER_FAST_GET_16_BITS_
}


GBL_DECLS_END


#endif // GIMBAL_HASH_H
