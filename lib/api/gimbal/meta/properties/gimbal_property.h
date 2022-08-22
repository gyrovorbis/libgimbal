/*! \file
 *  \brief GblProperty instance, DSL, and management API
 *  \ingroup metaBuiltinTypes
 */

#ifndef GIMBAL_PROPERTY_H
#define GIMBAL_PROPERTY_H

#include "../instances/gimbal_box.h"

#define GBL_PROPERTY_TYPE               (GblProperty_type())
#define GBL_GENERIC_PROPERTY_TYPE       GBL_PROPERTY_TYPE

#define GBL_PROPERTY(instance)          (GBL_INSTANCE_CAST(instance, GBL_PROPERTY_TYPE, GblProperty))
#define GBL_PROPERTY_CLASS(klass)       (GBL_CLASS_CAST(klass, GBL_PROPERTY_TYPE, GblPropertyClass))
#define GBL_PROPERTY_GET_CLASS(instance)(GBL_INSTANCE_GET_CLASS(instance, GBL_PROPERTY_TYPE, GblPropertyClass))

#define GBL_SELF    GblProperty* pSelf
#define GBL_CSELF   const GBL_SELF

GBL_DECLS_BEGIN

typedef enum GBL_PROPERTY_FLAG {
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
} GBL_PROPERTY_FLAG;

GBL_FORWARD_DECLARE_STRUCT(GblProperty);

GBL_CLASS_DERIVE(GblPropertyClass, GblBoxClass)
    GBL_RESULT  (*pFnInitOptionalArgs)(GBL_SELF, GblSize argCount, va_list* pVaList);
    GBL_RESULT  (*pFnDefaultValue)    (GBL_CSELF, GblVariant* pValue);
    GBL_RESULT  (*pFnValidateValue)   (GBL_CSELF, GblVariant* pValue);
    GBL_RESULT  (*pFnCheckValue)      (GBL_CSELF, const GblVariant* pValue);
GBL_CLASS_END

GBL_INSTANCE_DERIVE(GblProperty, GblBox)
    GBL_PRIVATE()
        GblProperty* pNext;
        GblType      objectType;
    GBL_PRIVATE_END

    GblQuark         name;
    GblType          valueType;
    GblFlags         flags;
    GblSize          id;
GBL_INSTANCE_END

// ===== MACRO DSL =====

#define GBL_PROPERTIES(object, ...)              GBL_PROPERTIES_(object, __VA_ARGS__)
#define GBL_PROPERTIES_REGISTER(/*object,*/ ...) GBL_PROPERTIES_REGISTER_(__VA_ARGS__)

// ===== STATICS =====

GBL_EXPORT GblType      GblProperty_type         (void)                                GBL_NOEXCEPT;
GBL_EXPORT GblSize      GblProperty_totalCount   (void)                                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblProperty_install      (GblType objType, GblProperty* pProp) GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblProperty_uninstall    (GblType objType, GblQuark pName)     GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblProperty_uninstallAll (GblType objType)                     GBL_NOEXCEPT;

GBL_EXPORT GblSize      GblProperty_count        (GblType objectType)                  GBL_NOEXCEPT;
GBL_EXPORT GblFlags     GblProperty_combinedFlags(GblType objectType)                  GBL_NOEXCEPT;

GBL_EXPORT const GblProperty*
                        GblProperty_find         (GblType objectType, GblQuark pName)  GBL_NOEXCEPT;
GBL_EXPORT const GblProperty*
                        GblProperty_next         (GblType            objectType,
                                                  const GblProperty* pPrev,
                                                  GblFlags           mask)             GBL_NOEXCEPT;
// ===== INSTANCE =====

GBL_EXPORT GblProperty* GblProperty_create       (GblType     derivedType,
                                                  const char* pName,
                                                  GblSize     id,
                                                  GblFlags    flags,
                                                  GblSize     optionalArgCount,
                                                  ...)                                 GBL_NOEXCEPT;

GBL_EXPORT GblProperty* GblProperty_createVaList (GblType     derivedType,
                                                  const char* pName,
                                                  GblSize     id,
                                                  GblFlags    flags,
                                                  GblSize     optionalArgCount,
                                                  va_list*    pList)                   GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblProperty_construct    (GBL_SELF,
                                                  GblType     derivedType,
                                                  const char* pName,
                                                  GblSize     id,
                                                  GblFlags    flags,
                                                  GblSize     optionalArgCount,
                                                  ...)                                 GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblProperty_constructVaList
                                                 (GBL_SELF,
                                                  GblType     derivedType,
                                                  const char* pName,
                                                  GblSize     id,
                                                  GblFlags    flags,
                                                  GblSize     optionalArgCount,
                                                  va_list*    pList)                   GBL_NOEXCEPT;

