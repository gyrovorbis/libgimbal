/*! \file
 *  \brief   MD5 cryptographic hashing algorithm and API
 *  \ingroup hashing
 *
 *  This file contains the libGimbal API providing the
 *  MD5 cryptographic hashing algorithm.
 *
 *  \author     1990 RLR
 *  \authors    1991 SRD, AJ, BSK, JT
 *  \author     2023 Falco Girgis
 *
 *  \copyright  1990 RSA Data Security, Inc.
 */
#ifndef GIMBAL_MD5_H
#define GIMBAL_MD5_H

#include "../core/gimbal_decls.h"

#define GBL_MD5_DIGEST_SIZE         16
#define GBL_MD5_DIGEST_STRING_SIZE  64

#define GBL_SELF_TYPE GblMd5Context

GBL_DECLS_BEGIN

/*! Data structure for MD5 (Message-Digest) computation
 *  \ingroup hashing
 */
typedef struct GblMd5Context {
  uint32_t   i[2];    //!< Number of _bits_ handled mod 2^64
  uint32_t  buf[4];   //!< Scratch buffer
  uint8_t   in[64];   //!< Input buffer
} GblMd5Context;

/*! \name Incremental hashing
 *  \brief Methods for operating on an MD5 context
 *  \relatesalso GblMd5context
 *  @{
 */
//! Initializes the given MD5 context
GBL_EXPORT void GblMd5_init   (GBL_SELF)                                      GBL_NOEXCEPT;
//! Adds the data in \p pData of \p bytes length to the running MD5 hash context
GBL_EXPORT void GblMd5_update (GBL_SELF, const void* pData, size_t bytes)     GBL_NOEXCEPT;
//! Finalizes the running MD5 hash context, returning its digest
GBL_EXPORT void GblMd5_final  (GBL_SELF, uint8_t digest[GBL_MD5_DIGEST_SIZE]) GBL_NOEXCEPT;
//! @}

//! Returns the display-friendly hexadecimal form of the given digest, with each 32-bit word separated by a space
GBL_EXPORT const char* GblMd5_string (const uint8_t digest[GBL_MD5_DIGEST_SIZE],
                                      char          buffer[GBL_MD5_DIGEST_STRING_SIZE]) GBL_NOEXCEPT;

//! Convenience function for generating a 32-bit MD5 hash from the given bytes
GBL_EXPORT GblHash gblHashMd5 (const void* pData, size_t bytes) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_MD5_H
