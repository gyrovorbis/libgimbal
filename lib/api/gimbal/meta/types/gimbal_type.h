/*! \file
 *  \brief   GblType UUID, meta type registration, and API
 *  \ingroup meta
 *
 *  This file provides the lowest-level entry-point
 *  and API into the meta type system, built around
 *  GblType, a UUID which is returned after registering
 *  a type.
 *
 *  To register a type with the type system and get
 *  a UUID for it, simply use GblType_register().
 *
 *  UUIDs are automatically made available for all
 *  builtin types.
 *
 *  \todo
 *      - GblType_next does not iterate over builtin types
 *          - GblType_foreach() is probably better!
 *      - GBL_PROTCOL_TYPE solidification and testing
 *
 *  \author    2023 Falco Girgis
 *  \copyright MIT License
 */
#ifndef GIMBAL_TYPE_H
#define GIMBAL_TYPE_H

#include "../../strings/gimbal_quark.h"
#include "gimbal_builtin_types.h"

//! GblType UUID of the invalid type
#define GBL_INVALID_TYPE ((GblType)0)

#define GBL_SELF_TYPE    GblType

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblClass);
GBL_FORWARD_DECLARE_STRUCT(GblInstance);
GBL_FORWARD_DECLARE_STRUCT(GblInterface);

/*! \brief   Meta Type UUID
 *  \ingroup meta
 *
 *  GblType is an integral-type representing a
 *  unique identifier for every type which has
 *  been registered with the type system, or
 *  GBL_INVALID_TYPE as an initial, invalid value.
 *
 *  \sa GblType_register()
 */
typedef uintptr_t GblType;

typedef GBL_RESULT (*GblClassInitFn)   (GblClass*, const void*); //!< Function type used as a GblType's GblClass initializer
typedef GBL_RESULT (*GblClassFinalFn)  (GblClass*, const void*); //!< Function type used as a GblType's GblClass finalizer
typedef GBL_RESULT (*GblInstanceInitFn)(GblInstance*);           //!< Function type used as a GblType's GblInstance initializer

//! Flags controlling behavior of GblTypes. These can be set at any level in a type heirarchy.
typedef enum GblTypeFlags {
    GBL_TYPE_FLAGS_NONE           = 0,         //!< Type which adds no additional flags beyond what's inherited
    GBL_TYPE_FLAG_BUILTIN         = (1 << 6),  //!< Type was automatically registered as a builtin type (do not use on a new type)
    GBL_TYPE_FLAG_TYPEINFO_STATIC = (1 << 7),  //!< Type's GblTypeInfo specified upon registration is static, so no internal storage has to be allocated for storing a copy
    GBL_TYPE_FLAG_CLASS_PINNED    = (1 << 8),  //!< Type's GblClass is never destroyed and remains persistent upon construction. The default behavior is to create/destroy as referenced.
    GBL_TYPE_FLAG_CLASS_PREINIT   = (1 << 9),  //!< Type's GblClass should be constructed immediately, rather than lazily upon use by default. Also implies class pinning.
    GBL_TYPE_FLAG_UNMAPPABLE      = (1 << 10), //!< Cannot obtain Type's associated GblInterface from a GblClass it has been mapped to. Disambiguates casting with common interface base classes.
    GBL_TYPE_FLAG_INCOMPLETE      = (1 << 11), //!< Incomplete/partial type missing some type dependency
    GBL_TYPE_FLAG_ABSTRACT        = (1 << 12), //!< Type cannot be instantiated without being derived
    GBL_TYPE_FLAG_FINAL           = (1 << 13), //!< Type cannot be derived from
    GBL_TYPE_FLAGS_MASK           = 0xffffffc0 //!< Mask of all user type flags
} GblTypeFlags;

//! Provides implementation details of a GblInterface for a type
typedef struct GblInterfaceImpl {
    GblType interfaceType; //!< GblType of the implemented GblInterface
    size_t  classOffset;   //!< offset of the GblInterface into GblClass (using offsetof())
} GblInterfaceImpl;

//! Provides type information when registering a new GblType
typedef struct GblTypeInfo {
    GblClassInitFn          pFnClassInit;        //!< Function used to initialize the values a GblType's associated GblClass
    GblClassFinalFn         pFnClassFinal;       //!< Function used to finalize the values of a GblType's associated GblClass
    size_t                  classSize;           //!< Size of a GblType's associated GblClass structure
    size_t                  classPrivateSize;    //!< Size of extra private storage to be associated with a GblType's GblClass
    const void*             pClassData;          //!< Size of extra private storage to be associated with a GblType's GblClass
    uint8_t                 interfaceCount;      //!< Number of GblInterface mappings in pInterfaceImpls array
    const GblInterfaceImpl* pInterfaceImpls;     //!< Array providing information for each interface implemented by the type
    uint8_t                 dependencyCount;     //!< Number of dependent GblTypes in pDependencies array
    const GblType*          pDependencies;       //!< Array providing a list of type dependencies that must be implemented a GblType
    GblInstanceInitFn       pFnInstanceInit;     //!< Function used to initialize the values a GblType's associated GblInstance
    size_t                  instanceSize;        //!< Size of a GblType's associated GblInstance structure
    size_t                  instancePrivateSize; //!< Size of extra private storage to be associated with a GblType's GblInstance.
} GblTypeInfo;

