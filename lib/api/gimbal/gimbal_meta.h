/*! \file
 *  \brief Meta type system module-wide include
 *  \ingroup meta
 *
 *  \author Falco Girgis
 */

#ifndef GIMBAL_META_H
#define GIMBAL_META_H

#include "meta/classes/gimbal_class.h"
#include "meta/classes/gimbal_enum.h"
#include "meta/classes/gimbal_flags.h"
#include "meta/classes/gimbal_opaque.h"
#include "meta/classes/gimbal_primitives.h"

#include "meta/ifaces/gimbal_iallocator.h"
#include "meta/ifaces/gimbal_ievent_filter.h"
#include "meta/ifaces/gimbal_ievent_handler.h"
#include "meta/ifaces/gimbal_ilogger.h"
#include "meta/ifaces/gimbal_interface.h"
#include "meta/ifaces/gimbal_iplugin.h"
#include "meta/ifaces/gimbal_itable.h"
#include "meta/ifaces/gimbal_ivariant.h"

#include "meta/instances/gimbal_box.h"
#include "meta/instances/gimbal_context.h"
#include "meta/instances/gimbal_event.h"
#include "meta/instances/gimbal_instance.h"
#include "meta/instances/gimbal_object.h"

#include "meta/properties/gimbal_property.h"

#include "meta/signals/gimbal_c_closure.h"
#include "meta/signals/gimbal_class_closure.h"
#include "meta/signals/gimbal_closure.h"
#include "meta/signals/gimbal_marshal.h"
#include "meta/signals/gimbal_signal.h"
#include "meta/signals/gimbal_signal_closure.h"

#include "meta/types/gimbal_builtin_types.h"
#include "meta/types/gimbal_pointer.h"
#include "meta/types/gimbal_type.h"
#include "meta/types/gimbal_variant.h"

/*! \defgroup meta Type System
 * 	\brief Object-oriented, cross-language runtime type system
 */

#endif // GIMBAL_META_H
