#ifndef GIMBAL_PROPERTY_H
#define GIMBAL_PROPERTY_H

#include "../meta/gimbal_type.h"
#include "../meta/gimbal_class.h"
#include "../meta/gimbal_instance.h"
#include "../strings/gimbal_quark.h"
#include "../containers/gimbal_linked_list.h"
#include <string.h>

#define GBL_PROPERTY_TYPE                   GblProperty_type()
#define GBL_PROPERTY_PARENT_PREFIX          GBL_INVALID_TYPE
#define GBL_PROPERTY_STRUCT                 GblProperty
#define GBL_PROPERTY_CLASS_STRUCT           GblPropertyClass

#define GBL_PROPERTY(instance)              GBL_INSTANCE_CAST_PREFIX(instance, GBL_PROPERTY)
#define GBL_PROPERTY_CHECK(instance)        GBL_INSTANCE_CHECK_PREFIX(instance, GBL_PROPERTY)
#define GBL_PROPERTY_TRY(instance)          GBL_INSTANCE_TRY_PREFIX(instance, GBL_PROPERTY)

#define GBL_PROPERTY_CLASS(klass)           GBL_CLASS_CAST_PREFIX(klass, GBL_PROPERTY)
#define GBL_PROPERTY_CLASS_CHECK(klass)     GBL_CLASS_CHECK_PREFIX(klass, GBL_PROPERTY)
#define GBL_PROPERTY_CLASS_TRY(klass)       GBL_CLASS_TRY_PREFIX(klass, GBL_PROPERTY)

#define GBL_PROPERTY_GET_CLASS(instance)    GBL_INSTANCE_CAST_CLASS_PREFIX(instance, GBL_PROPERTY)
#define GBL_PROPERTY_TRY_CLASS(instance)    GBL_INSTANCE_TRY_CLASS_PREFIX(instance, GBL_PROPERTY)

#define GBL_PROPERTY_ID(prefix, name) \
    prefix##_PROPERTY_ID_##name

#define GBL_PROPERTY_TABLE_BEGIN(prefix)                    \
    GBL_INLINE void prefix##_PROPERTY_TABLE(GblSize index,  \
        GblPropertyInfo* pInfo) {                           \
            switch(index) {

#define GBL_PROPERTY_ENTRY(name_, nick_, desc_, id_, type_, flags_) \
    case id_:                                   \
        pInfo->pName        = name_;            \
        pInfo->pNick        = nick_;            \
        pInfo->pDesc        = desc_;            \
        pInfo->id           = id_;              \
        pInfo->valueType    = type_;            \
        pInfo->flags        = flags_;           \
    break;

#define GBL_PROPERTY_FLAG_(suffix)              \
    GBL_PROPERTY_FLAG_##suffix |

#define GBL_PROPERTY_FLAGS_MASK(...)            \
    GBL_MAP(GBL_PROPERTY_FLAG_, __VA_ARGS__)0

#define GBL_PROPERTY_TABLE_END()                \
    default: memset(pInfo, 0, sizeof(GblPropertyInfo)); }}

#define GBL_PROPERTY_TABLE_REGISTER(prefix, klass)                          \
    GBL_STMT_START {                                                        \
        for(GblSize id = GBL_PROPERTY_ID(prefix, FIRST);                    \
            id < GBL_PROPERTY_ID(prefix, COUNT); ++id) {                    \
            GblPropertyInfo info;                                           \
            info.objectType = prefix##_TYPE;                                \
            prefix##_PROPERTY_TABLE(id, &info);                             \
            gblPropertyTableInsert(GBL_CLASS_TYPE(klass),                   \
                                   GblQuark_fromString(info.pName),         \
                                   id,                                      \
                                   info.valueType,                          \
                                   info.flags);                             \
        }                                                                   \
    } GBL_STMT_END

#define SELF    GblProperty* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

typedef struct GblPropertyInfo {
    GblType     objectType;
    const char* pName;
    const char* pNick;
    const char* pDesc;
    GblType     valueType;
    GblSize     id;
    GblFlags    flags;
} GblPropertyInfo;

typedef enum GBL_PROPERTY_FLAG {
    GBL_PROPERTY_FLAG_CONSTRUCT     = 0x1,
    GBL_PROPERTY_FLAG_READ          = 0x2,
    GBL_PROPERTY_FLAG_WRITE         = 0x4,
    GBL_PROPERTY_FLAG_SAVE          = 0x8,
    GBL_PROPERTY_FLAG_LOAD          = 0x10,
    GBL_PROPERTY_FLAG_OVERRIDE      = 0x20,
//GBL_PROPERTY_FLAG_LAX_VALIDATION = 0x40
    GBL_PROPERTY_FLAGS_READ_WRITE   = 0x3,
    GBL_PROPERTY_FLAGS_SAVE_LOAD    = 0x18,
    GBL_PROPERTY_FLAGS_ALL          = 0xffff
} GBL_PROPERTY_FLAG;

