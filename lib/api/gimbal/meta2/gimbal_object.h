#ifndef GIMBAL_OBJECT_H
#define GIMBAL_OBJECT_H

#include "gimbal_type.h"


#ifdef __cplusplus
extern "C" {
#endif

#define GBL_OBJECT          GblObject
#define GBL_OBJECT_TYPE     (GBL_API_INLINE_GblObject_TYPE(GBL_OBJECT))

GBL_TYPE(GBL_OBJECT)

struct GblClass;

typedef struct GblObject {
    const struct GblClass* pClass_;
} GblObject;

GBL_API GblObject_typeGet(GblType* pType);


GBL_API GblObject_new    (GblObject** pSelf, GblType classType, GblSize size, GblSize align, ...);
GBL_API GblObject_ref    (GblObject* pSelf, GblObject** pNewRef);
GBL_API GblObject_unref  (GblObject* pSelf);
GBL_API GblObject_asType (GblObject* pSelf, GblType toType, GblObject** ppNewRef);


GBL_RESULT GblObject_classGet(const GblObject* pSelf, struct GblClass** ppClass);



#ifdef __cplusplus
}
#endif



#endif // GIMBAL_OBJECT_H
