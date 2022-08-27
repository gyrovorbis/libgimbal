/*! \file
 *  \brief GblIEventFilter abstract event filter interface
 *  \ingroup interfaces
 */


#ifndef GIMBAL_IEVENT_FILTER_H
#define GIMBAL_IEVENT_FILTER_H

#include "gimbal_interface.h"
#include "../instances/gimbal_instance.h"

GBL_DECLS_BEGIN

#define GBL_IEVENT_FILTER_TYPE                      GBL_BUILTIN_TYPE(IEVENT_FILTER)
#define GBL_IEVENT_FILTER_STRUCT                    GblIEventFilter
#define GBL_IEVENT_FILTER_CLASS_STRUCT              GblIEventFilterIFace
#define GBL_IEVENT_FILTER(instance)                 GBL_INSTANCE_CAST_PREFIX(instance, GBL_IEVENT_FILTER)
#define GBL_IEVENT_FILTER_CHECK(instance)           GBL_INSTANCE_CHECK_PREFIX(instance, GBL_IEVENT_FILTER)
#define GBL_IEVENT_FILTER_IFACE(klass)              GBL_CLASS_CAST_PREFIX(klass, GBL_IEVENT_FILTER)
#define GBL_IEVENT_FILTER_IFACE_CHECK(klass)        GBL_CLASS_CHECK_PREFIX(klass, GBL_IEVENT_FILTER)
#define GBL_IEVENT_FILTER_GET_IFACE(instance)       GBL_INSTANCE_GET_CLASS_PREFIX(instance, GBL_IEVENT_FILTER)

#define GBL_SELF    GblIEventFilter* pSelf
#define GBL_CSELF   const GBL_SELF

GBL_FORWARD_DECLARE_STRUCT(GblIEventHandler);
GBL_FORWARD_DECLARE_STRUCT(GblEvent);

GBL_INTERFACE_DERIVE(GblIEventFilter)
    GBL_RESULT (*pFnEventFilter)(GBL_SELF, GblIEventHandler* pHandler, GblEvent* pEvent);
GBL_INTERFACE_END

GBL_API GblIEventFilter_eventFilter(GBL_SELF, GblIEventHandler* pHandler, GblEvent* pEvent) GBL_NOEXCEPT;

#undef GBL_CSELF
#undef GBL_SELF

GBL_DECLS_END


#endif // GIMBAL_IEVENT_FILTER_H
