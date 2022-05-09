#ifndef GIMBAL_TYPE__H
#define GIMBAL_TYPE__H

#include <gimbal/meta/gimbal_type.h>
#include <gimbal/containers/gimbal_vector.h>
#include <gimbal/containers/gimbal_hash_set.h>
#include <gimbal/types/gimbal_quark.h>
#include <stdatomic.h>
#define __USE_UNIX98 1
#include <pthread.h>

#define GBL_CLASS_FLAGS_BIT_COUNT_                      6
#define GBL_CLASS_FLAGS_BIT_MASK_                       0x3f
#define GBL_CLASS_PRIVATE_(klass)                       ((uintptr_t)((GblClass*)klass)->pPrivate_)
#define GBL_CLASS_FLAG_TEST_(klass, mask)               ((GblBool) (GBL_CLASS_PRIVATE_(klass) & mask))
#define GBL_CLASS_FLAG_SET_(klass, flag)                GBL_CLASS_PRIVATE_SET_(klass, GBL_CLASS_META_CLASS_(klass), GBL_CLASS_FLAGS_(klass) | flag)
#define GBL_CLASS_FLAG_CLEAR_(klass, flag)              GBL_CLASS_PRIVATE_SET_(klass, GBL_CLASS_META_CLASS_(klass), GBL_CLASS_FLAGS_(klass) & ~flag)
#define GBL_CLASS_FLAGS_(klass)                         ((GblFlags)(GBL_CLASS_PRIVATE_(klass) & GBL_CLASS_FLAGS_BIT_MASK_))
#define GBL_CLASS_META_CLASS_(klass)                    ((GblMetaClass*)(GBL_CLASS_PRIVATE_(klass) & ~GBL_CLASS_FLAGS_BIT_MASK_))
#define GBL_CLASS_TYPE_(klass)                          ((GblType)(GBL_CLASS_META_CLASS_(klass)))
#define GBL_CLASS_PRIVATE_SET_(klass, metaClass, flags)                             \
    GBL_STMT_START {                                                                \
        ((GblClass*)klass)->pPrivate_ = (void*)((uintptr_t)metaClass | flags);      \
    } GBL_STMT_END

#define GBL_META_CLASS_ALIGNMENT_               (1 << (GBL_CLASS_FLAGS_BIT_COUNT_))
#define GBL_META_CLASS_(type)                   ((GblMetaClass*)type)

#define GBL_TYPE_(meta)              ((GblType)meta)

#define GBL_TYPE_REGISTRY_HASH_MAP_CAPACITY_DEFAULT 32
#define GBL_TYPE_ENSURE_INITIALIZED_()  \
    if(!initializing_) pthread_once(&initOnce_, GblType_init_)

GBL_DECLS_BEGIN

GBL_DECLARE_ENUM(GBL_CLASS_FLAGS_) {
    GBL_CLASS_FLAG_OWNED_           = (1 << 0),
    GBL_CLASS_FLAG_IN_PLACE_        = (1 << 1),
    GBL_CLASS_FLAG_IFACE_IMPL_      = (1 << 2),
    GBL_CLASS_FLAG_CONTEXT_         = (1 << 3),
    GBL_CLASS_FLAG_CONTANER_        = (1 << 4), // + 16 byte header
    GBL_CLASS_FLAG_EXTENDED_INFO_   = (1 << 5)
};

typedef atomic_uint_fast16_t GblRefCounter;

typedef struct GblMetaClass {
    struct GblMetaClass*    pParent;
    GblQuark                name;
    GblRefCounter           refCount;
#ifdef GBL_TYPE_DEBUG
    GblRefCounter           instanceRefCount;
#endif
    GblTypeInfo             info;
    GblFlags                flags;
    GblIPlugin*             pPlugin;
    GblClass*               pClass;
    uint8_t                 depth;
    struct GblMetaClass**   pBases;
} GblMetaClass;


extern GblContext*              pCtx_;
extern pthread_once_t           initOnce_;
extern GblBool                  initialized;
extern GBL_THREAD_LOCAL GblBool initializing_;
extern pthread_mutex_t          typeRegMtx_;
extern GblHashSet               typeRegistry_;
extern struct TypeBuiltins_ {
    GblVector   vector;
    uint8_t     stackBuffer[sizeof(GblType)*GBL_TYPE_BUILTIN_COUNT];
} typeBuiltins_;


extern void GblType_init_(void);


extern GBL_RESULT GblIVariant_typeRegister_     (GblContext* pCtx);
extern GBL_RESULT gblValueTypesRegister_        (GblContext* pCtx);
extern GBL_RESULT GblITable_typeRegister_       (GblContext* pCtx);
extern GBL_RESULT GblIEventHandler_typeRegister_(GblContext* pCtx);
extern GBL_RESULT GblIEventFilter_typeRegister_ (GblContext* pCtx);
extern GBL_RESULT GblObject_typeRegister_       (GblContext* pCtx);
extern GBL_RESULT GblIAllocator_typeRegister_   (GblContext* pCtx);
extern GBL_RESULT GblILogger_typeRegister_      (GblContext* pCtx);
extern GBL_RESULT GblContext_typeRegister_      (GblContext* pCtx);
extern GBL_RESULT GblEvent_typeRegister_        (GblContext* pCtx);




GBL_DECLS_END

#endif // GIMBAL_TYPE__H
