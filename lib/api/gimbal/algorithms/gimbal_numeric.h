#ifndef GIMBAL_NUMERIC_H
#define GIMBAL_NUMERIC_H

/*! \file
 *  \brief General numeric computations
 *  \ingroup algorithms
 */

#include "../types/gimbal_typedefs.h"
#include "../preprocessor/gimbal_macro_utils.h"

GBL_DECLS_BEGIN


GBL_INLINE GBL_CONSTEXPR
uint8_t gblPow2Next_u8(uint8_t n) GBL_NOEXCEPT {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4;
    return n + 1;
}

GBL_INLINE GBL_CONSTEXPR
uint16_t gblPow2Next_u16(uint16_t n) GBL_NOEXCEPT {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8;
    return n + 1;
}

GBL_INLINE GBL_CONSTEXPR
uint32_t gblPow2Next_u32(uint32_t n) GBL_NOEXCEPT {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8; n |= n >> 16;
    return n + 1;
}

GBL_INLINE GBL_CONSTEXPR
uint64_t gblPow2Next_u64(uint64_t n) GBL_NOEXCEPT {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8; n |= n >> 16; n |= n >> 32;
    return n + 1;
}

#define GBL_POW2_NEXT_TRAITS (             \
        gblPow2Next_u64,                   \
        (                                  \
            (uint8_t,  gblPow2Next_u8),    \
            (uint16_t, gblPow2Next_u16),   \
            (uint32_t, gblPow2Next_u32),   \
            (uint64_t, gblPow2Next_u64)    \
        )                                  \
    )
/*! \addtogroup algorithms
 * @{
 */
#define gblPow2Next(X) GBL_META_GENERIC_MACRO_GENERATE(GBL_POW2_NEXT_TRAITS, X)(X);

GBL_INLINE GBL_CONSTEXPR
GblSize gblAlignedAllocSize(GblSize bytes) GBL_NOEXCEPT {
    const GblSize remainder = bytes % GBL_ALLOC_MIN_SIZE;
    GblSize newSize = bytes;
    if(remainder) {
        newSize += GBL_ALLOC_MIN_SIZE - remainder;
    }
    return newSize;
}

// GCD using Euclid's algorithm, only >0 values
GBL_INLINE GBL_CONSTEXPR
GblInt gblGcd(GblInt u, GblInt v) GBL_NOEXCEPT {
    GblInt R;
    //if(!u) return v;
    //if(!v) return u;
    while ((u % v) > 0) GBL_LIKELY {
      R = u % v;
      u = v;
      v = R;
    }
    return v;
}

GBL_INLINE GBL_CONSTEXPR
GblInt gblParity(uint8_t n) {
    int p = 0;
    while(n) GBL_LIKELY {
        p = !p;
        n &= (n-1);
    }
    return p;
}

GBL_INLINE GBL_CONSTEXPR
uint32_t gblNtohl(uint32_t n) GBL_NOEXCEPT {
    unsigned char *np = (unsigned char *)&n;
    return ((uint32_t)np[0] << 24) |
           ((uint32_t)np[1] << 16) |
           ((uint32_t)np[2] << 8)  |
            (uint32_t)np[3];
}

GBL_INLINE GBL_CONSTEXPR
uint32_t gblHtonl(uint32_t x) GBL_NOEXCEPT {
#if GBL_BIG_ENDIAN == 0
    uint8_t *s = (uint8_t *)&x;
    return (uint32_t)(s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3]);
#else
    return x;
#endif
}

/// @}


GBL_DECLS_END



#endif // GIMBAL_NUMERIC_H
