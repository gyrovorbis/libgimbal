/*! \file
 *  \brief   GblLibrary, dynamically linked library management
 *  \ingroup core
 *

 *
 *   \author    2023 Falco Girgis
 *   \copyright MIT License
 */
#ifndef GIMBAL_LIBRARY_H
#define GIMBAL_LIBRARY_H

#include "../strings/gimbal_string_list.h"

#define GBL_SELF_TYPE GblLibrary

GBL_DECLS_BEGIN

/*! \struct  GblLibrary
 *  \brief   Opaque shared library structure
 *  \ingroup core
 *
 *  GblLibrary is an opaque, reference-counted structure representing a dynamically-
 *  linked library, such as a .DLL or .so. Libraries are requested by name, using
 *  GblLibrary_open(), which will either:
 *      a. Search for the library by name within the list of search paths
 *      b. Return a reference to the library if it was already open
 */
GBL_FORWARD_DECLARE_STRUCT(GblLibrary)

//! Iterator function for use with GblLibrary_foreach(), exits early when GBL_TRUE is returned
typedef GblBool (*GblLibraryIterFn)(GblLibrary* pSelf, void* pClosure);

/*! \name  Static API
 *  \brief Methods for search paths and iteration
 *  @{
 */
//! Returns the list of paths which are searched when attempting to load a library by name with GblLibrary_open()
GBL_EXPORT const GblStringList*
                       GblLibrary_searchPaths   (void)                              GBL_NOEXCEPT;
//! Adds \p pPath to the list of paths to search when loading a library by name with GblLibrary_open()
GBL_EXPORT void        GblLibrary_addSearchPath (const char* pPath)                 GBL_NOEXCEPT;
//! Returns the total number of open dynamic libraries
GBL_EXPORT size_t      GblLibrary_count         (void)                              GBL_NOEXCEPT;
//! Returns the open dynamic library at the given \p index
GBL_EXPORT GblLibrary* GblLibrary_at            (size_t index)                      GBL_NOEXCEPT;
//! Iterates over each open dynamic library, calling \p pFnInit with the given userdata
GBL_EXPORT GblBool     GblLibrary_foreach       (GblLibraryIterFn pFnIt, void* pCl) GBL_NOEXCEPT;
//! @}

/*! \name Loading and Unloading
 *  \brief Methods for querying dynamic libraries and symbols
 *  \relatesalso GblLibrary
 *  @{
 */
//! Returns a reference to the library with the given file name, opening it if it wasn't already opened
GBL_EXPORT GblLibrary* GblLibrary_open   (const char* pName)            GBL_NOEXCEPT;
GBL_EXPORT GblRefCount GblLibrary_close  (GBL_CSELF)                    GBL_NOEXCEPT;
GBL_EXPORT GblPtr      GblLibrary_symbol (GBL_CSELF, const char* pName) GBL_NOEXCEPT;
//! @}

/*! \name Accessors
 *  \brief Methods for reading and writing properties
 *  \relatesalso GblLibrary
 *  @{
 */
GBL_EXPORT GblStringRef* GblLibrary_name         (GBL_CSELF) GBL_NOEXCEPT;
GBL_EXPORT GblStringRef* GblLibrary_path         (GBL_CSELF) GBL_NOEXCEPT;
GBL_EXPORT GblRefCount   GblLibrary_refCount     (GBL_CSELF) GBL_NOEXCEPT;
GBL_EXPORT GblBool       GblLibrary_resident     (GBL_CSELF) GBL_NOEXCEPT;
GBL_EXPORT void          GblLibrary_makeResident (GBL_SELF) GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#endif // GIMBAL_LIBRARY_H
