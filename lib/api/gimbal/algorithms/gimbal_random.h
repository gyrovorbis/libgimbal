/*! \file
 *  \brief Random number generators and utilities
 *  \ingroup random
 *
 *  This file contains the API and types surrounding
 *  random number generation. A series of utilities are
 *  provided for generating different types of random
 *  output (integer, float, bool, string, etc) as well
 *  as a variety of random number distributions.
 *
 *  \note
 *  gblRandLehmer() and the various statistical disbributions
 *  are based on:
 *  <i>"Random Number Generators: Good Ones Are Hard To Find"</i>
 *  Steve Park and Keith Miller
 *  Communications of the ACM, October 1988
 *
 *  \todo
 *  - Changing channels to swap between seeds
 *
 *  \author 1998 Steve Park
 *  \author 1998 Keith Miller
 *  \author 2023 Falco Girgis
 */
#ifndef GIMBAL_RANDOM_H
#define GIMBAL_RANDOM_H

#include "../core/gimbal_decls.h"

#define GBL_SEED_COUNT  2 //!< Number of different seeds maintained

#ifndef GBL_RAND_GENERATOR_DEFAULT
//! Default random number generator backing gblRand()
#   define GBL_RAND_GENERATOR_DEFAULT gblRandLehmer
#endif

GBL_DECLS_BEGIN

//! Function prototype for a custom random generator to be set with gblSetRand()
typedef int (*GblRandomGeneratorFn)(void);

/*! \defgroup random Random
 *  \ingroup algorithms
 *  \brief Random number generators and utilities
*/

/*! \name Basic API
 *  \brief Basic API for driving random number generation
 *  @{
 */
//! Returns the seed associated with the given \p index
GBL_EXPORT uint64_t gblSeed     (uint8_t index)                GBL_NOEXCEPT;
//! Sets the seed associated with the given \p index to \p seed
GBL_EXPORT void     gblSeedRand (uint8_t index, uint64_t seed) GBL_NOEXCEPT;
//! Returns a random number between 0 and RAND_MAX using the current random number generator
GBL_EXPORT int      gblRand     (void)                         GBL_NOEXCEPT;
//! Sets the current random number generator to \p pFnGen, which drives gblRand()
GBL_EXPORT void     gblSetRand  (GblRandomGeneratorFn pFnGen)  GBL_NOEXCEPT;
//! @}

/*! \name Generators
 *  \brief Builtin random number generators
 *  @{
 */
//! The builtin rand() generator from the C standard library
GBL_EXPORT int gblRandLibC   (void) GBL_NOEXCEPT;
//! Lehmer linear congruential random number generator
GBL_EXPORT int gblRandLehmer (void) GBL_NOEXCEPT;
//! @}

/*! \name Utilities
 *  \brief Utility functions for random data generation
 *  @{
 */
//! Generates a random boolean value using the current generator
GBL_EXPORT GblBool gblRandBool   (void)                           GBL_NOEXCEPT;
//! Generates a random single-precision float from FLT_MIN to FLT_MAX.
GBL_EXPORT float   gblRandf      (void)                           GBL_NOEXCEPT;
//! Fills in the given buffer with random bytes using the current generator
GBL_EXPORT void    gblRandBuffer (void* pData, size_t size)       GBL_NOEXCEPT;
//! Fills in buffer with a sized random word (optionally generated using a list of characters)
GBL_EXPORT int     gblRandString (char*       pBuffer,
                                  int         minSize,
                                  int         maxSize,
                                  const char* pCharList/*=NULL*/) GBL_NOEXCEPT;
//!@}

/*! \name Discrete Distributions
 *  \brief Random number generation for different discrete distributions
 *  @{
 */
//! Generates a random integer using the current random number generator over a Bernoulli distribution
GBL_EXPORT int gblRandBernoulli  (float p)        GBL_NOEXCEPT;
//! Generates a random integer using the current random number generator over a Binomial distribution
GBL_EXPORT int gblRandBinomial   (int n, float p) GBL_NOEXCEPT;
//! Generates a random integer using the current random number generator over the given range [a, b]
GBL_EXPORT int gblRandEquilikely (int a, int b)   GBL_NOEXCEPT;
//! Generates a random integer using the current random number generator over a Geometric distribution
GBL_EXPORT int gblRandGeometric  (float p)        GBL_NOEXCEPT;
//! Generates a random integer using the current random number generator over a Pascal distribution
GBL_EXPORT int gblRandPascal     (int n, float p) GBL_NOEXCEPT;
//! Generates a random integer using the current random number generator over a Poisson distribution
GBL_EXPORT int gblRandPoisson    (float m)        GBL_NOEXCEPT;
//! @}

/*! \name Continuous Distributions
 *  \brief Random number generation for different continuous distributions
 *  @{
 */
//! Generates a random float using the current random number generator over the given range [a, b]
GBL_EXPORT float gblRandUniform     (float a, float b) GBL_NOEXCEPT;
//! Generates a random float using the current random number generator over an Exponential distribution
GBL_EXPORT float gblRandExponential (float m)          GBL_NOEXCEPT;
//! Generates a random float using the current random number generator over an Erlang distribution
GBL_EXPORT float gblRandErlang      (int n, float b)   GBL_NOEXCEPT;
//! Generates a random integer using the current random number generator over a Normal distribution
GBL_EXPORT float gblRandNormal      (float m, float s) GBL_NOEXCEPT;
//! Generates a random integer using the current random number generator over a Log Normal distribution
GBL_EXPORT float gblRandLogNormal   (float a, float b) GBL_NOEXCEPT;
//! Generates a random integer using the current random number generator over a Chisquare distribution
GBL_EXPORT float gblRandChisquare   (int n)            GBL_NOEXCEPT;
//! Generates a random integer using the current random number generator over a Student distribution
GBL_EXPORT float gblRandStudent     (int n)            GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

//! \cond
#define gblRandString(...) \
    gblRandStringDefault_(__VA_ARGS__)
#define gblRandStringDefault_(...) \
    gblRandStringDefault__(__VA_ARGS__, GBL_NULL)
#define gblRandStringDefault__(buffer, min, max, chars, ...) \
    (gblRandString)(buffer, min, max, chars)
//! \endcond

#endif // GIMBAL_RANDOM_H
