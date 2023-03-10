/*! \file
 *  \brief ::GblType identifier and related functions
 *  \ingroup meta
 */
#ifndef GIMBAL_TYPE_H
#define GIMBAL_TYPE_H

#include "../../core/gimbal_typedefs.h"
#include "../../strings/gimbal_quark.h"
#include "gimbal_builtin_types.h"

/*! UUID of an invalid type
 * \ingroup metaBuiltinTypes
 */
#define GBL_INVALID_TYPE ((GblType)0)
#define GBL_SELF_TYPE    GblType

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblClass);
GBL_FORWARD_DECLARE_STRUCT(GblInstance);
GBL_FORWARD_DECLARE_STRUCT(GblInterface);
GBL_FORWARD_DECLARE_STRUCT(GblIPlugin);

/*! \brief Type representing meta type UUIDs
 *  \ingroup meta
 */
typedef uintptr_t                                   GblType;

typedef GBL_RESULT (*GblTypeClassInitializeFn)      (GblClass*, const void*, GblContext*);      ///< \details Function type used as a GblType's GblClass initializer
typedef GBL_RESULT (*GblTypeClassFinalizeFn)        (GblClass*, const void*, GblContext*);      ///< \details Function type used as a GblType's GblClass finalizer
typedef GBL_RESULT (*GblTypeInstanceInitializeFn)   (GblInstance*, GblContext*);                ///< \details Function type used as a GblType's GblInstance initializer

/// \details Flags controlling behavior of GblTypes. These can be set at any level in a type heirarchy.
typedef enum GblTypeFlags {
    GBL_TYPE_FLAGS_NONE             = 0,
    GBL_TYPE_FLAG_BUILTIN           = (1 << 6),             ///< Type was automatically registered as a builtin type
    GBL_TYPE_FLAG_TYPEINFO_STATIC   = (1 << 7),             ///< Type's GblTypeInfo specified upon registration is static, so no internal storage has to be allocated for storing a copy
    GBL_TYPE_FLAG_CLASS_PINNED      = (1 << 8),             ///< Type's GblClass is never destroyed and remains persistent upon construction. The default behavior is to create/destroy as referenced.
    GBL_TYPE_FLAG_CLASS_PREINIT     = (1 << 9),             ///< Type's GblClass should be constructed immediately, rather than lazily upon use by default. Also implies class pinning.
    GBL_TYPE_FLAG_UNMAPPABLE        = (1 << 10),            ///< Cannot obtain Type's associated GblInterface from a GblClass it has been mapped to. Disambiguates casting with common interface base classes.
    GBL_TYPE_FLAG_INCOMPLETE        = (1 << 11),            ///< Incomplete/partial type missing some type dependency
    GBL_TYPE_FLAG_ABSTRACT          = (1 << 12),            ///< Type cannot be instantiated without being derived
    GBL_TYPE_FLAG_FINAL             = (1 << 13),            ///< Type cannot be derived from
    GBL_TYPE_FLAGS_MASK             = 0xffffffc0
} GblTypeFlags;
///@}

/// \details Maps a GblInterface to a GblClass by using it as a member of the class at the specified offset.
typedef struct GblTypeInterfaceMapEntry {
    GblType      interfaceType; ///< GblType of the mapped interface
    GblSize      classOffset;   ///< offset of GblInterface into GblClass (using offsetof())
} GblTypeInterfaceMapEntry;

