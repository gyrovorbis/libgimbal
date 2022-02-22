#ifndef GIMBAL_IVALUE_H
#define GIMBAL_IVALUE_H

#include "gimbal_icomponent.h"
#include "../types/gimbal_variant.h"

#define IValue_IFACE                                                                            \
    vfunc(GBL_RESULT, construct,   VSelf, const GblVariant* pValue)                             \
    vfunc(GBL_RESULT, destruct,    VSelf)                                                       \
    vfunc(GBL_RESULT, convert,     const VSelf, GblVariant* pResult)                            \
    vfunc(GBL_RESULT, compare,     const VSelf, const GblVariant* pRhs, GblCmpResult* pResult)  \
    vfunc(GBL_RESULT, serialize,   const VSelf, void* pBuffer, GblSize* pSize)                  \
    vfunc(GBL_RESULT, deserialize, VSelf, const void* pBuffer, GblSize* pSize)                  \


#define IValue_EXTENDS (IComponent)

interface(IValue);


#endif // GIMBAL_IVALUE_H
