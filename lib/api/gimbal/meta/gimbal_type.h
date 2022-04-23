#ifndef GIMBAL_TYPE_H
#define GIMBAL_TYPE_H

#include "../types/gimbal_typedefs.h"

#define GBL_TYPE_INVALID                    ((GblType)0)
#define GBL_TYPE_INTERFACE                  (gblTypeBuiltin(0))
#define GBL_TYPE_IVARIANT                   (gblTypeBuiltin(1))
#define GBL_TYPE_NIL                        (gblTypeBuiltin(2))
#define GBL_TYPE_BOOL                       (gblTypeBuiltin(3))
#define GBL_TYPE_CHAR                       (gblTypeBuiltin(4))
#define GBL_TYPE_UINT8                      (gblTypeBuiltin(5))
#define GBL_TYPE_INT16                      (gblTypeBuiltin(6))
#define GBL_TYPE_UINT16                     (gblTypeBuiltin(7))
#define GBL_TYPE_INT32                      (gblTypeBuiltin(8))
#define GBL_TYPE_UINT32                     (gblTypeBuiltin(9))
#define GBL_TYPE_INT64                      (gblTypeBuiltin(10))
#define GBL_TYPE_UINT64                     (gblTypeBuiltin(11))
#define GBL_TYPE_ENUM                       (gblTypeBuiltin(12))
#define GBL_TYPE_FLAGS                      (gblTypeBuiltin(13))
#define GBL_TYPE_FLOAT                      (gblTypeBuiltin(14))
#define GBL_TYPE_DOUBLE                     (gblTypeBuiltin(15))
#define GBL_TYPE_POINTER                    (gblTypeBuiltin(16))
#define GBL_TYPE_STRING                     (gblTypeBuiltin(17))
#define GBL_TYPE_TYPE                       (gblTypeBuiltin(18))
#define GBL_TYPE_BOXED                      (gblTypeBuiltin(19))
#define GBL_TYPE_ITABLE                     (gblTypeBuiltin(20))
#define GBL_TYPE_IEVENT_HANDLER             (gblTypeBuiltin(21))
#define GBL_TYPE_IEVENT_FILTER              (gblTypeBuiltin(22))
#define GBL_TYPE_OBJECT                     (gblTypeBuiltin(23))
#define GBL_TYPE_IALLOCATOR                 (gblTypeBuiltin(24))
#define GBL_TYPE_ILOGGER                    (gblTypeBuiltin(25))
#define GBL_TYPE_CONTEXT                    (gblTypeBuiltin(26))
#define GBL_TYPE_EVENT                      (gblTypeBuiltin(27))

#define	GBL_TYPE_VARIANT                    GBL_TYPE_MAKE_FUNDAMENTAL(20)

#define GBL_TYPE_BUILTIN_COUNT              28

#define GBL_TYPE_FLAGS_TEST(type, flags)    (gblTypeFlagsTest(type, flags))
#define GBL_TYPE_IS_CLASSED(type)           (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED))
#define GBL_TYPE_IS_INSTANTIABLE(type)      (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE))
#define GBL_TYPE_IS_DERIVABLE(type)         (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE))
#define GBL_TYPE_IS_DEEP_DERIVABLE(type)    (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE))
#define GBL_TYPE_IS_BUILTIN(type)           (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FLAG_BUILITN))
#define GBL_TYPE_IS_ABSTRACT(type)          (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FLAG_ABSTRACT))
#define GBL_TYPE_IS_FINAL(type)             (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FLAG_FINAL))
#define GBL_TYPE_IS_FUNDAMENTAL(type)       (type == gblTypeFundamental(type))
#define GBL_TYPE_IS_INTERFACE(type)         (gblTypeFundamental(type) == GBL_TYPE_INTERFACE)
#define GBL_TYPE_IS_ROOT(type)              (gblTypeBase(type, 0) == type)

#define GBL_TYPE_CAST_INSTANCE(instance, toType, cType)         \
                                            ((cType*)gblTypeInstanceCast((GblInstance*)instance, toType))
