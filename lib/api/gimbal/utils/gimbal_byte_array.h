/*! \file
 *  \brief   GblByteArray structure for managing generic byte data
 *  \ingroup utils
 *
 *  This file includes the public definition of the GblByteArray
 *  structure as well as its related API. It is a reference-counted,
 *  mutable, dynamically resizable array of bytes
 *
 *  \todo
 *      - default argument overloading
 *      - consider making it dynamically resizable easily w/ capacity?
 *      - search/find subbytes
 *      - GblByteView type?
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_BYTE_ARRAY_H
#define GIMBAL_BYTE_ARRAY_H

#include "../strings/gimbal_string_view.h"

#define GBL_SELF_TYPE GblByteArray

GBL_DECLS_BEGIN

/*! \brief   Reference-counted resizable array of bytes
 *  \ingroup utils
 *
 *  GblByteArray is a reference-counted dynamic array of
 *  bytes with a list-style API as well as various methods
 *  for manipulating and extracting subarrays.
 *
 *  \sa GblBitView
 */
typedef struct GblByteArray {
    size_t   size;  //!< Size of the pData structure
    uint8_t* pData; //!< Actual data payload, contiguously-allocated array of bytes
} GblByteArray;

/*! \name  Lifetime Management
 *  \brief Methods for creation and reference management
 *  \relatesalso GblByteArray
 *  @{
 */
//! Creates and returns a new GblByteArray, with the given \p size and optional initial data and context
GBL_EXPORT GblByteArray* GblByteArray_create (size_t      size,
                                              const void* pData/*=NULL*/,
                                              GblContext* pCtx /*=NULL*/) GBL_NOEXCEPT;
//! Increments the reference counter of the given GblByteArray by 1, returning back a pointer to it
GBL_EXPORT GblByteArray* GblByteArray_ref    (GBL_SELF)                   GBL_NOEXCEPT;
//! Decrements the reference counter of the given GblByteArray by 1, destroying it upon hitting 0
GBL_EXPORT GblRefCount   GblByteArray_unref  (GBL_SELF)                   GBL_NOEXCEPT;
//! @}

/*! \name  Type Operators
 *  \brief Assignment and comparison methods
 *  \relatesalso GblByteArray
 *  @{
 */
//! Frees the existing allocation and copies over the allocation and size from \p pOther
GBL_EXPORT GBL_RESULT GblByteArray_copy    (GBL_SELF, const GblByteArray* pOther)   GBL_NOEXCEPT;
//! Frees the existing allocation and takes the allocation from \p pOther, clearing it
GBL_EXPORT GBL_RESULT GblByteArray_move    (GBL_SELF, GblByteArray* pOther)         GBL_NOEXCEPT;
//! Frees the existing allocation and takes the allocation given by \p pData with the given size
GBL_EXPORT GBL_RESULT GblByteArray_acquire (GBL_SELF, size_t bytes, void* pData)    GBL_NOEXCEPT;
//! Releases the internal allocation resource, copying it and its size out, clearing them from \p pSelf
GBL_EXPORT GBL_RESULT GblByteArray_release (GBL_SELF, size_t* pSize, void** ppData) GBL_NOEXCEPT;
//! Compares the two byte arrays with semantics similar to memcmp(), returning the result
GBL_EXPORT int        GblByteArray_compare (GBL_CSELF, const GblByteArray* pRhs)    GBL_NOEXCEPT;
//! Returns GBL_TRUE if the values stored within the two byte arrays are all equal
GBL_EXPORT GblBool    GblByteArray_equals  (GBL_CSELF, const GblByteArray* pRhs)    GBL_NOEXCEPT;
//! @}

/*! \name  Properties
 *  \brief Methods for getting values
 *  \relatesalso GblByteArray
 *  @{
 */
