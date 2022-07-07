#ifndef GIMBAL_TYPE_H
#define GIMBAL_TYPE_H

#include "../types/gimbal_typedefs.h"
#include "../strings/gimbal_quark.h"
#include "gimbal_builtin_types.h"

typedef uintptr_t                           GblType;

#define GBL_INVALID_TYPE                    ((GblType)0)

#define GBL_TYPE_IS_DEPENDENT(type)         (GblType_flagsTest(type, GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT))
#define GBL_TYPE_IS_CLASSED(type)           (GblType_flagsTest(type, GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED))
#define GBL_TYPE_IS_INTERFACED(type)        (GblType_flagsTest(type, GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED))
#define GBL_TYPE_IS_INSTANTIABLE(type)      (GblType_flagsTest(type, GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE))
#define GBL_TYPE_IS_DERIVABLE(type)         (GblType_flagsTest(type, GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE))
#define GBL_TYPE_IS_DEEP_DERIVABLE(type)    (GblType_flagsTest(type, GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE))
#define GBL_TYPE_IS_BUILTIN(type)           (GblType_flagsTest(type, GBL_TYPE_FLAG_BUILTIN))
#define GBL_TYPE_IS_ABSTRACT(type)          (GblType_flagsTest(type, GBL_TYPE_FLAG_ABSTRACT))
#define GBL_TYPE_IS_FINAL(type)             (GblType_flagsTest(type, GBL_TYPE_FLAG_FINAL))
#define GBL_TYPE_IS_FUNDAMENTAL(type)       (type == GblType_fundamental(type))
#define GBL_TYPE_IS_ROOT(type)              (GblType_base(type, 0) == type)
#define GBL_TYPE_IS_VALID(type)             (GblType_verify(type))
#define GBL_TYPE_IS_VALUE(type)             (!GblType_flagsTest(type, GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE) \
                                               && GblType_check(type, GBL_BUILTIN_TYPE(IVARIANT)))
#define SELF    GblType type

GBL_DECLS_BEGIN

GBL_DECLARE_FLAGS(GblTypeFundamentalFlags) {
    GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT         = (1 << 0),
    GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED           = (1 << 1),
    GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED        = (1 << 2),
    GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE      = (1 << 3),
    GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE         = (1 << 4),
    GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE    = (1 << 5),
    GBL_TYPE_FUNDAMENTAL_FLAGS_MASK             = GBL_TYPE_FUNDAMENTAL_FLAG_DEPENDENT       |
                                                  GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED         |
                                                  GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE    |
                                                  GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE       |
                                                  GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE  |
                                                  GBL_TYPE_FUNDAMENTAL_FLAG_INTERFACED
};

GBL_DECLARE_FLAGS(GblTypeFlags) {
    GBL_TYPE_FLAGS_NONE                         = 0,
    GBL_TYPE_FLAG_BUILTIN                       = (1 << 6),
    GBL_TYPE_FLAG_TYPEINFO_STATIC               = (1 << 7),
    GBL_TYPE_FLAG_CLASS_PINNED                  = (1 << 8),
    GBL_TYPE_FLAG_CLASS_PREINIT                 = (1 << 9),
    GBL_TYPE_FLAG_UNMAPPABLE                    = (1 << 10),
    GBL_TYPE_FLAG_ABSTRACT                      = (1 << 11),
    GBL_TYPE_FLAG_FINAL                         = (1 << 12),
    GBL_TYPE_FLAGS_MASK                         = GBL_TYPE_FLAG_BUILTIN                     |
                                                  GBL_TYPE_FLAG_TYPEINFO_STATIC             |
                                                  GBL_TYPE_FLAG_CLASS_PINNED                |
                                                  GBL_TYPE_FLAG_CLASS_PREINIT               |
                                                  GBL_TYPE_FLAG_UNMAPPABLE                  |
                                                  GBL_TYPE_FLAG_ABSTRACT                    |
                                                  GBL_TYPE_FLAG_FINAL
};

