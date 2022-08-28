#ifndef GIMBAL_BUILTIN_TYPES_H
#define GIMBAL_BUILTIN_TYPES_H

#define GBL_TYPE_BUILTIN_INDEX_PROTOCOL             0
#define GBL_TYPE_BUILTIN_INDEX_STATIC_CLASS         1
#define GBL_TYPE_BUILTIN_INDEX_INTERFACE            2
#define GBL_TYPE_BUILTIN_INDEX_INSTANCE             3
#define GBL_TYPE_BUILTIN_INDEX_IVARIANT             4
#define GBL_TYPE_BUILTIN_INDEX_NIL                  5
#define GBL_TYPE_BUILTIN_INDEX_BOOL                 6
#define GBL_TYPE_BUILTIN_INDEX_CHAR                 7
#define GBL_TYPE_BUILTIN_INDEX_UINT8                8
#define GBL_TYPE_BUILTIN_INDEX_INT16                9
#define GBL_TYPE_BUILTIN_INDEX_UINT16               10
#define GBL_TYPE_BUILTIN_INDEX_INT32                11
#define GBL_TYPE_BUILTIN_INDEX_UINT32               12
#define GBL_TYPE_BUILTIN_INDEX_INT64                13
#define GBL_TYPE_BUILTIN_INDEX_UINT64               14
#define GBL_TYPE_BUILTIN_INDEX_FLOAT                15
#define GBL_TYPE_BUILTIN_INDEX_DOUBLE               16
#define GBL_TYPE_BUILTIN_INDEX_STRING               17
#define GBL_TYPE_BUILTIN_INDEX_POINTER              18

#define GBL_TYPE_BUILTIN_COUNT                      19

#define GBL_BUILTIN_TYPE(prefix) \
    (GblType_fromBuiltinIndex(GBL_TYPE_BUILTIN_INDEX_##prefix))


/** @name Type Flag Tests
 *  @details Convenience macros for testing individual type flags
 */
///@{
#define GBL_TYPE_DEPENDENT_CHECK(type)         (GblType_flagsCheck(type, GBL_TYPE_ROOT_FLAG_DEPENDENT))          ///< \details Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_DEPENDENT flag
#define GBL_TYPE_CLASSED_CHECK(type)           (GblType_flagsCheck(type, GBL_TYPE_ROOT_FLAG_CLASSED))            ///< \details Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_CLASSED flag
#define GBL_TYPE_INTERFACED_CHECK(type)        (GblType_flagsCheck(type, GBL_TYPE_ROOT_FLAG_INTERFACED))         ///< \details Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_INTERFACED flag
#define GBL_TYPE_INSTANTIABLE_CHECK(type)      (GblType_flagsCheck(type, GBL_TYPE_ROOT_FLAG_INSTANTIABLE))       ///< \details onvenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_INSTANTIABLE flag
#define GBL_TYPE_DERIVABLE_CHECK(type)         (GblType_flagsCheck(type, GBL_TYPE_ROOT_FLAG_DERIVABLE))          ///< \details Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_DERIVABLE flag
#define GBL_TYPE_DEEP_DERIVABLE_CHECK(type)    (GblType_flagsCheck(type, GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE))     ///< \details Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE flag
#define GBL_TYPE_BUILTIN_CHECK(type)           (GblType_flagsCheck(type, GBL_TYPE_FLAG_BUILTIN))                 ///< \details Convenience macro checking a GblType's GBL_TYPE_BUILTIN flag
#define GBL_TYPE_ABSTRACT_CHECK(type)          (GblType_flagsCheck(type, GBL_TYPE_FLAG_ABSTRACT))                ///< \details Convenience macro checking a GblType's GBL_TYPE_ABSTRACT flag
#define GBL_TYPE_FINAL_CHECK(type)             (GblType_flagsCheck(type, GBL_TYPE_FLAG_FINAL))                   ///< \details Convenience macro checking a GblType's GBL_TYPE_FINAL flag                                  ///< \details Convenience macro for testing whether a given type is fundamental                                              ///< \details Convenience macro for testing whether a given type is valid               ///< \details Convenience macro for testing whether a given type can be stored within a GblVariant
#define GBL_TYPE_ROOT_CHECK(type)              (GblType_root(type) == type)
///@}

/** \name Type Flags
 *  \details Flags control the behavior of a GblType. A type's flags is the combination of its
 *  inherited GblTypeRootFlags OR'd with its own GblTypeFlags.
 */
///@{
/// \details Flags controlling behavior of root or fundemental GblTypes. These can only be set on root types and are inherited.
typedef enum GblTypeRootFlags {
    GBL_TYPE_ROOT_FLAG_DEPENDENT         = (1 << 0),                                     ///< Type is dependent upon other types in dependency list
    GBL_TYPE_ROOT_FLAG_CLASSED           = (1 << 1),                                     ///< Type has an associated GblClass
    GBL_TYPE_ROOT_FLAG_INTERFACED        = (1 << 2),                                     ///< Type has an associated GblInterface as its class (cannot be instantiable)
    GBL_TYPE_ROOT_FLAG_INSTANTIABLE      = (1 << 3),                                     ///< Type has an associated GblInstance
    GBL_TYPE_ROOT_FLAG_DERIVABLE         = (1 << 4),                                     ///< Type supports single-level inheritance
    GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE    = (1 << 5),                                     ///< Type suppports multi-level inheritance
    GBL_TYPE_ROOT_FLAGS_MASK             = GBL_TYPE_ROOT_FLAG_DEPENDENT       |          ///< Mask of all GblFundamentalTypeFlag values
                                           GBL_TYPE_ROOT_FLAG_CLASSED         |
                                           GBL_TYPE_ROOT_FLAG_INSTANTIABLE    |
                                           GBL_TYPE_ROOT_FLAG_DERIVABLE       |
                                           GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE  |
                                           GBL_TYPE_ROOT_FLAG_INTERFACED
} GblTypeRootFlags;


#endif // GIMBAL_BUILTIN_TYPES_H
