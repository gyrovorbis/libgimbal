/*! \file
 *  \brief ::GblQuark and related functions.
 *  \ingroup strings
 *
 *  A GblQuark is an integral unique identifier associated with a given string.
 *  Two different C string pointers containing the same string value will have
 *  the same GblQuark. GblQuark is libGimbal's implementation of string interning.
 *
 *  A GblQuark is highly useful for situations when string comparison speed is
 *  what matters the most, such as for hash tables. For example, libGimbal's
 *  property system internally represents property names as GblQuarks so that
 *  accessing a property by name is optimized.
 *
 *  This string interning mechanism is implemented as a hash table
 *  mapping a given regular string to a GblQuark unique identifier. When a
 *  non-static string is registered, it is allocated within a paged static
 *  buffer, whose lifetime is that of the program.
 *
 *  \note
 *  When working with string literals, the internal allocations can be
 *  elided by using GblQuark_fromStringStatic() or GblQuark_internStringStatic(),
 *  since a string literal's lifetime is global.
 *
 *  \note
 *  All functions in this API are thread-safe.
 *
 *  ### Example Usage
 *
 *      // create a quark from a string literal
 *      GblQuark quark1 = GblQuark_fromStringStatic("String");
 *
 *      // create a quark from a regular string buffer
 *      char buffer[] = "String";
 *      GblQuark quark2 = GblQuark_fromString(buffer);
 *
 *      // the two values are equal
 *      GBL_ASSERT(quark1 == quark2);
 *
 *      // convert them back to regular strings to show values are equal
 *      GBL_ASSERT(strcmp(GblQuark_toString(quark1),
 *                        GblQuark_toString(quark2) == 0);
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_QUARK_H
#define GIMBAL_QUARK_H

#include "../core/gimbal_result.h"

/// \details Value of an invalid or NULL ::GblQuark
#define GBL_QUARK_INVALID ((GblQuark)0)

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblContext);

/*! \brief Uniquely identifiable interned string type
 *
 *  Two quarks may be directly compared to one another for equality.
 *
 *  \note
 *  To test whether a given GblQuark is valid or not, it can be
 *  compared to #GBL_QUARK_INVALID.
 *  \ingroup strings
 */
typedef uintptr_t       GblQuark;

GBL_EXPORT GBL_RESULT   GblQuark_init               (GblContext* pCtx,
                                                     size_t  extraPageSize,
                                                     size_t  initialEntries)              GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblQuark_final              (void)                                GBL_NOEXCEPT;

GBL_EXPORT size_t       GblQuark_pageCount          (void)                                GBL_NOEXCEPT;
GBL_EXPORT size_t       GblQuark_pageSize           (void)                                GBL_NOEXCEPT;
GBL_EXPORT size_t       GblQuark_bytesUsed          (void)                                GBL_NOEXCEPT;
GBL_EXPORT size_t       GblQuark_bytesAvailable     (void)                                GBL_NOEXCEPT;
GBL_EXPORT size_t       GblQuark_totalCapacity      (void)                                GBL_NOEXCEPT;
GBL_EXPORT size_t       GblQuark_fragmentedBytes    (void)                                GBL_NOEXCEPT;
GBL_EXPORT float        GblQuark_utilization        (void)                                GBL_NOEXCEPT;

GBL_EXPORT size_t       GblQuark_count              (void)                                GBL_NOEXCEPT;
GBL_EXPORT GblContext*  GblQuark_context            (void)                                GBL_NOEXCEPT;

GBL_EXPORT GblQuark     GblQuark_fromString         (const char* pString)                 GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblQuark_fromStringSized    (const char* pString, size_t  length) GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblQuark_fromStringStatic   (const char* pSstring)                GBL_NOEXCEPT;

GBL_EXPORT GblQuark     GblQuark_tryString          (const char* pString)                 GBL_NOEXCEPT;
GBL_EXPORT GblQuark     GblQuark_tryStringSized     (const char* pString, size_t  length) GBL_NOEXCEPT;

GBL_EXPORT const char*  GblQuark_toString           (GblQuark quark)                      GBL_NOEXCEPT;

GBL_EXPORT const char*  GblQuark_internString       (const char* pString)                 GBL_NOEXCEPT;
GBL_EXPORT const char*  GblQuark_internStringSized  (const char* pString, size_t  length) GBL_NOEXCEPT;
GBL_EXPORT const char*  GblQuark_internStringStatic (const char* pString)                 GBL_NOEXCEPT;

/// \cond
GBL_DECLS_END
/// \endcond

/*!
    \fn GblQuark_init(GblContext* pCtx, size_t  extraPageSize, size_t  initialEntries)
    \details
        Initializes the GblQuark API with the given configuration parameters or
        default if they're all set to NULL and 0.
    \note
        The API is already initialized internally as soon as it's used, so this is
        typically not required.
    \attention
        This will invalidate any exiting ::GblQuark values!
    \param pCtx context from which to allocate and log
    \param extraPageSize size of each chunk allocation used beyond the builtin static initial page
    \param initialEntries initial number of entries to preallocate in the hash table
    \returns result code
*/

