#include <gimbal/core/gimbal_call_stack.h>
#include <gimbal/objects/gimbal_object.h>


GBL_RESULT GBL_API_STACK_FRAME_CONSTRUCT(GblStackFrame* pFrame, GblObject* pObject, GBL_RESULT initialResult, GblSourceLocation entryLoc) {
    GBL_RESULT result               = GBL_RESULT_SUCCESS;
    GblContext* pContext            = GBL_NULL;
    void* pObjectUserdata           = GBL_NULL;
    void* pContextUserdata          = GBL_NULL;

    if(!pObject) {
        pContext = gblThreadContext(NULL);
    }

    if(pObject) {
        pContext = GblObject_contextFind(pObject);
        pObjectUserdata = GblObject_userdata(pObject);
    }

    if(pContext) {
        pContextUserdata = GblObject_userdata((GblObject*)pContext);
    }

    memset(pFrame, 0, sizeof(GblStackFrame));
    GBL_CALL_RECORD_CONSTRUCT(&pFrame->record, pObject, initialResult, entryLoc, gblResultString(initialResult));
    pFrame->sourceEntry     = entryLoc;
    pFrame->sourceCurrent   = entryLoc;
    pFrame->pObject         = pObject;
    pFrame->pContext        = pContext;
    pFrame->pContextUd      = pContextUserdata;
    pFrame->pObjectUd       = pObjectUserdata;
    pFrame->pPrevFrame      = NULL;
    return result;
}