//! Returns the current number of active references to the given GblByteArray
GBL_EXPORT GblRefCount   GblByteArray_refCount   (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the GblContext pointer the GblByteArray was constructed with
GBL_EXPORT GblContext*   GblByteArray_context    (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the size of the GblByteArray (GblByteArray::size)
GBL_EXPORT size_t        GblByteArray_size       (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the data pointer of the GblByteArray (GblByteArray::pData)
GBL_EXPORT void*         GblByteArray_data       (GBL_CSELF) GBL_NOEXCEPT;
//! Return GBL_TRUE if the given GblByteArray is empty (0 bytes)
GBL_EXPORT GblBool       GblByteArray_empty      (GBL_CSELF) GBL_NOEXCEPT;
//! Computes a hash over the GblByteArray, returning its value
GBL_EXPORT GblHash       GblByteArray_hash       (GBL_CSELF) GBL_NOEXCEPT;
//! Returns the GblByteArray as a C string, or NULL if it's not NULL-terminated
GBL_EXPORT const char*   GblByteArray_cString    (GBL_CSELF) GBL_NOEXCEPT;
//! Returns a GblStringView spanning the bytes of the GblByteArray
GBL_EXPORT GblStringView GblByteArray_stringView (GBL_CSELF) GBL_NOEXCEPT;
//! @}

/*! \name Reading and Writing
 *  \brief Methods for fetching and storing data
 *  \relatesalso GblByteArray
 *  @{
 */
//! Returns the value of the byte located at the given \p index, raising an error upon out-of-range access
GBL_EXPORT uint8_t    GblByteArray_at    (GBL_CSELF, size_t index)                                GBL_NOEXCEPT;
//! Attempts to read a range of \p bytes starting at \p offset, copying them to \p pOut, returning a result code
GBL_EXPORT GBL_RESULT GblByteArray_read  (GBL_CSELF, size_t offset, size_t bytes, void* pOut)     GBL_NOEXCEPT;
//! Attempts to write a range of \p bytes starting at \p offset, copying from \p pIn, returning a result code
GBL_EXPORT GBL_RESULT GblByteArray_write (GBL_SELF, size_t offset, size_t bytes, const void* pIn) GBL_NOEXCEPT;
//! Resizes the GblByteArray to \p bytes, copying \p pData over into its internal array
GBL_EXPORT GBL_RESULT GblByteArray_set   (GBL_SELF, size_t bytes, const void* pData)              GBL_NOEXCEPT;
//! @}

/*! \name List API
 *  \brief Methods providing a standard list-style interface
 *  \relatesalso GblByteArray
 *  @{
 */
//! Appends \p bytes from \p pData to the end of the given GblByteArray, returning a status code
GBL_EXPORT GBL_RESULT GblByteArray_append  (GBL_SELF, size_t bytes, const void* pData)                GBL_NOEXCEPT;
//! Prepends \p bytes from \p pData to the beginning of the given GblByteArray, returning a status code
GBL_EXPORT GBL_RESULT GblByteArray_prepend (GBL_SELF, size_t bytes, const void* pData)                GBL_NOEXCEPT;
//! Inserts \p bytes into the given GblByteArray at \p offset, copying from \p pData, returning a status code and resizing as needed
GBL_EXPORT GBL_RESULT GblByteArray_insert  (GBL_SELF, size_t offset, size_t bytes, const void* pData) GBL_NOEXCEPT;
//! Erases \p bytes from the given GblByteArray starting at \p offset, returning a status code
GBL_EXPORT GBL_RESULT GblByteArray_erase   (GBL_SELF, size_t offset, size_t bytes)                    GBL_NOEXCEPT;
//! Clears all bytes from the given GblByteArray, freeing its allocation and resetting its size to 0
GBL_EXPORT GBL_RESULT GblByteArray_clear   (GBL_SELF)                                                 GBL_NOEXCEPT;
//! Resizes the given GblByteArray to a size of \p bytes, appending byte values of 0 when growing, returning a status code
GBL_EXPORT GBL_RESULT GblByteArray_resize  (GBL_SELF, size_t bytes)                                   GBL_NOEXCEPT;
//! Grows the given GblByteArray by the given number of \p bytes, filling the new ones with 0s, returning a status code
GBL_EXPORT GBL_RESULT GblByteArray_grow    (GBL_SELF, size_t bytes)                                   GBL_NOEXCEPT;
//! Shrinks the given GblByteArray by the given number of \p bytes, returning a status code
GBL_EXPORT GBL_RESULT GblByteArray_shrink  (GBL_SELF, size_t bytes)                                   GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

//! \cond
#define GblByteArray_create(...) \
    GblByteArray_createDefault_(__VA_ARGS__)
#define GblByteArray_createDefault_(...) \
    GblByteArray_createDefault__(__VA_ARGS__, GBL_NULL, GBL_NULL)
#define GblByteArray_createDefault__(bytes, data, ctx, ...) \
    (GblByteArray_create)(bytes, data, ctx)
//! \endcond

#undef GBL_SELF_TYPE

#endif // GIMBAL_BYTE_ARRAY_H
