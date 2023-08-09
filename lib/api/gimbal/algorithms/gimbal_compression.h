#ifndef GIMBAL_COMPRESSION_H
#define GIMBAL_COMPRESSION_H

#include "../core/gimbal_decls.h"

GBL_DECLS_BEGIN

GBL_EXPORT int gblCompress(const void* pSrc,
                           void*       pDst,
                           size_t      srcSize,
                           size_t      dstCapacity,
                           int         compressionLevel) GBL_NOEXCEPT;

GBL_EXPORT int gblDecompress(const void* pSrc,
                             void*       pDst,
                             size_t      srcSize,
                             size_t      dstCapacity) GBL_NOEXCEPT;

GBL_DECLS_END

#endif // GIMBAL_COMPRESSION_H
