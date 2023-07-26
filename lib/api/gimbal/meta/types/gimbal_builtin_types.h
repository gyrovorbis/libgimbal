/*! \file
 *  \brief   Builtin type UUIDs and Semi-Private GblType API
 *  \ingroup meta
 *
 *  This is a semi-private header which gets automatically
 *  included by gimbal_type.h, providing support for all
 *  builtin, auto-registered types as well as advanced
 *  type configuration flags for root types.
 *
 *  \author    2023 Falco Girgis
 *  \copyright MIT License
 */

#include "../../core/gimbal_typedefs.h"
#include "../../core/gimbal_decls.h"

#ifndef GIMBAL_BUILTIN_TYPES_H
#define GIMBAL_BUILTIN_TYPES_H

/*! \name  Builtin Indices
 *  \brief Definitions providing each builtin type's index
 *  @{
 */
#define GBL_TYPE_BUILTIN_INDEX_PROTOCOL         0   //!< Index of the builtin protocol type
#define GBL_TYPE_BUILTIN_INDEX_STATIC_CLASS     1   //!< Index of the builtin static class type
#define GBL_TYPE_BUILTIN_INDEX_INTERFACE        2   //!< Index of the builtin interface type
#define GBL_TYPE_BUILTIN_INDEX_INSTANCE         3   //!< Index of the builtin instance type
#define GBL_TYPE_BUILTIN_INDEX_IVARIANT         4   //!< Index of the builtin GblIVariant type
#define GBL_TYPE_BUILTIN_INDEX_NIL              5   //!< Index of the builtin nil type
#define GBL_TYPE_BUILTIN_INDEX_BOOL             6   //!< Index of the builtin GblBool type
#define GBL_TYPE_BUILTIN_INDEX_CHAR             7   //!< Index of the builtin char type
#define GBL_TYPE_BUILTIN_INDEX_UINT8            8   //!< Index of the builtin uint8_t type
#define GBL_TYPE_BUILTIN_INDEX_INT16            9   //!< Index of the builtin int16_t type
#define GBL_TYPE_BUILTIN_INDEX_UINT16           10  //!< Index of the builtin uint16_t type
#define GBL_TYPE_BUILTIN_INDEX_INT32            11  //!< Index of the builtin int32_t type
#define GBL_TYPE_BUILTIN_INDEX_UINT32           12  //!< Index of the builtin uint32_t type
#define GBL_TYPE_BUILTIN_INDEX_INT64            13  //!< Index of the builtin int64_t type
#define GBL_TYPE_BUILTIN_INDEX_UINT64           14  //!< Index of the builtin uint64_t type
#define GBL_TYPE_BUILTIN_INDEX_FLOAT            15  //!< Index of the builtin float type
#define GBL_TYPE_BUILTIN_INDEX_DOUBLE           16  //!< Index of the builtin double type
#define GBL_TYPE_BUILTIN_INDEX_STRING           17  //!< Index of the builtin string type
#define GBL_TYPE_BUILTIN_INDEX_POINTER          18  //!< Index of the builtin pointer type
//! @}

#define GBL_TYPE_BUILTIN_COUNT                  19  //!< Number of builtin types

//! Returns a type from the macro prefix of a builtin type
#define GBL_BUILTIN_TYPE(prefix) \
    (GblType_fromBuiltinIndex(GBL_TYPE_BUILTIN_INDEX_##prefix))


/*! \name  Type Flag Tests
 *  \brief Convenience macros for testing individual type flags
 */
///@{
#define GBL_TYPE_DEPENDENT_CHECK(type)         (GblType_flags(type) & GBL_TYPE_ROOT_FLAG_DEPENDENT)      //!< Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_DEPENDENT flag
#define GBL_TYPE_CLASSED_CHECK(type)           (GblType_flags(type) & GBL_TYPE_ROOT_FLAG_CLASSED)        //!< Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_CLASSED flag
#define GBL_TYPE_INTERFACED_CHECK(type)        (GblType_flags(type) & GBL_TYPE_ROOT_FLAG_INTERFACED)     //!< Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_INTERFACED flag
#define GBL_TYPE_INSTANTIABLE_CHECK(type)      (GblType_flags(type) & GBL_TYPE_ROOT_FLAG_INSTANTIABLE)   //!< Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_INSTANTIABLE flag
#define GBL_TYPE_DERIVABLE_CHECK(type)         (GblType_flags(type) & GBL_TYPE_ROOT_FLAG_DERIVABLE)      //!< Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_DERIVABLE flag
#define GBL_TYPE_DEEP_DERIVABLE_CHECK(type)    (GblType_flags(type) & GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE) //!< Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE flag
#define GBL_TYPE_BUILTIN_CHECK(type)           (GblType_flags(type) & GBL_TYPE_FLAG_BUILTIN)             //!< Convenience macro checking a GblType's GBL_TYPE_BUILTIN flag
#define GBL_TYPE_ABSTRACT_CHECK(type)          (GblType_flags(type) & GBL_TYPE_FLAG_ABSTRACT)            //!< Convenience macro checking a GblType's GBL_TYPE_ABSTRACT flag
#define GBL_TYPE_FINAL_CHECK(type)             (GblType_flags(type) & GBL_TYPE_FLAG_FINAL)               //!< Convenience macro checking a GblType's GBL_TYPE_FINAL flag                                  ///< \details Convenience macro for testing whether a given type is fundamental                                              ///< \details Convenience macro for testing whether a given type is valid               ///< \details Convenience macro for testing whether a given type can be stored within a GblVariant
#define GBL_TYPE_ROOT_CHECK(type)              (GblType_root(type) == type)
///@}

//! Flags controlling behavior of root or fundemental GblTypes. These can only be set on root types and are inherited.
typedef enum GblTypeRootFlags {
    GBL_TYPE_ROOT_FLAG_DEPENDENT         = (1 << 0),                            //!< Type is dependent upon other types in dependency list
    GBL_TYPE_ROOT_FLAG_CLASSED           = (1 << 1),                            //!< Type has an associated GblClass
    GBL_TYPE_ROOT_FLAG_INTERFACED        = (1 << 2),                            //!< Type has an associated GblInterface as its class (cannot be instantiable)
    GBL_TYPE_ROOT_FLAG_INSTANTIABLE      = (1 << 3),                            //!< Type has an associated GblInstance
    GBL_TYPE_ROOT_FLAG_DERIVABLE         = (1 << 4),                            //!< Type supports single-level inheritance
    GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE    = (1 << 5),                            //!< Type suppports multi-level inheritance
    GBL_TYPE_ROOT_FLAGS_MASK             = GBL_TYPE_ROOT_FLAG_DEPENDENT       | //!< Mask of all GblRootTypeFlag values
                                           GBL_TYPE_ROOT_FLAG_CLASSED         |
                                           GBL_TYPE_ROOT_FLAG_INSTANTIABLE    |
                                           GBL_TYPE_ROOT_FLAG_DERIVABLE       |
                                           GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE  |
                                           GBL_TYPE_ROOT_FLAG_INTERFACED
} GblTypeRootFlags;

//! Retrieves the GblType UUID associated with the given \p index of a builtin type
GBL_EXPORT uintptr_t GblType_fromBuiltinIndex (size_t index) GBL_NOEXCEPT;

#endif // GIMBAL_BUILTIN_TYPES_H
