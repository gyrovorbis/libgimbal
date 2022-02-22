#ifndef GIMBAL_IPARENT_H
#define GIMBAL_IPARENT_H

#include "gimbal_icomponent.h"

#define IParent_IFACE                                                                   \
    vfunc(GBL_RESULT, childGet,     const VSelf, GblSize* pIndex, GblObject** ppChild)  \
    vfunc(GBL_RESULT, childSet,     VSelf,       GblSize index, GblObject* pChild)

#define IParent_EXTENDS (IParent)

interface(IParent);

#endif // GIMBAL_IPARENT_H
