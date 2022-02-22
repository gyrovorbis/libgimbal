#ifndef GIMBAL_IITERABLE_H
#define GIMBAL_IITERABLE_H

#include "gimbal_icomponent.h"

#define IIterable_IFACE \
    vfunc(GBL_RESULT, next, const VSelf, const GblVariant* pPrevKey, const GblVariant* pNextKey, const GblVariant* pNextValue)  \

#define IIterable_EXTENDS (IComponent)

interface(IIterable);

#endif // GIMBAL_IITERABLE_H
