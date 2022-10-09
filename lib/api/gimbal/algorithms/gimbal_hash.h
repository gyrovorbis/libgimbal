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

#define GBL_SEED_COUNT                  2

GBL_DECLS_BEGIN


GBL_INLINE int      gblRand              (void)             GBL_NOEXCEPT;
GBL_INLINE int      gblRandRange         (int min, int max) GBL_NOEXCEPT;
GBL_INLINE int      gblRandString        (char* pBuffer, int minSize, int maxSize, const char* pCharList) GBL_NOEXCEPT;
GBL_INLINE uint64_t gblSeed              (uint8_t index)    GBL_NOEXCEPT;

typedef    GblHash (*GblHashFn)(const void*, GblSize);

// make default hash function of entire bitch
GBL_INLINE GblHash  gblHash              (const void* pData, GblSize size, GblHashFn pFnHash) GBL_NOEXCEPT;


GBL_EXPORT GblHash  gblHashSip           (const void* pData, GblSize size) GBL_NOEXCEPT;
GBL_INLINE GblHash  gblHashMurmur        (const void* pData, GblSize size) GBL_NOEXCEPT;
GBL_INLINE GblHash  gblHashFnv1          (const void* pData, GblSize size) GBL_NOEXCEPT;
GBL_INLINE GblHash  gblHashCrc           (const void* pData, GblSize size) GBL_NOEXCEPT;

/// @}








// ===== INLINE IMPLEMENTATIONS =====

GBL_MAYBE_UNUSED GBL_INLINE
GblHash gblHash(const void* pData, GblSize size, GblHashFn pFnHash) GBL_NOEXCEPT {
    GBL_ASSERT(pFnHash);
    return pFnHash(pData, size);
}

GBL_MAYBE_UNUSED GBL_INLINE GblHash gblCrc32LittleEndian (const void* pData, GblSize size) GBL_NOEXCEPT {
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


GBL_FORCE_INLINE uint32_t fmix32 ( uint32_t h ) GBL_NOEXCEPT {
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here
#if !defined(__DREAMCAST__) && !defined(__VITA__)
#   define getblock(k, p, i) (k = p[i])
#else
#   define getblock(k, p, i) memcpy(&k, p + i, sizeof(uint32_t))
#endif
GBL_INLINE void MurmurHash3_x86_32 ( const void * key, int len,
                          uint32_t seed, void * out ) GBL_NOEXCEPT {
#define	ROTL32(x, r) ((x << r) | (x >> (32 - r)))
#define FMIX32(h) h^=h>>16; h*=0x85ebca6b; h^=h>>13; h*=0xc2b2ae35; h^=h>>16;
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
      getblock(k1, blocks, i);

      k1 *= c1;
      k1 = ROTL32(k1,15);
      k1 *= c2;

      h1 ^= k1;
      h1 = ROTL32(h1,13);
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
            k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
    };

    //----------
    // finalization

    h1 ^= len;

    h1 = fmix32(h1);

    *(uint32_t*)out = h1;
}

GBL_INLINE GblHash gblHashMurmur(const void* pData, GblSize size) GBL_NOEXCEPT {
    uint32_t out;
#if 0
    MurmurHash3_x86_32(pData, size, gblSeed(0), &out);
#else
    return gblHashFnv1(pData, size);
#endif
    return out;
}

GBL_INLINE GblHash gblHashFnv1(const void* pData, GblSize size) GBL_NOEXCEPT {
    uint32_t prime = 0x01000193;
    uint32_t hash = 0x811C9DC5;
    const unsigned char* p = (const unsigned char*)pData;
    for (size_t i = 0; i < size; ++i) {
        hash = (hash * prime) ^ *p++;
    }
    return hash;
}


GBL_DECLS_END


#endif // GIMBAL_HASH_H
