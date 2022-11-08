#ifndef GIMBAL_NUMERIC_H
#define GIMBAL_NUMERIC_H

/*! \file
 *  \brief General numeric computations
 *  \ingroup algorithms
 */

#include "../core/gimbal_decls.h"
#include "../preprocessor/gimbal_macro_utils.h"

GBL_DECLS_BEGIN

/*! \addtogroup algorithms
 * @{
 */

#define           gblPow2Next(X)             GBL_META_GENERIC_MACRO_GENERATE(GBL_POW2_NEXT_TRAITS_, X)(X)
GBL_INLINE GBL_CONSTEXPR
    int           gblFibonacci               (int n)                       GBL_NOEXCEPT;
GBL_INLINE GBL_CONSTEXPR
    int           gblGcd                     (int u, int v)                GBL_NOEXCEPT;
GBL_INLINE GBL_CONSTEXPR
    int           gblLcm                     (int u, int v)                GBL_NOEXCEPT;
GBL_INLINE
    uint32_t      gblNtohl                   (uint32_t n)                  GBL_NOEXCEPT;
GBL_INLINE
    uint32_t      gblHtonl                   (uint32_t x)                  GBL_NOEXCEPT;
GBL_INLINE GBL_CONSTEXPR
    int           gblParity                  (uint8_t n)                   GBL_NOEXCEPT;
GBL_INLINE GBL_CONSTEXPR
    GblSize       gblAlignedAllocSizeDefault (GblSize bytes)               GBL_NOEXCEPT;
GBL_INLINE GBL_CONSTEXPR
    GblSize       gblAlignedAllocSize        (GblSize size, GblSize align) GBL_NOEXCEPT;
GBL_EXPORT
    uint32_t      gblPrimeNextDouble         (uint32_t number)             GBL_NOEXCEPT;
GBL_INLINE GBL_CONSTEXPR
    GblBool       gblPrimeCheck              (int n)                       GBL_NOEXCEPT;
GBL_INLINE GBL_CONSTEXPR
    unsigned long gblBinomialCoeff           (unsigned n, unsigned k)      GBL_NOEXCEPT;
GBL_INLINE
    unsigned long gblCatalan                 (unsigned n)                  GBL_NOEXCEPT;

/// @}

// ===== IMPL =====

#define GBL_POW2_NEXT_TRAITS_ (            \
        gblPow2Next_u64,                   \
        (                                  \
            (uint8_t,  gblPow2Next_u8),    \
            (uint16_t, gblPow2Next_u16),   \
            (uint32_t, gblPow2Next_u32),   \
            (uint64_t, gblPow2Next_u64)    \
        )                                  \
    )

GBL_INLINE GBL_CONSTEXPR uint8_t gblPow2Next_u8(uint8_t n) GBL_NOEXCEPT {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4;
    return n + 1;
}

GBL_INLINE GBL_CONSTEXPR uint16_t gblPow2Next_u16(uint16_t n) GBL_NOEXCEPT {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8;
    return n + 1;
}

GBL_INLINE GBL_CONSTEXPR uint32_t gblPow2Next_u32(uint32_t n) GBL_NOEXCEPT {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8; n |= n >> 16;
    return n + 1;
}

GBL_INLINE GBL_CONSTEXPR uint64_t gblPow2Next_u64(uint64_t n) GBL_NOEXCEPT {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8; n |= n >> 16; n |= n >> 32;
    return n + 1;
}

GBL_INLINE GBL_CONSTEXPR
GblBool gblPrimeCheck(int n) GBL_NOEXCEPT {
    for(int i = 2; i <= n/2; i++)
        if(n % i == 0) return GBL_FALSE;
    return GBL_TRUE;
}

GBL_INLINE GBL_CONSTEXPR
int gblFibonacci(int n) GBL_NOEXCEPT {
    int num1 = 0, num2 = 1, num3 = 0;

    if(!n)
      return num1;
    for(int i = 2; i <= n; i++) {
      num3 = num1 + num2;
      num1 = num2;
      num2 = num3;
    }

    return num2;
}

GBL_INLINE GBL_CONSTEXPR
unsigned long gblBinomialCoeff(unsigned n, unsigned k) GBL_NOEXCEPT {
    unsigned long int res = 1;

    // Since C(n, k) = C(n, n-k)
    if (k > n - k)
        k = n - k;

    // Calculate value of [n*(n-1)*---*(n-k+1)] /
    // [k*(k-1)*---*1]
    for (unsigned i = 0; i < k; ++i) {
        res *= (n - i);
        res /= (i + 1);
    }

    return res;
}

// Find nth catalan number in O(n) time
GBL_INLINE unsigned long int gblCatalan(unsigned int n) GBL_NOEXCEPT {
    // Calculate value of 2nCn
    unsigned long int c = gblBinomialCoeff(2 * n, n);

    // return 2nCn/(n+1)
    return c / (n + 1);
}

GBL_INLINE GBL_CONSTEXPR GblSize gblAlignedAllocSizeDefault(GblSize bytes) GBL_NOEXCEPT {
    const GblSize remainder = bytes % GBL_ALLOC_MIN_SIZE;
    GblSize newSize = bytes;
    if(remainder) {
        newSize += GBL_ALLOC_MIN_SIZE - remainder;
    }
    return newSize;
}

GBL_INLINE GBL_CONSTEXPR GblSize gblAlignedAllocSize(GblSize size, GblSize align) GBL_NOEXCEPT {
    if(!align) align = GBL_ALIGNOF(GBL_MAX_ALIGN_T);
    const GblSize alignRem = size % align;
    GblSize newSize = size;
    if(alignRem) {
        newSize += (align - (alignRem));
    }
    return newSize;
}

// GCD using Euclid's algorithm, only >0 values
GBL_INLINE GBL_CONSTEXPR int gblGcd(int u, int v) GBL_NOEXCEPT {
    int R = 0;
    //if(!u) return v;
    //if(!v) return u;
    while ((u % v) > 0) GBL_LIKELY {
      R = u % v;
      u = v;
      v = R;
    }
    return v;
}

GBL_INLINE GBL_CONSTEXPR int gblLcm(int u, int v) GBL_NOEXCEPT {
    if(!u || !v) GBL_UNLIKELY return 0;
    const int absU      = abs(u);
    const int absV      = abs(v);
    const int absHigher = GBL_MAX(absU, absV);
    const int absLower  = GBL_MIN(absU, absV);
    int       lcm       = absHigher;
    while(lcm % absLower) {
        lcm += absHigher;
    }
    return lcm;
}

GBL_INLINE GBL_CONSTEXPR int gblParity(uint8_t n) GBL_NOEXCEPT {
    int p = 0;
    while(n) GBL_LIKELY {
        p = !p;
        n &= (n-1);
    }
    return p;
}

GBL_INLINE uint32_t gblNtohl(uint32_t n) GBL_NOEXCEPT {
    unsigned char *np = (unsigned char *)&n;
    return ((uint32_t)np[0] << 24) |
           ((uint32_t)np[1] << 16) |
           ((uint32_t)np[2] << 8)  |
            (uint32_t)np[3];
}

GBL_INLINE uint32_t gblHtonl(uint32_t x) GBL_NOEXCEPT {
#if GBL_BIG_ENDIAN == 0
    uint8_t *s = (uint8_t *)&x;
    return (uint32_t)(s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3]);
#else
    return x;
#endif
}


GBL_DECLS_END

#endif // GIMBAL_NUMERIC_H