typedef GBL_RESULT (*GblTypeClassInitializeFn)      (GblClass*, const void*, GblContext*);
typedef GBL_RESULT (*GblTypeClassFinalizeFn)        (GblClass*, const void*, GblContext*);
typedef GBL_RESULT (*GblTypeInstanceInitializeFn)   (GblInstance*, GblContext*);

typedef struct GblTypeInterfaceMapEntry {
    GblType      interfaceType;
    uint16_t     classOffset;
} GblTypeInterfaceMapEntry;

typedef struct GblTypeInfo {
    GblTypeClassInitializeFn        pFnClassInit;
    GblTypeClassFinalizeFn          pFnClassFinal;
    uint16_t                        classSize;
    uint16_t                        classPrivateSize;
    const void*                     pClassData;
    uint8_t                         interfaceCount;
    const GblTypeInterfaceMapEntry* pInterfaceMap;
    uint8_t                         dependencyCount;
    const GblType*                  pDependencies;
    GblTypeInstanceInitializeFn     pFnInstanceInit;
    uint16_t                        instanceSize;
    uint16_t                        instancePrivateSize;
} GblTypeInfo;

GBL_EXPORT GBL_RESULT           GblType_init            (GblContext* pDefaultCtx,
                                                         GblSize     typeBuiltinInitialCount,
                                                         GblSize     typeTotalInitialCount) GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblType_final           (void)                              GBL_NOEXCEPT;
GBL_EXPORT GblContext*          GblType_contextDefault  (void)                              GBL_NOEXCEPT;

GBL_EXPORT GblType              GblType_registerStatic  (GblType              superType,
                                                         const char*          pName,
                                                         const GblTypeInfo*   pInfo,
                                                         GblFlags             flags)        GBL_NOEXCEPT;

GBL_EXPORT GblType              GblType_registerDynamic (GblType              parent,
                                                         const char*          pName,
                                                         GblIPlugin*          pPlugin,
                                                         GblFlags             flags)        GBL_NOEXCEPT;

GBL_EXPORT GblType              GblType_registerBuiltin (GblSize              expectedIndex,
                                                         GblType              parentType,
                                                         const char*          pName,
                                                         const GblTypeInfo*   pTypeInfo,
                                                         GblTypeFlags         flags)        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT           GblType_unregister      (GblType type)                      GBL_NOEXCEPT;
GBL_EXPORT GblSize              GblType_registeredCount (void)                              GBL_NOEXCEPT;
GBL_EXPORT GblSize              GblType_builtinCount    (void)                              GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_nextRegistered  (GblType previousType)              GBL_NOEXCEPT;

GBL_EXPORT GblType              GblType_fromClass       (const GblClass* pClass)            GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_fromBuiltinIndex(GblUint index)                     GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_fromName        (const char* pName)                 GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_fromNameQuark   (GblQuark quark)                    GBL_NOEXCEPT;

GBL_EXPORT const char*          GblType_name            (SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblQuark             GblType_nameQuark       (SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_parent          (SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_fundamental     (SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_base            (SELF, GblUint depth)               GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_ancestor        (SELF, GblUint level)               GBL_NOEXCEPT;
GBL_EXPORT GblUint              GblType_depth           (SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_flagsTest       (SELF, GblFlags mask)               GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_verify          (SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_check           (SELF, GblType other)               GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_depends         (SELF, GblType dependency)          GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_derives         (SELF, GblType superType)           GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_implements      (SELF, GblType superType)           GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_common          (SELF, GblType other)               GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_maps            (SELF, GblType iface)               GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_conforms        (SELF, GblType dependent)           GBL_NOEXCEPT;
GBL_EXPORT const GblTypeInfo*   GblType_info            (SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblIPlugin*          GblType_plugin          (SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblRefCount          GblType_classRefCount   (SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblRefCount          GblType_instanceRefCount(SELF)                              GBL_NOEXCEPT;

GBL_DECLS_END

#undef SELF

#endif // GIMBAL_TYPE_H