GBL_INLINE GblType      GblProperty_objectType   (GBL_CSELF)                           GBL_NOEXCEPT;
GBL_INLINE const char*  GblProperty_nameString   (GBL_CSELF)                           GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT   GblProperty_defaultValue (GBL_CSELF, GblVariant* pValue)       GBL_NOEXCEPT;
GBL_EXPORT GblBool      GblProperty_checkValue   (GBL_CSELF, const GblVariant* pValue) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT   GblProperty_validateValue(GBL_CSELF, GblVariant* pValue)       GBL_NOEXCEPT;

// ===== IMPL =====

GBL_INLINE const char* GblProperty_nameString(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf? GblQuark_toString(pSelf->name) : NULL;
}

GBL_INLINE GblType GblProperty_objectType(GBL_CSELF) GBL_NOEXCEPT {
    return pSelf? GBL_PRIV_REF(pSelf).objectType : GBL_INVALID_TYPE;
}

#define GBL_PROPERTIES_(object, ...)                        \
    GBL_PROPERTIES_IDS_(object, __VA_ARGS__)                \
    GBL_PROPERTIES_REGISTER_DEFINE_(object, __VA_ARGS__)

#define GBL_PROPERTIES_REGISTER_(...) \
    GBL_VA_OVERLOAD_SELECT(GBL_PROPERTIES_REGISTER, GBL_VA_OVERLOAD_SUFFIXER_ARGC, __VA_ARGS__)(__VA_ARGS__)

#define GBL_PROPERTIES_REGISTER_1(object) \
    GBL_PROPERTIES_REGISTER_2(object, NULL)

#define GBL_PROPERTIES_REGISTER_2(object, list)                     \
    GBL_STMT_START {                                                \
        for(GblSize p = 0; p < object##_Property_Id_count; ++p) {   \
            object##_registerProperty_(p,                           \
                                       list? &list[p] : NULL);      \
        }                                                           \
    } GBL_STMT_END

#define GBL_PROPERTIES_IDS_(object, ...)                        \
    typedef enum object##_Property_Id_ {                        \
        FOREACH(GBL_PROPERTY_ID_, object, (__VA_ARGS__))        \
        object##_Property_Id_count                              \
    } object##_Property_Id_;

#define GBL_PROPERTY_ID_(object, property)  \
    GBL_PROPERTY_ID__(object, EXPAND property)

#define GBL_PROPERTY_ID__(...) \
    GBL_PROPERTY_ID___(__VA_ARGS__)

#define GBL_PROPERTY_ID___(object, name, type, flags, ...) \
    object##_Property_Id_##name,

