/*! \file
 *  \brief GblBitStructClass and API for managing bitfields
 *  \ingroup meta
 *
 *  This file contains the type definition for GblBitStructClass,
 *  which enables a 32 to 64-bit integer to be accessed via a
 *  GblVariant as though it were a regular structure type with
 *  string-type keys and values of arbitrary type.
 *
 *  \author     2023 Falco Girgis
 *  \copyright  MIT License
 */
#ifndef GIMBAL_BIT_STRUCT_H
#define GIMBAL_BIT_STRUCT_H

#include "gimbal_primitives.h"
#include "../ifaces/gimbal_itable_variant.h"

#define GBL_BIT_STRUCT_TYPE                     (GBL_TYPEID(GblBitStruct))
#define GBL_BIT_STRUCT_CLASS(klass)             (GBL_CLASS_CAST(GblBitStruct, klass))

#define GBL_BIT_STRUCT_ENTRY(name, mask, type)  { name, mask, type }
#define GBL_BIT_STRUCT_LAST()                   { 0, 0x00, GBL_INVALID_TYPE }

#define GBL_SELF_TYPE GblBitStructClass

GBL_DECLS_BEGIN

typedef uint64_t GblBitStruct;

//! Contains meta data for a single field of a GblBitStruct
typedef struct GblBitStructField {
    const char* pName;      //!< Field name
    GblBitmask  mask;       //!< Field mask
    GblType     valueType;  //!< Field value
} GblBitStructField;

// Should we just give them properties?!
GBL_CLASS_DERIVE(GblBitStruct, GblPrimitive, GblITableVariant)
    GBL_RESULT (*pFnCheckFieldValue)(GBL_CSELF, size_t fieldIndex, const GblVariant* pValue);
    GblBitmask valueMask;
    size_t     fieldCount;
GBL_CLASS_END

GBL_EXPORT GBL_DECLARE_TYPE(GblBitStruct);

/*! \name Field Name
 *  \brief Methods for retrieving field name
 *  \relatesalso GblBitStructClass
 *  @{
 */
GBL_EXPORT const char* GblBitStructClass_nameFromIndex        (GBL_CSELF, size_t index)   GBL_NOEXCEPT;
GBL_EXPORT const char* GblBitStructClass_nameFromBitmask      (GBL_CSELF, GblBitmask msk) GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblBitStructClass_nameQuarkFromIndex   (GBL_CSELF, size_t index)   GBL_NOEXCEPT;
GBL_EXPORT GblQuark    GblBitStructClass_nameQuarkFromBitmask (GBL_CSELF, GblBitmask msk) GBL_NOEXCEPT;
//! @}

/*! \name Field Mask
 *  \brief Methods for retrieving field mask
 *  \relatesalso GblBitStructClass
 *  @{
 */
GBL_EXPORT GblBitmask GblBitStructClass_maskFromIndex     (GBL_CSELF, size_t index)      GBL_NOEXCEPT;
GBL_EXPORT GblBitmask GblBitStructClass_maskFromName      (GBL_CSELF, const char* pName) GBL_NOEXCEPT;
GBL_EXPORT GblBitmask GblBitStructClass_maskFromNameQuark (GBL_CSELF, GblQuark quark)    GBL_NOEXCEPT;
//! @}

/*! \name Field Type
 *  \brief Methods for retrieving field type
 *  \relatesalso GblBitStructClass
 *  @{
 */
GBL_EXPORT GblType GblBitStructClass_typeFromIndex     (GBL_CSELF, size_t index)      GBL_NOEXCEPT;
GBL_EXPORT GblType GblBitStructClass_typeFromName      (GBL_CSELF, const char* pName) GBL_NOEXCEPT;
GBL_EXPORT GblType GblBitStructClass_typeFromNameQuark (GBL_CSELF, GblQuark quark)    GBL_NOEXCEPT;
GBL_EXPORT GblType GblBitStructClass_typeFromBitmask   (GBL_CSELF, GblBitmask msk)    GBL_NOEXCEPT;
//! @}

#if 0

GBL_EXPORT GblBool GblBitStructClass_checkValueForIndex     (GBL_CSELF, size_t index, GblVariant* pValue)      GBL_NOEXCEPT;
GBL_EXPORT GblBool GblBitStructClass_checkValueForName      (GBL_CSELF, const char* pName, GblVariant* pValue) GBL_NOEXCEPT;
GBL_EXPORT GblBool GblBitStructClass_checkValueForNameQuark (GBL_CSELF, GblQuark quark, GblVariant* pValue)    GBL_NOEXCEPT;
GBL_EXPORT GblBool GblBitStructClass_checkValueForBitmask   (GBL_CSELF, GblBitmask msk, GblVariant* pValue) GBL_NOEXCEPT;

GBL_EXPORT GblBool GblBitStructClass_checkValue (GBL_CSELF, uint64_t value) GBL_NOEXCEPT;

GBL_EXPORT GblBool GblBitStruct_checkValue(GblBitStruct value, GblType type);

GBL_EXPORT GBL_RESULT GblBitStruct_field(GblBitStruct value, GblType type, const char* pName, ...);
GBL_EXPORT GBL_RESULT GblBitStruct_mask(GblBitStruct value, GblType type, GblBitmask mask, ...);
GBL_EXPORT GBL_RESULT GblBitStruct_index(GblBitStruct value, GblType type, size_t index, ...);

GBL_EXPORT GBL_RESULT GblBitStruct_fields(GblBitStruct value, GblType type, ...);
GBL_EXPORT GBL_RESULT GblBitStruct_masks(GblBitStruct value, GblType type, ...);
GBL_EXPORT GBL_RESULT GblBitStruct_indices(GblBitStruct value, GblType type, ...);

GBL_EXPORT GBL_RESULT GblBitStruct_setField(GblBitStruct* pValue, GblType type, const char* pName, ...);
GBL_EXPORT GBL_RESULT GblBitStruct_setMask(GblBitStruct* pValue, GblType type, GblBitmask mask, ...);
GBL_EXPORT GBL_RESULT GblBitStruct_setIndex(GblBitStruct* pValue, GblType type, size_t index, ...);

GBL_EXPORT GBL_RESULT GblBitStruct_setFields(GblBitStruct* pValue, GblType type, const char* pName, ...);
GBL_EXPORT GBL_RESULT GblBitStruct_setMasks(GblBitStruct* pValue, GblType type, GblBitmask mask, ...);
GBL_EXPORT GBL_RESULT GblBitStruct_setIndices(GblBitStruct* pValue, GblType type, size_t index, ...);

// Returns key type based on type of empty value first passed in?
GBL_EXPORT GBL_RESULT GblBitStruct_next(GblBitStruct value, GblType type, GblVariant* pkey, GblVariant* pValue);

#endif
GBL_EXPORT GblType GblBitStruct_register(const char* pName, const GblBitStructField* pFields) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_BIT_STRUCT_H
