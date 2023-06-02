/*! \file
 *  \brief GblProperty instance, DSL, and management API
 *  \ingroup meta
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_PROPERTY_H
#define GIMBAL_PROPERTY_H

#include "../instances/gimbal_box.h"

#define GBL_PROPERTY_TYPE                       (GBL_TYPEOF(GblProperty))
#define GBL_GENERIC_PROPERTY_TYPE                GBL_PROPERTY_TYPE

#define GBL_PROPERTY(instance)                  (GBL_INSTANCE_CAST(instance, GblProperty))
#define GBL_PROPERTY_CLASS(klass)               (GBL_CLASS_CAST(klass, GblPropertyClass))
#define GBL_PROPERTY_GET_CLASS(instance)        (GBL_INSTANCE_GET_CLASS(instance, GblProperty))

#define GBL_SELF_TYPE GblProperty

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblProperty);

typedef GblBool (*GblPropertyIterFn)(const GblProperty* pProp, void* pClosure);

GBL_DECLARE_ENUM(GBL_PROPERTY_FLAG) {
    GBL_PROPERTY_FLAG_CONSTRUCT     = 0x1,
    GBL_PROPERTY_FLAG_READ          = 0x2,
    GBL_PROPERTY_FLAG_WRITE         = 0x4,
    GBL_PROPERTY_FLAG_SAVE          = 0x8,
    GBL_PROPERTY_FLAG_LOAD          = 0x10,
    GBL_PROPERTY_FLAG_ABSTRACT      = 0x20,
    GBL_PROPERTY_FLAG_OVERRIDE      = 0x40,
    GBL_PROPERTY_FLAG_READ_WRITE    = 0x6,
    GBL_PROPERTY_FLAG_SAVE_LOAD     = 0x18,
    GBL_PROPERTY_FLAG_ALL           = 0xffff
};

GBL_CLASS_DERIVE(GblProperty, GblBox)
    GBL_RESULT (*pFnInitOptionalArgs)(GBL_SELF, size_t  argCount, va_list* pVaList);
    GBL_RESULT (*pFnDefaultValue)    (GBL_CSELF, GblVariant* pValue);
    GBL_RESULT (*pFnCheckValue)      (GBL_CSELF, const GblVariant* pValue);
    GBL_RESULT (*pFnValidateValue)   (GBL_CSELF, GblVariant* pValue);
    GBL_RESULT (*pFnCompareValues)   (GBL_CSELF, const GblVariant* pV1, const GblVariant* pV2, int* pResult);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblProperty, GblBox)
    GBL_PRIVATE()
        GblProperty* pNext;
        GblType      objectType;
    GBL_PRIVATE_END
    GblQuark    name;
    size_t      id;
    GblFlags    flags;
    GblType     valueType;
GBL_INSTANCE_END

// ===== MACRO DSL ====

#define GBL_PROPERTIES(object, ...)              GBL_PROPERTIES_(object, __VA_ARGS__)
#define GBL_PROPERTIES_REGISTER(/*object,*/ ...) GBL_PROPERTIES_REGISTER_(__VA_ARGS__)

// ===== STATICS =====

GBL_EXPORT GblType      GblProperty_type           (void)                                  GBL_NOEXCEPT;
GBL_EXPORT size_t       GblProperty_totalCount     (void)                                  GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblProperty_install        (GblType objType, GblProperty* pProp)   GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblProperty_uninstall      (GblType objType, const char* pName)    GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblProperty_uninstallQuark (GblType objType, GblQuark name)        GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblProperty_uninstallAll   (GblType objType)                       GBL_NOEXCEPT;

GBL_EXPORT size_t       GblProperty_count          (GblType objectType)                    GBL_NOEXCEPT;
GBL_EXPORT GblFlags     GblProperty_combinedFlags  (GblType objectType)                    GBL_NOEXCEPT;

GBL_EXPORT const GblProperty*
                        GblProperty_find           (GblType objectType, const char* pName) GBL_NOEXCEPT;
