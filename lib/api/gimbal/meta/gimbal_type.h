/*! \file
 *  \brief ::GblType identifier and related functions
 *  \ingroup meta
 */
#ifndef GIMBAL_TYPE_H
#define GIMBAL_TYPE_H

#include "../types/gimbal_typedefs.h"
#include "../strings/gimbal_quark.h"
#include "gimbal_builtin_types.h"

/*! UUID of an invalid type
 * \ingroup metaBuiltinTypes
 */
#define GBL_INVALID_TYPE                    ((GblType)0)

/** @name Type Flag Tests
 *  @details Convenience macros for testing individual type flags
 */
///@{
#define GBL_TYPE_DEPENDENT_CHECK(type)         (GblType_flagsTest(type, GBL_TYPE_ROOT_FLAG_DEPENDENT))          ///< \details Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_DEPENDENT flag
#define GBL_TYPE_CLASSED_CHECK(type)           (GblType_flagsTest(type, GBL_TYPE_ROOT_FLAG_CLASSED))            ///< \details Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_CLASSED flag
#define GBL_TYPE_INTERFACED_CHECK(type)        (GblType_flagsTest(type, GBL_TYPE_ROOT_FLAG_INTERFACED))         ///< \details Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_INTERFACED flag
#define GBL_TYPE_INSTANTIABLE_CHECK(type)      (GblType_flagsTest(type, GBL_TYPE_ROOT_FLAG_INSTANTIABLE))       ///< \details onvenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_INSTANTIABLE flag
#define GBL_TYPE_DERIVABLE_CHECK(type)         (GblType_flagsTest(type, GBL_TYPE_ROOT_FLAG_DERIVABLE))          ///< \details Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_DERIVABLE flag
#define GBL_TYPE_DEEP_DERIVABLE_CHECK(type)    (GblType_flagsTest(type, GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE))     ///< \details Convenience macro checking a GblType's GBL_TYPE_ROOT_FLAG_DEEP_DERIVABLE flag
#define GBL_TYPE_BUILTIN_CHECK(type)           (GblType_flagsTest(type, GBL_TYPE_FLAG_BUILTIN))                 ///< \details Convenience macro checking a GblType's GBL_TYPE_BUILTIN flag
#define GBL_TYPE_ABSTRACT_CHECK(type)          (GblType_flagsTest(type, GBL_TYPE_FLAG_ABSTRACT))                ///< \details Convenience macro checking a GblType's GBL_TYPE_ABSTRACT flag
#define GBL_TYPE_FINAL_CHECK(type)             (GblType_flagsTest(type, GBL_TYPE_FLAG_FINAL))                   ///< \details Convenience macro checking a GblType's GBL_TYPE_FINAL flag                                  ///< \details Convenience macro for testing whether a given type is fundamental                                              ///< \details Convenience macro for testing whether a given type is valid               ///< \details Convenience macro for testing whether a given type can be stored within a GblVariant
#define GBL_TYPE_ROOT_CHECK(type)              (GblType_root(type) == type)
///@}

#define SELF    GblType self

GBL_DECLS_BEGIN

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

/// \details Flags controlling behavior of GblTypes. These can be set at any level in a type heirarchy.
typedef enum GblTypeFlags {
    GBL_TYPE_FLAGS_NONE                 = 0,
    GBL_TYPE_FLAG_BUILTIN               = (1 << 6),                             ///< Type was automatically registered as a builtin type
    GBL_TYPE_FLAG_TYPEINFO_STATIC       = (1 << 7),                             ///< Type's GblTypeInfo specified upon registration is static, so no internal storage has to be allocated for storing a copy
    GBL_TYPE_FLAG_CLASS_PINNED          = (1 << 8),                             ///< Type's GblClass is never destroyed and remains persistent upon construction. The default behavior is to create/destroy as referenced.
    GBL_TYPE_FLAG_CLASS_PREINIT         = (1 << 9),                             ///< Type's GblClass should be constructed immediately, rather than lazily upon use by default. Also implies class pinning.
    GBL_TYPE_FLAG_UNMAPPABLE            = (1 << 10),                            ///< Cannot obtain Type's associated GblInterface from a GblClass it has been mapped to. Disambiguates casting with common interface base classes.
    GBL_TYPE_FLAG_INCOMPLETE            = (1 << 11),                            ///< Incomplete/partial type missing some type dependency
    GBL_TYPE_FLAG_ABSTRACT              = (1 << 12),                            ///< Type cannot be instantiated without being derived
    GBL_TYPE_FLAG_FINAL                 = (1 << 13),                            ///< Type cannot be derived from
    GBL_TYPE_FLAGS_MASK                 = GBL_TYPE_FLAG_BUILTIN             |   ///< Mask of all GblTypeFlag values
                                          GBL_TYPE_FLAG_TYPEINFO_STATIC     |
                                          GBL_TYPE_FLAG_CLASS_PINNED        |
                                          GBL_TYPE_FLAG_CLASS_PREINIT       |
                                          GBL_TYPE_FLAG_UNMAPPABLE          |
                                          GBL_TYPE_FLAG_INCOMPLETE          |
                                          GBL_TYPE_FLAG_ABSTRACT            |
                                          GBL_TYPE_FLAG_FINAL
} GblTypeFlags;
///@}


