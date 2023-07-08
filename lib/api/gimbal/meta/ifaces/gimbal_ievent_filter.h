/*! \file
 *  \brief GblIEventFilter abstract event filter interface
 *  \ingroup interfaces
 *  \todo
 *      - combine with GblIEventHandler
 *
 *  \author Falco Girgis
 */


#ifndef GIMBAL_IEVENT_FILTER_H
#define GIMBAL_IEVENT_FILTER_H

#include "gimbal_interface.h"
#include "../instances/gimbal_instance.h"

#define GBL_IEVENT_FILTER_TYPE                      (GBL_TYPEOF(GblIEventFilter))
#define GBL_IEVENT_FILTER(instance)                 (GBL_INSTANCE_CAST(instance, GblIEventFilter))
#define GBL_IEVENT_FILTER_CLASS(klass)              (GBL_CLASS_CAST(klass, GblIEventFilter))
#define GBL_IEVENT_FILTER_GET_CLASS(instance)       (GBL_INSTANCE_GET_CLASS(instance, GblIEventFilter))

#define GBL_SELF_TYPE                               GblIEventFilter

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblIEventHandler);
GBL_FORWARD_DECLARE_STRUCT(GblEvent);

/*! \struct GblIEventFilterClass
 *  \extends GblInterface
 *  \brief GblClass structure for GblIEventFilter
 */
GBL_INTERFACE_DERIVE(GblIEventFilter)
    GBL_RESULT (*pFnEventFilter)(GBL_SELF, GblIEventHandler* pHandler, GblEvent* pEvent);
GBL_INTERFACE_END

GBL_EXPORT GblType    GblIEventFilter_type        (void)                        GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblIEventFilter_eventFilter (GBL_SELF,
                                                   GblIEventHandler* pHandler,
                                                   GblEvent* pEvent)            GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE


#endif // GIMBAL_IEVENT_FILTER_H