GBL_EXPORT const GblProperty*
                        GblProperty_findQuark      (GblType objectType, GblQuark name)     GBL_NOEXCEPT;
GBL_EXPORT const GblProperty*
                        GblProperty_next           (GblType            objectType,
                                                    const GblProperty* pPrev,
                                                    GblFlags           mask)               GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblProperty_foreach        (GblType            objectType,
                                                    GBL_PROPERTY_FLAG  flags,
                                                    GblPropertyIterFn  pFnIt,
                                                    void*              pClosure)           GBL_NOEXCEPT;
// ===== INSTANCE =====

GBL_EXPORT GblProperty* GblProperty_create         (GblType     derivedType,
                                                    const char* pName,
                                                    size_t      id,
                                                    GblFlags    flags,
                                                    size_t      optionalArgCount,
                                                    ...)                                   GBL_NOEXCEPT;

GBL_EXPORT GblProperty* GblProperty_createVaList   (GblType     derivedType,
                                                    const char* pName,
                                                    size_t      id,
                                                    GblFlags    flags,
                                                    size_t      optionalArgCount,
                                                    va_list*    pList)                     GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblProperty_construct      (GBL_SELF,
                                                    GblType     derivedType,
                                                    const char* pName,
                                                    size_t      id,
                                                    GblFlags    flags,
                                                    size_t      optionalArgCount,
                                                    ...)                                   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblProperty_constructVaList(GBL_SELF,
                                                    GblType     derivedType,
                                                    const char* pName,
                                                    size_t      id,
                                                    GblFlags    flags,
                                                    size_t      optionalArgCount,
                                                    va_list*    pList)                     GBL_NOEXCEPT;

GBL_INLINE GblType      GblProperty_objectType     (GBL_CSELF)                             GBL_NOEXCEPT;
GBL_INLINE const char*  GblProperty_nameString     (GBL_CSELF)                             GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblProperty_defaultValue   (GBL_CSELF, GblVariant* pValue)         GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblProperty_checkValue     (GBL_CSELF, const GblVariant* pValue)   GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblProperty_validateValue  (GBL_CSELF, GblVariant* pValue)         GBL_NOEXCEPT;

GBL_EXPORT int          GblProperty_compareValues  (GBL_CSELF,
                                                    const GblVariant* pV1,
                                                    const GblVariant* pV2)                 GBL_NOEXCEPT;
// ===== IMPL =====

GBL_INLINE const char* GblProperty_nameString(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf? GblQuark_toString(pSelf->name) : NULL;
}

GBL_INLINE GblType GblProperty_objectType(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf? GBL_PRIV_REF(pSelf).objectType : GBL_INVALID_TYPE;
}

GBL_INLINE GBL_RESULT GblProperty_createOrConstruct_(GblProperty** ppSelf,
                                                     GblType       derivedType,
                                                     const char*   pName,
                                                     size_t        id,
                                                     GblFlags      flags,
                                                     size_t        optionalArgCount,
                                                     ...)
{
    va_list varArgs;
    va_start(varArgs, optionalArgCount);
    if(ppSelf && *ppSelf) {
        return GblProperty_constructVaList(*ppSelf,
                                           derivedType,
                                           pName,
                                           id,
                                           flags,
                                           optionalArgCount,
                                           &varArgs);
    } else {
        GblProperty* pProp = GblProperty_createVaList(derivedType,
                                                      pName,
                                                      id,
                                                      flags,
                                                      optionalArgCount,
                                                      &varArgs);
        if(ppSelf) *ppSelf = pProp;
        return pProp? GBL_RESULT_SUCCESS : GBL_RESULT_ERROR_INVALID_POINTER;
    }
}

///\cond
#define GBL_PROPERTIES_(object, ...)                        \
    GBL_PROPERTIES_IDS_(object, __VA_ARGS__)                \
    GBL_PROPERTIES_REGISTER_DEFINE_(object, __VA_ARGS__)

#define GBL_PROPERTIES_REGISTER_(...) \
    GBL_VA_OVERLOAD_CALL(GBL_PROPERTIES_REGISTER, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)

