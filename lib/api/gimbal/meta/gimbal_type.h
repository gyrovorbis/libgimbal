#ifndef GIMBAL_TYPE_H
#define GIMBAL_TYPE_H


#include "../types/gimbal_typedefs.h"

GBL_DECLS_BEGIN

#define GBL_TYPE_FUNDAMENTAL(type)              (gblTypeFundamental(type))
#define	GBL_TYPE_FUNDAMENTAL_MAX                (255 << GBL_TYPE_FUNDAMENTAL_SHIFT)
#define GBL_TYPE_INVALID                        (0)
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
#define GBL_TYPE_BOXED                          GBL_TYPE_MAKE_FUNDAMENTAL(18)
#define GBL_TYPE_OBJECT                         GBL_TYPE_MAKE_FUNDAMENTAL(19)
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
#define GBL_TYPE_INSTANCE_IS_A(instance, toType)                (gblTypeInstanceIsA(instance, toType))
#define GBL_TYPE_CLASS_CAST(klass, toType, cType)               ((cType*)gblTypeClassCast((GblClass*)klass, toType))
#define GBL_TYPE_CLASS_IS_A(klass, toType)                      (gblTypeClassIsA(klass, toType))
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


GBL_EXPORT GBL_RESULT           gblTypeReinit(GblContext hCtx);

GBL_EXPORT GblType              gblTypeRegisterStatic(GblType                   parent,
                                                const char*                     pName,
                                                const GblTypeInfo*              pInfo,
                                                GblFlags                        flags);
