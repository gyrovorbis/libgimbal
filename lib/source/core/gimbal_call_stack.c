#include <gimbal/core/gimbal_call_stack.h>
#include <gimbal/objects/gimbal_object.h>

// I think this can all become a tiny macro
GBL_RESULT GBL_API_STACK_FRAME_CONSTRUCT(GblStackFrame* pFrame, GblObject* pObject, GBL_RESULT initialResult, GblSourceLocation entryLoc) {
    GBL_RESULT result               = GBL_RESULT_SUCCESS;
    GblContext* pContext            = GBL_NULL;

    if(pObject) {
        pContext = GblObject_contextFind(pObject);
 //       pObjectUserdata = GblObject_userdata(pObject);
    }

    if(!pContext) {
        pContext = GblThread_context(NULL);
    }
#if 0
    if(pContext) {
        pContextUserdata = GblObject_userdata((GblObject*)pContext);
    }
#endif
    memset(pFrame, 0, sizeof(GblStackFrame));
#if 1
    GBL_CALL_RECORD_CONSTRUCT(&pFrame->record, pObject, initialResult, entryLoc, NULL);
#else
    pRecord->srcLocation    = source;
    pRecord->pObject        = pObject;
    pRecord->result         = resultCode;
#endif
    pFrame->sourceEntry     = entryLoc;
    pFrame->sourceCurrent   = entryLoc;
    pFrame->pObject         = pObject;
    pFrame->pContext        = pContext;
    pFrame->pPrevFrame      = NULL;
    return result;
}