#define GBL_PROPERTIES_REGISTER_1(object) \
    GBL_PROPERTIES_REGISTER_2(object, NULL)

#define GBL_PROPERTIES_REGISTER_2(object, list)                     \
    GBL_STMT_START {                                                \
        for(size_t  p = 0; p < object##_Property_Id_count; ++p) {   \
            object##_registerProperty_(p,                           \
                                       list? &list[p] : GBL_NULL);  \
        }                                                           \
    } GBL_STMT_END

#define GBL_PROPERTIES_IDS_(object, ...)                            \
    typedef enum object##_Property_Id_ {                            \
        GBL_TUPLE_FOREACH(GBL_PROPERTY_ID_, object, (__VA_ARGS__))  \
        object##_Property_Id_count                                  \
    } object##_Property_Id_;

#define GBL_PROPERTY_ID_(object, property)  \
    GBL_PROPERTY_ID__(object, GBL_EVAL property)

#define GBL_PROPERTY_ID__(...) \
    GBL_PROPERTY_ID___(__VA_ARGS__)

#define GBL_PROPERTY_ID___(object, name, type, flags, ...) \
    object##_Property_Id_##name,

#define GBL_PROPERTIES_REGISTER_DEFINE_(object, ...)                                    \
    GBL_EXPORT GblType object##_type(void) GBL_NOEXCEPT;                                \
    GBL_INLINE GBL_RESULT object##_registerProperty_(GblEnum id,                        \
                                                     GblProperty** ppProp) GBL_NOEXCEPT \
    {                                                                                   \
        GBL_CTX_BEGIN(NULL);                                                            \
        GblProperty* pProp = ppProp? *ppProp : GBL_NULL;                                \
        switch(id) {                                                                    \
            GBL_TUPLE_FOREACH(GBL_PROPERTY_REGISTER_, object, (__VA_ARGS__))            \
            default: GBL_CTX_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_INVALID_PROPERTY);      \
        }                                                                               \
        if(pProp) {                                                                     \
            GBL_CTX_CALL(GblProperty_install(object##_type(), pProp));                  \
            if(ppProp) *ppProp = pProp;                                                 \
        }                                                                               \
        GBL_CTX_END();                                                                  \
    }

#define GBL_PROPERTY_REGISTER_(object, property) \
    GBL_PROPERTY_REGISTER__(object, GBL_EVAL property)

#define GBL_PROPERTY_REGISTER__(...) \
    GBL_PROPERTY_REGISTER___(__VA_ARGS__)

#define GBL_PROPERTY_REGISTER___(object, name, type, ...)              \
    case object##_Property_Id_##name:                                  \
        GBL_CTX_VERIFY_CALL(GblProperty_createOrConstruct_(&pProp,     \
                type##_PROPERTY_TYPE,                                  \
                GblQuark_internStringStatic(GBL_STRINGIFY(name)),      \
                id,                                                    \
                GBL_PROPERTY_FLAGS_MASK_ GBL_TUPLE_FIRST(__VA_ARGS__), \
                GBL_PROPERTY_VARARGS_((__VA_ARGS__))));                \
        break;

#define GBL_PROPERTY_VARARGS_(...) \
    GBL_VA_OVERLOAD_CALL(GBL_PROPERTY_VARARGS_, GBL_VA_OVERLOAD_SUFFIXER_1_N, __VA_ARGS__)

#define GBL_PROPERTY_VARARGS__1(flags) \
    0

#define GBL_PROPERTY_VARARGS__N(...) \
    GBL_NARG __VA_ARGS__ - 1, GBL_TUPLE_REST __VA_ARGS__

#define GBL_PROPERTY_FLAGS_MASK_(...) \
    GBL_MAP(GBL_PROPERTY_FLAGS_MASK__, __VA_ARGS__)0

#define GBL_PROPERTY_FLAGS_MASK__(suffix)  \
    GBL_PROPERTY_FLAG_##suffix |

///\endcond

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_PROPERTY_H
