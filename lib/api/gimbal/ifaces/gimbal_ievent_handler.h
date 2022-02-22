#ifndef GIMBAL_IEVENT_HANDLER_H
#define GIMBAL_IEVENT_HANDLER_H

#include "gimbal_icomponent.h"

#define IEventHandler_IFACE \
    vfunc(GBL_RESULT, onEvent, VSelf, GblApiStackFrame* pFrame, GblEvent* pEvent, GBL_EVENT_TYPE type, GblSize pSize)\

#define IEventHandler_EXTENDS (IComponent)

interface(IEventHandler)

#endif // GIMBAL_IEVENT_HANDLER_H
