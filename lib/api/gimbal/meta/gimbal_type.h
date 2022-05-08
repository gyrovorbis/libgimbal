#ifndef GIMBAL_TYPE_H
#define GIMBAL_TYPE_H

#include "../types/gimbal_typedefs.h"
#include "gimbal_builtin_types.h"

#define GBL_INVALID_TYPE                    ((GblType)0)

#define GBL_TYPE_IS_CLASSED(type)           (GblType_flagsTest(type, GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED))
#define GBL_TYPE_IS_INSTANTIABLE(type)      (GblType_flagsTest(type, GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE))
#define GBL_TYPE_IS_DERIVABLE(type)         (GblType_flagsTest(type, GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE))
#define GBL_TYPE_IS_DEEP_DERIVABLE(type)    (GblType_flagsTest(type, GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE))
#define GBL_TYPE_IS_BUILTIN(type)           (GblType_flagsTest(type, GBL_TYPE_FLAG_BUILITN))
#define GBL_TYPE_IS_ABSTRACT(type)          (GblType_flagsTest(type, GBL_TYPE_FLAG_ABSTRACT))
#define GBL_TYPE_IS_FINAL(type)             (GblType_flagsTest(type, GBL_TYPE_FLAG_FINAL))
#define GBL_TYPE_IS_FUNDAMENTAL(type)       (type == GblType_fundamental(type))
#define GBL_TYPE_IS_INTERFACE(type)         (GblType_fundamental(type) == GBL_INTERFACE_TYPE)
#define GBL_TYPE_IS_ROOT(type)              (GblType_base(type, 0) == type)
//#define GBL_TYPE_IS_PROTOCOL
#define GBL_TYPE_IS_VALUE(type)             (!GblType_flagsTest(type, GBL_TYPE_FLAG_FUNDAMENTAL_FLAG_INSTANTIABLE) \
                                               && GblType_check(type, GBL_IVARIANT_TYPE))
#define SELF    GblType type

GBL_DECLS_BEGIN

// wrap using DSL
GBL_DECLARE_FLAGS(GblType_fundamentalFlags) {
    GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED           = (1 << 0),
    GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE      = (1 << 1),
    GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE         = (1 << 2),
    GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE    = (1 << 3),
    GBL_TYPE_FUNDAMENTAL_FLAGS_MASK             = GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED         |
                                                  GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE    |
                                                  GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE       |
                                                  GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE
};

GBL_DECLARE_FLAGS(GblTypeFlags) {
    GBL_TYPE_FLAGS_NONE                         = 0,
    GBL_TYPE_FLAG_BUILTIN                       = (1 << 4),
    GBL_TYPE_FLAG_ABSTRACT                      = (1 << 5),
    GBL_TYPE_FLAG_FINAL                         = (1 << 6),
    GBL_TYPE_FLAGS_MASK                         = GBL_TYPE_FLAG_BUILTIN  |
                                                  GBL_TYPE_FLAG_ABSTRACT |
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
    const void*                     pClassData;
    uint16_t                        interfaceCount;
    const GblTypeInterfaceMapEntry* pInterfaceMap;
    uint16_t                        prerequesiteCount;
    GblType*                        pPrerequesites;
    GblTypeInstanceInitializeFn     pFnInstanceInit;
    uint16_t                        instanceSize;
} GblTypeInfo;

GBL_EXPORT GBL_RESULT           GblType_init            (GblContext* pDefaultCtx,
                                                         GblSize     typeBuiltinInitialCount,
                                                         GblSize     typeTotalInitialCount) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           GblType_final           (void)                              GBL_NOEXCEPT;
GBL_EXPORT GblContext*          GblType_contextDefault  (void)                              GBL_NOEXCEPT;

GBL_EXPORT GblType              GblType_registerStatic  (GblType              parent,
                                                         const char*          pName,
                                                         const GblTypeInfo*   pInfo,
                                                         GblFlags             flags)        GBL_NOEXCEPT;
GBL_EXPORT GblType              GblType_registerBuiltin (GblSize              expectedIndex,
                                                         GblType              parentType,
                                                         const char*          pName,
                                                         const GblTypeInfo*   pTypeInfo,
                                                         GblTypeFlags         flags)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           GblType_unregister      (GblType type)                      GBL_NOEXCEPT;
GBL_EXPORT GblSize              GblType_registeredCount (void)                              GBL_NOEXCEPT;
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
GBL_EXPORT GblContext*          GblType_context         (SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_check           (SELF, GblType other)               GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_subsumes        (SELF, GblType superType)           GBL_NOEXCEPT;
GBL_EXPORT GblBool              GblType_implements      (SELF, GblType iface)               GBL_NOEXCEPT;
//GBL_EXPORT GblBool            GblType_conforms        (SELF, GblType protocol)            GBL_NOEXCEPT;
GBL_EXPORT const GblTypeInfo*   GblType_info            (SELF)                              GBL_NOEXCEPT;
GBL_EXPORT GblIPlugin*          GblType_plugin          (SELF)                              GBL_NOEXCEPT;

GBL_DECLS_END

#undef SELF

#endif // GIMBAL_TYPE_H
