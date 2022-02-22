#ifndef GIMBAL_ICONTAINER_H
#define GIMBAL_ICONTAINER_H

#include "gimbal_iiterable.h"

#define IContainer_IFACE \
//    vfunc(GBL_RESULT, onEvent, VSelf, GblApiStackFrame* pFrame, GblEvent* pEvent, GBL_EVENT_TYPE type, GblSize pSize)\
//first, last, insert, clear, erase, remove, front, back, size, count
// push_back, emplace_back, etc

#define IContainer_EXTENDS (IIterable)

interface(IContainer)

#endif // GIMBAL_ICONTAINER_H
