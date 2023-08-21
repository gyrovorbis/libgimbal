/*! \file
 *  \brief   GblPattern: RegExp-style pattern matching
 *  \ingroup strings
 *
 *  This file contains the API and opaque structure declaration
 *  for GblPattern, which represents a compiled regular expression,
 *  used for string matching operations.
 *
 *  \note
 *  LibGimbal uses a highly modified fork of the the Tiny C RegEx
 *  library for its back-end, lowest-level regular expression
 *  pattern matching.
 *
 *  \todo
 *      - Wrap Tiny C Regex tests in gimbal Unit tests
 *      - `\b` word-boundary character class
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_PATTERN_H
#define GIMBAL_PATTERN_H

#include "../core/gimbal_decls.h"

#define GBL_SELF_TYPE GblPattern

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblStringView);
GBL_FORWARD_DECLARE_STRUCT(GblStringBuffer);

/*! \struct GblPattern
 *  \brief Opaque structure containing a compiled regular expression
 *
 *  GblPattern is the compiled version of a regular expression
 *  string, produced by GblPattern_create(). It is faster
 *  than working with raw regular expression strings, since
 *  preprocessing has already been done.
 *
 *  It is advised that when you are repeatedly using the same
 *  regular expression, you store it and use it as a GblPattern.
 *
 *  \note
 *  GblPattern is a reference-counted shared pointer type.
 *
 *  \ingroup strings
 */
struct GblPattern;
typedef struct GblPattern GblPattern;

/*! \name Lifetime Management
 *  \brief Methods for creating, referencing, and unreferencing patterns
 *  \relatesalso GblPattern
 *  @{
 */
//! Compiles the given regular expression into a pre-processed GblPattern
GBL_EXPORT const GblPattern* GblPattern_create     (const char* pRegExp) GBL_NOEXCEPT;
//! Returns a new reference to an existing pattern, incrementing its refcount
GBL_EXPORT const GblPattern* GblPattern_ref        (GBL_CSELF)           GBL_NOEXCEPT;
//! Releases a reference to a pattern, deallocating it upon reaching zero
GBL_EXPORT GblRefCount       GblPattern_unref      (GBL_CSELF)           GBL_NOEXCEPT;
//! Returns the number of active references held to the given compiled pattern
GBL_EXPORT GblRefCount       GblPattern_refCount   (GBL_CSELF)           GBL_NOEXCEPT;
//! Returns the total number of active, allocated, compiled regex patterns
GBL_EXPORT GblRefCount       GblPattern_totalCount (void)                GBL_NOEXCEPT;
//! @}

/*! \name Properties and Operators
 *  \brief Methods for getters and basic operations
 *  \relatesalso GblPattern
 *  @{
 */
//! Compares two different compiled patterns to see if they are equivalent regexes
GBL_EXPORT int         GblPattern_compare (GBL_CSELF, const GblPattern* pRhs) GBL_NOEXCEPT;
//! Compares two different compiled patterns for exact value equality of their regexes
GBL_EXPORT GblBool     GblPattern_equals  (GBL_CSELF, const GblPattern* pRhs) GBL_NOEXCEPT;
//! Counts the total size of a compiled pattern and returns it in bytes
GBL_EXPORT size_t      GblPattern_bytes   (GBL_CSELF)                         GBL_NOEXCEPT;
//! Reconstructs an approximate string representation of the compiled pattern
GBL_EXPORT const char* GblPattern_string  (GBL_CSELF, GblStringBuffer* pBuff) GBL_NOEXCEPT;
//! @}

/*! \name Matching
 *  \brief Methods for generic pattern matching
 *  \relatesalso GblPattern
 *  @{
 */
/*! Finds the numbered match given by \p pCount, or 1 if NULL.
 *
 *  GblPattern_match() searches through \p pString for substrings
 *  which match the pattern given by \p pSelf. When it has found
 *  the number of matches equal to the value given by \p pCount
 *  (or 1 if \p pCount is NULL), \p pMatch is set to the substring
 *  of the final match (or ignored when NULL) and GBL_TRUE is
 *  returned. If the match could not be found, GBL_FALSE is returned.
 *
 *  \note
 *  When \p pCount points to a value of 0, no matches are expected
 *  to be found. If a match is found, GBL_FALSE is returned, otherwise
 *  GBL_TRUE is returned (inverted from normal).
 *
 *  \note
 *  When \p pCount points to a value of -1, all matches are searched,
 *  with \p pMatch being updated to point to the final one and \p pCount
 *  being updated to store the final number of matches.
 *
 *  \param pSelf        The precompiled regular expression pattern
 *  \param pString      The input string to search for matches within
 *  \param pMatch       (Optional) Output used to hold the final match substring
 *  \param pCount       (Optional, defaults to 1) Number of matches to find
 *
 *  \retval GBL_FALSE   The requested match number wasn't found.
 *  \retval GBL_TRUE    The requested match number was found.
 *
 *  \sa GblPattern_matchNot()
 */
