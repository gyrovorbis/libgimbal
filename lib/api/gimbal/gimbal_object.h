#ifndef GIMBAL_OBJECT_H
#define GIMBAL_OBJECT_H

#include "gimbal_variant.h"

#ifdef __cplusplus
extern "C" {
#endif

GBL_DECLARE_OPAQUE(GblObject);

GBL_DECLARE_ENUM(GBL_OBJECT_TYPE) {
    GBL_OBJECT_TYPE_OBJECT,
    GBL_OBJECT_TYPE_HANDLE,
    GBL_OBJECT_TYPE_CONTEXT,
    GBL_OBJECT_TYPE_BUILTIN_COUNT,
    GBL_OBJECT_TYPE_USER,
};

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
