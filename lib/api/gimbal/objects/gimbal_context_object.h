#ifndef GIMBAL_CONTEXT_OBJECT_H
#define GIMBAL_CONTEXT_OBJECT_H

#include "gimbal_object.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define GBL_CONTEXT_OBJECT_INVALID GBL_OBJECT_INVALID

//GBL_DECLARE_OPAQUE(GblContextObject);

#define GBL_META_CONTEXT_OBJECT_TABLE ( \
    (   \
        (GblContextObject, ContextObject, GBL_OBJECT_TYPE_CONTEXT_OBJECT),    \
        (   \
            (GblObjectEventFn,            pFnEvent)\
        ),  \
    ),  \
       GBL_META_OBJECT_TABLE    \
)


#if 0
#define GBL_ENUM_TABLE_DECLARE(table) \
    GBL_DECLARE_ENUM(GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME))) { \
        GBL_MAP_TUPLES(GBL_ENUM_TABLE_DECL_ENUM,  GBL_MAP_TUPLES(GBL_EVAL, GBL_META_ENUM_TUPLE_VALUE_ARRAY table)) \
    }; \
    GBL_ENUM_TABLE_DECLARE_STRINGIFIER(table)

#define GBL_ENUM_TABLE_RETURN_STRING(cName, value, name, string) \
    case value: return string;

#define GBL_ENUM_TABLE_TO_STRING(table, value) \
    switch(value) { \
        GBL_MAP_TUPLES(GBL_ENUM_TABLE_RETURN_STRING, GBL_MAP_TUPLES(GBL_EVAL, GBL_META_ENUM_TUPLE_VALUE_ARRAY table)) \
    }

#define GBL_ENUM_TABLE_DECLARE_STRINGIFIER(table) \
    GBL_MAYBE_UNUSED GBL_CONSTEXPR GBL_INLINE const char* GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, STRINGIFIER))(GBL_EVAL(GBL_META_ENUM_TYPE_PROPERTY(table, CNAME)) value) { \
        GBL_ENUM_TABLE_TO_STRING(table, value); \
        return ""; \
    }


#endif




typedef struct GblContextObjectCreateInfo {
    GblObjectCreateInfo objectInfo;
    GblContext          hCtx;
} GblContextObjectCreateInfo;

typedef GblObjectVTable     GblContextObjectVTable;

typedef struct GblContextObject {
    GblObject  baseObject;
} GblContextObject;


GBL_API gblContextObjectMetaObjectStatic   (const GblMetaObject** ppMetaObject);

// Instance
GBL_API gblContextObjectMetaObject         (GblContextObject hObject, const GblMetaObject** ppMetaObject);
GBL_API gblContextObjectMetaInstance       (GblContextObject hObject, const GblMetaInstance** ppMetaInstance);
GBL_API gblContextObjectExtraSpace         (GblContextObject hObject, void** ppExtraData, GblSize* pSize);
GBL_API gblContextObjectDispatcher         (GblContextObject hObject, const GblDispatcher** ppDispatcher);
GBL_API gblContextObjectContext            (GblContextObject hObject, GblContext* phCtx);

// Overridden from GblMetaType
GBL_API gblContextObjectCreate             (GblContextObject* phObject, GblContext hCtx, const GblContextObjectCreateInfo* pCreateInfo);
GBL_API gblContextObjectDestroy            (GblContextObject hObject);
GBL_API gblContextObjectConvert            (GblContextObject hObject, GblVariant* pTo);
GBL_API gblContextObjectCompare            (GblContextObject hObject, const GblVariant* pRhs, GblInt* pResult);
GBL_API gblContextObjectSerialize          (GblContextObject hObject, void* pBuffer, GblSize* pSize);
GBL_API gblContextObjectDeserialize        (GblContextObject hObject, const void* pBuffer, GblSize* pSize);

// New Virtuals
GBL_API gblContextObjectPropertyGet        (GblContextObject hObject, const GblVariant* pKey, GblVariant* pValue);
GBL_API gblContextObjectPropertySet        (GblContextObject hObject, const GblVariant* pKey, const GblVariant* pValue);
GBL_API gblContextObjectPropertyNext       (GblContextObject hObject, const GblVariant* pCurKey, GblVariant* pNextKey, GblVariant* pNextValue);


#ifdef __cplusplus
}
#endif

#endif
