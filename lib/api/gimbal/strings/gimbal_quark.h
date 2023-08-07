/*! \file
 *  \brief ::GblQuark and string interning API
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
 *  elided by using GblQuark_fromStatic() or GblQuark_internStatic(),
 *  since a string literal's lifetime is global.
 *
 *  \note
 *  All functions in this API are thread-safe.
 *
 *  ### Example Usage
 *
 *      // create a quark from a string literal
 *      GblQuark quark1 = GblQuark_fromStatic("String");
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
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_QUARK_H
#define GIMBAL_QUARK_H

#include "../core/gimbal_result.h"

//! Value of an invalid or NULL ::GblQuark
#define GBL_QUARK_INVALID ((GblQuark)0)

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblContext);

/*! Uniquely identifiable interned string type
 *  \ingroup strings
 *
 *  Two quarks may be directly compared to one another for equality.
 *
 *  \note
 *  To test whether a given GblQuark is valid or not, it can be
 *  compared to #GBL_QUARK_INVALID.
 */
typedef uintptr_t GblQuark;

/*! \name Static State
 *  \brief Methods for static initialization, finalization, and state
 *  @{
 */
//! Initializes the GblQuark registry with the given capacities (called automatically with defaults)
GBL_EXPORT GBL_RESULT  GblQuark_init    (GblContext* pCtx,
                                         size_t      extraPageSize,
                                         size_t      initialEntries) GBL_NOEXCEPT;
//! Finalizes the GblQuark registry, releasing all resources (called automatically upon shutdown)
GBL_EXPORT GBL_RESULT  GblQuark_final   (void)                       GBL_NOEXCEPT;
//! Returns a pointer to the GblContext object associated with the GblQuark registry
GBL_EXPORT GblContext* GblQuark_context (void)                       GBL_NOEXCEPT;
//! @}

/*! \name Statistics
 *  \brief Methods for querying and reporting usage stats
 *  @{
 */
//! Returns the total number of quarks maintained within the registry
GBL_EXPORT size_t GblQuark_count           (void) GBL_NOEXCEPT;
//! Returns the total number of allocation pages used by the registry
GBL_EXPORT size_t GblQuark_pageCount       (void) GBL_NOEXCEPT;
//! Returns the size of each dynamically allocated page used by the registry
GBL_EXPORT size_t GblQuark_pageSize        (void) GBL_NOEXCEPT;
//! Returns the total number of bytes used for string allocations by the registry
GBL_EXPORT size_t GblQuark_bytesUsed       (void) GBL_NOEXCEPT;
//! Returns the total number of bytes remaining available on the current allocation page
GBL_EXPORT size_t GblQuark_bytesAvailable  (void) GBL_NOEXCEPT;
//! Returns the total number of bytes allocated (used or unused) for string storage
GBL_EXPORT size_t GblQuark_totalCapacity   (void) GBL_NOEXCEPT;
//! Returns the total number of unused, unavailable, but allocated bytes for string storage
GBL_EXPORT size_t GblQuark_fragmentedBytes (void) GBL_NOEXCEPT;
//! Returns the utilization factor of total capacity vs bytes used (ranging 0.0-1.0)
GBL_EXPORT float  GblQuark_utilization     (void) GBL_NOEXCEPT;
//! @}

/*! \name String Interning
 *  \brief Methods for creating, retrieving, and converting GblQuarks
 *  @{
 */
//! Returns the GblQuark associated with the given string, adding a new entry to the registry if necessary
GBL_EXPORT GblQuark    GblQuark_fromString   (const char* pStr, size_t len/*=0*/) GBL_NOEXCEPT;
//! Returns the GblQuark associated with the given STATIC string, which can save an allocation when initially registering
GBL_EXPORT GblQuark    GblQuark_fromStatic   (const char* pSstring)               GBL_NOEXCEPT;
//! Returns the GblQuark associated with the given string, returning GBL_QUARK_INVALID if it was not previously registered
GBL_EXPORT GblQuark    GblQuark_tryString    (const char* pStr, size_t len/*=0*/) GBL_NOEXCEPT;
//! Returns the NULL-terminated interned C string associated with a given GblQuark
GBL_EXPORT const char* GblQuark_toString     (GblQuark quark)                     GBL_NOEXCEPT;
//! Creates a GblQuark from the given string (if necessary), also returning its interned string
GBL_EXPORT const char* GblQuark_internString (const char* pStr, size_t len/*=0*/) GBL_NOEXCEPT;
//! Creates a GblQuark from the given STATIC string (if necessary, saving on allocating), also returning its interned string
GBL_EXPORT const char* GblQuark_internStatic (const char* pString)                GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

//! \cond
#define GblQuark_fromString(...) \
    GblQuark_fromStringDefault_(__VA_ARGS__)
#define GblQuark_fromStringDefault_(...) \
    GblQuark_fromStringDefault__(__VA_ARGS__, 0)
#define GblQuark_fromStringDefault__(str, len, ...) \
    (GblQuark_fromString)(str, len)

#define GblQuark_internString(...) \
    GblQuark_internStringDefault_(__VA_ARGS__)
#define GblQuark_internStringDefault_(...) \
    GblQuark_internStringDefault__(__VA_ARGS__, 0)
#define GblQuark_internStringDefault__(str, len, ...) \
    (GblQuark_internString)(str, len)

#define GblQuark_tryString(...) \
    GblQuark_tryStringDefault_(__VA_ARGS__)
#define GblQuark_tryStringDefault_(...) \
    GblQuark_tryStringDefault__(__VA_ARGS__, 0)
#define GblQuark_tryStringDefault__(str, len, ...) \
    (GblQuark_tryString)(str, len)
//! \endcond

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
    \fn GblQuark_fromStatic(const char* pString)
    \details
        Equivalent to GblQuark_fromString(), except for no internal storage
        is allocated for the string, since its lifetime is known to be global.
    \param pString string literal
    \returns quark value or #GBL_QUARK_INVALID if the string is NULL
    \sa GblQuark_fromString
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
    \fn GblQuark_internStatic(const char* pString)
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
