/*! \file
 *  \brief GblIEventHandler abstract event handler interface
 *  \ingroup interfaces
 *  \todo
 *      - combine me with event filter
 *
 *  \author Falco Girgis
 */


#ifndef GIMBAL_IEVENT_HANDLER_H
#define GIMBAL_IEVENT_HANDLER_H

#include "gimbal_interface.h"
#include "../instances/gimbal_instance.h"

#define GBL_IEVENT_HANDLER_TYPE                 (GBL_TYPEID(GblIEventHandler))
#define GBL_IEVENT_HANDLER(instance)            (GBL_CAST(GblIEventHandler, instance))
#define GBL_IEVENT_HANDLER_CLASS(klass)         (GBL_CLASS_CAST(GblIEventHandler, klass))
#define GBL_IEVENT_HANDLER_GET_CLASS(instance)  (GBL_CLASSOF(GblIEventHandler, instance))

#define GBL_SELF_TYPE GblIEventHandler

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblEvent);

/*! \struct GblIEventHandlerClass
 *  \extends GblInterface
 *  \brief GblClass structure for GblIEventHandler
 */
GBL_INTERFACE_DERIVE(GblIEventHandler)
    GBL_RESULT (*pFnEvent)(GBL_SELF, GblEvent* pEvent);
GBL_INTERFACE_END

GBL_EXPORT GblType     GblIEventHandler_type  (void)                       GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT  GblIEventHandler_event (GBL_SELF, GblEvent* pEvent) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_IEVENT_HANDLER_H
