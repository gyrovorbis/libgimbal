/*! \file
 *  \brief ::GblStringRef type and related functions
 *  \ingroup strings
 *
 *  ::GblStringRef is a read-only, reference-counted string used to
 *  efficiently store, reference, and copy strings. It is also
 *  type-compatible with const char*, so it masquerades as a regular
 *  C-string to conveniently support regular C string APIs and character
 *  array accesses.
 *
 *  \note
 *  ::GblStringRef also stores its length internally, so
 *  the GblStringRef_length() function and GblStringRef_view()
 *  functions are extremely efficient. Favor this over strlen(),
 *  when you know you're working with a ::GblStringRef.
 *
 *  The following example illustrates how it may be used:
 *
 *      GblStringRef* pRef = GblStringRef_create("lolol");
 *
 *      // Look, I'm a regular C string!
 *      assert(pRef[0] == 'l');
 *      assert(strlen(pRef) == 5);
 *      assert(strcmp(pRef, "lolol") == 0);
 *
 *      // owait, no, I'm not.
 *      GblStringRef* pRef2 = GblStringRef_ref(pRef);
 *      assert(GblStringRef_refCount(pRef2) == 2);
 *
 *      // even stores length to save on strlen() / StringView time
 *      assert(GblStringRef_length(pRef2) == 5);
 *
 *      // no strlen() happening here, fast.
 *      GblStringView view = GblStringRef_view(pRef);
 *
 *      GblStringRef_unref(pRef);  // now refCount is 1
 *      GblStringRef_unref(pRef2); // now it's deleted
 *
 *  \author    2023 Falco Girgis
 *  \copyright MIT License
 */

#ifndef GIMBAL_STRING_REF_H
#define GIMBAL_STRING_REF_H

#include "gimbal_string_view.h"

#define GBL_SELF_TYPE GblStringRef

GBL_DECLS_BEGIN

/*! \brief Reference-counted, const char*-compatible string type
 *  \ingroup strings
 *
 *  GblStringRef is a const char*-compatible reference-counted,
 *  read-only string type allowing for the sharing of a single
 *  string allocation between multiple different locations.
 *
 *  \sa gimbal_string_ref.h
 */
typedef const char GblStringRef;

/*! \name  Lifetime Management
 *  \brief Methods for managing GblStringRef references
 *  \relatesalso GblStringRef
 *  @{
 */
//! Creates and returns a reference containing \p pString, with optional length and context
GBL_EXPORT GblStringRef* GblStringRef_create (const char* pString,
                                              size_t      len/*=0*/,
                                              GblContext* pCtx/*=NULL*/) GBL_NOEXCEPT;
//! Returns a new reference to \p pRef, incrementing its internal reference count rather than actually copying
GBL_EXPORT GblStringRef* GblStringRef_ref    (const GblStringRef* pRef)  GBL_NOEXCEPT;
//! Releases a reference to \p pRef, freeing the allocation if it was the last, returning the new refCount
GBL_EXPORT GblRefCount   GblStringRef_unref  (const GblStringRef* pRef)  GBL_NOEXCEPT;
//! @}

/*! \name  Properties
 *  \brief Methods for reading stored and derived data
 *  \relatesalso GblStringRef
 *  @{
 */
//! Returns the GblContext that was created with the given GblStringRef
GBL_EXPORT GblContext* GblStringRef_context  (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the number of active references remaining to the given GblStringRef
GBL_EXPORT GblRefCount GblStringRef_refCount (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the cached length of the given GblStringRef
GBL_EXPORT size_t      GblStringRef_length   (GBL_CSELF) GBL_NOEXCEPT;
//! Returns whether the given GblStringRef is valid (not NULL)
GBL_EXPORT GblBool     GblStringRef_valid    (GBL_CSELF) GBL_NOEXCEPT;
//! Returns whether the given GblStringRef is empty, with nothing but a NULL terminator
GBL_EXPORT GblBool     GblStringRef_empty    (GBL_CSELF) GBL_NOEXCEPT;
//! Returns whether the given GblStringRef is blank, containing only NULL or spacing characters
GBL_EXPORT GblBool     GblStringRef_blank    (GBL_CSELF) GBL_NOEXCEPT;
//! Calculates and returns the 32-bit hash value associated with the givne GblStringRef
GBL_EXPORT GblHash     GblStringRef_hash     (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/*! \name  Accessors
 *  \brief Methods for accessing character and substrings
 *  \relatesalso GblStringRef
 *  @{
 */
//! Returns the character located at position \p idx, raising an error upon out-of-range
GBL_EXPORT char          GblStringRef_at   (GBL_CSELF, size_t idx) GBL_NOEXCEPT;
//! Returns a GblStringView containing the character window given by \p offset and \p len
GBL_EXPORT GblStringView GblStringRef_view (GBL_CSELF,
                                            size_t offset/*=0*/,
                                            size_t len/*=0*/)      GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

//! \cond
#define GblStringRef_create(...) \
    GblStringRef_createDefault_(__VA_ARGS__)
#define GblStringRef_createDefault_(...) \
    GblStringRef_createDefault__(__VA_ARGS__, 0, NULL)
#define GblStringRef_createDefault__(str, len, ctx, ...) \
    ((GblStringRef_create)(str, len, ctx))

#define GblStringRef_view(...) \
    GblStringRef_viewDefault_(__VA_ARGS__)
#define GblStringRef_viewDefault_(...) \
    GblStringRef_viewDefault__(__VA_ARGS__, 0, 0)
#define GblStringRef_viewDefault__(ref, offset, len, ...) \
    ((GblStringRef_view)(ref, offset, len))
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_STRING_REF_H
