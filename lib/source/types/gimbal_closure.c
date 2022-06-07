#include <gimbal/types/gimbal_closure.h>
#include <gimbal/core/gimbal_api_frame.h>

GBL_EXPORT GblClosure* GblClosure_new(GblSize               allocSize,
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
    GBL_API_VERIFY_POINTER(pSelf->pFnMarshal);
    GBL_API_VERIFY_CALL(pSelf->pFnMarshal(pSelf,
                                          pRetValue,
                                          argCount,
                                          pArgValues));
    GBL_API_END();
}
