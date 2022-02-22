#ifndef GIMBAL_IUSERDATA_H
#define GIMBAL_IUSERDATA_H

#include "gimbal_icomponent.h"

#define IUserdata_IFACE                                         \
    vfunc(GBL_RESULT, udGet, const VSelf, void** ppUserdata)    \
    vfunc(GBL_RESULT, udSet, VSelf,       void*  pUserdata)

#define IUserdata_EXTENDS (IComponent)

interface(IUserdata);


#endif // GIMBAL_IUSERDATA_H
