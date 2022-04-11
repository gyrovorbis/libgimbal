#ifndef GIMBAL_TYPE_H
#define GIMBAL_TYPE_H


#include "../types/gimbal_typedefs.h"

GBL_DECLS_BEGIN

#define GBL_TYPE_FUNDAMENTAL(type)              (gblTypeFundamental(type))
#define	GBL_TYPE_FUNDAMENTAL_MAX                (255 << GBL_TYPE_FUNDAMENTAL_SHIFT)
#define GBL_TYPE_INVALID                        ((GblType)0)
#define GBL_TYPE_INTERFACE                      (gblTypeBuiltin(0))
#define GBL_TYPE_IVARIANT                       (gblTypeBuiltin(1))
#define GBL_TYPE_NIL                            (gblTypeBuiltin(2))
#define GBL_TYPE_BOOL                           (gblTypeBuiltin(3))
#define GBL_TYPE_CHAR                           (gblTypeBuiltin(4))
#define GBL_TYPE_UINT8                          (gblTypeBuiltin(5))
#define GBL_TYPE_INT16                          (gblTypeBuiltin(6))
#define GBL_TYPE_UINT16                         (gblTypeBuiltin(7))
#define GBL_TYPE_INT32                          (gblTypeBuiltin(8))
#define GBL_TYPE_UINT32                         (gblTypeBuiltin(9))
#define GBL_TYPE_INT64                          (gblTypeBuiltin(10))
#define GBL_TYPE_UINT64                         (gblTypeBuiltin(11))
#define GBL_TYPE_ENUM                           (gblTypeBuiltin(12))
#define GBL_TYPE_FLAGS                          (gblTypeBuiltin(13))
#define GBL_TYPE_FLOAT                          (gblTypeBuiltin(14))
#define GBL_TYPE_DOUBLE                         (gblTypeBuiltin(15))
#define GBL_TYPE_POINTER                        (gblTypeBuiltin(16))
#define GBL_TYPE_STRING                         (gblTypeBuiltin(17))
#define GBL_TYPE_TYPE                           (gblTypeBuiltin(18))
#define GBL_TYPE_BOXED                          (gblTypeBuiltin(19))
#define GBL_TYPE_OBJECT                         (gblTypeBuiltin(20))
#define	GBL_TYPE_VARIANT                        GBL_TYPE_MAKE_FUNDAMENTAL(20)
//#define GBL_TYPE_META_INSTANCE                  GBL_TYPE_MAKE_FUNDAMENTAL(21)
//#define GBL_TYPE_DISPATCHABLE

#define GBL_TYPE_BUILTIN_COUNT                  18


#define GBL_TYPE_FLAGS_TEST(type, flags)    (gblTypeFlagsTest(type, flags))
#define GBL_TYPE_IS_CLASSED(type)           (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED))
#define GBL_TYPE_IS_INSTANTIABLE(type)      (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE))
#define GBL_TYPE_IS_DERIVABLE(type)         (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE))
#define GBL_TYPE_IS_DEEP_DERIVABLE(type)    (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE))
#define GBL_TYPE_IS_ABSTRACT(type)          (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FLAG_ABSTRACT))
#define GBL_TYPE_IS_FINAL(type)             (GBL_TYPE_FLAGS_TEST(type, GBL_TYPE_FLAG_FINAL))
#define GBL_TYPE_IS_FUNDAMENTAL(type)       (type == gblTypeFundamental(type))
#define GBL_TYPE_IS_INTERFACE(type)         (gblTypeFundamental(type) == GBL_TYPE_INTERFACE)

#define GBL_TYPE_INSTANCE_CAST(instance, toType, cType)         ((cType*)gblTypeInstanceCast((GblInstance*)instance, toType))
#define GBL_TYPE_INSTANCE_IS_A(instance, toType)                (gblTypeInstanceIsA((GblInstance*)instance, toType))
#define GBL_TYPE_CLASS_CAST(klass, toType, cType)               ((cType*)gblTypeClassCast((GblClass*)klass, toType))
#define GBL_TYPE_CLASS_IS_A(klass, toType)                      (gblTypeClassIsA((GblClass*)klass, toType))
#define GBL_TYPE_INSTANCE_CLASS_CAST(instance, toType, cType)   ((cType*)gblTypeClassCast(gblTypeClassFromInstance((GblInstance*)instance), toType))

// wrap using DSL
GBL_DECLARE_FLAGS(GblTypeFundamentalFlags) {
    GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED             = (1 << 0),
    GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE        = (1 << 1),
    GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE           = (1 << 2),
    GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE      = (1 << 3),
    GBL_TYPE_FUNDAMENTAL_FLAGS_MASK               = GBL_TYPE_FUNDAMENTAL_FLAG_CLASSED | GBL_TYPE_FUNDAMENTAL_FLAG_INSTANTIABLE |
                                                    GBL_TYPE_FUNDAMENTAL_FLAG_DEEP_DERIVABLE | GBL_TYPE_FUNDAMENTAL_FLAG_DERIVABLE
};