#define GBL_PROPERTIES_REGISTER_DEFINE_(object, ...)                                          \
    GBL_EXPORT GblType object##_type(void) GBL_NOEXCEPT;    \
    GBL_INLINE GBL_RESULT object##_registerProperty_(GblEnum id,                        \
                                                     GblProperty** ppProp) GBL_NOEXCEPT \
    {                                                                                  \
        GBL_API_BEGIN(NULL);                                                           \
        GblProperty* pProp = ppProp? *ppProp : GBL_NULL;                               \
        switch(id) {                                                                   \
            FOREACH(GBL_PROPERTY_REGISTER_, object, (__VA_ARGS__))                     \
            default: GBL_API_VERIFY(GBL_FALSE, GBL_RESULT_ERROR_INVALID_PROPERTY);     \
        }                                                                              \
        if(pProp) {                                                                    \
            GBL_API_CALL(GblProperty_install(object##_type(), pProp));                 \
            if(ppProp) *ppProp = pProp;                                                \
        }                                                                              \
        GBL_API_END();                                                                 \
    }

#define GBL_PROPERTY_REGISTER_(object, property) \
    GBL_PROPERTY_REGISTER__(object, EXPAND property)

#define GBL_PROPERTY_REGISTER__(...) \
    GBL_PROPERTY_REGISTER___(__VA_ARGS__)

#define GBL_PROPERTY_REGISTER___(object, name, type, ...)              \
    case object##_Property_Id_##name:                                  \
        GBL_API_VERIFY_CALL(GblProperty_createOrConstruct_(&pProp,     \
                type##_PROPERTY_TYPE,                                  \
                GblQuark_internStringStatic(GBL_STRINGIFY(name)),      \
                id,                                                    \
                GBL_PROPERTY_FLAGS_MASK_ FIRST(__VA_ARGS__),           \
                GBL_PROPERTY_VARARGS_((__VA_ARGS__))));                \
        break;

#define GBL_PROPERTY_VARARGS_(...) \
    GBL_VA_OVERLOAD_CALL(GBL_PROPERTY_VARARGS_, GBL_VA_OVERLOAD_SUFFIXER_1_N, __VA_ARGS__)

#define GBL_PROPERTY_VARARGS__1(flags) \
    0

#define GBL_PROPERTY_VARARGS__N(...) \
    PP_NARG __VA_ARGS__ - 1, REST __VA_ARGS__

#define GBL_PROPERTY_FLAGS_MASK_(...) \
    GBL_MAP(GBL_PROPERTY_FLAGS_MASK__, __VA_ARGS__)0

#define GBL_PROPERTY_FLAGS_MASK__(suffix)  \
    GBL_PROPERTY_FLAG_##suffix |

GBL_INLINE GBL_RESULT GblProperty_createOrConstruct_(GblProperty** ppSelf,
                                                     GblType       derivedType,
                                                     const char*   pName,
                                                     GblSize       id,
                                                     GblFlags      flags,
                                                     GblSize       optionalArgCount,
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



#if 0
#define GBL_STRING_PROPERTY_TYPE GBL_PROPERTY_TYPE
#define GBL_UINT_PROPERTY_TYPE GBL_PROPERTY_TYPE
GBL_PROPERTIES(GblProperty,
    (prefix,      GBL_STRING, (READ, WRITE, LOAD, SAVE, CONSTRUCT)),
    (version,     GBL_UINT,   (READ, WRITE, LOAD, SAVE, CONSTRUCT)),
    (author,      GBL_STRING, (READ, WRITE, LOAD, SAVE), "Unknown"),
    (description, GBL_STRING, (READ, WRITE, LOAD, SAVE), "None"),
    (typeCount,   GBL_UINT,   (READ), GBL_UINT16_TYPE)
)
#endif


/// \cond

#define GBL_PROPERTY_IDGEN(prefix, name) \
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

#define GBL_PROPERTY_FLAGY_(suffix)              \
    GBL_PROPERTY_FLAG_##suffix |

#define GBL_PROPERTY_FLAGS_MASK(...)            \
    GBL_MAP(GBL_PROPERTY_FLAGY_, __VA_ARGS__)0

#define GBL_PROPERTY_TABLE_END()                \
    default: memset(pInfo, 0, sizeof(GblPropertyInfo)); }}

#define GBL_PROPERTY_TABLE_REGISTER(prefix, klass)                          \
    GBL_STMT_START {                                                        \
        for(GblSize id = GBL_PROPERTY_IDGEN(prefix, FIRST);                   \
            id < GBL_PROPERTY_IDGEN(prefix, COUNT); ++id) {                   \
            GblPropertyInfo info;                                           \
            info.objectType = prefix##_TYPE;                                \
            prefix##_PROPERTY_TABLE(id, &info);                             \
            gblPropertyTableInsert(GBL_CLASS_TYPEOF(klass),                 \
                                   GblQuark_fromString(info.pName),         \
                                   id,                                      \
                                   info.valueType,                          \
                                   info.flags);                             \
        }                                                                   \
    } GBL_STMT_END


typedef struct GblPropertyInfo {
    GblType     objectType;
    const char* pName;
    const char* pNick;
    const char* pDesc;
    GblType     valueType;
    GblSize     id;
    GblFlags    flags;
} GblPropertyInfo;



GBL_EXPORT const GblProperty* gblPropertyTableInsert        (GblType             objectType,
                                                             GblQuark            pName,
                                                             GblSize             id,
                                                             GblType             valueType,
                                                             GblFlags            flags)             GBL_NOEXCEPT;


/// \endcond

GBL_DECLS_END

#undef GBL_CSELF
#undef GBL_SELF

#endif // GIMBAL_PROPERTY_H
