#ifndef GIMBAL_IEVENT_FILTER_H
#define GIMBAL_IEVENT_FILTER_H

#include "gimbal_type.h"
#include "../core/gimbal_api_frame.h"

GBL_DECLS_BEGIN

#define GBL_IEVENT_FILTER(instance)                 GBL_TYPE_INSTANCE_CAST(instance, GBL_TYPE_IEVENT_FILTER, GblIEventFilter)
#define GBL_IEVENT_FILTER_COMPATIBLE(instance)      GBL_TYPE_INSTANCE_IS_A(instance, GBL_TYPE_IEVENT_FILTER)
#define GBL_IEVENT_FILTER_IFACE(klass)              GBL_TYPE_CLASS_CAST(klass, GBL_TYPE_IEVENT_FILTER, GblIEventFilterIFace)
#define GBL_IEVENT_FILTER_IFACE_COMPATIBLE(klass)   GBL_TYPE_CLASS_IS_A(klass, GBL_TYPE_IEVENT_FILTER)
#define GBL_IEVENT_FILTER_GET_IFACE(instance)       GBL_TYPE_INSTANCE_CLASS_CAST(instance, GBL_TYPE_IEVENT_FILTER, GblIEventFilterIFace)

#define SELF    GblIEventFilter* pSelf
#define CSELF   const SELF

GBL_FORWARD_DECLARE_STRUCT(GblIEventFilter);
GBL_FORWARD_DECLARE_STRUCT(GblIEventHandler);
GBL_FORWARD_DECLARE_STRUCT(GblEvent);

typedef struct GblIEventFilterIFace {
    GblInterface    base;
    GBL_RESULT      (*pFnEventFilter)(SELF, GblIEventHandler* pHandler, GblEvent* pEvent);
} GblIEventFilterIFace;

GBL_API GblIEventFilter_eventFilter(SELF, GblIEventHandler* pHandler, GblEvent* pEvent) GBL_NOEXCEPT;

#undef CSELF
#undef SELF

GBL_DECLS_END


#endif // GIMBAL_IEVENT_FILTER_H
