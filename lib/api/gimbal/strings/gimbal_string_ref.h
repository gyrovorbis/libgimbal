/*! \file
 *  \brief ::GblStringRef type and related functions
 *
 *  ::GblStringRef is a read-only reference-counted string used to efficiently
 *  store, reference, and copy strings. It is also type-compatible
 *  with const char*, so it masquerades as a regular C-string
 *  to conveniently support regular C string APIs and character array
 *  accesses.
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
 *      GblStringRef* pRef2 = GblStringRef_acquire(pRef);
 *      assert(GblStringRef_refCount(pRef2) == 2);
 *
 *      // even stores length to save on strlen() / StringView time
 *      assert(GblStringRef_length(pRef2) == 5);
 *
 *      // no strlen() happening here, fast as fuck
 *      GblStringView view = GblStringRef_view(pRef);
 *
 *      GblStringRef_release(pRef);  // now refCount is 1
 *      GblStringRef_release(pRef2); // now it's deleted
 *  \ingroup strings
 *
 *  \todo
 *      - clean up API to use overloads and default methods
 *      - rename acquire/release to ref/unref
 *      - make it use some epic buddy allocator shit
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_STRING_REF_H
#define GIMBAL_STRING_REF_H

#include "../utils/gimbal_ref.h"
#include "gimbal_string_view.h"

#define GBL_SELF_TYPE GblStringRef

GBL_DECLS_BEGIN

/*! \brief Reference-counted, char*-compatible string type
 *
 *  GblStringRef is a const char*-compatible reference-counted,
 *  read-only string type allowing for the sharing of a single
 *  string allocation between two different locations.
 *
 *  \ingroup strings
 *  \sa gimbal_string_ref.h
 */
typedef char GblStringRef;

GBL_EXPORT GblStringRef* GblStringRef_create         (const char* pString)      GBL_NOEXCEPT;

GBL_EXPORT GblStringRef* GblStringRef_createWithContext
                                                     (const char* pString,
                                                      GblContext* pCtx)         GBL_NOEXCEPT;

GBL_EXPORT GblStringRef* GblStringRef_createFromView (GblStringView view)       GBL_NOEXCEPT;

GBL_EXPORT GblStringRef* GblStringRef_createFromViewWithContext
                                                     (GblStringView view,
                                                      GblContext*   pCtx)       GBL_NOEXCEPT;

GBL_EXPORT GblStringRef* GblStringRef_createFormat   (const char* pFmt, ...)    GBL_NOEXCEPT;

GBL_EXPORT GblStringRef* GblStringRef_createFormatVaList
                                                     (const char* pFmt,
                                                      va_list*    pList)        GBL_NOEXCEPT;

GBL_EXPORT GblStringRef* GblStringRef_acquire        (GBL_CSELF)                GBL_NOEXCEPT;
GBL_EXPORT GblRefCount   GblStringRef_release        (GBL_CSELF)                GBL_NOEXCEPT;

GBL_EXPORT char          GblStringRef_at             (GBL_CSELF, size_t  index) GBL_NOEXCEPT;

GBL_EXPORT GblContext*   GblStringRef_context        (GBL_CSELF)                GBL_NOEXCEPT;
GBL_EXPORT GblRefCount   GblStringRef_refCount       (GBL_CSELF)                GBL_NOEXCEPT;
GBL_EXPORT size_t        GblStringRef_length         (GBL_CSELF)                GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringRef_empty          (GBL_CSELF)                GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblStringRef_valid          (GBL_CSELF)                GBL_NOEXCEPT;
GBL_EXPORT GblStringView GblStringRef_view           (GBL_CSELF)                GBL_NOEXCEPT;
GBL_EXPORT GblHash       GblStringRef_hash           (GBL_CSELF)                GBL_NOEXCEPT;

GBL_DECLS_END

//! \cond
#define GblStringRef_create(...) GBL_VA_OVERLOAD_CALL(GblStringRef_create_, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)
#define GblStringRef_ref(str)    GblStringRef_acquire(str)
#define GblStringRef_unref(str)  GblStringRef_release(str)

#define GblStringRef_create__1(str)             (GblStringRef_create)(str)
#define GblStringRef_create__2(str, len)        GblStringRef_createFromView(GblStringView_fromString(str, len))
#define GblStringRef_create__3(str, len, ctx)   GblStringRef_createFromViewWithContext(GblStringView_fromString(str, len), ctx)
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_STRING_REF_H
