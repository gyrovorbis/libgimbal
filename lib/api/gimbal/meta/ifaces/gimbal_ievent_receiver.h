/*! \file
 *  \brief GblIEventReciever abstract event listener interface
 *  \ingroup interfaces
 *
 *  This file provides the interface type definition and API for
 *  listening to and receiving GblEvents.
 * 
 *  \author    2025 Falco Girgis
 *  \copyright MIT License
 */
#ifndef GIMBAL_IEVENT_RECEIVER_H
#define GIMBAL_IEVENT_RECEIVER_H

#include "gimbal_interface.h"
#include "../instances/gimbal_instance.h"

/*! \name  Type System
 *  \brief Type UUID and cast operators.
 *  @{
 */
#define GBL_IEVENT_RECEIVER_TYPE          GBL_TYPEID(GblIEventReceiver)             //!< Type UUID for GblIEventReceiver.
#define GBL_IEVENT_RECEIVER(self)         GBL_CAST(GblIEventReceiver, self)         //!< Casts a GblInstance to a GblIEventReceiver.
#define GBL_IEVENT_RECEIVER_CLASS(klass)  GBL_CLASS_CAST(GblIEventReceiver, klass)  //!< Casts a GblClass to a GblIEventReceiverClass.
#define GBL_IEVENT_RECEIVER_CLASSOF(self) GBL_CLASSOF(GblIEventReceiver, self)       //!< Extracts a GblIEventReceiverClass from a GblInstance.
//! @}

#define GBL_SELF_TYPE GblIEventReceiver

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblEvent);

/*! \struct GblIEventReceiverClass
 *  \extends GblInterface
 *  \brief GblClass structure for GblIEventReceiver
 * 
 *  A GblIEventReceiver is a type implementing the basic interface required
 *  for event processing. There are typically two different types of receivers:
 *      - Handlers: intended recipients of a GblEvent
 *      - Filters: observers which can intercept events not directly sent to them.
 *  
 *  The same interface can be used to implement both types of receivers, depending
 *  on whether the self parameter is the same instance which is the destination
 *  receiver of GblIEventReceiverClass::pFnReceiveEvent().
 * 
 *  Once received, a GblEvent may either be ignored, allowing to potentially
 *  propagate onward through other chained receivers, or it may be "accepted,"
 *  which terminates its propagation and marks it as handled.
 * 
 *  \sa GblEvent, GblObject
 */
GBL_INTERFACE_DERIVE(GblIEventReceiver)
    //! Invoked any time an event is received or intercepted by the given instance.
    GBL_RESULT (*pFnReceiveEvent)(GBL_SELF, GblIEventReceiver* pDest, GblEvent* pEvent);
GBL_INTERFACE_END

//! Returns the GblType UUID associated with the GBL_IEVENT_RECEIVER_TYPE.
GBL_EXPORT GblType GblIEventReceiver_type (void) GBL_NOEXCEPT;

//! Forwards the given event, sent to the given destination, to the current receiver.
GBL_EXPORT GBL_RESULT GblIEventReceiver_receiveEvent (GBL_SELF,
                                                      GblIEventReceiver* pDest,
                                                      GblEvent*          pEvent) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_IEVENT_HANDLER_H