GBL_DECLARE_FLAGS(GblTypeFlags) {
    GBL_TYPE_FLAG_ABSTRACT                        = (1 << 4),
    GBL_TYPE_FLAG_FINAL                           = (1 << 5),
    GBL_TYPE_FLAGS_MASK                           = GBL_TYPE_FLAG_ABSTRACT | GBL_TYPE_FLAG_FINAL
};

typedef GBL_RESULT (*GblTypeClassInitFn)        (GblClass*, const void*, GblContext);
typedef GBL_RESULT (*GblTypeClassFinalizeFn)    (GblClass*, const void*, GblContext);
typedef GBL_RESULT (*GblTypeInstanceInitFn)     (GblInstance*, GblContext);

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

typedef struct GblClass {
    GblType             type;
} GblClass;

typedef struct GblInstance {
    GblClass*           pClass;
} GblInstance;

typedef struct GblInterface {
    GblClass            base;
    int16_t             offsetToTop;
} GblInterface;

GBL_FORWARD_DECLARE_STRUCT(GblIPlugin);


GBL_EXPORT GBL_RESULT           gblTypeReinit(GblContext hCtx);

GBL_EXPORT GblType              gblTypeRegisterStatic(GblType                   parent,
                                                const char*                     pName,
                                                const GblTypeInfo*              pInfo,
                                                GblFlags                        flags);
GBL_EXPORT GblType              gblTypeRegisterBuiltinType(GblSize              expectedIndex,
                                                           GblType              parentType,
                                                           const char*          pName,
                                                           const GblTypeInfo*   pTypeInfo,
                                                           GblTypeFlags         flags);
GBL_EXPORT GBL_RESULT           gblTypeUnregister(GblType type);

GBL_EXPORT GblType              gblTypeFromClass(const GblClass* pClass);
GBL_EXPORT GblType              gblTypeBuiltin(GblUint index);
GBL_EXPORT GblType              gblTypeFind(const char* pName);
GBL_EXPORT GblSize              gblTypeCount(void);

GBL_EXPORT const char*          gblTypeName(GblType type);
GBL_EXPORT GblType              gblTypeParent(GblType type);
GBL_EXPORT GblType              gblTypeFundamental(GblType type);
GBL_EXPORT GblUint              gblTypeDepth(GblType type);
GBL_EXPORT GblBool              gblTypeFlagsTest(GblType type, GblFlags mask);
GBL_EXPORT const GblTypeInfo*   gblTypeInfo(GblType type);
GBL_EXPORT GblPlugin*           gblTypePlugin(GblType type);
GBL_EXPORT GblRefCount          gblTypeClassRefCount(GblType type);
#ifdef GBL_TYPE_DEBUG
GBL_EXPORT GblRefCount          gblTypeInstanceRefCount(GblType type);
#endif
GBL_EXPORT GblBool              gblTypeIsA(GblType type, GblType other);
GBL_EXPORT GblBool              gblTypeIsOrInherits(GblType derived, GblType super);
GBL_EXPORT GblBool              gblTypeMapsInterface(GblType concrete, GblType iface);

GBL_EXPORT GblClass*            gblTypeClassFromInstance(const GblInstance* pInstance);
GBL_EXPORT GblClass*            gblTypeClassRef(GblType typeId);
GBL_EXPORT GblRefCount          gblTypeClassUnref(GblClass* pClass);
GBL_EXPORT GblClass*            gblTypeClassPeek(GblType typeId);
GBL_EXPORT GblBool              gblTypeClassIsA(const GblClass* pClass, GblType toType);
GBL_EXPORT GblClass*            gblTypeClassCast(const GblClass* pClass, GblType toType);
GBL_EXPORT GblClass*            gblTypeClassTryCast(const GblClass* pClass, GblType toType);

GBL_EXPORT GblInstance*         gblTypeInstanceCreate(GblType type);
GBL_EXPORT GBL_RESULT           gblTypeInstanceConstruct(GblType type, GblInstance* pInstance);
GBL_EXPORT GblRefCount          gblTypeInstanceDestruct(GblInstance* pInstance);
GBL_EXPORT GblRefCount          gblTypeInstanceDestroy(GblInstance* pInstance);
GBL_EXPORT GblBool              gblTypeInstanceIsA(const GblInstance* pInstance, GblType toType);
GBL_EXPORT GblInstance*         gblTypeInstanceCast(const GblInstance* pInstance, GblType toType);
GBL_EXPORT GblInstance*         gblTypeInstanceTryCast(const GblInstance* pInstance, GblType toType);


GBL_DECLS_END


#endif // GIMBAL_TYPE_H
