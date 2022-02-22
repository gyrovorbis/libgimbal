#ifndef GIMBAL_CLASS_H
#define GIMBAL_CLASS_H

#include "gimbal_object.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GBL_CLASS           GblClass
GBL_TYPE(GBL_CLASS)
#define GBL_CLASS_TYPE     (GBL_API_INLINE_GblObject_TYPE(GBL_CLASS))



typedef struct GblTypeInfo {
    GblType             typeId;
    const char*         pTypeNameName;
    GblTypeFlags        typeflags;
};


typedef struct GblClass {
    GblObject               base_;
    GBL_DECLARE_VFUNC       (TypeInfo,     const GblTypeInfo** ppTypeInfo);
    GBL_DECLARE_VFUNC       (InstanceInfo,  const GblInstanceInfo** ppInstanceInfo);
    GBL_DECLARE_VFUNC       (VTable,        GblSize* pSize, void** ppVTable);
    GBL_DECLARE_VFUNC       (ITable,        GblType typeId, GblSize* pSize, void** ppITable);
    GBL_DECLARE_VFUNC       (Alloc,         void** ppOutRef);
    GBL_DECLARE_VFUNC_0     (Dealloc);
    GBL_DECLARE_VFUNC       (Ctor,          va_list* pArgs);
    GBL_DECLARE_VFUNC       (Dtor);

};


typedef struct GblClass {
    const struct GblObject  base_;
    const char*             pName;


    GblType                 superClass;
    GblSize                 instanceSize;
    GblSize                 instanceAlign;


} GblClass;


GBL_API GblClass_typeGet (GblType* pType);

GBL_API GblClass_new     (GblClass** ppSelf, GblType classType, const char* pName, GblType superClass,
                                                GblSize instanceSize, GblSize instanceAlign,
                                                GblSize classSize, GblSize classAlign);


GBL_API GblClass_nameGet (const GblClass* pSelf, const char** ppString);
GBL_API GblClass_typeOf  (const GblClass* pSelf, GblType* pType);
GBL_API GblClass_superGet(const GblClass* pSelf, GblClass** ppSuper);




typedef struct GblObjectInfo {
    GblAllocInfo*   pAllocInfo;
    GblSize         privateSize;
    GblSize         vtableSize;
    void*           pVTable;
} GblObjectCreateInfo;


GBL_API GblObject_new(GblClass* pClass,
                      GblObjectCreateInfo*)

GBL_API GblClass_new(GblClass* pBase, const char* pName,
                     const GblClassCreateInfo* pInfo,
                     ...)



#ifdef __cplusplus
}
#endif



#endif // GIMBAL_CLASS_H
