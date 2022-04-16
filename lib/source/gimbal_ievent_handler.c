#include <gimbal/meta/gimbal_ievent_handler.h>

GBL_API  GblIEventHandler_event(GblIEventHandler* pSelf, GblEvent* pEvent) GBL_NOEXCEPT {
    GBL_API_BEGIN(NULL); {
        GBL_API_VERIFY_POINTER(pEvent);
        GBL_API_VERIFY_POINTER(pSelf);
        GblIEventHandlerIFace* pIFace = GBL_IEVENT_HANDLER_GET_IFACE(pSelf);
        if(pIFace && pIFace->pFnEvent) {
            GBL_API_CALL(pIFace->pFnEvent(pSelf, pEvent));
        }
    } GBL_API_END();
}

extern GBL_RESULT GblIEventHandler_typeRegister_(GblContext hCtx) {
    GBL_API_BEGIN(hCtx);
    gblTypeRegisterBuiltin(21,
                            GBL_TYPE_INTERFACE,
                           "IEventHandler",
                           &((const GblTypeInfo) {
                               .classSize    = sizeof(GblIEventHandlerIFace),
                               .classAlign   = GBL_ALIGNOF(GblIEventHandlerIFace),
                           }),
                           GBL_TYPE_FLAG_ABSTRACT);
    GBL_API_END();

}
