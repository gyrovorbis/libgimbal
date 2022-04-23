#ifndef GIMBAL_IEVENT_HANDLER_H
#define GIMBAL_IEVENT_HANDLER_H

#include "../meta/gimbal_interface.h"

#define GBL_IEVENT_HANDLER(instance)                GBL_TYPE_CAST_INSTANCE(instance, GBL_TYPE_IEVENT_HANDLER, GblIEventHandler)
#define GBL_IEVENT_HANDLER_CHECK(instance)     GBL_TYPE_CHECK_INSTANCE(instance, GBL_TYPE_IEVENT_HANDLER)
#define GBL_IEVENT_HANDLER_IFACE(klass)             GBL_TYPE_CAST_CLASS(klass, GBL_ITYPE_IEVENT_HANDLER, GblIEventHandlerIFace)
#define GBL_IEVENT_HANDLER_IFACE_CHECK(klass)  GBL_TYPE_CHECK_CLASS(klass, GBL_TYPE_IVENT_HANDLER)
#define GBL_IEVENT_HANDLER_GET_IFACE(instance)      GBL_TYPE_CAST_GET_CLASS(instance, GBL_TYPE_IEVENT_HANDLER, GblIEventHandlerIFace)

#define SELF    GblIEventHandler* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblEvent);
GBL_FORWARD_DECLARE_STRUCT(GblIEventHandler);

typedef struct GblIEventHandlerIFace {
    GblInterface    base;
    GBL_RESULT      (*pFnEvent)(SELF, GblEvent* pEvent);   //handle all events
} GblIEventHandlerIFace;

GBL_API GblIEventHandler_event(SELF, GblEvent* pEvent) GBL_NOEXCEPT;

GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_IEVENT_HANDLER_H
