#ifndef GIMBAL_OBJECT_H
#define GIMBAL_OBJECT_H

#include "gimbal_variant.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct GblObject {
    GblMetaType* pMetaType;

} GblObject;

GBL_API gblObjectMetaType(GblObject* pObj, const GblMetaType** pMetaType);



#define GBL_OBJECT_VCALL(H, METHOD, ...) \
    H->pFnV##METHOD##(__VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif // GIMBAL_OBJECT_H
