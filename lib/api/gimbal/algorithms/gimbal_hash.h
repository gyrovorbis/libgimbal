/*! \file
 *  \brief Collection of hashing algorithms
 *  \ingroup hashing
 *
 *  This file contains the hashing API, which
 *  is a collection of various hashing algorithms
 *  for calculating the hashes of either arbitrary
 *  buffers of data or primitive types of specific
 *  sizes.
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_HASH_H
#define GIMBAL_HASH_H

#include "../core/gimbal_decls.h"


/*! \defgroup hashing Hashing
 *  \ingroup algorithms
 *  \brief Collection of hash algorithms.
*/

//! Default hashing algorithm used internally by libGimbal
#ifndef gblHash
#   define gblHash  gblHashFnv1
#endif

GBL_DECLS_BEGIN

/*! \name Fixed data sizes
 *  \brief Methods for calculating the hash of primitive types
 *  @{
 */

//! Calculate the hash of a given 32-bit value
GBL_EXPORT GblHash  gblHash32Bit   (uint32_t value) GBL_NOEXCEPT;
//! Retrieve original 32-bit value from its hash
GBL_EXPORT uint32_t gblUnhash32Bit (GblHash hash)   GBL_NOEXCEPT;
//! Calculate the hash of the given 16-bit value
GBL_EXPORT GblHash  gblHash16Bit   (uint16_t value) GBL_NOEXCEPT;
//! @}

//! Calculates the hash of a given buffer continuing the partial hash from a previous iteration (or taking NULL for none)
GBL_EXPORT uint16_t gblHashCrc16BitPartial (const void* pData,
                                            size_t      size,
                                            uint16_t*   pPartial) GBL_NOEXCEPT;
//! @}

/*! \name  Arbitrarily-sized data
 *  \brief Methods for calculating the hash of generic buffers
 *  \ingroup hashing
 *  @{
 */
//! Returns the hash of the given data buffer, calculated using the SIP algorithm
GBL_EXPORT GblHash  gblHashSip       (const void* pData, size_t size) GBL_NOEXCEPT;
//! Returns the hash of the given data buffer, calculated using the MurmurHash32 algorithm
GBL_EXPORT GblHash  gblHashMurmur    (const void* pData, size_t size) GBL_NOEXCEPT;
//! Returns the hash of the given data buffer, calculated using the a custom FNV1 algorithm
GBL_EXPORT GblHash  gblHashFnv1      (const void* pData, size_t size) GBL_NOEXCEPT;
//! Returns the hash of the given data buffer, calculated using the xxHash algorithm
GBL_EXPORT GblHash  gblHashXx        (const void* pData, size_t size) GBL_NOEXCEPT;
//! Returns the hash of the given data buffer, calculated using the SuperFastHash algorithm
GBL_EXPORT GblHash  gblHashSuperFast (const void* pData, size_t size) GBL_NOEXCEPT;
//! Returns the hash of the given data buffer, calculated using the Pearson hashing algorithm
GBL_EXPORT GblHash  gblHashPearson   (const void* pData, size_t size) GBL_NOEXCEPT;
//! Returns the hash of the given data buffer, calculated using the Jenkins hashing algorithm
GBL_EXPORT GblHash  gblHashJenkins   (const void* pData, size_t size) GBL_NOEXCEPT;
//! Returns the CRC value of the given data buffer
GBL_EXPORT GblHash  gblHashCrc       (const void* pData, size_t size) GBL_NOEXCEPT;
//! Returns the SHA1 hash calculated for the given data buffer
GBL_EXPORT GblHash  gblHashSha1      (const void* pData, size_t size) GBL_NOEXCEPT;
//! Returns the MD5 hash calculated for the given data buffer
GBL_EXPORT GblHash  gblHashMd5       (const void* pData, size_t size) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#endif // GIMBAL_HASH_H
