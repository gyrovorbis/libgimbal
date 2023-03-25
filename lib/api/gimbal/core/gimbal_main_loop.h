#ifndef GIMBAL_MAIN_LOOP_H
#define GIMBAL_MAIN_LOOP_H

#include "../meta/instances/gimbal_object.h"
#include "../meta/signals/gimbal_signal.h"

#define GBL_PRIORITY_HIGH       -100
#define GBL_PRIORITY_DEFUAULT   0
#define GBL_PRIORITY_LOW        300
#define GBL_PRIORITY_HIGH_IDLE  100
#define GBL_PRIORITY_IDLE       200

#define GBL_SELF_TYPE GblMainLoop

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblTask);
GBL_FORWARD_DECLARE_STRUCT(GblMainLoop);

GBL_CLASS_DERIVE(GblMainLoop, GblObject)
    GBL_RESULT (*pFnEnqueueTask)(GBL_SELF, GblTask* pTask);
    GBL_RESULT (*pFnExecTask)  (GBL_SELF, GblTask* pTask);
    GBL_RESULT (*pFnCancelTask)(GBL_SELF, GblTask* pTask);
    GBL_RESULT (*pFnExecIdle)  (GBL_SELF);
GBL_CLASS_END

// Needs a context with queue

GBL_INSTANCE_DERIVE(GblMainLoop, GblObject)
    GblBool     pending;
GBL_INSTANCE_END

GBL_SIGNALS(GblMainLoop,
    (execIdle,     GBL_INSTANCE_TYPE),
    (taskEnqueued, GBL_INSTANCE_TYPE, GBL_INSTANCE_TYPE)
)

GBL_EXPORT GBL_RESULT GblMainLoop_enqueue (GBL_SELF, GblTask* pTask) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblMainLoop_cancel  (GBL_SELF, GblTask* pTask) GBL_NOEXCEPT;
GBL_EXPORT GblTask*   GblMainLoop_top     (GBL_CSELF)                GBL_NOEXCEPT;
GBL_EXPORT size_t     GblMainLoop_depth   (GBL_CSELF)                GBL_NOEXCEPT;

GBL_EXPORT GBL_RESULT GblMainLoop_iteration (GBL_SELF)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblMainLoop_exec      (GBL_SELF)  GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GblMainLoop_stop      (GBL_SELF)  GBL_NOEXCEPT;
GBL_EXPORT GblBool    GblMainLoop_isRunning (GBL_CSELF) GBL_NOEXCEPT;



GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GIMBAL_MAIN_LOOP_H
