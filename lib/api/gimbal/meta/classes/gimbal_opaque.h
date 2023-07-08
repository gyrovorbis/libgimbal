/*! \file
 *  \brief      GblOpaqueClass and API for managing Opaque types
 *  \ingroup    meta
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_OPAQUE_H
#define GIMBAL_OPAQUE_H

#include "gimbal_primitives.h"

/*! \name Type System
 *  \brief Type UUID and cast macros
 *  @{
 */
#define GBL_OPAQUE_TYPE         (GBL_TYPEOF(GblOpaque))             //!< Type UUID for GblOpaque
#define GBL_OPAQUE_CLASS(klass) (GBL_CLASS_CAST(klass, GblOpaque))  //!< Function-style GblOpaqueClass cast from GblClass
//! @}

GBL_DECLS_BEGIN

//! Function signature for for an opaque copy operation, see \ref GblOpaqueClassVTable::pFnCopy
typedef GBL_RESULT (*GblOpaqueCopyFn)(void* pOpaque, void** ppNewOpaque);
//! Function signature for an opaque free operation, see \ref GblOpaqueClassVTable::pFnFree
typedef GBL_RESULT (*GblOpaqueFreeFn)(void* pOpaque);

//! Virtual table structure for a GblOpaqueClass
typedef struct GblOpaqueClassVTable {
    GblOpaqueCopyFn pFnCopy;    //!< Copy method
    GblOpaqueFreeFn pFnFree;    //!< Free method
} GblOpaqueClassVTable;

/*! \struct GblOpaqueClass
 *  \extends GblPrimitiveClass
 *  \brief GblClass structure for opaque types
 *
 *  Simply contains a virtual table pointer
 *  to the implementation details.
 */
GBL_CLASS_DERIVE(GblOpaque, GblPrimitive)
    const GblOpaqueClassVTable* pVTable; //!< Pointer to the virtual table structure
GBL_CLASS_END

/*! \name Static Methods
 *  \brief Miscellaenous and Utility methods
 *  @{
 */
//! Returns the GblType UUID for GblOpaque
GBL_EXPORT GblType GblOpaque_type     (void)                                GBL_NOEXCEPT;
//! Registers an opaque subtype with the virtual table given by \p pVTable
GBL_EXPORT GblType GblOpaque_register (const char*                 pName,
                                       const GblOpaqueClassVTable* pVTable) GBL_NOEXCEPT;
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
