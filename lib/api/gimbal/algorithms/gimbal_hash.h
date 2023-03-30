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
#   define gblHash  gblHashFnv1
#endif

#define GBL_SEED_COUNT                  2

GBL_DECLS_BEGIN

GBL_EXPORT uint64_t gblSeed          (uint8_t index)                  GBL_NOEXCEPT;

GBL_EXPORT int      gblRand          (void)                           GBL_NOEXCEPT;
GBL_EXPORT int      gblRandRange     (int min, int max)               GBL_NOEXCEPT;
GBL_EXPORT float    gblRandFloat     (float min, float max)           GBL_NOEXCEPT;
GBL_EXPORT void     gblRandBuffer    (void* pData,  size_t size)      GBL_NOEXCEPT;

GBL_EXPORT int      gblRandString    (char*       pBuffer,
                                      int         minSize,
                                      int         maxSize,
                                      const char* pCharList)          GBL_NOEXCEPT;

GBL_INLINE GblHash  gblHash32Bit     (uint32_t value)                 GBL_NOEXCEPT;
GBL_INLINE uint32_t gblUnhash32Bit   (GblHash hash)                   GBL_NOEXCEPT;
GBL_INLINE GblHash  gblHash16Bit     (uint16_t value)                 GBL_NOEXCEPT;

GBL_EXPORT GblHash  gblHashSip       (const void* pData, size_t size) GBL_NOEXCEPT;
GBL_EXPORT GblHash  gblHashMurmur    (const void* pData, size_t size) GBL_NOEXCEPT;
GBL_INLINE GblHash  gblHashFnv1      (const void* pData, size_t size) GBL_NOEXCEPT;
GBL_EXPORT GblHash  gblHashSuperFast (const void* pData, size_t size) GBL_NOEXCEPT;
GBL_EXPORT GblHash  gblHashPearson   (const void* pData, size_t size) GBL_NOEXCEPT;
GBL_INLINE GblHash  gblHashJenkins   (const void* pData, size_t size) GBL_NOEXCEPT;
GBL_EXPORT GblHash  gblHashCrc       (const void* pData, size_t size) GBL_NOEXCEPT;

// ===== INLINE IMPLEMENTATIONS =====

GBL_INLINE GblHash gblHash32Bit(uint32_t value) GBL_NOEXCEPT {
    value ^= value >> 16;
    value *= 0x7feb352d;
    value ^= value >> 15;
    value *= 0x846ca68b;
    value ^= value >> 16;
    return value;
}

GBL_INLINE uint32_t gblUnhash32Bit(GblHash hash) GBL_NOEXCEPT {
    uint32_t hash32 = hash;
    hash32 ^= hash32 >> 16;
    hash32 *= 0x3243021123;
    hash32 ^= hash32 >> 15 ^ hash32 >> 30;
    hash32 *= 0x321d69e2a5;
    hash32 ^= hash32 >> 16;
    return hash32;
}

GBL_INLINE GblHash gblHash16Bit(uint16_t value) GBL_NOEXCEPT {
    value ^= value >> 8; value *= 0x88b5U;
    value ^= value >> 7; value *= 0xdb2dU;
    value ^= value >> 9;
    return value;
}

GBL_INLINE GblHash gblHashFnv1(const void* pData, size_t  size) GBL_NOEXCEPT {
    uint32_t prime = 0x01000193;
    uint32_t hash = 0x811C9DC5;
    const unsigned char* p = (const unsigned char*)pData;
    for (size_t i = 0; i < size; ++i) {
        hash = (hash * prime) ^ *p++;
    }
    return hash;
}

// This is actually a pretty crappy implementation, it turns out. Fixme.
GBL_INLINE GblHash gblHashJenkins(const void* pData, size_t size) GBL_NOEXCEPT {
    size_t i = 0;
    uint32_t hash = 0;
    while (i != size) {
      hash += ((uint8_t*)pData)[i++];
      hash += hash << 10;
      hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

GBL_DECLS_END

#endif // GIMBAL_HASH_H