/*! \name  Registry
 *  \brief Methods for managing the type registry
 *  @{
 */
//! Registers a new type with the given information, returning a unique identifier for it
GBL_EXPORT GblType    GblType_register   (const char*        pName,
                                          GblType            baseType,
                                          const GblTypeInfo* pInfo,
                                          GblFlags           flags) GBL_NOEXCEPT;
//! Unregisters a type from the type system. Typically not done, better not be used.
GBL_EXPORT GBL_RESULT GblType_unregister (GblType type)             GBL_NOEXCEPT;
//! Returns the total number of types registered to the type system
GBL_EXPORT size_t     GblType_count      (void)                     GBL_NOEXCEPT;
//! Iterates over the registry by passing a previously returned type or GBL_TYPE_INVALID initially
GBL_EXPORT GblType    GblType_next       (GblType previousType)     GBL_NOEXCEPT;
//! @}

/*! \name  Querying
 *  \brief Methods for looking up or querying types
 *  @{
 */
//! Queries the internal type registry for a GblType by its type name
GBL_EXPORT GblType GblType_find      (const char* pName) GBL_NOEXCEPT;
//! Queries the registry for a GblType using a GblQuark of its type name (faster)
GBL_EXPORT GblType GblType_findQuark (GblQuark quark)    GBL_NOEXCEPT;
//! @}

/*! \name  Attributes
 *  \brief Accessor methods for getting a type's information
 *  @{
 */
//! Returns the type name string associated with the given GblType
GBL_EXPORT const char*        GblType_name      (GBL_VSELF) GBL_NOEXCEPT;
//! Returns the type name quark associated with the given GblType
GBL_EXPORT GblQuark           GblType_nameQuark (GBL_VSELF) GBL_NOEXCEPT;
//! Returns the given type's parent's GblType or GBL_INVALID_TYPE if it's a root type
GBL_EXPORT GblType            GblType_parent    (GBL_VSELF) GBL_NOEXCEPT;
//! Returns the GblTypeInfo struct for the given type, containing its meta information
GBL_EXPORT const GblTypeInfo* GblType_info      (GBL_VSELF) GBL_NOEXCEPT;
//! Returns the combined flags for a given type, including flags it has inherited
GBL_EXPORT GblFlags           GblType_flags     (GBL_VSELF) GBL_NOEXCEPT;
//! @}

/*! \name  Resource Tracking
 *  \brief Methods for querying a type's active resource counters
 *  @{
 */
//! Returns the reference counter for the given GblType's internally-managed default GblClass
GBL_EXPORT GblRefCount GblType_classRefCount (GBL_VSELF) GBL_NOEXCEPT;
//! Returns the number of active instances of the given GblType
GBL_EXPORT GblRefCount GblType_instanceCount (GBL_VSELF) GBL_NOEXCEPT;
//! @}

/*! \name  Hierarchy
 *  \brief Methods for querying a type's hierarchy
 *  @{
 */
//! Returns the root parent GblType of the given GblType
GBL_EXPORT GblType GblType_root     (GBL_VSELF)               GBL_NOEXCEPT;
//! Returns the base GblType of the given GblType, where a \p depth of 0 is its root
GBL_EXPORT GblType GblType_base     (GBL_VSELF, size_t depth) GBL_NOEXCEPT;
//! Similar to GblType_base(), but in the reverse direction. \p level 0 is identity.
GBL_EXPORT GblType GblType_ancestor (GBL_VSELF, size_t level) GBL_NOEXCEPT;
//! Returns the depth of the given GblType, where a depth of 0 means it's a root type
GBL_EXPORT size_t  GblType_depth    (GBL_VSELF)               GBL_NOEXCEPT;
//! @}

/*! \name  Type Checking
 *  \brief Methods performing type system queries on a type
 *  @{
 */
//! Returns GBL_TRUE if the given type is a valid, registred type
GBL_EXPORT GblBool GblType_verify     (GBL_VSELF)                     GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given type can be safely cast to the \p other type
GBL_EXPORT GblBool GblType_check      (GBL_VSELF, GblType other)      GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given type depends on the \p dependency type
GBL_EXPORT GblBool GblType_depends    (GBL_VSELF, GblType dependency) GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given type is a subtype, inheriting from \p superType
GBL_EXPORT GblBool GblType_derives    (GBL_VSELF, GblType superType)  GBL_NOEXCEPT;
//! Returns GBL_TRUE if the given type has implemented \p ifaceType
GBL_EXPORT GblBool GblType_implements (GBL_VSELF, GblType ifaceType)  GBL_NOEXCEPT;
//! Returns the most-derived, deepest common-type between the given type and \p other
GBL_EXPORT GblType GblType_common     (GBL_VSELF, GblType other)      GBL_NOEXCEPT;
//! Returns true if the given type's class contains an implementation of the \p iface interface
GBL_EXPORT GblBool GblType_maps       (GBL_VSELF, GblType iface)      GBL_NOEXCEPT;
//! Returns true if the given type meets all type dependencies for the \p dependent type
GBL_EXPORT GblBool GblType_conforms   (GBL_VSELF, GblType dependent)  GBL_NOEXCEPT;
//! @}

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_TYPE_H