/*! \brief Type representing meta type UUIDs
 *  \ingroup meta
 *  \details
 *
 */
typedef uintptr_t                                   GblType;

///\name Type Functions
/// @{
typedef GBL_RESULT (*GblTypeClassInitializeFn)      (GblClass*, const void*, GblContext*);      ///< \details Function type used as a GblType's GblClass initializer
typedef GBL_RESULT (*GblTypeClassFinalizeFn)        (GblClass*, const void*, GblContext*);      ///< \details Function type used as a GblType's GblClass finalizer
typedef GBL_RESULT (*GblTypeInstanceInitializeFn)   (GblInstance*, GblContext*);                ///< \details Function type used as a GblType's GblInstance initializer
///@}

/// \details Maps a GblInterface to a GblClass by using it as a member of the class at the specified offset.
typedef struct GblTypeInterfaceMapEntry {
    GblType      interfaceType; ///< GblType of the mapped interface
    uint16_t     classOffset;   ///< offset of GblInterface into GblClass (using offsetof())
} GblTypeInterfaceMapEntry;

/// \details Provides type information when registering a new GblType
typedef struct GblTypeInfo {
    GblTypeClassInitializeFn        pFnClassInit;           ///< Function used to initialize the values a GblType's associated GblClass
    GblTypeClassFinalizeFn          pFnClassFinal;          ///< Function used to finalize the values of a GblType's associated GblClass
    uint16_t                        classSize;              ///< Size of a GblType's associated GblClass structure
    uint16_t                        classPrivateSize;       ///< Size of extra private storage to be associated with a GblType's GblClass
    const void*                     pClassData;             ///< Size of extra private storage to be associated with a GblType's GblClass
    uint8_t                         interfaceCount;         ///< Number of GblInterface mappings in pInterfaceMap array
    const GblTypeInterfaceMapEntry* pInterfaceMap;          ///< Array providing mappings for each GblInterface contained within a GblType's GblClass
    uint8_t                         dependencyCount;        ///< Number of dependent GblTypes in pDependencies array
    const GblType*                  pDependencies;          ///< Array providing a list of type dependencies that must be implemented a GblType
    GblTypeInstanceInitializeFn     pFnInstanceInit;        ///< Function used to initialize the values a GblType's associated GblInstance
    uint16_t                        instanceSize;           ///< Size of a GblType's associated GblInstance structure
    uint16_t                        instancePrivateSize;    ///< Size of extra private storage to be associated with a GblType's GblInstance.
} GblTypeInfo;

/// \details Initializes the type system with the given default parameters. This function is called automatically when the API is used.

/// \details Finalizes the type system and frees resources, should be called upon program exit
GBL_EXPORT GBL_RESULT           GblType_final           (void)                          GBL_NOEXCEPT;

/// \details Registers a new GblType with the type system whose GblTypeInfo is provided up-front
GBL_EXPORT GblType              GblType_registerStatic  (const char*          pName,
                                                         GblType              baseType,
                                                         const GblTypeInfo*   pInfo,
                                                         GblFlags             flags)    GBL_NOEXCEPT;
/// \details Registers a new GblType with the type system whose GblTypeInfo is queried for dynamically
GBL_EXPORT GblType              GblType_registerDynamic (const char*          pName,
                                                         GblType              parent,
                                                         GblIPlugin*          pPlugin,
                                                         GblFlags             flags)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblType_unregister      (GblType type)                  GBL_NOEXCEPT;

GBL_EXPORT GblSize              GblType_registeredCount (void)                          GBL_NOEXCEPT;
GBL_EXPORT GblSize              GblType_builtinCount    (void)                          GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_nextRegistered  (GblType previousType)          GBL_NOEXCEPT;

GBL_EXPORT GblType              GblType_fromClass       (const GblClass* pClass)        GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_fromBuiltinIndex(GblSize index)                 GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_fromName        (const char* pName)             GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_fromNameQuark   (GblQuark quark)                GBL_NOEXCEPT;

GBL_EXPORT const char*          GblType_name            (SELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblQuark             GblType_nameQuark       (SELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_parent          (SELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_root            (SELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_base            (SELF, GblSize depth)           GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_ancestor        (SELF, GblSize level)           GBL_NOEXCEPT;
GBL_EXPORT GblSize              GblType_depth           (SELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_flagsTest       (SELF, GblFlags mask)           GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_verify          (SELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_check           (SELF, GblType other)           GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_depends         (SELF, GblType dependency)      GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_derives         (SELF, GblType superType)       GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_implements      (SELF, GblType superType)       GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_common          (SELF, GblType other)           GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_maps            (SELF, GblType iface)           GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_conforms        (SELF, GblType dependent)       GBL_NOEXCEPT;
GBL_EXPORT const GblTypeInfo*   GblType_info            (SELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblIPlugin*          GblType_plugin          (SELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblRefCount          GblType_classRefCount   (SELF)                          GBL_NOEXCEPT;
GBL_EXPORT GblRefCount          GblType_instanceRefCount(SELF)                          GBL_NOEXCEPT;

GBL_DECLS_END

#undef SELF


#endif // GIMBAL_TYPE_H
