/*! \file
 *  \brief General numeric constants and computations
 *  \ingroup numeric
 *
 *  \author Shirobon
 *  \author Falco Girgis
 */

#ifndef GIMBAL_NUMERIC_H
#define GIMBAL_NUMERIC_H

#include "../core/gimbal_decls.h"
#include "../preprocessor/gimbal_macro_utils.h"
#include <float.h>
#include <math.h>

#ifndef M_PI
#   define M_PI (3.14159265358979323846264338327950288)
#endif

#ifndef M_E
#   define M_E (2.71828182845904523536)
#endif

#define gblPow2Next(X)  GBL_META_GENERIC_MACRO_GENERATE(GBL_POW2_NEXT_TRAITS_, X)(X)

GBL_DECLS_BEGIN

/*! \defgroup numeric Numeric
 *  \ingroup algorithms
 *  \brief Collection of assorted numeric algorithms.
 * @{
 */
GBL_EXPORT uint8_t       gblPow2Next_u8             (uint8_t n)                 GBL_NOEXCEPT;
GBL_EXPORT uint16_t      gblPow2Next_u16            (uint16_t n)                GBL_NOEXCEPT;
GBL_EXPORT uint32_t      gblPow2Next_u32            (uint32_t n)                GBL_NOEXCEPT;
GBL_EXPORT uint64_t      gblPow2Next_u64            (uint64_t n)                GBL_NOEXCEPT;
GBL_EXPORT int           gblFibonacci               (int n)                     GBL_NOEXCEPT;
GBL_EXPORT int           gblGcd                     (int u, int v)              GBL_NOEXCEPT;
GBL_EXPORT int           gblLcm                     (int u, int v)              GBL_NOEXCEPT;
GBL_EXPORT uint32_t      gblNtohl                   (uint32_t n)                GBL_NOEXCEPT;
GBL_EXPORT uint32_t      gblHtonl                   (uint32_t x)                GBL_NOEXCEPT;
GBL_EXPORT int           gblParity                  (uint8_t n)                 GBL_NOEXCEPT;
GBL_EXPORT size_t        gblAlignedAllocSizeDefault (size_t bytes)              GBL_NOEXCEPT;
GBL_EXPORT size_t        gblAlignedAllocSize        (size_t size, size_t align/*=0*/) GBL_NOEXCEPT;
GBL_EXPORT uint32_t      gblPrimeNextDouble         (uint32_t number)           GBL_NOEXCEPT;
GBL_EXPORT GblBool       gblPrimeCheck              (int n)                     GBL_NOEXCEPT;
GBL_EXPORT unsigned long gblBinomialCoeff           (unsigned n, unsigned k)    GBL_NOEXCEPT;
GBL_EXPORT unsigned long gblCatalan                 (unsigned n)                GBL_NOEXCEPT;
//! \todo rename me gblEqualsf
GBL_EXPORT GblBool       gblFloatEquals             (double a,
                                                     double b,
                                                     double e/*=DBL_EPSILON*/)  GBL_NOEXCEPT;
#if 0
GBL_EXPORT float         gblDegToRadf (float degrees) GBL_NOEXCEPT;
GBL_EXPORT float         gblRadToDegf (float radians) GBL_NOEXCEPT;
GBL_EXPORT void          gblCartesianToPolarf (float x, float y, float* pRad, float* pAng) GBL_NOEXCEPT;
GBL_EXPORT void          gblPolarToCartesianf (float rad, float ang, float* pX, float* pY) GBL_NOEXCEPT;
GBL_EXPORT float         gblLerpf (float v1, float v2, float frac) GBL_NOEXCEPT;
// various interpolation functions
GBL_EXPORT float         gblDynamicWeightedMovingAverage(float current, float prev, float maxDist, float minWeight, float maxWeight) GBL_NOEXCEPT;
GBL_EXPORT float         gblRangePctf(float min, float max, float value) GBL_NOEXCEPT;
GBL_EXPORT GblBool       gblPow2Checkz(size_t value) GBL_NOEXCEPT;
GBL_EXPORT GblBool       gblWithinCheckf(float value, float min, float max) GBL_NOEXCEPT;
GBL_EXPORT GblBool       gblWithinInclusiveCheckf(float value, float min, float max) GBL_NOEXCEPT;
//GBL_EXPORT float gblPulsatef(float inTime, float inPulsesPerSecond, float inPhase) GBL_NOEXCEPT;
//GBL_EXPORT float gblPerlinNoise(float value) GBL_NOEXCEPT;
//GBL_EXPORT float gblWrapf(float value) GBL_NOEXCEPT;
#endif

/// @}

GBL_DECLS_END

///\cond
#define gblAlignedAllocSize(...) \
    gblAlignedAllocSizeDefault_(__VA_ARGS__)
#define gblAlignedAllocSizeDefault_(...) \
    gblAlignedAllocSizeDefault__(__VA_ARGS__, 0)
#define gblAlignedAllocSizeDefault__(size, align, ...) \
    (gblAlignedAllocSize)(size, align)

#define GBL_POW2_NEXT_TRAITS_ (            \
        gblPow2Next_u64,                   \
        (                                  \
            (uint8_t,  gblPow2Next_u8),    \
            (uint16_t, gblPow2Next_u16),   \
            (uint32_t, gblPow2Next_u32),   \
            (uint64_t, gblPow2Next_u64)    \
        )                                  \
    )

#define gblFloatEquals(...) \
    gblFloatEqualsDefault_(__VA_ARGS__, DBL_EPSILON)
#define gblFloatEqualsDefault_(a, b, e, ...) \
    (gblFloatEquals)(a, b, e)
///\endcond

#endif // GIMBAL_NUMERIC_H