/// \details Provides type information when registering a new GblType
typedef struct GblTypeInfo {
    GblTypeClassInitializeFn        pFnClassInit;           ///< Function used to initialize the values a GblType's associated GblClass
    GblTypeClassFinalizeFn          pFnClassFinal;          ///< Function used to finalize the values of a GblType's associated GblClass
    GblSize                         classSize;              ///< Size of a GblType's associated GblClass structure
    GblSize                         classPrivateSize;       ///< Size of extra private storage to be associated with a GblType's GblClass
    const void*                     pClassData;             ///< Size of extra private storage to be associated with a GblType's GblClass
    uint8_t                         interfaceCount;         ///< Number of GblInterface mappings in pInterfaceMap array
    const GblTypeInterfaceMapEntry* pInterfaceMap;          ///< Array providing mappings for each GblInterface contained within a GblType's GblClass
    uint8_t                         dependencyCount;        ///< Number of dependent GblTypes in pDependencies array
    const GblType*                  pDependencies;          ///< Array providing a list of type dependencies that must be implemented a GblType
    GblTypeInstanceInitializeFn     pFnInstanceInit;        ///< Function used to initialize the values a GblType's associated GblInstance
    GblSize                         instanceSize;           ///< Size of a GblType's associated GblInstance structure
    GblSize                         instancePrivateSize;    ///< Size of extra private storage to be associated with a GblType's GblInstance.
} GblTypeInfo;

GBL_EXPORT GblType            GblType_registerStatic   (const char*          pName,
                                                        GblType              baseType,
                                                        const GblTypeInfo*   pInfo,
                                                        GblFlags             flags)    GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT         GblType_unregister       (GblType type)                  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT         GblType_addExtension     (GblType type,
                                                        GblInterface* pIFace)          GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT         GblType_removeExtension  (GblType type, GblType iface)   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT         GblType_final            (void)                          GBL_NOEXCEPT;

GBL_EXPORT GblSize            GblType_registeredCount  (void)                          GBL_NOEXCEPT;
GBL_EXPORT GblSize            GblType_builtinCount     (void)                          GBL_NOEXCEPT;
GBL_EXPORT GblType            GblType_nextRegistered   (GblType previousType)          GBL_NOEXCEPT;

GBL_EXPORT GblType            GblType_fromName         (const char* pName)             GBL_NOEXCEPT;
GBL_EXPORT GblType            GblType_fromNameQuark    (GblQuark quark)                GBL_NOEXCEPT;
GBL_EXPORT GblType            GblType_fromBuiltinIndex (GblSize index)                 GBL_NOEXCEPT;

GBL_EXPORT const char*        GblType_name             (GBL_VSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblQuark           GblType_nameQuark        (GBL_VSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblType            GblType_parent           (GBL_VSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblType            GblType_root             (GBL_VSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblType            GblType_base             (GBL_VSELF, GblSize depth)      GBL_NOEXCEPT;
GBL_EXPORT GblType            GblType_ancestor         (GBL_VSELF, GblSize level)      GBL_NOEXCEPT;
GBL_EXPORT GblSize            GblType_depth            (GBL_VSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblBool            GblType_flagsCheck       (GBL_VSELF, GblFlags mask)      GBL_NOEXCEPT;
GBL_EXPORT GblBool            GblType_verify           (GBL_VSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblBool            GblType_check            (GBL_VSELF, GblType other)      GBL_NOEXCEPT;
GBL_EXPORT GblBool            GblType_depends          (GBL_VSELF, GblType dependency) GBL_NOEXCEPT;
GBL_EXPORT GblBool            GblType_derives          (GBL_VSELF, GblType superType)  GBL_NOEXCEPT;
GBL_EXPORT GblBool            GblType_implements       (GBL_VSELF, GblType superType)  GBL_NOEXCEPT;
GBL_EXPORT GblType            GblType_common           (GBL_VSELF, GblType other)      GBL_NOEXCEPT;
GBL_EXPORT GblBool            GblType_maps             (GBL_VSELF, GblType iface)      GBL_NOEXCEPT;
GBL_EXPORT GblBool            GblType_conforms         (GBL_VSELF, GblType dependent)  GBL_NOEXCEPT;
GBL_EXPORT const GblTypeInfo* GblType_info             (GBL_VSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblIPlugin*        GblType_plugin           (GBL_VSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblInterface*      GblType_extension        (GBL_VSELF, GblType iface)      GBL_NOEXCEPT;
GBL_EXPORT GblRefCount        GblType_classRefCount    (GBL_VSELF)                     GBL_NOEXCEPT;
GBL_EXPORT GblRefCount        GblType_instanceRefCount (GBL_VSELF)                     GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE


#endif // GIMBAL_TYPE_H
