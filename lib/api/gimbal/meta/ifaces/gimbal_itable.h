/*! \file
 *  \brief GblITable abstract table interface
 *  \ingroup interfaces
 */


#ifndef GIMBAL_ITABLE_H
#define GIMBAL_ITABLE_H

#include "gimbal_interface.h"
#include "../instances/gimbal_instance.h"

GBL_DECLS_BEGIN

#define GBL_ITABLE_TYPE                 (GBL_BUILTIN_TYPE(ITABLE))
#define GBL_ITABLE_STRUCT               GblITable
#define GBL_ITABLE_CLASS_STRUCT         GblITableIFace
#define GBL_ITABLE(instance)            (GBL_INSTANCE_CAST_PREFIX       (instance, GBL_ITABLE))
#define GBL_ITABLE_CHECK(instance)      (GBL_INSTANCE_CHECK_PREFIX      (instance, GBL_ITABLE))
#define GBL_ITABLE_IFACE(klass)         (GBL_CLASS_CAST_PREFIX          (klass,    GBL_ITABLE))
#define GBL_ITABLE_IFACE_CHECK(klass)   (GBL_CLASS_CHECK_PREFIX         (klass,    GBL_ITABLE))
#define GBL_ITABLE_GET_IFACE(instance)  (GBL_INSTANCE_GET_CLASS_PREFIX  (instance, GBL_ITABLE))

#define SELF    GblITable* pSelf
#define CSELF   const SELF

GBL_INTERFACE_DERIVE(GblITable)
    GBL_RESULT (*pFnIndex)    (CSELF, const GblVariant* pKey, GblVariant* pValue);
    GBL_RESULT (*pFnNewIndex) (SELF, const GblVariant* pKey, const GblVariant* pValue);
    GBL_RESULT (*pFnNextIndex)(CSELF, const GblVariant* pKey, GblVariant* pNextKey, GblVariant* pNextValue);
GBL_INTERFACE_END

GBL_EXPORT GblType  GblITable_type(void) GBL_NOEXCEPT;
GBL_API             GblITable_index(CSELF, const GblVariant* pKey, GblVariant* pValue) GBL_NOEXCEPT;
GBL_API             GblITable_newIndex(SELF, const GblVariant* pKey, const GblVariant* pValue) GBL_NOEXCEPT;
GBL_API             GblITable_nextIndex(CSELF, const GblVariant* pKey, GblVariant* pNextKey, GblVariant* pNextValue) GBL_NOEXCEPT;

//GBL_API GblITable_print(CSELF);
//GBL_API GblITable_save(CSELF, GblStringBuffer* pString);
//GBL_API GblITable_load(SELF, const GblStringBuffer* pString);

#undef CSELF
#undef SELF

GBL_DECLS_END


#endif // GIMBAL_ITABLE_H
