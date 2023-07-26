/*! \file
 *  \brief Random number generation, hashing, and CRC algorithms
 *  \ingroup hashing
 *
 *  \author 2023 Falco Girgis
 *  \copyright MIT License
 */

#ifndef GIMBAL_HASH_H
#define GIMBAL_HASH_H

#include "../core/gimbal_decls.h"

// default hashing algorithm
#ifndef gblHash
#   define gblHash  gblHashFnv1
#endif

#define GBL_SEED_COUNT      2

GBL_DECLS_BEGIN

/*! \defgroup hashing Hashing
 *  \ingroup algorithms
 *  \brief Collection of hash algorithms.
 * @{
*/
GBL_EXPORT uint64_t gblSeed          (uint8_t index)                  GBL_NOEXCEPT;

GBL_EXPORT int      gblRand          (void)                           GBL_NOEXCEPT;
GBL_EXPORT int      gblRandRange     (int min, int max)               GBL_NOEXCEPT;
GBL_EXPORT float    gblRandFloat     (float min, float max)           GBL_NOEXCEPT;
GBL_EXPORT void     gblRandBuffer    (void* pData,  size_t size)      GBL_NOEXCEPT;

GBL_EXPORT int      gblRandString    (char*       pBuffer,
                                      int         minSize,
                                      int         maxSize,
                                      const char* pCharList)          GBL_NOEXCEPT;

GBL_EXPORT GblHash  gblHash32Bit     (uint32_t value)                 GBL_NOEXCEPT;
GBL_EXPORT uint32_t gblUnhash32Bit   (GblHash hash)                   GBL_NOEXCEPT;
GBL_EXPORT GblHash  gblHash16Bit     (uint16_t value)                 GBL_NOEXCEPT;

GBL_EXPORT uint16_t gblHashCrc16BitPartial
                                     (const void* pData,
                                      size_t      size,
                                      uint16_t*   pPartial)           GBL_NOEXCEPT;

GBL_EXPORT GblHash  gblHashSip       (const void* pData, size_t size) GBL_NOEXCEPT;
GBL_EXPORT GblHash  gblHashMurmur    (const void* pData, size_t size) GBL_NOEXCEPT;
GBL_EXPORT GblHash  gblHashFnv1      (const void* pData, size_t size) GBL_NOEXCEPT;
GBL_EXPORT GblHash  gblHashSuperFast (const void* pData, size_t size) GBL_NOEXCEPT;
GBL_EXPORT GblHash  gblHashPearson   (const void* pData, size_t size) GBL_NOEXCEPT;
GBL_EXPORT GblHash  gblHashJenkins   (const void* pData, size_t size) GBL_NOEXCEPT;
GBL_EXPORT GblHash  gblHashCrc       (const void* pData, size_t size) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#endif // GIMBAL_HASH_H