/*!
    \fn GblQuark_final(void)
    \details
        Finalizes the GblQuark API, releasing all internally allocated paged data
        and clearing the hash map.
    \note
        This will invalidate all existing GblQuark values and is typically
        called when the application exits
    \returns result code
*/

/*!
    \fn GblQuark_pageSize(void)
    \returns size of each additional page beyond the default static one for holding
             interned strings
*/

/*!
    \fn GblQuark_pageCount(void)
    \returns number of pages which have been allocated to store interned strings
*/

/*!
    \fn GblQuark_bytesUsed(void)
    \returns total number of bytes used on all pages by interned strings
*/

/*!
    \fn GblQuark_bytesAvailable(void)
    \returns total number of bytes available on the current page
*/

/*!
    \fn GblQuark_totalCapacity(void)
    \returns total combined byte size of all active pages
*/

/*!
    \fn GblQuark_fragmentedBytes(void)
    \returns total number of leftover/wasted bytes at the end of a page that were too small to fill
*/

/*!
    \fn GblQuark_utilization(void)
    \returns Ratio of GblQuark_bytesUsed() over GblQuark_totalCapacity(),
            representing what percentage of available resources is being utilized
*/
/*!
    \fn GblQuark_count(void)
    \returns total number of GblQuarks which have been created internally
*/

/*!
    \fn GblQuark_context(void)
    \returns GblContext object used for logging and allocation internally
*/

/*!
    \fn GblQuark_fromString(const char* pString)
    \details
        Returns a ::GblQuark value associated with the given string.
    \param pString NULL-terminated C string
    \returns quark value or #GBL_QUARK_INVALID if the string is NULL
    \sa GblQuark_fromStringSized, GblQuark_fromStringStatic
*/

/*!
    \fn GblQuark_fromStringSized(const char* pString, size_t  length)
    \details
        Equivalent to GblQuark_fromString(), except for only the specified
        length of the string. This is useful for either substrings, or
        for non NULL-teriminated strings.
    \param pString array of characters
    \param length of array
    \returns quark value or #GBL_QUARK_INVALID if the string is NULL or length is 0
    \sa GblQuark_fromString
*/

/*!
    \fn GblQuark_fromStringStatic(const char* pString)
    \details
        Equivalent to GblQuark_fromString(), except for no internal storage
        is allocated for the string, since its lifetime is known to be global.
    \param pString string literal
    \returns quark value or #GBL_QUARK_INVALID if the string is NULL
    \sa GblQuark_fromString
*/


/*!
    \fn GblQuark_tryString(const char* pString)
    \details
        Returns the ::GblQuark value corresponding to the given string only
        if it has already been created, otherwise returning #GBL_QUARK_INVALID.
    \param pString NULL-terminated C string
    \returns quark value or #GBL_QUARK_INVALID
    \sa GblQuark_tryStringSized
*/

/*!
    \fn GblQuark_tryStringSized(const char* pString, size_t  length)
    \details
        Equivalent to GblQuark_tryString() with a substring.
    \param pString array of characters
    \param length length of array
    \returns quark value or #GBL_QUARK_INVALID
    \sa GblQuark_tryStringSized
*/

/*!
    \fn GblQuark_toString(GblQuark quark)
    \details
        Retrieves the internal C string representation of the given quark
        or NULL if the quark's value is #GBL_QUARK_INVALID
    \param quark value of existing quark
    \returns NULL-terminated C string or NULL
*/

/*!
    \fn GblQuark_internString(const char* pString)
    \details
        Retrieves the internal C string representation of the given C string
        or NULL if the C string is NULL.
    \note
        This is useful for when you want the string's lifetime to be static
        or when it will be used as a GblQuark later, but is currently needed
        as a C string.
    \param pString NULL-teriminated C string or NULL
    \returns NULL-terminated C string or NULL
    \sa GblQuark_internStringSized, GblQuark_internStringStatic
*/

/*!
    \fn GblQuark_internStringSized(const char* pString, size_t  length)
    \details
        Equivalent to GblQuark_internString() with a substring.
    \param pString NULL-teriminated C string or NULL
    \param length number of characters from pString to use
    \returns NULL-terminated C string or NULL
    \sa GblQuark_internString
*/

/*!
    \fn GblQuark_internStringStatic(const char* pString)
    \details
        Equivalent to GblQuark_internString() except that the
        internal allocation of the string is ellided, because the string's
        lifetime is known to be global. So the return value is the same as the
        argument.
    \param pString static or literal string or NULL
    \returns pString
    \sa GblQuark_internString
*/

#endif // GIMBAL_QUARK_H
