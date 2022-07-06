#ifndef GIMBAL_INSTANCE_H
#define GIMBAL_INSTANCE_H

#include "gimbal/core/gimbal_api_frame.h"
#include "gimbal_class.h"

#define GBL_INSTANCE(instance)              ((GblInstance*)instance)
#define GBL_INSTANCE_CLASS(instance)        (GblClass_peekFromInstance(GBL_INSTANCE(instance)))
#define GBL_INSTANCE_CLASS_SUPER(instance)  (GblInstance_classSuper(GBL_INSTANCE(instance)))
#define GBL_INSTANCE_CLASS_DEFAULT(instance)(GblInstance_classDefault(GBL_INSTANCE(instance)))
#define GBL_INSTANCE_TYPE(instance)         (GBL_CLASS_TYPE(GBL_INSTANCE_CLASS(instance)))
#define GBL_INSTANCE_PRIVATE(instance, type)(GblInstance_private(GBL_INSTANCE(instance), type))

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
     ((cType*)GblClass_cast(GblClass_peekFromInstance((GblInstance*)instance), toType))
#define GBL_INSTANCE_CAST_CLASS_PREFIX(instance, typePrefix) \
    (GBL_INSTANCE_CAST_CLASS(instance, typePrefix##_TYPE, typePrefix##_CLASS_STRUCT))

#define GBL_INSTANCE_TRY_CLASS(instance, toType, cType)     \
    ((cType*)GblClass_try(GblClass_peekFromInstance((GblInstance*)instance), toType))
#define GBL_INSTANCE_TRY_CLASS_PREFIX(instance, typePrefix) \
    (GBL_INSTANCE_TRY_CLASS(instance, typePrefix##_TYPE, typePrefix##_CLASS_STRUCT))

#define GBL_INSTANCE_VCALL__N(method, ...)      \
    GBL_API_CALL(pClass->method(__VA_ARGS__))
#define GBL_INSTANCE_VCALL__2(method, inst)     \
    GBL_API_VCALL__N(method, inst)
#define GBL_INSTANCE_VCALL__(...)               \
    GBL_VA_OVERLOAD_SELECT(GBL_INSTANCE_VCALL_, GBL_VA_OVERLOAD_SUFFIXER_2_N, __VA_ARGS__)(__VA_ARGS__);
#define GBL_INSTANCE_VCALL_(op, type, classType, method, ...)                                   \
    GBL_STMT_START {                                                                            \
        classType* pClass = GBL_INSTANCE_CAST_CLASS(GBL_ARG_1(__VA_ARGS__), type, classType);   \
        pClass = (classType*)op(pClass);                                                        \
        GBL_API_VERIFY(pClass, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);                          \
        GBL_API_VERIFY(pClass->method, GBL_RESULT_ERROR_INVALID_VIRTUAL_CALL);                  \
        GBL_API_CALL(pClass->method(__VA_ARGS__));                                              \
    } GBL_STMT_END

#define GBL_INSTANCE_VCALL(type, classType, method, ...)                            \
    GBL_INSTANCE_VCALL_((void*), type, classType, method, __VA_ARGS__)
#define GBL_INSTANCE_VCALL_PREFIX(prefix, method, ...)                              \
    GBL_INSTANCE_VCALL(prefix##_TYPE, prefix##_CLASS_STRUCT, method, __VA_ARGS__)

#define GBL_INSTANCE_VCALL_SUPER(type, classType, method, ...)                      \
    GBL_INSTANCE_VCALL_(GBL_CLASS_SUPER, type, classType,                           \
                        method, __VA_ARGS__)
#define GBL_INSTANCE_VCALL_SUPER_PREFIX(prefix, method, ...)                        \
    GBL_INSTANCE_VCALL_SUPER(prefix##_TYPE, prefix##_CLASS_STRUCT,                  \
                        method, __VA_ARGS__)

#define GBL_INSTANCE_VCALL_DEFAULT(type, classType, method, ...)                    \
    GBL_INSTANCE_VCALL_(GBL_CLASS_DEFAULT, type, classType,                         \
                        method, __VA_ARGS__)
#define GBL_INSTANCE_VCALL_DEFAULT_PREFIX(prefix, method, ...)                      \
    GBL_INSTANCE_VCALL_DEFAULT(prefix##_TYPE, prefix##_CLASS_STRUCT,                \
                        method, __VA_ARGS__)

#define SELF    GblInstance*       pSelf
#define CSELF   const GblInstance* pSelf

GBL_DECLS_BEGIN

typedef struct GblInstance {
    GblClass*   pClass;
} GblInstance;

// Creates an instance on the heap with a reference to the default class for the given type
GBL_EXPORT GblInstance* GblInstance_create              (GblType type)              GBL_NOEXCEPT;
// Creates an instance with a floating override class
GBL_EXPORT GblInstance* GblInstance_createWithClass     (GblClass* pClass)          GBL_NOEXCEPT;
// Constructs an instance in-place with the default class
GBL_EXPORT GBL_RESULT   GblInstance_construct           (SELF, GblType type)        GBL_NOEXCEPT;
// Creates an instance on the heap with a floating override class
GBL_EXPORT GBL_RESULT   GblInstance_constructWithClass  (SELF, GblClass* pClass)    GBL_NOEXCEPT;

// Deletes an instance that was created with GblInstance_create on the heap
GBL_EXPORT GblRefCount  GblInstance_destroy             (SELF)                      GBL_NOEXCEPT;
// Destructs an instance created in-place with GblInstance_construct
GBL_EXPORT GblRefCount  GblInstance_destruct            (SELF)                      GBL_NOEXCEPT;

// Replaces the existing class with a type-compatible class. releasing ownership
GBL_EXPORT GBL_RESULT   GblInstance_classSwizzle        (SELF, GblClass* pClass)    GBL_NOEXCEPT;
// Claims ownership of the previously floating class contained by the instance
GBL_EXPORT GBL_RESULT   GblInstance_classSink           (SELF)                      GBL_NOEXCEPT;
// Releases ownership of the owned class contained by the instance
GBL_EXPORT GBL_RESULT   GblInstance_classFloat          (SELF)                      GBL_NOEXCEPT;

// Returns true if the given instance can be casted to an instance of toType
GBL_EXPORT GblBool      GblInstance_check               (CSELF, GblType toType)     GBL_NOEXCEPT;
// Attempts to cast the given instance to toType and errors out upon failure
GBL_INLINE GblInstance* GblInstance_cast                (SELF, GblType toType)      GBL_NOEXCEPT;
// Attempts to cast the given instance to toType but gracefully returns NULL upon failure
GBL_INLINE GblInstance* GblInstance_try                 (SELF, GblType toType)      GBL_NOEXCEPT;
// Returns the private data associated with the given self or base type
GBL_EXPORT void*        GblInstance_private             (CSELF, GblType base)       GBL_NOEXCEPT;

GBL_INLINE GblType      GblInstance_type                (CSELF)                     GBL_NOEXCEPT;
GBL_INLINE const char*  GblInstance_typeName            (CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblSize      GblInstance_size                (CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblSize      GblInstance_privateSize         (CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblSize      GblInstance_totalSize           (CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblClass*    GblInstance_class               (CSELF)                     GBL_NOEXCEPT;
GBL_INLINE GblClass*    GblInstance_classSuper          (CSELF)                     GBL_NOEXCEPT;



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
GBL_INLINE GblClass* GblInstance_classSuper(CSELF) GBL_NOEXCEPT {
    return pSelf? GblClass_super(pSelf->pClass) : NULL;
}
GBL_INLINE GblClass* GblInstance_classDefault(CSELF) GBL_NOEXCEPT {
    return pSelf? GblClass_default(pSelf->pClass) : NULL;
}

GBL_INLINE GblInstance* GblInstance_convert_(GblInstance* pSelf, GblType type, GblBool check) GBL_NOEXCEPT {
    GblInstance* pOutInstance = NULL;
    GBL_API_BEGIN(GblType_contextDefault());
    if(!pSelf) {
        if(type == GBL_INVALID_TYPE || !check) GBL_API_DONE();
    } else {
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
