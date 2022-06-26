#ifndef GIMBAL_HASH_H
#define GIMBAL_HASH_H

#include "../types/gimbal_typedefs.h"
#include "../core/gimbal_api_frame.h"
#include <time.h>

#define GBL_SEED_COUNT                  2

GBL_DECLS_BEGIN


GBL_INLINE int      gblRand              (void)             GBL_NOEXCEPT;
GBL_INLINE int      gblRandRange         (int min, int max) GBL_NOEXCEPT;
GBL_INLINE uint64_t gblSeed              (uint8_t index)    GBL_NOEXCEPT;

typedef    GblHash (*GblHashFn)(const void*, GblSize);

// make default hash function of entire bitch
GBL_INLINE GblHash  gblHash              (const void* pData, GblSize size, GblHashFn pFnHash) GBL_NOEXCEPT;


GBL_EXPORT GblHash  gblHashSip           (const void* pData, GblSize size) GBL_NOEXCEPT;
GBL_EXPORT GblHash  gblHashMurmur        (const void* pData, GblSize size) GBL_NOEXCEPT;
GBL_INLINE GblHash  gblHashCrc           (const void* pData, GblSize size) GBL_NOEXCEPT;










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


GBL_DECLS_END


#endif // GIMBAL_HASH_H
