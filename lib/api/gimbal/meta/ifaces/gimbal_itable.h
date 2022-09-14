/*! \file
 *  \brief GblITable abstract table interface
 *  \ingroup interfaces
 */


#ifndef GIMBAL_ITABLE_H
#define GIMBAL_ITABLE_H

#include "gimbal_interface.h"
#include "../instances/gimbal_instance.h"

GBL_DECLS_BEGIN

#define GBL_ITABLE_TYPE                 (GblITable_type())
#define GBL_ITABLE_STRUCT               GblITable
#define GBL_ITABLE_CLASS_STRUCT         GblITableClass
#define GBL_ITABLE(instance)            (GBL_INSTANCE_CAST_PREFIX       (instance, GBL_ITABLE))
#define GBL_ITABLE_CHECK(instance)      (GBL_INSTANCE_CHECK_PREFIX      (instance, GBL_ITABLE))
#define GBL_ITABLE_IFACE(klass)         (GBL_CLASS_CAST_PREFIX          (klass,    GBL_ITABLE))
#define GBL_ITABLE_IFACE_CHECK(klass)   (GBL_CLASS_CHECK_PREFIX         (klass,    GBL_ITABLE))
#define GBL_ITABLE_GET_IFACE(instance)  (GBL_INSTANCE_GET_CLASS_PREFIX  (instance, GBL_ITABLE))

#define GBL_SELF    GblITable* pSelf
#define GBL_CSELF   const GBL_SELF

GBL_INTERFACE_DERIVE(GblITable)
    GBL_RESULT (*pFnIndex)    (GBL_CSELF, const GblVariant* pKey, GblVariant* pValue);
    GBL_RESULT (*pFnNewIndex) (GBL_SELF, const GblVariant* pKey, const GblVariant* pValue);
    GBL_RESULT (*pFnNextIndex)(GBL_CSELF, const GblVariant* pKey, GblVariant* pNextKey, GblVariant* pNextValue);
GBL_INTERFACE_END

GBL_EXPORT GblType  GblITable_type(void) GBL_NOEXCEPT;
GBL_API             GblITable_index(GBL_CSELF, const GblVariant* pKey, GblVariant* pValue) GBL_NOEXCEPT;
GBL_API             GblITable_newIndex(GBL_SELF, const GblVariant* pKey, const GblVariant* pValue) GBL_NOEXCEPT;
GBL_API             GblITable_nextIndex(GBL_CSELF, const GblVariant* pKey, GblVariant* pNextKey, GblVariant* pNextValue) GBL_NOEXCEPT;

//GBL_API GblITable_print(GBL_CSELF);
//GBL_API GblITable_save(GBL_CSELF, GblStringBuffer* pString);
//GBL_API GblITable_load(GBL_SELF, const GblStringBuffer* pString);

#undef GBL_CSELF
#undef GBL_SELF

GBL_DECLS_END


#endif // GIMBAL_ITABLE_H
