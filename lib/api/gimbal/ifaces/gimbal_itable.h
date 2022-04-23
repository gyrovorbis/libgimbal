#ifndef GIMBAL_ITABLE_H
#define GIMBAL_ITABLE_H

#include "../meta/gimbal_interface.h"

GBL_DECLS_BEGIN

//#define GBL_ITABLE_TYPE                     (GblITable_type())
#define GBL_ITABLE(instance)                GBL_TYPE_CAST_INSTANCE(instance, GBL_TYPE_ITABLE, GblITable)
#define GBL_ITABLE_CHECK(instance)     GBL_TYPE_CHECK_INSTANCE(instance, GBL_TYPE_ITABLE,)
#define GBL_ITABLE_IFACE(klass)             GBL_TYPE_CAST_CLASS(klass, GBL_TYPE_ITABLE, GblITableIFace)
#define GBL_ITABLE_IFACE_CHECK(klass)  GBL_TYPE_CHECK_CLASS(klass, GBL_TYPE_ITABLE)
#define GBL_ITABLE_GET_IFACE(instance)      GBL_TYPE_CAST_GET_CLASS(instance, GBL_TYPE_ITABLE, GblITableIFace)

#define SELF    GblITable* pSelf
#define CSELF   const SELF

GBL_FORWARD_DECLARE_STRUCT(GblITable);

typedef struct GblITableIFace {
    GblInterface base;
    GBL_RESULT (*pFnIndex)    (CSELF, const GblVariant* pKey, GblVariant* pValue);
    GBL_RESULT (*pFnNewIndex) (SELF, const GblVariant* pKey, const GblVariant* pValue);
    GBL_RESULT (*pFnNextIndex)(CSELF, const GblVariant* pKey, GblVariant* pNextKey, GblVariant* pNextValue);
} GblITableIFace;


GBL_EXPORT GblType  GblITable_type(void) GBL_NOEXCEPT;
GBL_API             GblITable_index(CSELF, const GblVariant* pKey, GblVariant* pValue) GBL_NOEXCEPT;
GBL_API             GblITable_newIndex(SELF, const GblVariant* pKey, const GblVariant* pValue) GBL_NOEXCEPT;
GBL_API             GblITable_nextIndex(CSELF, const GblVariant* pKey, GblVariant* pNextKey, GblVariant* pNextValue) GBL_NOEXCEPT;

//GBL_API GblITable_print(CSELF);
//GBL_API GblITable_save(CSELF, GblString* pString);
//GBL_API GblITable_load(SELF, const GblString* pString);

#undef CSELF
#undef SELF

GBL_DECLS_END


#endif // GIMBAL_ITABLE_H
