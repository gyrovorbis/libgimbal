/*! \file
 *  \brief GblMainLoop task scheduler priority queue
 *  \ingroup core
 *  \todo
 *   - implement me
 *
 *   \author    2023 Falco Girgis
 *   \copyright MIT License
 */

#ifndef GIMBAL_MAIN_LOOP_H
#define GIMBAL_MAIN_LOOP_H

#include "../meta/instances/gimbal_object.h"
#include "../meta/signals/gimbal_signal.h"
#include "../containers/gimbal_linked_list.h"

#define GBL_SELF_TYPE GblMainLoop

GBL_DECLS_BEGIN

GBL_FORWARD_DECLARE_STRUCT(GblTask);
GBL_FORWARD_DECLARE_STRUCT(GblMainLoop);

//! Enumeration for
typedef enum GBL_PRIORITY_LEVEL {
    GBL_PRIORITY_IDLE,
    GBL_PRIORITY_HIGH_IDLE,
    GBL_PRIORITY_LOW,
    GBL_PRIORITY_DEFAULT,
    GBL_PRIORITY_HIGH,
    GBL_PRIORITY_COUNT
} GBL_PRIORITY_LEVEL;

/*! \struct  GblMainLoopClass
 *  \extends GblObjectClass
 *  \brief   GblClass VTable structure for GblMainLoop
 *
 *  WIP
 *
 *  \sa GblMainLoop
 */
GBL_CLASS_DERIVE(GblMainLoop, GblObject)
    GBL_RESULT (*pFnEnqueueTask)(GBL_SELF, GblTask* pTask);
    GBL_RESULT (*pFnExecTask)   (GBL_SELF, GblTask* pTask);
    GBL_RESULT (*pFnCancelTask) (GBL_SELF, GblTask* pTask);
    GBL_RESULT (*pFnExecIdle)   (GBL_SELF);
GBL_CLASS_END

// Needs a context with queue
/*! \struct  GblMainLoop
 *  \extends GblObject
 *  \brief   Sequentially executing priority queue of GblTask objects
 *
 *  WIP
 *
 *  \sa GblMainLoopClass
 */
GBL_INSTANCE_DERIVE(GblMainLoop, GblObject)
    union {
        GblTask*           pHead;
        GblLinkedListNode  listNode;
    }                  taskQueue[GBL_PRIORITY_COUNT];
    size_t             taskCount;
    uint32_t           taskBitMap;
    GblTask*           pActiveTask;
    GBL_PRIORITY_LEVEL prevTaskPriority;
GBL_INSTANCE_END

GBL_SIGNALS(GblMainLoop,
    (execIdle,     GBL_INSTANCE_TYPE),
    (taskEnqueued, GBL_INSTANCE_TYPE, GBL_INSTANCE_TYPE)
)

GBL_EXPORT GblType    GblMainLoop_type    (void) GBL_NOEXCEPT;

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
