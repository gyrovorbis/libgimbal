/*! \file
 *  \brief   SHA1 cryptographic hashing algorithm and API
 *  \ingroup hashing
 *
 *  This file contains the libGimbal API providing the
 *  SHA1 cryptographic hashing algorithm.
 *
 *  \warning
 *  Even though the implementation has been validated via
 *  unit testing, it has never been validated by NIST, so
 *  it cannot be said to be in official compliance with the
 *  standard.
 *
 *  \author     1998 Steve Reid
 *  \author     1998 James H. Brown
 *  \author     2001 Saul Kravitz
 *  \author     2002 Ralph giles
 *  \author     2023 Falco Girgis
 *
 *  \copyright  Public Domain
 */
#ifndef GIMBAL_SHA1_H
#define GIMBAL_SHA1_H

#include "../core/gimbal_decls.h"

#define GBL_SHA1_DIGEST_SIZE           20   //!< Size of a SHA1 digest
#define GBL_SHA1_DIGEST_STRING_SIZE    80   //!< Buffer sized required for GblSha1_string()

#define GBL_SELF_TYPE GblSha1Context

GBL_DECLS_BEGIN

/*! Context structure used for maintaining SHA1 hashing state
 *  \ingroup hashing
 */
typedef struct GblSha1Context {
    uint32_t      state[5];   //!< Current accumulated hash state
    uint32_t      count[2];   //!< Running counter of # of hashed blocks
    unsigned char buffer[64]; //!< Buffer containing current 512-bit block of data
} GblSha1Context;

/*! \name Incremental hashing
 *  \brief Methods for operating on an SHA1 context
 *  \relatesalso GblSha1Context
 *  @{
 */
//! Initializes the given SHA1 context
GBL_EXPORT void GblSha1_init   (GBL_SELF) GBL_NOEXCEPT;

//! Adds the data in \p pData of \p length bytes to the running SHA1 hash context
GBL_EXPORT void GblSha1_update (GBL_SELF,
                                const void* pData,
                                size_t      length) GBL_NOEXCEPT;
//! Finalizes the running SHA1 hash context, returning its digest
GBL_EXPORT void GblSha1_final  (GBL_SELF,
                                uint8_t digest[GBL_SHA1_DIGEST_SIZE]) GBL_NOEXCEPT;
//! @}

//! Returns the display-friendly hexadecimal form of the given digest, with each 32-bit word separated by a space
GBL_EXPORT const char*
                GblSha1_string (const uint8_t digest[GBL_SHA1_DIGEST_SIZE],
                                char          buffer[GBL_SHA1_DIGEST_STRING_SIZE]) GBL_NOEXCEPT;

/*! Standalone convenience function for generating a 32-bit SHA1 hash from the given bytes
 * \ingroup hashing
*/
GBL_EXPORT GblHash gblHashSha1 (const void* pData, size_t size) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_SHA1_H
