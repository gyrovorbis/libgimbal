/*! \file
 *  \brief Low-level compression API
 *  \ingroup algorithms
 *
 *  This file contains the lowest-level core API for
 *  compression and decompression. It operates on raw
 *  byte buffers.
 *
 *  \note
 *  The underlying compression library and algorithm
 *  used by this API is LZ4.
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_COMPRESSION_H
#define GIMBAL_COMPRESSION_H

#include "../core/gimbal_decls.h"

GBL_DECLS_BEGIN

//! Compresses the source buffer into the sized destination buffer, returning its actual compressed size
GBL_EXPORT int gblCompress(const void* pSrc,
                           void*       pDst,
                           size_t      srcSize,
                           size_t      dstCapacity,
                           int         compressionLevel) GBL_NOEXCEPT;

//! Decompresses the source buffer into the sized destination buffer, returning its actual decompressed size
GBL_EXPORT int gblDecompress(const void* pSrc,
                             void*       pDst,
                             size_t      srcSize,
                             size_t      dstCapacity) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_COMPRESSION_H
