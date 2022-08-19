/*! \file
 *  \brief GblIEventHandler abstract event handler interface
 *  \ingroup interfaces
 */


#ifndef GIMBAL_IEVENT_HANDLER_H
#define GIMBAL_IEVENT_HANDLER_H

#include "gimbal_interface.h"
#include "../instances/gimbal_instance.h"

#define GBL_IEVENT_HANDLER_TYPE                     GBL_BUILTIN_TYPE(IEVENT_HANDLER)
#define GBL_IEVENT_HANDLER_STRUCT                   GblIEventHandler
#define GBL_IEVENT_HANDLER_CLASS_STRUCT             GblIEventHandlerIFace
#define GBL_IEVENT_HANDLER(instance)                GBL_INSTANCE_CAST_PREFIX(instance, GBL_IEVENT_HANDLER)
#define GBL_IEVENT_HANDLER_CHECK(instance)          GBL_INSTANCE_CHECK_PREFIX(instance, GBL_IEVENT_HANDLER)
#define GBL_IEVENT_HANDLER_IFACE(klass)             GBL_CLASS_CAST_PREFIX(klass, GBL_IEVENT_HANDLER)
#define GBL_IEVENT_HANDLER_IFACE_CHECK(klass)       GBL_CLASS_CHECK_PREFIX(klass, GBL_IEVENT_HANDLER)
#define GBL_IEVENT_HANDLER_GET_IFACE(instance)      GBL_INSTANCE_GET_CLASS_PREFIX(instance, GBL_IEVENT_HANDLER)
#define GBL_IEVENT_HANDLER_TRY_IFACE(instance)      GBL_INSTANCE_TRY_CLASS_PREFIX(instance, GBL_IEVENT_HANDLER)

#define SELF    GblIEventHandler* pSelf
#define CSELF   const SELF

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblEvent);

GBL_INTERFACE_DERIVE(GblIEventHandler)
    GBL_RESULT      (*pFnEvent)(SELF, GblEvent* pEvent);   //handle all events
GBL_INTERFACE_END

GBL_API GblIEventHandler_event(SELF, GblEvent* pEvent) GBL_NOEXCEPT;

GBL_DECLS_END

#undef CSELF
#undef SELF

#endif // GIMBAL_IEVENT_HANDLER_H
