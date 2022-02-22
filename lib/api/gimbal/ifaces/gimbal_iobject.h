#ifndef GIMBAL_IOBJECT_H
#define GIMBAL_IOBJECT_H

#include "gimbal_ivalue.h"
#include "gimbal_itable.h"
#include "gimbal_iparent.h"
#include "gimbal_inamed.h"
#include "gimbal_iuserdata.h"
#include "gimbal_ichild.h"

#define IObject_IFACE
    vfunc(GBL_RESULT, privateDataSet, VSelf,        void* pData)     \
    vfunc(GBL_RESULT, privateDataGet, const VSelf,  void* pData)

#define IObject_EXTENDS (IValue)
#define IObject_EXTENDS (ITable)
#define IObject_EXTENDS (IParent)
#define IObject_EXTENDS (IChild)
#define IObject_EXTENDS (INamed)
#define IObject_EXTENDS (IUserdata)

interface(IObject)

#endif // GIMBAL_IOBJECT_H
