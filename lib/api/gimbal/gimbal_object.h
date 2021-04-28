#ifndef GIMBAL_OBJECT_H
#define GIMBAL_OBJECT_H

#include "gimbal_variant.h"

#ifdef __cplusplus
extern "C" {
#endif

GBL_DECLARE_OPAQUE(GblObject);

#define GBL_META_OBJECT_TYPE_TABLE (                                                            \
        ( GBL_OBJECT_TYPE, ObjectType, "Enum for GblObject runtime types"),                     \
        (                                                                                       \
            (GBL_OBJECT_TYPE_OBJECT,            0x0,    Object,         "Object"),              \
            (GBL_OBJECT_TYPE_HANDLE,            0x1,    Handle,         "Handle"),              \
            (GBL_OBJECT_TYPE_CONTEXT,           0x2,    Context,        "Context"),             \
            (GBL_OBJECT_TYPE_BUILTIN_COUNT,     0x3,    BuiltinCount,   "# of Builtin Types"),  \
            (GBL_OBJECT_TYPE_USER,              0x4,    User,           "First User Type")      \
        )                                                                                       \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_OBJECT_TYPE_TABLE);

#define GBL_OBJECT_INVALID NULL

typedef struct GblObject_ {
    GblMetaType* pMetaType;

} GblObject_;

GBL_API gblObjectMetaType(GblObject pObj, const GblMetaType** pMetaType);



#define GBL_OBJECT_VCALL(H, METHOD, ...) \
    H->pFnV##METHOD##(__VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_OBJECT_H