GBL_FORWARD_DECLARE_STRUCT(GblProperty);

typedef struct GblPropertyClass {
    GblClass        base;
    GblType         valueType;
    GBL_RESULT      (*pFnDestruct)       (GblProperty*);
    GBL_RESULT      (*pFnVariantDefault) (const GblProperty*, GblVariant*);
    GBL_RESULT      (*pFnVariantValidate)(const GblProperty*, GblVariant*);
    GBL_RESULT      (*pFnVariantCompare) (const GblProperty*, const GblVariant*, const GblVariant*, GblInt*);
    GBL_RESULT      (*pFnVariantCheck)   (const GblProperty*, const GblVariant*);
} GblPropertyClass;

typedef struct GblProperty {
    union {
        GblPropertyClass*   pClass;
        GblInstance         base;
    };
    GblType                 objectType;
    GblQuark                name;
    GblType                 valueType;
    GblSize                 id;
    GblFlags                flags;
    union {
        struct {
            const char*     pNick;
            const char*     pDesc;
        };
        struct {
            GblProperty*    pLast_;
            GblSize         propertyCount_;
        };
    };
    GblProperty*            pNext_;
} GblProperty;


GBL_EXPORT GblType            GblProperty_type              (void)                                  GBL_NOEXCEPT;

GBL_EXPORT GblProperty*       GblProperty_create            (GblType propertyType,
                                                             const GblPropertyInfo* pInfo)          GBL_NOEXCEPT;
GBL_EXPORT GblProperty*       GblProperty_ref               (SELF)                                  GBL_NOEXCEPT;
GBL_EXPORT GblRefCount        GblProperty_unref             (SELF)                                  GBL_NOEXCEPT;
GBL_EXPORT GblRefCount        GblProperty_refCount          (CSELF)                                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT         GblProperty_variantDefault    (CSELF, GblVariant* pVariant)           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GblProperty_variantValidate   (CSELF, GblVariant* pVariant)           GBL_NOEXCEPT;
GBL_EXPORT GblInt             GblProperty_variantCompare    (CSELF,
                                                             const GblVariant* pVariant1,
                                                             const GblVariant* pVariant2)           GBL_NOEXCEPT;
GBL_EXPORT GblBool            GblProperty_variantCheck      (CSELF,
                                                             const GblVariant* pVariant)            GBL_NOEXCEPT;

GBL_EXPORT const char*        GblProperty_nameString        (CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT GblQuark           GblProperty_nameQuark         (CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT GblType            GblProperty_valueType         (CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT GblType            GblProperty_objectType        (CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT GblSize            GblProperty_id                (CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT GblFlags           GblProperty_flags             (CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT const char*        GblProperty_nick              (CSELF)                                 GBL_NOEXCEPT;
GBL_EXPORT const char*        GblProperty_description       (CSELF)                                 GBL_NOEXCEPT;

GBL_API                       gblPropertyTableInitialize    (GblContext* pCtx,
                                                             GblSize initialCapacity)               GBL_NOEXCEPT;
GBL_API                       gblPropertyTableFinalize      (void)                                  GBL_NOEXCEPT;
GBL_API                       gblPropertyTableSize          (void)                                  GBL_NOEXCEPT;

GBL_EXPORT const GblProperty* gblPropertyTableInsert        (GblType             objectType,
                                                             GblQuark            pName,
                                                             GblSize             id,
                                                             GblType             valueType,
                                                             GblFlags            flags)             GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT         gblPropertyTableInsertExisting(GblProperty* pProperty)                GBL_NOEXCEPT;

GBL_EXPORT GblBool            gblPropertyTableErase         (GblType objectType, GblQuark pName)    GBL_NOEXCEPT;
GBL_EXPORT GblBool            gblPropertyTableEraseAll      (GblType objectType)                    GBL_NOEXCEPT;
GBL_EXPORT const GblProperty* gblPropertyTableFind          (GblType objectType, GblQuark pName)    GBL_NOEXCEPT;

GBL_EXPORT const GblProperty* gblPropertyTableNext          (GblType            objectType,
                                                             const GblProperty* pPrev,
                                                             GblFlags           mask)               GBL_NOEXCEPT;

GBL_EXPORT GblSize            gblPropertyTableCount         (GblType objectType)                    GBL_NOEXCEPT;
GBL_EXPORT GblFlags           gblPropertyTableFlags         (GblType objectType)                    GBL_NOEXCEPT;


GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_PROPERTY_H
