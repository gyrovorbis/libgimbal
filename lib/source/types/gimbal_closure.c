#include <gimbal/types/gimbal_closure.h>
#include <gimbal/core/gimbal_api_frame.h>

static GBL_THREAD_LOCAL GblClosure* pCurrentClosure_ = NULL;

GBL_EXPORT GblClosure* GblClosure_current(void) {
    return pCurrentClosure_;
}

GBL_EXPORT GblClosure* GblClosure_createWithContext(GblSize               allocSize,
                                                    void*                 pUserdata,
                                                    GblClosureDestructFn  pFnDtor,
                                                    GblContext*           pCtx)
{
    GblClosure* pClosure = NULL;
    GBL_API_BEGIN(pCtx);
    GBL_API_VERIFY_ARG(allocSize >= sizeof(GblClosure));
    pClosure = GblRef_allocWithContext(allocSize, pCtx);
    GBL_API_VERIFY_LAST_RECORD();
    pClosure->pFnDestructor = pFnDtor;
    pClosure->pUserdata = pUserdata;
    GBL_API_END_BLOCK();
    return pClosure;
}

GBL_EXPORT GBL_RESULT GblClosure_invoke(GblClosure*     pSelf,
                                        GblVariant*     pRetValue,
                                        GblSize         argCount,
                                        GblVariant*     pArgValues)
{
    GBL_API_BEGIN(GblClosure_context(pSelf));

    GblMarshalFn    pFnMarshal;
    GblPtr          pMarshalData;

    if(pSelf->pFnMetaMarshal) {
        pFnMarshal = pSelf->pFnMetaMarshal;
        pMarshalData.pData = pSelf->pMetaMarshalData;
    } else {
        pFnMarshal = pSelf->pFnMarshal;
        pMarshalData.pData = NULL;
    }

    GBL_API_VERIFY_POINTER(pFnMarshal);

    GblClosure* prevActiveClosure = pCurrentClosure_;
    pCurrentClosure_ = pSelf;

    GBL_API_VERIFY_CALL(pFnMarshal(pSelf,
                                   pRetValue,
                                   argCount,
                                   pArgValues,
                                   pMarshalData));

    pCurrentClosure_ = prevActiveClosure;

    GBL_API_END();
}
