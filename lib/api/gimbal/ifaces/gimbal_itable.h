#ifndef GIMBAL_ITABLE_H
#define GIMBAL_ITABLE_H

#include "gimbal_iiterable.h"

#define ITable_IFACE \
    vfunc(GBL_RESULT, propertyGet, const VSelf, const GblVariant* pKey, GblVariant* pValue) \
    vfunc(GBL_RESULT, propertySet,       VSelf, const GblVariant* pKey, const GblVariant* pValue)

#define ITable_EXTENDS (IIterable)

interface(ITable);

#endif // GIMBAL_ITABLE_H
