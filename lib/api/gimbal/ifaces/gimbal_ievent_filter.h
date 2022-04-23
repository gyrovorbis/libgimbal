#ifndef GIMBAL_IEVENT_FILTER_H
#define GIMBAL_IEVENT_FILTER_H

#include "../meta/gimbal_interface.h"

GBL_DECLS_BEGIN

#define GBL_IEVENT_FILTER(instance)                 GBL_TYPE_CAST_INSTANCE(instance, GBL_TYPE_IEVENT_FILTER, GblIEventFilter)
#define GBL_IEVENT_FILTER_CHECK(instance)      GBL_TYPE_CHECK_INSTANCE(instance, GBL_TYPE_IEVENT_FILTER)
#define GBL_IEVENT_FILTER_IFACE(klass)              GBL_TYPE_CAST_CLASS(klass, GBL_TYPE_IEVENT_FILTER, GblIEventFilterIFace)
#define GBL_IEVENT_FILTER_IFACE_CHECK(klass)   GBL_TYPE_CHECK_CLASS(klass, GBL_TYPE_IEVENT_FILTER)
#define GBL_IEVENT_FILTER_GET_IFACE(instance)       GBL_TYPE_CAST_GET_CLASS(instance, GBL_TYPE_IEVENT_FILTER, GblIEventFilterIFace)

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
