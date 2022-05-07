#ifndef GIMBAL_INSTANCE_H
#define GIMBAL_INSTANCE_H

#include "gimbal/core/gimbal_api_frame.h"
#include "gimbal_class.h"

#define GBL_INSTANCE(instance)              ((GblInstance*)instance)
#define GBL_INSTANCE_CLASS(instance)        (GblClass_fromInstance(GBL_INSTANCE(instance)))
#define GBL_INSTANCE_SUPER(instance)        (GblInstance_super(GBL_INSTANCE(instance)))
#define GBL_INSTANCE_TYPE(instance)         (GBL_CLASS_TYPE(GBL_INSTANCE_CLASS(instance)))

#define GBL_INSTANCE_CHECK(instance, toType)                \
    (GblInstance_check((GblInstance*)instance, toType))
#define GBL_INSTANCE_CHECK_PREFIX(instance, typePrefix)     \
    (GBL_INSTANCE_CHECK(instance, typePrefix##_TYPE))

#define GBL_INSTANCE_CAST(instance, toType, cType)          \
    ((cType*)GblInstance_cast((GblInstance*)instance, toType))
#define GBL_INSTANCE_CAST_PREFIX(instance, typePrefix)      \
    (GBL_INSTANCE_CAST(instance, typePrefix##_TYPE, typePrefix##_STRUCT))

#define GBL_INSTANCE_TRY(instance, toType, cType)           \
    ((cType*)GblInstance_try((GblInstance*)instance, toType))
#define GBL_INSTANCE_TRY_PREFIX(instance, typePrefix)       \
    (GBL_INSTANCE_TRY(instance, typePrefix##_TYPE, typePrefix##_STRUCT))

#define GBL_INSTANCE_CAST_CLASS(instance, toType, cType)     \
     ((cType*)GblClass_cast(GblClass_fromInstance((GblInstance*)instance), toType))
#define GBL_INSTANCE_CAST_CLASS_PREFIX(instance, typePrefix) \
    (GBL_INSTANCE_CAST_CLASS(instance, typePrefix##_TYPE, typePrefix##_CLASS_STRUCT))

#define GBL_INSTANCE_TRY_CLASS(instance, toType, cType)     \
    ((cType*)GblClass_try(GblClass_fromInstance((GblInstance*)instance), toType))
#define GBL_INSTANCE_TRY_CLASS_PREFIX(instance, typePrefix) \
    (GBL_INSTANCE_TRY_CLASS(instance, typePrefix##_TYPE, typePrefix##_CLASS_STRUCT))


#define GBL_INSTANCE_VCALL_(op, type, classType, method, inst, ...)             \
    GBL_STMT_START {                                                            \
        classType* pClass = GBL_INSTANCE_CAST_CLASS(inst, type, classType);\
        pClass = (classType*)op(pClass);                                        \
        GBL_API_VERIFY(pClass, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);          \
        GBL_API_VERIFY(pClass->method, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);  \
        GBL_API_CALL(pClass->method(inst GBL_VA_ARGS(__VA_ARGS__)));            \
    } GBL_STMT_END

#define GBL_INSTANCE_VCALL(type, classType, method, inst, ...)                  \
    GBL_INSTANCE_VCALL_((void*), type, classType, method, inst, __VA_ARGS__)
#define GBL_INSTANCE_VCALL_PREFIX(prefix, method, inst, ...)                    \
    GBL_INSTANCE_VCALL_((void*), prefix##_TYPE, prefix##_CLASS_STRUCT, method,  \
                        inst, __VA_ARGS__)

#define GBL_INSTANCE_VCALL_SUPER(type, classType, method, inst, ...)            \
    GBL_INSTANCE_VCALL_(GBL_CLASS_SUPER, type, classType,                       \
                        method, inst, __VA_ARGS__)
#define GBL_INSTANCE_VCALL_SUPER_PREFIX(prefix, method, inst, ...)              \
    GBL_INSTANCE_VCALL_(GBL_CLASS_SUPER, prefix##_TYPE, prefix##_CLASS_STRUCT,  \
                        method, inst, __VA_ARGS__)


#define SELF    GblInstance*       pSelf
#define CSELF   const GblInstance* pSelf

GBL_DECLS_BEGIN

typedef struct GblInstance {
    GblClass*   pClass;
} GblInstance;

GBL_EXPORT GblInstance* GblInstance_create              (GblType type)              GBL_NOEXCEPT;
GBL_EXPORT GblInstance* GblInstance_createWithClass     (GblClass* pClass)          GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblInstance_construct           (SELF, GblType type)        GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblInstance_constructWithClass  (SELF, GblClass* pClass)    GBL_NOEXCEPT;

GBL_EXPORT GblRefCount  GblInstance_destroy             (SELF)                      GBL_NOEXCEPT;
GBL_EXPORT GblRefCount  GblInstance_destruct            (SELF)                      GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblInstance_classSwizzle        (SELF, GblClass* pClass)    GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblInstance_classSink           (SELF)                      GBL_NOEXCEPT;

GBL_INLINE GblType      GblInstance_type                (CSELF)                     GBL_NOEXCEPT;
GBL_INLINE const char*  GblInstance_typeName            (CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblSize      GblInstance_size                (CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblClass*    GblInstance_class               (CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblClass*    GblInstance_super               (CSELF)                     GBL_NOEXCEPT;

GBL_EXPORT GblBool      GblInstance_check               (CSELF, GblType toType)     GBL_NOEXCEPT;
GBL_INLINE GblInstance* GblInstance_cast                (SELF, GblType toType)      GBL_NOEXCEPT;
GBL_INLINE GblInstance* GblInstance_try                 (SELF, GblType toType)      GBL_NOEXCEPT;

#ifdef GBL_TYPE_DEBUG
GBL_EXPORT GblRefCount  GblInstance_refCountFromType    (GblType type)              GBL_NOEXCEPT;
#endif

// ========== IMPL ==========


GBL_INLINE GblType GblInstance_type(CSELF) GBL_NOEXCEPT {
    return pSelf? GblClass_type(pSelf->pClass) : GBL_INVALID_TYPE;
}

GBL_INLINE const char* GblInstance_typeName(CSELF) GBL_NOEXCEPT {
    return pSelf? GblClass_typeName(GBL_INSTANCE_CLASS(pSelf)) : "Invalid";
}

GBL_INLINE GblSize GblInstance_size(CSELF) GBL_NOEXCEPT {
    GblSize size = 0;
    if(pSelf) {
        const GblTypeInfo* pInfo = GblType_info(GBL_INSTANCE_TYPE(pSelf));
        if(pInfo) size = pInfo->instanceSize;
    }
    return size;
}
GBL_INLINE GblClass* GblInstance_class(CSELF) GBL_NOEXCEPT {
    return pSelf? pSelf->pClass : NULL;
}
GBL_INLINE GblClass* GblInstance_super(CSELF) GBL_NOEXCEPT {
    return pSelf? GblClass_super(pSelf->pClass) : NULL;
}

GBL_INLINE GblInstance* GblInstance_convert_(GblInstance* pSelf, GblType type, GblBool check) GBL_NOEXCEPT {
    GblInstance* pOutInstance = NULL;
    GBL_API_BEGIN(GblType_contextDefault());
    if(!(!pSelf && type == GBL_INVALID_TYPE)) {
        GBL_API_VERIFY_POINTER(pSelf);
        if(GblInstance_check(pSelf, type)) {
            pOutInstance = pSelf;
        } else if(check) {
            GBL_API_WARN("Failed to cast instance from %s to %s.",
                         GblInstance_typeName(pSelf),
                         GblType_name(type));
        }
    }
    GBL_API_END_BLOCK();
    return pOutInstance;
}


GBL_INLINE GblInstance* GblInstance_try(GblInstance* pSelf, GblType type) GBL_NOEXCEPT {
    return GblInstance_convert_(pSelf, type, GBL_FALSE);
}

GBL_INLINE  GblInstance* GblInstance_cast(GblInstance* pSelf, GblType type) GBL_NOEXCEPT {
    return GblInstance_convert_(pSelf, type, GBL_TRUE);
}

GBL_DECLS_END

#undef SELF
#undef CSELF


#endif // GIMBAL_INSTANCE_H
