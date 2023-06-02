/*! \file
 *  \brief GblTask high-level concurrent runnable object
 *  \ingroup core
 *  \todo
 *   - implement me
 *
 *   \author Falco Girgis
 */

#ifndef GIMBAL_TASK_H
#define GIMBAL_TASK_H

#include "../meta/instances/gimbal_object.h"

#define GBL_SELF_TYPE GblTask

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblMainLoop);
GBL_FORWARD_DECLARE_STRUCT(GblTask);

typedef int8_t GblPriority;

// Abstract
GBL_CLASS_DERIVE(GblTask, GblObject)
    GBL_RESULT (*pFnExec)    (GBL_SELF);
    GBL_RESULT (*pFnCancel)  (GBL_SELF);
    GBL_RESULT (*pFnTimeout) (GBL_SELF);
    GBL_RESULT (*pFnPriority)(GBL_CSELF, GblPriority* pPriority);
GBL_CLASS_END

GBL_INSTANCE_DERIVE_EMPTY(GblTask, GblObject);

GBL_EXPORT GblPriority GblTask_priority(GBL_CSELF) GBL_NOEXCEPT;


GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_TASK_H
