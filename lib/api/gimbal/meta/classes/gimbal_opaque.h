/*! \file
 *  \brief      GblOpaqueClass and API for managing Opaque types
 *  \ingroup    meta
 *
 *  This file contains the API for registering and managing the lifetime of "opaque"
 *  user data types. This is an intermediate libGimbal type, which inherits from
 *  GBL_POINTER_TYPE, adding additional functionality. Rather than simple pointer
 *  assignments for copy semantics with no lifetime management, a type which derives
 *  from GBL_OPAQUE_TYPE adds the bare minimum virtual functions required to implement
 *  lifetime management.
 *
 *  When assigning the value of a GBL_OPAQUE_TYPE to another, the "copy" virtual method
 *  is called, which may implement value-based copying via something like memcpy(), or
 *  reference-based copying via something like incrementing a reference counter.
 *
 *  When the GBL_OPAQUE_TYPE is destroyed, the "free" virtual method is called, which
 *  may opt to actually deallocate the object for value-based copied objects OR it may
 *  simply decrement a reference counter for reference-based semantics. 
 *
 *  \author     2023, 2025 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_OPAQUE_H
#define GIMBAL_OPAQUE_H

#include "gimbal_primitives.h"

/*! \name Type System
 *  \brief Type UUID and cast macros
 *  @{
 */
#define GBL_OPAQUE_TYPE         (GBL_TYPEID(GblOpaque))             //!< Type UUID for GblOpaque
#define GBL_OPAQUE_CLASS(klass) (GBL_CLASS_CAST(GblOpaque, klass))  //!< Function-style GblOpaqueClass cast from GblClass
//! @}

GBL_DECLS_BEGIN

//! Function signature for for an opaque copy operation, see \ref GblOpaqueVTable::pFnCopy
typedef GBL_RESULT (*GblOpaqueCopyFn)(void* pOpaque, void** ppNewOpaque);
//! Function signature for an opaque free operation, see \ref GblOpaqueVTable::pFnFree
typedef GBL_RESULT (*GblOpaqueFreeFn)(void* pOpaque);

//! Virtual table structure for a GblOpaqueClass
typedef struct GblOpaqueVTable {
    GblOpaqueCopyFn pFnCopy;    //!< Copy method
    GblOpaqueFreeFn pFnFree;    //!< Free method
} GblOpaqueVTable;

/*! \struct GblOpaqueClass
 *  \extends GblPrimitiveClass
 *  \brief GblClass structure for opaque types
 *
 *  Simply contains a virtual table pointer to the implementation details.
 */
GBL_CLASS_DERIVE(GblOpaque, GblPrimitive)
    const GblOpaqueVTable* pVTable; //!< Pointer to the virtual table structure
GBL_CLASS_END

/*! \name Static Methods
 *  \brief Miscellaenous and Utility methods
 *  @{
 */
//! Returns the GblType UUID for GblOpaque.
GBL_EXPORT GblType GblOpaque_type        (void)                           GBL_NOEXCEPT;
//! Registers a new opaque subtype with the virtual table given by \p pVTable.
GBL_EXPORT GblType GblOpaque_register    (const char*            pName,
                                          const GblOpaqueVTable* pVTable) GBL_NOEXCEPT;
//! Convenience function which registers a new type meta type which is used with GblRef to manage its lifetime.
GBL_EXPORT GblType GblOpaque_registerRef (const char* pName)              GBL_NOEXCEPT;
//! @}

/*! \name Instance Methods
 *  \brief Methods for dealing with opaque instances (usually handled internally)
 *  @{
 */
//! Copies an opaque structure with the given type into \p ppNewOpaque, using its vtable
GBL_EXPORT GBL_RESULT GblOpaque_copy (void*      pOpaque,
                                      GblType    type,
                                      void**     ppNewOpaque) GBL_NOEXCEPT;
//! Frees an opaque structure with the given type using its vtable
GBL_EXPORT GBL_RESULT GblOpaque_free (void*      pOpaque,
                                      GblType    type)        GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#endif // GIMBAL_Opaque_H
