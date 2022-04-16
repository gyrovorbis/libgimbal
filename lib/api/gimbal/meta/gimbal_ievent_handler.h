#ifndef GIMBAL_IEVENT_HANDLER_H
#define GIMBAL_IEVENT_HANDLER_H

#include "gimbal_type.h"
#include "../core/gimbal_api_frame.h"

GBL_DECLS_BEGIN

#define GBL_IEVENT_HANDLER(instance)                GBL_TYPE_INSTANCE_CAST(instance, GBL_TYPE_IEVENT_HANDLER, GblIEventHandler)
#define GBL_IEVENT_HANDLER_COMPATIBLE(instance)     GBL_TYPE_INSTANCE_IS_A(instance, GBL_TYPE_IEVENT_HANDLER)
#define GBL_IEVENT_HANDLER_IFACE(klass)             GBL_TYPE_CLASS_CAST(klass, GBL_ITYPE_IEVENT_HANDLER, GblIEventHandlerIFace)
#define GBL_IEVENT_HANDLER_IFACE_COMPATIBLE(klass)  GBL_TYPE_CLASS_IS_A(klass, GBL_TYPE_IVENT_HANDLER)
#define GBL_IEVENT_HANDLER_GET_IFACE(instance)      GBL_TYPE_INSTANCE_CLASS_CAST(instance, GBL_TYPE_IEVENT_HANDLER, GblIEventHandlerIFace)

#define SELF    GblIEventHandler* pSelf
#define CSELF   const SELF

GBL_FORWARD_DECLARE_STRUCT(GblEvent);
GBL_FORWARD_DECLARE_STRUCT(GblIEventHandler);

typedef struct GblIEventHandlerIFace {
    GblInterface    base;
    GBL_RESULT      (*pFnEvent)(SELF, GblEvent* pEvent);   //handle all events
} GblIEventHandlerIFace;

GBL_API GblIEventHandler_event(SELF, GblEvent* pEvent) GBL_NOEXCEPT;


#undef CSELF
#undef SELF

GBL_DECLS_END

#endif // GIMBAL_IEVENT_HANDLER_H
