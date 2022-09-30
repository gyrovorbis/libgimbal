#ifndef GIMBAL_NUMERIC_C
#define GIMBAL_NUMERIC_C

#include <gimbal/algorithms/gimbal_numeric.h>

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
    const static GblSize entries = GBL_ARRAY_SIZE(primes_);

    for(GblSize p = 0; p < entries; ++p) {
        if(number < primes_[p])
            return primes_[p];
    }

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
