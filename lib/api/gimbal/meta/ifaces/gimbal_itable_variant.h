#ifndef GIMBAL_ITABLE_VARIANT_H
#define GIMBAL_ITABLE_VARIANT_H

#include "gimbal_ivariant.h"

/*! \name Type System
 *  \brief Type UUID and cast operators
 *  @{
 */
#define GBL_ITABLE_VARIANT_TYPE             (GBL_TYPEOF(GblITableVariant))                   //!< Type UUID for GblITableVariant
#define GBL_ITABLE_VARIANT(self)            (GBL_INSTANCE_CAST(self, GblITableVariant))      //!< Casts a GblInstance to GblITableVariant
#define GBL_ITABLE_VARIANT_CLASS(klass)     (GBL_CLASS_CAST(klass, GblITableVariant))        //!< Casts a GblClass to GblITableVariantClass
#define GBL_ITABLE_VARIANT_GET_CLASS(self)  (GBL_INSTANCE_GET_CLASS(self, GblITableVariant)) //!< Gets a GblITableVariantClass from a GblInstance
//! @}

#define GBL_SELF_TYPE GblVariant

GBL_DECLS_BEGIN

/*! \struct  GblITableVariantClass
 *  \extends GblInterface
 *  \brief   GblClass structure for GblITableVariant
 *
 * Provides overridable virtual methods which are roughly
 * analogous to the Lua __index, __newIndex, and __length
 * metamethods used for managing table accesses.
 *
 *  \sa GblITableVariant
 */
GBL_INTERFACE_DERIVE(GblITableVariant)
    GBL_RESULT (*pFnIndex)   (GBL_CSELF, const GblVariant* pKey, GblVariant* pValue);
    GBL_RESULT (*pFnSetIndex)(GBL_CSELF, const GblVariant* pKey, GblVariant* pValue);
    GBL_RESULT (*pFnNext)    (GBL_CSELF, GblVariant* pKey, GblVariant* pValue);
    GBL_RESULT (*pFnCount)   (GBL_CSELF, size_t* pSize);
GBL_INTERFACE_END

/*! \struct GblITableVariant
 *  \brief  Adds Lua table-like functionality to GblVariant-compatible types
 *
 *  GblITableVariant is an extension interface of GblIVariant. It adds
 *  additional methods for implementing subscripting and iteration behavior
 *  on a GblVariant-compatible type.
 *
 *  \note
 *  This whole mechanism is was inspired by Lua's metatables, and the
 *  virtual methods within the interface class are analogous to metamethods
 *
 *  \sa GblITableVariantClass
 */

//! Returns the GblType UUID associated with GblITableVariant
GBL_EXPORT GblType GblITableVariant_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_ITABLE_VARIANT_H
