#ifndef GIMBAL_INSTANCE_H
#define GIMBAL_INSTANCE_H


#include "gimbal_class.h"

#define GBL_INSTANCE(instance)              ((GblInstance*)instance)
#define GBL_INSTANCE_GET_CLASS(instance)    (gblTypeClassFromInstance(GBL_INSTANCE(instance)))
#define GBL_INSTANCE_TYPE(instance)         GBL_CLASS_TYPE(GBL_INSTANCE_GET_CLASS(instance))

#define GBL_INSTANCE_VCALL_(op, type, classType, method, inst, ...)             \
    GBL_STMT_START {                                                            \
        classType* pClass = GBL_TYPE_CAST_GET_CLASS(inst, type, classType);\
        pClass = (classType*)op(pClass);                                        \
        GBL_API_VERIFY(pClass, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);          \
        GBL_API_VERIFY(pClass->method, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);  \
        GBL_API_CALL(pClass->method(inst GBL_VA_ARGS(__VA_ARGS__)));            \
    } GBL_STMT_END

#define GBL_INSTANCE_VCALL(type, classType, method, inst, ...)                  \
    GBL_INSTANCE_VCALL_((void*), type, classType, method, inst, __VA_ARGS__)

#define GBL_INSTANCE_PREFIX_VCALL(prefix, method, inst, ...)                    \
    GBL_INSTANCE_VCALL_((void*), prefix##_TYPE, prefix##_CLASS_STRUCT, method,  \
                        inst, __VA_ARGS__)

#define GBL_INSTANCE_VCALL_SUPER(type, classType, method, inst, ...)            \
    GBL_INSTANCE_VCALL_(GBL_CLASS_SUPER, type, classType,                       \
                        method, inst, __VA_ARGS__)

#define SELF    GblInstance*       pSelf
#define CSELF   const GblInstance* pSelf

GBL_DECLS_BEGIN

typedef struct GblInstance {
    GblClass*   pClass;
} GblInstance;

GBL_INLINE GblType      GblInstance_typeOf      (CSELF);
GBL_INLINE GblSize      GblInstance_sizeOf      (CSELF);
GBL_INLINE GblSize      GblInstance_alignOf     (CSELF);
GBL_INLINE GblClass*    GblInstance_classOf     (CSELF);
GBL_INLINE GblClass*    GblInstance_superOf     (CSELF);







// ====== INLINEZ ======


GBL_INLINE GblType      GblInstance_typeOf      (CSELF) {
    return pSelf? GblClass_typeOf(pSelf->pClass) : GBL_TYPE_INVALID;
}
GBL_INLINE GblSize      GblInstance_sizeOf      (CSELF) {
    return pSelf? gblTypeInfo(GBL_INSTANCE_TYPE(pSelf))->instanceSize : 0;
}
GBL_INLINE GblSize      GblInstance_alignOf     (CSELF) {
    return pSelf? gblTypeInfo(GBL_INSTANCE_TYPE(pSelf))->instanceAlign : 0;
}
GBL_INLINE GblClass*    GblInstance_classOf     (CSELF) {
    return pSelf? pSelf->pClass : NULL;
}
GBL_INLINE GblClass*    GblInstance_superOf     (CSELF) {
    return pSelf? GblClass_superOf(pSelf->pClass) : NULL;
}

GBL_DECLS_END

#undef SELF
#undef CSELF


#endif // GIMBAL_INSTANCE_H