#define GBL_TYPE_CAST_INSTANCE_PREFIX(instance, typePrefix)     \
                                            GBL_TYPE_CAST_INSTANCE(instance, typePrefix##_TYPE, typePrefix##_STRUCT)
#define GBL_TYPE_CHECK_INSTANCE(instance, toType)               \
                                            (gblTypeInstanceCheck((GblInstance*)instance, toType))
#define GBL_TYPE_CHECK_INSTANCE_PREFIX(instance, typePrefix)    \
                                            (GBL_TYPE_CHECK_INSTANCE(instance, typePrefix##_TYPE))
#define GBL_TYPE_CAST_CLASS(klass, toType, cType)               \
                                            ((cType*)gblTypeClassCast((GblClass*)klass, toType))
#define GBL_TYPE_CAST_CLASS_PREFIX(klass, typePrefix)           \
                                            GBL_TYPE_CAST_CLASS(klass, typePrefix##_TYPE, typePrefix##_CLASS_STRUCT)
#define GBL_TYPE_CHECK_CLASS(klass, toType)                     \
                                            (gblTypeClassCheck((GblClass*)klass, toType))
#define GBL_TYPE_CHECK_CLASS_PREFIX(klass, typePrefix)          \
                                            (GBL_TYPE_CHECK_CLASS(klass, typePrefix##_TYPE))
#define GBL_TYPE_CAST_GET_CLASS(instance, toType, cType)        \
                                            ((cType*)gblTypeClassCast(gblTypeClassFromInstance((GblInstance*)instance), toType))
#define GBL_TYPE_CAST_GET_CLASS_PREFIX(instance, typePrefix)    \
                                            GBL_TYPE_CAST_GET_CLASS(instance, typePrefix##_TYPE, typePrefix##_CLASS_STRUCT)
GBL_DECLS_BEGIN

// wrap using DSL
GBL_DECLARE_FLAGS(GblTypeFundamentalFlags) {
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

typedef GBL_RESULT (*GblTypeClassInitFn)        (GblClass*, const void*, GblContext*);
typedef GBL_RESULT (*GblTypeClassFinalizeFn)    (GblClass*, const void*, GblContext*);
typedef GBL_RESULT (*GblTypeInstanceInitFn)     (GblInstance*, GblContext*);

typedef struct GblTypeInterfaceMapEntry {
    GblType      interfaceType;
    uint16_t     classOffset;
} GblTypeInterfaceMapEntry;

typedef struct GblTypeInfo {
    GblTypeClassInitFn              pFnClassInit;
    GblTypeClassFinalizeFn          pFnClassFinalize;
    GblSize                         classSize;
    GblSize                         classAlign;
    const void*                     pClassData;
    GblSize                         interfaceCount;
    const GblTypeInterfaceMapEntry* pInterfaceMap;
    GblTypeInstanceInitFn           pFnInstanceInit;
    GblSize                         instanceSize;
    GblSize                         instanceAlign;
} GblTypeInfo;

GBL_EXPORT GBL_RESULT           gblTypeReinit(GblContext* pCtx)                                              GBL_NOEXCEPT;

GBL_EXPORT GblType              gblTypeRegisterStatic(GblType               parent,
                                                const char*                 pName,
                                                const GblTypeInfo*          pInfo,
                                                GblFlags                    flags)                          GBL_NOEXCEPT;
GBL_EXPORT GblType              gblTypeRegisterBuiltin(GblSize              expectedIndex,
                                                       GblType              parentType,
                                                       const char*          pName,
                                                       const GblTypeInfo*   pTypeInfo,
                                                       GblTypeFlags         flags)                          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           gblTypeUnregister(GblType type)                                             GBL_NOEXCEPT;

GBL_EXPORT GblType              gblTypeFromClass(const GblClass* pClass)                                    GBL_NOEXCEPT;
GBL_EXPORT GblType              gblTypeBuiltin(GblUint index)                                               GBL_NOEXCEPT;
GBL_EXPORT GblType              gblTypeFind(const char* pName)                                              GBL_NOEXCEPT;
GBL_EXPORT GblSize              gblTypeCount(void)                                                          GBL_NOEXCEPT;

GBL_EXPORT const char*          gblTypeName(GblType type)                                                   GBL_NOEXCEPT;
GBL_EXPORT GblType              gblTypeParent(GblType type)                                                 GBL_NOEXCEPT;
GBL_EXPORT GblType              gblTypeFundamental(GblType type)                                            GBL_NOEXCEPT;
GBL_EXPORT GblType              gblTypeBase(GblType type, GblUint depth)                                    GBL_NOEXCEPT;
GBL_EXPORT GblType              gblTypeAncestor(GblType type, GblUint level)                                GBL_NOEXCEPT;
GBL_EXPORT GblUint              gblTypeDepth(GblType type)                                                  GBL_NOEXCEPT;
GBL_EXPORT GblBool              gblTypeFlagsTest(GblType type, GblFlags mask)                               GBL_NOEXCEPT;
GBL_EXPORT const GblTypeInfo*   gblTypeInfo(GblType type)                                                   GBL_NOEXCEPT;
GBL_EXPORT GblIPlugin*          gblTypePlugin(GblType type)                                                 GBL_NOEXCEPT;
GBL_EXPORT GblContext*          gblTypeContext(GblType type)                                                GBL_NOEXCEPT;
GBL_EXPORT GblRefCount          gblTypeClassRefCount(GblType type)                                          GBL_NOEXCEPT;
#ifdef GBL_TYPE_DEBUG
GBL_EXPORT GblRefCount          gblTypeInstanceRefCount(GblType type)                                       GBL_NOEXCEPT;
#endif
GBL_EXPORT GblBool              gblTypeIsA(GblType type, GblType other)                                     GBL_NOEXCEPT;
GBL_EXPORT GblBool              gblTypeIsOrInherits(GblType derived, GblType super)                         GBL_NOEXCEPT;
GBL_EXPORT GblBool              gblTypeMapsInterface(GblType concrete, GblType iface)                       GBL_NOEXCEPT;

GBL_EXPORT GblClass*            gblTypeClassFromInstance(const GblInstance* pInstance)                      GBL_NOEXCEPT;
GBL_EXPORT GblClass*            gblTypeClassRef(GblType type)                                               GBL_NOEXCEPT;
GBL_EXPORT GblRefCount          gblTypeClassUnref(GblClass* pClass)                                         GBL_NOEXCEPT;
GBL_EXPORT GblClass*            gblTypeClassPeek(GblType typeId)                                            GBL_NOEXCEPT;
GBL_EXPORT GblClass*            gblTypeClassCreateFloating(GblType type)                                    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           gblTypeClassConstructFloating(GblType type, GblClass* pClass)               GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           gblTypeClassDestructFloating(GblClass* pClass)                              GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           gblTypeClassDestroyFloating(GblClass* pClass)                               GBL_NOEXCEPT;
GBL_EXPORT GblBool              gblTypeClassIsFloating(const GblClass* pClass)                              GBL_NOEXCEPT;
GBL_EXPORT GblBool              gblTypeClassCheck(const GblClass* pClass, GblType toType)                   GBL_NOEXCEPT;
GBL_EXPORT GblClass*            gblTypeClassCast(const GblClass* pClass, GblType toType)                    GBL_NOEXCEPT;
GBL_EXPORT GblClass*            gblTypeClassTryCast(const GblClass* pClass, GblType toType)                 GBL_NOEXCEPT;

GBL_EXPORT GblInstance*         gblTypeInstanceCreate(GblType type)                                         GBL_NOEXCEPT;
GBL_EXPORT GblInstance*         gblTypeInstanceCreateWithClass(GblClass* pClass)                            GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           gblTypeInstanceConstruct(GblType type, GblInstance* pInstance)              GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           gblTypeInstanceConstructWithClass(GblInstance* pInstance, GblClass* pClass) GBL_NOEXCEPT;
GBL_EXPORT GblRefCount          gblTypeInstanceDestruct(GblInstance* pInstance)                             GBL_NOEXCEPT;
GBL_EXPORT GblRefCount          gblTypeInstanceDestroy(GblInstance* pInstance)                              GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           gblTypeInstanceClassSwizzle(GblInstance* pInstance, GblClass* pClass)       GBL_NOEXCEPT;
GBL_EXPORT GblBool              gblTypeInstanceCheck(const GblInstance* pInstance, GblType toType)          GBL_NOEXCEPT;
GBL_EXPORT GblInstance*         gblTypeInstanceCast(const GblInstance* pInstance, GblType toType)           GBL_NOEXCEPT;
GBL_EXPORT GblInstance*         gblTypeInstanceTryCast(const GblInstance* pInstance, GblType toType)        GBL_NOEXCEPT;


GBL_DECLS_END


#endif // GIMBAL_TYPE_H
