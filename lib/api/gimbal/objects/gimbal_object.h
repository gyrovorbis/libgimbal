#ifndef GIMBAL_OBJECT__H
#define GIMBAL_OBJECT__H

#include "../types/gimbal_typedefs.h"
#include "../meta/gimbal_meta.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GBL_OBJECT_INVALID NULL

//typedef GBL_RESULT (*GblMetaObjectPropertyInfoFn)(const GblObject_, const GblVariant*, const GblProperty**);
typedef GBL_RESULT (*GblMetaObjectPropertyGetFn)(const GblObject_, const GblVariant*, GblVariant*);
typedef GBL_RESULT (*GblMetaObjectPropertySetFn)(const GblObject_, const GblVariant*, const GblVariant*);
typedef GBL_RESULT (*GblMetaObjectPropertyNextFn)(const GblObject_, const GblVariant*, GblVariant*, GblVariant*);

#if 0
typedef struct GblProperty {
    GblEnum             id;
    GblVariant          key;
    const char*         pDescription;
    GBL_VARIANT_TYPE    valueType;
    GblSize             valueSize;
} GblProperty;
#endif


#define GBL_META_OBJECT_VTABLE_TABLE (                                                                                                      \
        ( GBL_META_OBJECT_VTABLE, MetaObjectVTable, "Vtable for GblMetaObjects", gblMetaObjectVTableString),                                \
        (                                                                                                                                   \
            (GBL_META_OBJECT_VTABLE_TO,         GBL_META_TYPE_VTABLE_TO,                To,             "To"),                              \
            (GBL_META_OBJECT_VTABLE_COMPARE,    GBL_META_TYPE_VTABLE_COMPARE,           Compare,        "Compare"),                         \
            (GBL_META_OBJECT_VTABLE_DESTRUCTOR, GBL_META_TYPE_VTABLE_DESTRUCTOR,        Destructor,     "Destructor"),                      \
            (GBL_META_OBJECT_PROPERTY_GET,      GBL_META_TYPE_VTABLE_DESTRUCTOR+1,      PropertyGet,    "PropertyGet"),                     \
            (GBL_META_OBJECT_PROPERTY_SET,      GBL_META_TYPE_VTABLE_DESTRUCTOR+2,      PropertySet,    "PropertySet"),                     \
            (GBL_META_OBJECT_PROPERTY_NEXT,     GBL_META_TYPE_VTABLE_DESTRUCTOR+3,      PropertyNext,   "PropertyNext"),                    \
            (GBL_META_OBJECT_VTABLE_COUNT,      GBL_META_TYPE_VTABLE_DESTRUCTOR+4,      BuiltinCount,   "Builtin virtual method count")     \
        )                                                                                                                                   \
    )

GBL_ENUM_TABLE_DECLARE(GBL_META_OBJECT_VTABLE_TABLE);

typedef struct GblMetaObject {
    GblMetaType             metaType;
    const char*             pObjectTypeName;
    GBL_OBJECT_TYPE         objectType;
    struct GblMetaObject*   pParent;
} GblMetaObject;


typedef struct GblObject__ {
    GblMetaObject* pMetaObject;
} GblObject__;


#define GBL_OBJECT_VCALL(obj, methodIdx, ...)                           \
    do {                                                                \
        const GblVirtualFn* pMethod = NULL;                             \
        GBL_API_CALL(gblObjectVirtualMethod(obj, methodIdx, &pMethod)); \
        GBL_API_VERIFY_POINTER(pMethod);                                \
        GBL_API_CALL(pMethod(__VA_ARGS__));                             \
    } while(0)


typedef struct GblObjectCreateInfo {
    GblMemAllocInfo*    pExtraSpaceInfo;
} GblObjectCreateInfo;


GBL_RESULT gblObjectMetaObject(const GblObject_ hObject, const GblMetaObject**);
GBL_RESULT gblObjectPropertyGet(const GblObject_ hObject, const GblVariant* pkey, GblVariant* pValue);
GBL_RESULT gblObjectPropertySet(GblObject_ hObject, const GblVariant* pKey, const GblVariant* pValue);
GBL_RESULT gblObjectPropertyNext(const GblObject_ hObject, const GblVariant* pCurKey, GblVariant* pNextKey, GblVariant* pNextValue);
GBL_RESULT gblObjectVirtualMethod(const GblObject_ hObject, GblEnum methodIndex, const GblVirtualFn** pMethod);
GBL_RESULT gblObjectParentVirtualMethod(const GblObject_ hObject, GblEnum methodIndex, const GblVirtualFn** pMethod);

GBL_RESULT gblObjectConstruct(GblObject_* phObject, const GblMetaObject* pMetaObject);
GBL_RESULT gblObjectDestruct(GblObject_ hObject);

//GBL_RESULT gblObjectConstructFromString();
//GBL_RESULT gblObjectCopyConstruct();
//GBL_RESULT gblObjectMoveConstruct();
//GBL_RESULT gblObjectConstructFromLuaTable();



#ifdef __cplusplus
}
#endif

#endif // GIMBAL_OBJECT_H
