/*! \file
 *  \brief GblITable abstract table interface
 *  \ingroup interfaces
 *  \todo
 *      - get rid of me
 *
 *  \author Falco Girgis
 */


#ifndef GIMBAL_ITABLE_H
#define GIMBAL_ITABLE_H

#include "gimbal_interface.h"
#include "../instances/gimbal_instance.h"

GBL_DECLS_BEGIN

#define GBL_ITABLE_TYPE                 (GBL_TYPEOF(GblITable))
#define GBL_ITABLE(instance)            (GBL_INSTANCE_CAST(instance, GblITable))
#define GBL_ITABLE_CLASS(klass)         (GBL_CLASS_CAST(klass, GblITable))
#define GBL_ITABLE_GET_CLASS(instance)  (GBL_INSTANCE_GET_CLASS(instance, GblITable))

#define GBL_SELF_TYPE GblITable

GBL_FORWARD_DECLARE_STRUCT(GblVariant);

GBL_INTERFACE_DERIVE(GblITable)
    GBL_RESULT (*pFnIndex)    (GBL_CSELF, const GblVariant* pKey, GblVariant* pValue);
    GBL_RESULT (*pFnNewIndex) (GBL_SELF, const GblVariant* pKey, const GblVariant* pValue);
    GBL_RESULT (*pFnNextIndex)(GBL_CSELF, const GblVariant* pKey, GblVariant* pNextKey, GblVariant* pNextValue);
GBL_INTERFACE_END

GBL_EXPORT GblType    GblITable_type      (void)                                                                            GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblITable_index     (GBL_CSELF, const GblVariant* pKey, GblVariant* pValue)                           GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblITable_newIndex  (GBL_SELF, const GblVariant* pKey, const GblVariant* pValue)                      GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblITable_nextIndex (GBL_CSELF, const GblVariant* pKey, GblVariant* pNextKey, GblVariant* pNextValue) GBL_NOEXCEPT;

#undef GBL_SELF_TYPE

GBL_DECLS_END


#endif // GIMBAL_ITABLE_H
