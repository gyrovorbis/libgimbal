/*! \file
 *  \brief Random number generators and utilities
 *  \ingroup random
 *
 *  This file contains the API and utilties surrounding
 *  random number generation. A series of utilities are
 *  provided for generating different types of random
 *  output (integer, float, bool, string, etc) as well
 *  as a variety of random number distributions.
 *
 *  \note
 *  Random Number Generators: Good Ones Are Hard To Find"
 *  Steve Park and Keith Miller
 *  Communications of the ACM, October 1988
 *
 *  \author 1998 Steve Park
 *  \author 1998 Keith Miller
 *  \author 2023 Falco Girgis
 */
#ifndef GIMBAL_RANDOM_H
#define GIMBAL_RANDOM_H

#include "../core/gimbal_decls.h"

#define GBL_SEED_COUNT      2

GBL_DECLS_BEGIN

/*! \defgroup random Random
 *  \ingroup algorithms
 *  \brief Random number generators and utilities
 * @{
*/

GBL_EXPORT uint64_t gblSeed          (uint8_t index)            GBL_NOEXCEPT;

GBL_EXPORT int      gblRand          (void)                     GBL_NOEXCEPT;
GBL_EXPORT int      gblRandRange     (int min, int max)         GBL_NOEXCEPT;
GBL_EXPORT GblBool  gblRandBool      (void)                     GBL_NOEXCEPT;
GBL_EXPORT float    gblRandFloat     (float min, float max)     GBL_NOEXCEPT;
GBL_EXPORT void     gblRandBuffer    (void* pData, size_t size) GBL_NOEXCEPT;
GBL_EXPORT int      gblRandString    (char*       pBuffer,
                                      int         minSize,
                                      int         maxSize,
                                      const char* pCharList)    GBL_NOEXCEPT;

GBL_EXPORT int gblRandBernoulli (float p)        GBL_NOEXCEPT;
GBL_EXPORT int gblRandBinomial  (int n, float p) GBL_NOEXCEPT;
GBL_EXPORT int gblRandGeometric (float p)        GBL_NOEXCEPT;
GBL_EXPORT int gblRandPascal    (int n, float p) GBL_NOEXCEPT;
GBL_EXPORT int gblRandPoisson   (float m)        GBL_NOEXCEPT;

GBL_EXPORT float gblRandExponential (float m)          GBL_NOEXCEPT;
GBL_EXPORT float gblRandErlang      (int n, float b)   GBL_NOEXCEPT;
GBL_EXPORT float gblRandNormal      (float m, float s) GBL_NOEXCEPT;
GBL_EXPORT float gblRandLogNormal   (float a, float b) GBL_NOEXCEPT;
GBL_EXPORT float gblRandChisquare   (int n)            GBL_NOEXCEPT;
GBL_EXPORT float gblRandStudent     (int n)            GBL_NOEXCEPT;

//! @}

GBL_DECLS_END

#endif // GIMBAL_RANDOM_H
