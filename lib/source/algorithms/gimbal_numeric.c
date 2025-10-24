#ifndef GIMBAL_NUMERIC_C
#define GIMBAL_NUMERIC_C

#include <gimbal/algorithms/gimbal_numeric.h>

GBL_EXPORT uint8_t gblPow2Next_u8(uint8_t n) {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4;
    return n + 1;
}

GBL_EXPORT uint16_t gblPow2Next_u16(uint16_t n) {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8;
    return n + 1;
}

GBL_EXPORT uint32_t gblPow2Next_u32(uint32_t n) {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8; n |= n >> 16;
    return n + 1;
}

GBL_EXPORT uint64_t gblPow2Next_u64(uint64_t n) {
    GBL_ASSERT(n >= 2);
    --n;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8; n |= n >> 16; n |= n >> 32;
    return n + 1;
}

GBL_EXPORT GblBool gblPow2Check(size_t n) {
    return (n & (n - 1)) == 0;
}

GBL_EXPORT
GblBool gblPrimeCheck(int n) {
    for(int i = 2; i <= n/2; i++)
        if(n % i == 0) return GBL_FALSE;
    return GBL_TRUE;
}

GBL_EXPORT
int gblFibonacci(int n) {
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

GBL_EXPORT
unsigned long gblBinomialCoeff(unsigned n, unsigned k) {
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
GBL_EXPORT unsigned long int gblCatalan(unsigned int n) {
    // Calculate value of 2nCn
    unsigned long int c = gblBinomialCoeff(2 * n, n);

    // return 2nCn/(n+1)
    return c / (n + 1);
}


GBL_EXPORT size_t gblAlignedAllocSizeDefault(size_t bytes) {
    return gblAlignedAllocSize(bytes, 0);
}

GBL_EXPORT size_t (gblAlignedAllocSize)(size_t size, size_t align) {
    if(!align) align = GBL_ALIGNOF(GBL_MAX_ALIGN_T);
    const size_t alignRem = size % align;
    size_t newSize = size;

    if(alignRem) {
        newSize += (align - (alignRem));
    }

    return newSize;
}

// GCD using Euclid's algorithm, only >0 values
GBL_EXPORT int gblGcd(int u, int v) {
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

GBL_EXPORT int gblLcm(int u, int v) {
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

GBL_EXPORT int gblParity(uint8_t n) {
    int p = 0;
    while(n) GBL_LIKELY {
        p = !p;
        n &= (n-1);
    }
    return p;
}

GBL_EXPORT uint32_t gblNtohl(uint32_t n) {
    unsigned char *np = (unsigned char *)&n;
    return ((uint32_t)np[0] << 24) |
           ((uint32_t)np[1] << 16) |
           ((uint32_t)np[2] << 8)  |
            (uint32_t)np[3];
}

GBL_EXPORT uint32_t gblHtonl(uint32_t x) {
#if GBL_BIG_ENDIAN == 0
    uint8_t *s = (uint8_t *)&x;
    return (uint32_t)(s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3]);
#else
    return x;
#endif
}

GBL_EXPORT GblBool gblFloatEquals(double a, double b, double e) {
    GBL_ASSERT(e >= DBL_EPSILON, "Epsilon value is smaller than system supports.");
    GBL_ASSERT(e < 1.0f, "Epsilon must be fractional value.");

#if 0
    const double diff = fabs(a - b);

    if(a == b)
        return GBL_TRUE;
    else if(a == 0 || b == 0 || diff < DBL_MIN)
        return (diff < (e * DBL_MIN));
    else
        return (diff / (fabs(a) + fabs(b)) < e);
#else
    // Calculate the difference.
    const double diff = fabs(a - b);
    a = fabs(a);
    b = fabs(b);
    // Find the largest
    const double max = GBL_MAX(a, b);

    if (diff <= max * e)
        return GBL_TRUE;
    return GBL_FALSE;
#endif
}

GBL_FP_PRECISE
GBL_EXPORT double gblAbsoluteError(double approximate, double exact) {
    return fabs(exact - approximate);
}

GBL_FP_PRECISE
GBL_EXPORT double gblRelativeError(double approximate, double exact) {
    if(fabs(exact) > DBL_EPSILON)
        return gblAbsoluteError(approximate, exact) / fabs(exact);
    else if(gblAbsoluteError(approximate, exact) < 0.0001)
        return 0.0f;
    else
        return INFINITY;
}

GBL_EXPORT float gblDegToRadf(float degrees) {
    return degrees * (GBL_F_PI / 180.0f);
}

GBL_EXPORT float gblRadToDegf(float radians) {
    return radians * (180.0f / GBL_F_PI);
}

static uint32_t primes_[] = {
    5,
    11,
    23,
    47,
    97,
    197,
    397,
    797,
    1597,
    3203,
    6421,
    12853,
    25717,
    51437,
    102877,
    205759,
    411527,
    823117,
    1646237,
    3292489,
    6584983,
    13169977
};

GBL_EXPORT uint32_t gblPrimeNextDouble(uint32_t number) {
    const static size_t entries = GBL_COUNT_OF(primes_);

    for(size_t p = 0; p < entries; ++p)
        if(number < primes_[p])
            return primes_[p];

    return primes_[entries-1];
}

#if 0
5
11
23
47
97
191
367
701
1361
2591
4931
9371
17807
33851
64319

int main(void)
{
    #define CEILING 65535
    uint64_t last_prime = 2;
    for(uint64_t prime = 2; prime <= CEILING; prime++)
    {
        if(isprime(prime))
        {
            float ratio = (float)prime / (float)last_prime;
            if(ratio >= 1.9)
            {
                last_prime = prime;
                printf("%d\n", prime);
            }
        }
    }
    return 0;
}
#endif

#endif // GIMBAL_NUMERIC_C