// needs to properly init gblType, can't be called anywhere!
GBL_EXPORT GBL_RESULT           gblTypeRegisterBuiltinType(GblSize              expectedIndex,
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


// ===== / META TYPES =======

// ========= STATIC TYPES ===========
#if 0
typedef struct GblTypeBaseInfo {
    GblType             parentType;
    GblTypeFlags        typeFlags;
} GblTypeBaseInfo;

typedef struct GblTypeFundamentalInfo {
    struct GblTypeBaseInfo      baseTypeInfo;
} GblTypeFundamentalInfo;
#endif





#if 0
GBL_API gblTypeRegisterClass(const char*                            pTypeName,
                             const GblTypeClassInfo*                pClassInfo,
                             GblType*                               pNewType);


GBL_API gblTypeRegisterFundamental(const char*                      pTypeName,
                                   const GblTypeFundamentalInfo*    pFundamentalInfo,
                                   GblType*                         pNewType);
#endif

// ========= / STATIC TYPES ============

// ========= FUNDAMENTAL TYPES ===========





// ==========/  FUNDAMENTAL TYPES ===========

#if 0

/* Type Checking Macros
 */
#define GBL_TYPE_IS_FUNDAMENTAL(type)             ((type) <= GBL_TYPE_FUNDAMENTAL_MAX)
#define GBL_TYPE_IS_DERIVED(type)                 ((type) > GBL_TYPE_FUNDAMENTAL_MAX)
#define GBL_TYPE_IS_INTERFACE(type)               (GBL_TYPE_FUNDAMENTAL(type) == GBL_TYPE_INTERFACE)
#define GBL_TYPE_IS_CLASSED(type)                 (gblTypeFlagsTest((type), GBL_TYPE_FLAG_CLASSED))
#define GBL_TYPE_IS_INSTANTIATABLE(type)          (gblTypeFlagsTest((type), GBL_TYPE_FLAG_INSTANTIATABLE))
#define GBL_TYPE_IS_DERIVABLE(type)               (gblTypeFlagsTest((type), GBL_TYPE_FLAG_DERIVABLE))
#define GBL_TYPE_IS_DEEP_DERIVABLE(type)          (gblTypeFlagsTest((type), GBL_TYPE_FLAG_DEEP_DERIVABLE))
#define GBL_TYPE_IS_ABSTRACT(type)                (gblTypeFlagsTest((type), GBL_TYPE_FLAG_ABSTRACT))
#define GBL_TYPE_IS_VALUE_ABSTRACT(type)          (gblTypeFlagsTest((type), GBL_TYPE_FLAG_VALUE_ABSTRACT))
#define GBL_TYPE_IS_VALUE_TYPE(type)              (gblTypeCheckValueType(type))
#define GBL_TYPE_HAS_VALUE_TABLE(type)            (gblTypeValueTablePeek(type) != NULL)

// reserve one bit of GblType for whether it's dynamic or not?


#define GBL_TYPE_CHECK_INSTANCE(instance)                           (_GBL_TYPE_CHI ((GblTypeInstance*) (instance)))
#define GBL_TYPE_CHECK_INSTANCE_CAST(instance, g_type, c_type)      (_GBL_TYPE_CIC ((instance), (g_type), c_type))
#define GBL_TYPE_CHECK_INSTANCE_TYPE(instance, g_type)              (_GBL_TYPE_CIT ((instance), (g_type)))
#define GBL_TYPE_CHECK_INSTANCE_FUNDAMENTAL_TYPE(instance, g_type)  (_GBL_TYPE_CIFT ((instance), (g_type)))
#define GBL_TYPE_INSTANCE_GET_CLASS(instance, g_type, c_type)       (_GBL_TYPE_IGC ((instance), (g_type), c_type))
#define GBL_TYPE_INSTANCE_GET_INTERFACE(instance, g_type, c_type)   (_GBL_TYPE_IGI ((instance), (g_type), c_type))
#define GBL_TYPE_CHECK_CLASS_CAST(g_class, g_type, c_type)          (_GBL_TYPE_CCC ((g_class), (g_type), c_type))
#define GBL_TYPE_CHECK_CLASS_TYPE(g_class, g_type)                  (_GBL_TYPE_CCT ((g_class), (g_type)))
#define GBL_TYPE_CHECK_VALUE(value)                                 (_GBL_TYPE_CHV ((value)))
#define GBL_TYPE_CHECK_VALUE_TYPE(value, g_type)                    (_GBL_TYPE_CVH ((value), (g_type)))
#define GBL_TYPE_FROM_INSTANCE(instance)                            (GBL_TYPE_FROM_CLASS (((GblTypeInstance*) (instance))->g_class))
#define GBL_TYPE_FROM_CLASS(g_class)                                (((GblTypeClass*) (g_class))->g_type)
#define GBL_TYPE_FROM_INTERFACE(g_iface)                            (((GblTypeInterface*) (g_iface))->g_type)
#define GBL_TYPE_INSTANCE_GET_PRIVATE(instance, g_type, c_type)     ((c_type*) GBL_TYPE_instance_get_private ((GblTypeInstance*) (instance), (g_type))) GLIB_DEPRECATED_MACRO_IN_2_58_FOR(G_ADD_PRIVATE)
#define GBL_TYPE_CLASS_GET_PRIVATE(klass, g_type, c_type)           ((c_type*) GBL_TYPE_class_get_private ((GblTypeClass*) (klass), (g_type)))


/* --- prototypes --- */

/* DEPRECATED
 *
typedef enum {
  GBL_TYPE_DEBUG_NONE           = 0,
  GBL_TYPE_DEBUG_OBJECTS        = 1 << 0,
  GBL_TYPE_DEBUG_SIGNALS        = 1 << 1,
  GBL_TYPE_DEBUG_INSTANCE_COUNT = 1 << 2,
  GBL_TYPE_DEBUG_MASK           = 0x07
} GblTypeDebugFlags;

void            gblTypeInit                     (void); // set GblRuntime/GblState/GblVm here?
void            gblTypeInitWithDebugFlags       (GblTypeDebugFlags  debugFlags);
*/
const char*     gblTypeName                     (GblType type);
GblType         gblTypeFromName                 (const char* pName);
GblType         gblTypeParent                   (GblType type);
uint16_t        gblTypeDepth                    (GblType type);
GblType         gblTypeNextBase                 (GblType leafType, GblType rootType);
GblBool         gblTypeIsA                      (GblType type, GblType expected);
void*           gblTypeClassRef                 (GblType type);
void*           gblTypeClassPeek                (GblType type);
void*           gblTypeClassPeekStatic          (GblType type);
void            gblTypeClassUnref               (void* pClass);
void*           gblTypeClassPeekParent          (void* pClass);
void*           gblTypeInterfacePeek            (void* pInstanceClass, GblType iFaceType);
void*           gblTypeInterfacePeekParent      (void* pIFace);
void*           gblTypeInterfaceDefaultRef      (GblType type);
void*           gblTypeInterfaceDefaultPeek     (GblType type);
void            gblTypeInterfaceDefaultUnref    (void* pIFace);
/* g_free() the returned arrays (dynamic and owned by user) */
GblType*        gblTypeChildren                 (GblType type, GblSize* pChildCount);
GblType*        gblTypeInterfaces               (GblType type, GblSize* pInterfaceCount);
/* per-type _static_ data */
void            gblTypeDataSet                  (GblType type, GblGuid quark, void* pData);
void*           gblTypeDataGet                  (GblType type, GblGuid quark);
void            gblTypeQuery                    (GblType type, GblTypeQuery* pQuery);
/* debug-only? */
GblSize         gblTypeInstanceCount            (GblType type);



GblType gblTypeRegisterStatic(GblType       parentType,
                     const char*		    pTypeName,
                     const GblTypeInfo*     pInfo,
                     GblTypeFlags           flags);

GblType gblTypeRegisterStaticSimple(GblType     parentType,
                     const char*                pTypeName,
                     GblSize                    classSize,
                     GblClassInitFn           pFnClassInit,
                     GblSize                    instanceSize,
                     GblInstanceInitFn        pFnInstanceInit,
                     GblTypeFlags               flags);

GblType gblTypeRegisterDynamic(GblType          parentType,
                     const char*                pTypeName,
                     //GblTypePlugin*             pPlugin,
                     GblTypeFlags               flags);

GblType gblTypeRegisterFundamental(GblType          typeId,
                     const char*                    pTypeName,
                     const GblTypeInfo*             pTypeInfo,
                     const GblTypeFundamentalInfo*  pFundamentalInfo,
                     GblTypeFlags                   flags);

void  gblTypeAddInterfaceStatic(GblType         instanceType,
                     GblType                    interfaceType,
                     const GblInterfaceInfo*    pInterfaceInfo);

void  gblTypeAddInterfaceDynamic(GblType		instanceType,
                     GblType                    interfaceType
                    //,GblTypePlugin*             pPlugin
                    );

void  gblTypeInterfaceAddPrerequesite(GblType interfaceType,
                     GblType prereqType);
fuc
GblType* gblTypeinterfacePrerequisites(GblType interfaceType,
                     GblSize* pPrereqCount);

GblType gblTypeInterfaceInstantiablePrerequesite(GblType interfaceType);

void    gblTypeClassAddPrivate(void* pClass,
                                GblSize privateSize);

int32_t gblTypeAddInstancePrivate(GblType classType,
                                      GblSize privateSize);

void*   gblTypeInstancePrivateGet(GblInstance* pInstance,
                                GblType privateType);

void    gblTypeClassAdjustPrivateOffset(void* pClass,
                                        GblSize* pPrivateSizeOrOffset);

void    gblTypeAddClassPrivate(GblType classType,
                                GblSize  privateSize);

void*   gblTypeClassPrivateGet(GblClass*    pKlass,
                               GblType      privateType);

GblSize  gblTypeClassInstanceOffsetPrivateGet(void* pClass);

void     gblTypeEnsure(GblType type);

GblSize  gblTypeRegistrationSerialGet(void);



#if 0


/* --- protected (for fundamental type implementations) --- */
GLIB_AVAILABLE_IN_ALL
GTypePlugin*	 g_type_get_plugin		(GType		     type);
GLIB_AVAILABLE_IN_ALL
GTypePlugin*	 g_type_interface_get_plugin	(GType		     instance_type,
                         GType               interface_type);
GLIB_AVAILABLE_IN_ALL
GType		 g_type_fundamental_next	(void);
GLIB_AVAILABLE_IN_ALL
GType		 g_type_fundamental		(GType		     type_id);
GLIB_AVAILABLE_IN_ALL
GTypeInstance*   g_type_create_instance         (GType               type);
GLIB_AVAILABLE_IN_ALL
void             g_type_free_instance           (GTypeInstance      *instance);

GLIB_AVAILABLE_IN_ALL
void		 g_type_add_class_cache_func    (gpointer	     cache_data,
                         GTypeClassCacheFunc cache_func);
GLIB_AVAILABLE_IN_ALL
void		 g_type_remove_class_cache_func (gpointer	     cache_data,
                         GTypeClassCacheFunc cache_func);
GLIB_AVAILABLE_IN_ALL
void             g_type_class_unref_uncached    (gpointer            g_class);

GLIB_AVAILABLE_IN_ALL
void             g_type_add_interface_check     (gpointer	         check_data,
                         GTypeInterfaceCheckFunc check_func);
GLIB_AVAILABLE_IN_ALL
void             g_type_remove_interface_check  (gpointer	         check_data,
                         GTypeInterfaceCheckFunc check_func);

GLIB_AVAILABLE_IN_ALL
GTypeValueTable* g_type_value_table_peek        (GType		     type);


/*< private >*/
GLIB_AVAILABLE_IN_ALL
gboolean	 g_type_check_instance          (GTypeInstance      *instance) G_GNUC_PURE;
GLIB_AVAILABLE_IN_ALL
GTypeInstance*   g_type_check_instance_cast     (GTypeInstance      *instance,
                         GType               iface_type);
GLIB_AVAILABLE_IN_ALL
gboolean         g_type_check_instance_is_a	(GTypeInstance      *instance,
                         GType               iface_type) G_GNUC_PURE;
GLIB_AVAILABLE_IN_2_42
gboolean         g_type_check_instance_is_fundamentally_a (GTypeInstance *instance,
                                                           GType          fundamental_type) G_GNUC_PURE;
GLIB_AVAILABLE_IN_ALL
GTypeClass*      g_type_check_class_cast        (GTypeClass         *g_class,
                         GType               is_a_type);
GLIB_AVAILABLE_IN_ALL
gboolean         g_type_check_class_is_a        (GTypeClass         *g_class,
                         GType               is_a_type) G_GNUC_PURE;
GLIB_AVAILABLE_IN_ALL
gboolean	 g_type_check_is_value_type     (GType		     type) G_GNUC_CONST;
GLIB_AVAILABLE_IN_ALL
gboolean	 g_type_check_value             (const GValue       *value) G_GNUC_PURE;
GLIB_AVAILABLE_IN_ALL
gboolean	 g_type_check_value_holds	(const GValue	    *value,
                         GType		     type) G_GNUC_PURE;
GLIB_AVAILABLE_IN_ALL
gboolean         g_type_test_flags              (GType               type,
                         guint               flags) G_GNUC_CONST;


/* --- debugging functions --- */
GLIB_AVAILABLE_IN_ALL
const gchar *    g_type_name_from_instance      (GTypeInstance	*instance);
GLIB_AVAILABLE_IN_ALL
const gchar *    g_type_name_from_class         (GTypeClass	*g_class);

/* Typedefs
 */
/**
 * GblType:
 *
 * A numerical value which represents the unique identifier of a registered
 * type.
 */
#if     GLIB_SIZEOF_SIZE_T != GLIB_SIZEOF_LONG || !defined __cplusplus
typedef gsize                           GblType;
#else   /* for historic reasons, C++ links against gulong GblTypes */
typedef gulong                          GblType;
#endif
typedef struct _GValue                  GValue;
typedef union  _GblTypeCValue             GblTypeCValue;
typedef struct _GblTypePlugin             GblTypePlugin;
typedef struct _GblTypeClass              GblTypeClass;
typedef struct _GblTypeInterface          GblTypeInterface;
typedef struct _GblTypeInstance           GblTypeInstance;
typedef struct _GblTypeInfo               GblTypeInfo;
typedef struct _GblTypeFundamentalInfo    GblTypeFundamentalInfo;
typedef struct _GInterfaceInfo          GInterfaceInfo;
typedef struct _GblTypeValueTable         GblTypeValueTable;
typedef struct _GblTypeQuery		GblTypeQuery;

#endif

#endif

GBL_DECLS_END




#endif // GIMBAL_TYPE_H
