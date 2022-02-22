#ifndef GIMBAL_ICHILD_H
#define GIMBAL_ICHILD_H

#include "gimbal_icomponent.h"

#define IChild_IFACE                                                 \
    vfunc(GBL_RESULT, parentGet, const VSelf, GblObject** ppParent)  \
    vfunc(GBL_RESULT, parentSet, VSelf,       GblObject* pParent)

#define IChild_EXTENDS (IComponent)

interface(IChild);


#endif // GIMBAL_ICHILD_H
