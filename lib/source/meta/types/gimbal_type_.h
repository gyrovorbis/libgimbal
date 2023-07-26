#ifndef GIMBAL_TYPE__H
#define GIMBAL_TYPE__H

#include <gimbal/meta/types/gimbal_type.h>
#include <gimbal/containers/gimbal_array_list.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/containers/gimbal_nary_tree.h>
#include <gimbal/strings/gimbal_quark.h>
#include <gimbal/core/gimbal_atomics.h>

#include <tinycthread.h>

#define GBL_CLASS_FLAGS_BIT_COUNT_                      5
#define GBL_CLASS_FLAGS_BIT_MASK_                       0x1f
#define GBL_CLASS_INFO_(klass)                          ((uintptr_t)GBL_PRIV_REF(((GblClass*)klass)).metaClassInfo)
#define GBL_CLASS_FLAG_TEST_(klass, mask)               ((GblBool)(GBL_CLASS_INFO_(klass) & mask))
#define GBL_CLASS_FLAG_SET_(klass, flag)                GBL_CLASS_INFO_SET_(klass, GBL_CLASS_META_CLASS_(klass), GBL_CLASS_FLAGS_(klass) | flag)
#define GBL_CLASS_FLAG_CLEAR_(klass, flag)              GBL_CLASS_INFO_SET_(klass, GBL_CLASS_META_CLASS_(klass), GBL_CLASS_FLAGS_(klass) & ~flag)
#define GBL_CLASS_FLAGS_(klass)                         ((GblFlags)(GBL_CLASS_INFO_(klass) & GBL_CLASS_FLAGS_BIT_MASK_))
#define GBL_CLASS_META_CLASS_(klass)                    ((GblMetaClass*)(GBL_CLASS_INFO_(klass) & ~GBL_CLASS_FLAGS_BIT_MASK_))
#define GBL_CLASS_TYPE_(klass)                          ((GblType)(GBL_CLASS_META_CLASS_(klass)))
#define GBL_CLASS_INFO_SET_(klass, meta, flags)          \
    GBL_STMT_START {                                     \
        GBL_PRIV_REF(((GblClass*)klass)).metaClassInfo = \
            ((uintptr_t)meta | flags);                   \
    } GBL_STMT_END

#define GBL_META_CLASS_ALIGNMENT_                       (1 << (GBL_CLASS_FLAGS_BIT_COUNT_))
#define GBL_META_CLASS_(type)                           ((GblMetaClass*)type)
#define GBL_TYPE_(meta)                                 ((GblType)meta)

#define GBL_TYPE_REGISTRY_HASH_MAP_CAPACITY_DEFAULT_    32

#define GBL_TYPE_ENSURE_INITIALIZED_()                  \
    GBL_STMT_START {                                    \
        if(!initializing_) {                            \
            if(!inittedOnce_)                           \
                call_once(&initOnce_, GblType_init_);   \
            else if(!initialized_) GblType_init_();     \
        }                                               \
    } GBL_STMT_END

GBL_DECLS_BEGIN

GBL_DECLARE_ENUM(GBL_CLASS_FLAGS_) {
    GBL_CLASS_FLAG_OWNED_           = (1 << 0),
    GBL_CLASS_FLAG_IN_PLACE_        = (1 << 1),
    GBL_CLASS_FLAG_IFACE_IMPL_      = (1 << 2),
    GBL_CLASS_FLAG_CONTEXT_         = (1 << 3),
    GBL_CLASS_FLAG_CONTANER_        = (1 << 4),
};

GBL_FORWARD_DECLARE_STRUCT(GblArrayMap);
GBL_FORWARD_DECLARE_STRUCT(GblInterface);

typedef struct GblMetaClass {
    union {
        struct GblMetaClass*    pParent;
        GblNaryTreeNode         treeNode;
    };
    GblQuark                    name;
    GBL_ATOMIC_INT16            refCount;
    GBL_ATOMIC_INT16            instanceRefCount;
    const GblTypeInfo*          pInfo;
    GblFlags                    flags;
    GblArrayMap*                pExtensions;
    GblClass*                   pClass;
    uint8_t                     depth;
    ptrdiff_t                   classPrivateOffset;
    ptrdiff_t                   instancePrivateOffset;
    struct GblMetaClass*        pBases[];
} GblMetaClass;

extern GblContext*              pCtx_;
extern once_flag                initOnce_;
extern GblBool                  inittedOnce_;
extern GblBool                  initialized_;
extern GBL_THREAD_LOCAL GblBool initializing_;
extern mtx_t                    typeRegMtx_;
extern GblHashSet               typeRegistry_;
extern struct TypeBuiltins_ {
    GblArrayList   vector;
    uint8_t        stackBuffer[sizeof(GblType)*GBL_TYPE_BUILTIN_COUNT];
} typeBuiltins_;


extern void          GblType_init_                     (void);
extern GBL_RESULT    GblType_refresh_                  (GblType type);
extern GblInterface* GblType_extension_                (GblType type, GblType ifaceType);

extern GblType       GblType_registerBuiltin_          (size_t               expectedIndex,
                                                        GblType              parentType,
                                                        const char*          pName,
                                                        const GblTypeInfo*   pTypeInfo,
                                                        GblTypeFlags         flags);

extern GBL_RESULT    GblThread_final_                  (void);
extern GBL_RESULT    GblModule_final_                  (void);

extern GBL_RESULT    GblProperty_init_                 (GblContext* pCtx);
extern GBL_RESULT    GblProperty_final_                (GblContext* pCtx);

extern GBL_RESULT    GblSignal_init_                   (GblContext* pCtx);
extern GBL_RESULT    GblSignal_final_                  (GblContext* pCtx);
extern GBL_RESULT    GblSignal_removeInstance_         (GblInstance* pInstance);

extern GBL_RESULT    GblVariant_init_                  (GblContext* pCtx);
extern GBL_RESULT    GblVariant_final_                 (GblContext* pCtx);

extern GBL_RESULT    GblIVariant_typeRegister_         (GblContext* pCtx);
extern GBL_RESULT    GblPrimitive_valueTypesRegister_  (GblContext* pCtx);
extern GBL_RESULT    GblPointer_typeRegister_          (GblContext* pCtx);


GBL_DECLS_END

#endif // GIMBAL_TYPE__H
