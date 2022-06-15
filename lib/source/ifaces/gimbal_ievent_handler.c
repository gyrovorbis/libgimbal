#include <gimbal/ifaces/gimbal_ievent_handler.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/strings/gimbal_quark.h>

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

extern GBL_RESULT GblIEventHandler_typeRegister_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblType_registerBuiltin(GBL_TYPE_BUILTIN_INDEX_IEVENT_HANDLER,
                            GBL_INTERFACE_TYPE,
                           GblQuark_internStringStatic("IEventHandler"),
                           &(const GblTypeInfo) {
                               .classSize    = sizeof(GblIEventHandlerIFace)
                           },
                           GBL_TYPE_FLAG_ABSTRACT);
    GBL_API_END();

}
