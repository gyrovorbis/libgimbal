#include <gimbal/algorithms/gimbal_random.h>
#include <gimbal/algorithms/gimbal_hash.h>
#include <math.h>
#include <time.h>

#define LEHMER_MODULUS    2147483647 /* DON'T CHANGE THIS VALUE                  */
#define LEHMER_MULTIPLIER 48271      /* DON'T CHANGE THIS VALUE                  */
#define LEHMER_CHECK      399268537  /* DON'T CHANGE THIS VALUE                  */
#define LEHMER_STREAMS    256        /* # of streams, DON'T CHANGE THIS VALUE    */
#define LEHMER_A256       22925      /* jump multiplier, DON'T CHANGE THIS VALUE */
#define LEHMER_DEFAULT    123456789  /* initial seed, use 0 < DEFAULT < MODULUS  */

static uint64_t seeds_[GBL_SEED_COUNT] = { LEHMER_DEFAULT };
static GblBool  init_ = GBL_FALSE;

GBL_EXPORT int gblRandLehmer(void) {

    const int Q = LEHMER_MODULUS / LEHMER_MULTIPLIER;
    const int R = LEHMER_MODULUS % LEHMER_MULTIPLIER;
          int t;

    t = LEHMER_MULTIPLIER * (gblSeed(0) % Q) - R * (gblSeed(0) / Q);

    if (t > 0)
      seeds_[0] = t;
    else
      seeds_[0] = t + LEHMER_MODULUS;

    // returns a number between 0.0f and 1.0f
    return ((double) seeds_[0] / LEHMER_MODULUS);
}

GBL_EXPORT uint64_t gblSeed(uint8_t index) {

    if(!init_) GBL_UNLIKELY {
        seeds_[0] = (uint64_t)time(NULL);
        for(unsigned i = 1; i < GBL_SEED_COUNT; ++i) {
            seeds_[i] = gblHashCrc(&seeds_[i-1], sizeof(uint64_t));
        }
        init_ = GBL_TRUE;
    }
    return seeds_[index];
}


GBL_EXPORT int gblRand(void) {
    return gblRandRange(0, RAND_MAX);
}

GBL_EXPORT int gblRandRange(int min, int max)  {
    GBL_ASSERT(max <= RAND_MAX);
    static GblBool seeded = GBL_FALSE;
    if(!seeded) GBL_UNLIKELY {
        srand((unsigned)gblSeed(0));
        seeded = GBL_TRUE;
    }
    return (rand() % (max - min + 1)) + min;
}

GBL_EXPORT GblBool gblRandBool(void) {
    return gblRand() & 0x1;
}

GBL_EXPORT float gblRandFloat(float min, float max) {
    return min + (float)(gblRand()) / ((float)RAND_MAX / (max - min));
}

GBL_EXPORT int gblRandString(char* pBuffer, int minSize, int maxSize, const char* pCharList) {
    static const char* pWordChars = "abcdefghijklmnopqrstuvwxyz "
                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                    "1234567890";

    if(!pCharList)
        pCharList = pWordChars;

    const int size       = gblRandRange(minSize, maxSize);
    const int listLength = strlen(pCharList);

    for(size_t  c = 0; c < (size_t )size; ++c)
        pBuffer[c] = pCharList[gblRandRange(0, listLength-1)];

    pBuffer[size] = '\0';

    return size;
}

GBL_EXPORT void gblRandBuffer(void* pData, size_t  size) {
    for(size_t  i = 0; i < size; ++i) {
        (((uint8_t*)pData)[i]) = (uint8_t)gblRandRange(0, 255);
    }
}

#define gblRandf() (gblRandFloat(0.0f, 1.0f))

GBL_EXPORT int gblRandBernoulli(float p) {
    return ((gblRandf() < (1.0f - p)) ? 0 : 1);
}

GBL_EXPORT int gblRandBinomial(int n, float p) {
    int i, x = 0;

    for (i = 0; i < n; ++i)
      x += gblRandBernoulli(p);

    return x;
}

GBL_EXPORT int gblRandGeometric(float p) {
    return ((int) (log(1.0 - gblRandf()) / log(p)));
}

GBL_EXPORT int gblRandPascal(int n, float p) {
    int i, x = 0;

    for (i = 0; i < n; ++i)
        x += gblRandGeometric(p);

    return x;
}

GBL_EXPORT int gblRandPoisson(float m) {
    float t = 0.0f;
    int   x = 0;

    while (t < m) {
      t += gblRandExponential(1.0f);
      x++;
    }
    return (x - 1);
}

GBL_EXPORT float gblRandExponential(float m) {
  return (-m * log(1.0 - gblRandf()));
}

GBL_EXPORT float gblRandErlang(int n, float b) {
    int   i;
    float x = 0.0f;

    for (i = 0; i < n; i++)
      x += gblRandExponential(b);

    return x;
}

/* ========================================================================
 * Returns a normal (Gaussian) distributed real number.
 * NOTE: use s > 0.0
 *
 * Uses a very accurate approximation of the normal idf due to Odeh & Evans,
 * J. Applied Statistics, 1974, vol 23, pp 96-97.
 * ========================================================================
 */
GBL_EXPORT float gblRandNormal(float m, float s) {
    const float p0 = 0.322232431088f;     const float q0 = 0.099348462606f;
    const float p1 = 1.0f;                const float q1 = 0.588581570495f;
    const float p2 = 0.342242088547f;     const float q2 = 0.531103462366f;
    const float p3 = 0.204231210245e-1f;  const float q3 = 0.103537752850f;
    const float p4 = 0.453642210148e-4f;  const float q4 = 0.385607006340e-2f;
    double u, t, p, q, z;

    u   = gblRandf();

    if (u < 0.5f)
      t = sqrt(-2.0f * log(u));
    else
      t = sqrt(-2.0f * log(1.0f - u));

    p   = p0 + t * (p1 + t * (p2 + t * (p3 + t * p4)));
    q   = q0 + t * (q1 + t * (q2 + t * (q3 + t * q4)));

    if (u < 0.5f)
      z = (p / q) - t;
    else
      z = t - (p / q);

    return m + (s * z);
}

GBL_EXPORT float gblRandLogNormal(float a, float b) {
    return (exp(a + b * gblRandNormal(0.0f, 1.0f)));
}

GBL_EXPORT float gblRandChisquare(int n) {
    int   i;
    float z, x = 0.0f;

    for (i = 0; i < n; ++i) {
      z  = gblRandNormal(0.0f, 1.0f);
      x += z * z;
    }

    return x;
}

GBL_EXPORT float gblRandStudent(int n) {
    return (gblRandNormal(0.0f, 1.0f) / sqrt(gblRandChisquare(n) / n));
}
