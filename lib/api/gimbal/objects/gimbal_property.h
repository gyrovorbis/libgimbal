#ifndef GIMBAL_PROPERTY_H
#define GIMBAL_PROPERTY_H

#include "../meta/gimbal_type.h"
#include "../types/gimbal_quark.h"

GBL_DECLS_BEGIN

#define GBL_PROPERTY_ID(prefix, name) \
    prefix##_PROPERTY_ID_##name

#define GBL_PROPERTY_TABLE_BEGIN(prefix)                    \
    GBL_INLINE void prefix##_PROPERTY_TABLE(GblSize index,  \
        GblPropertyInfo* pInfo) {                           \
            const GblType objType = prefix##_TYPE;          \
            switch(index) {

#define GBL_PROPERTY(id_, name_, type_, flags_) \
    case id_:                                   \
        pInfo->objectType   = objType;          \
        pInfo->pName        = name_;            \
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
            prefix##_PROPERTY_TABLE(id, &info);                             \
            gblPropertyTableInsert(GBL_CLASS_TYPE(klass),                   \
                                   GblQuark_fromString(info.pName),         \
                                   id,                                      \
                                   info.valueType,                          \
                                   info.flags);                             \
        }                                                                   \
    } GBL_STMT_END


typedef struct GblPropertyInfo {
    GblType     objectType;
    const char* pName;
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
    GBL_PROPERTY_FLAGS_ALL          = 0xffff
} GBL_PROPERTY_FLAG;

GBL_FORWARD_DECLARE_STRUCT_PRIVATE(GblProperty);

GBL_EXPORT const char*        GblProperty_nameString    (const GblProperty* pSelf)                  GBL_NOEXCEPT;
GBL_EXPORT GblQuark           GblProperty_nameQuark     (const GblProperty* pSelf)                  GBL_NOEXCEPT;
GBL_EXPORT GblType            GblProperty_valueType     (const GblProperty* pSelf)                  GBL_NOEXCEPT;
GBL_EXPORT GblType            GblProperty_objectType    (const GblProperty* pSelf)                  GBL_NOEXCEPT;
GBL_EXPORT GblSize            GblProperty_id            (const GblProperty* pSelf)                  GBL_NOEXCEPT;
GBL_EXPORT GblFlags           GblProperty_flags         (const GblProperty* pSelf)                  GBL_NOEXCEPT;



GBL_API                       gblPropertyTableInitialize(GblContext* pCtx, GblSize initialCapacity) GBL_NOEXCEPT;
GBL_API                       gblPropertyTableFinalize  (void)                                      GBL_NOEXCEPT;
GBL_API                       gblPropertyTableSize      (void)                                      GBL_NOEXCEPT;

GBL_EXPORT const GblProperty* gblPropertyTableInsert    (GblType             objectType,
                                                         GblQuark            pName,
                                                         GblSize             id,
                                                         GblType             valueType,
                                                         GblFlags            flags)                 GBL_NOEXCEPT;
GBL_EXPORT GblBool            gblPropertyTableErase     (GblType objectType, GblQuark pName)        GBL_NOEXCEPT;
GBL_EXPORT GblBool            gblPropertyTableEraseAll  (GblType objectType)                        GBL_NOEXCEPT;
GBL_EXPORT const GblProperty* gblPropertyTableFind      (GblType objectType, GblQuark pName)        GBL_NOEXCEPT;
GBL_EXPORT const GblProperty* gblPropertyTableNext      (GblType objectType,
                                                         const GblProperty* pPrev,
                                                         GblFlags           mask)                   GBL_NOEXCEPT;
GBL_EXPORT GblSize            gblPropertyTableCount     (GblType objectType)                        GBL_NOEXCEPT;
GBL_EXPORT GblFlags           gblPropertyTableFlags     (GblType objectType)                        GBL_NOEXCEPT;


GBL_DECLS_END

#endif // GIMBAL_PROPERTY_H