GBL_EXPORT GblBool GblPattern_match    (GBL_CSELF,
                                        const char*    pString,
                                        GblStringView* pMatch/*=NULL*/,
                                        int*           pCount/*=NULL*/) GBL_NOEXCEPT;
//! Dynamically-compiled string-based version of GblPattern_match()
GBL_EXPORT GblBool GblPattern_matchStr (const char*    pRegExp,
                                        const char*    pString,
                                        GblStringView* pMatch/*=NULL*/,
                                        int*           pCount/*=NULL*/) GBL_NOEXCEPT;
//! @}

/*! \name Inverse Matching
 *  \brief Methods for returning non matches
 *  \relatesalso GblPattern
 *  @{
 */
//! Behaves like GblPattern_match() except searching for NON-MATCHES
GBL_EXPORT GblBool GblPattern_matchNot    (GBL_CSELF,
                                           const char*    pString,
                                           GblStringView* pMatch/*=NULL*/,
                                           int*           pCount/*=NULL*/) GBL_NOEXCEPT;
//! Dynamically-compiled string-based version of GblPattern_matchNot()
GBL_EXPORT GblBool GblPattern_matchNotStr (const char*    pRegExp,
                                           const char*    pString,
                                           GblStringView* pMatch/*=NULL*/,
                                           int*           pCount/*=NULL*/) GBL_NOEXCEPT;
//! @}


/*! \name Exact Matching
 *  \brief Methods for checking exact matches
 *  \relatesalso GblPattern
 *  @{
 */
 //! Returns GBL_TRUE if the given string EXACTLY matches the given pattern or GBL_FALSE otherwise
GBL_EXPORT GblBool GblPattern_matchExact    (GBL_CSELF, const char* pString) GBL_NOEXCEPT;
//! Dynamically compiled string-based version of GblPattern_matchExact()
GBL_EXPORT GblBool GblPattern_matchExactStr (const char* pRegExp,
                                             const char* pString)            GBL_NOEXCEPT;
//! @}

/*! \name Match Counting
 *  \brief Methods used for counting matches
 *  \relatesalso GblPattern
 *  @{
 */
//! Returns the number of pattern matches found in \p pString
GBL_EXPORT size_t GblPattern_matchCount    (GBL_CSELF, const char* pString) GBL_NOEXCEPT;
//! Dynamically compiled, string-based version of GblPattern_matchExact()
GBL_EXPORT size_t GblPattern_matchCountStr (const char* pRegExp,
                                            const char* pString)            GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

//! \cond
#define GblPattern_match(...) \
    (GblPattern_matchDefault_(__VA_ARGS__))
#define GblPattern_matchDefault_(...) \
    (GblPattern_matchDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL))
#define GblPattern_matchDefault__(pat, str, match, count, ...) \
    ((GblPattern_match)(pat, str, match, count))

#define GblPattern_matchStr(...) \
    (GblPattern_matchStrDefault_(__VA_ARGS__))
#define GblPattern_matchStrDefault_(...) \
    (GblPattern_matchStrDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL))
#define GblPattern_matchStrDefault__(exp, str, match, count, ...) \
    ((GblPattern_matchStr)(exp, str, match, count))

#define GblPattern_matchNot(...) \
    (GblPattern_matchNotDefault_(__VA_ARGS__))
#define GblPattern_matchNotDefault_(...) \
    (GblPattern_matchNotDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL))
#define GblPattern_matchNotDefault__(pat, str, match, count, ...) \
    ((GblPattern_matchNot)(pat, str, match, count))

#define GblPattern_matchNotStr(...) \
    (GblPattern_matchNotStrDefault_(__VA_ARGS__))
#define GblPattern_matchNotStrDefault_(...) \
    (GblPattern_matchNotStrDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL))
#define GblPattern_matchNotStrDefault__(exp, str, match, count, ...) \
    ((GblPattern_matchNotStr)(exp, str, match, count))
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_PATTERN_H
