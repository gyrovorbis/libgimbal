#include <gimbal/algorithms/gimbal_compression.h>
#include <lz4.h>

GBL_EXPORT int gblCompress(const void* pSrc,
                           void*       pDst,
                           size_t      srcSize,
                           size_t      dstCapacity,
                           int         compressionLevel)
{
    return LZ4_compress_fast(pSrc, pDst, srcSize, dstCapacity, compressionLevel);
}

GBL_EXPORT int gblDecompress(const void* pSrc,
                             void*       pDst,
                             size_t      srcSize,
                             size_t      dstCapacity)
{
    return LZ4_decompress_safe(pSrc, pDst, srcSize, dstCapacity);
}
