#ifndef GIMBAL_ICOMPONENT_H
#define GIMBAL_ICOMPONENT_H

#include <interface99.h>
#include "../types/gimbal_result.h"

#define IComponent_IFACE \
    vfunc(GBL_RESULT, componentQuery, VSelf, GblType compType, void** ppObject)    \
    vfunc(GBL_RESULT, refAdd,         VSelf, uint32_t* pCount)                     \
    vfunc(GBL_RESULT, refRelease,     VSelf, uint32_t* pCount)

interface(IComponent);


#endif // GIMBAL_COMPONENT_H
