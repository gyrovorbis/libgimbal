#include <gimbal/meta/ifaces/gimbal_ievent_handler.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/strings/gimbal_quark.h>
#include "../types/gimbal_type_.h"

GBL_EXPORT GBL_RESULT GblIEventHandler_event(GblIEventHandler* pSelf, GblEvent* pEvent) GBL_NOEXCEPT {
    GBL_CTX_BEGIN(NULL); {
        GBL_CTX_VERIFY_POINTER(pEvent);
        GBL_CTX_VERIFY_POINTER(pSelf);
        GblIEventHandlerClass* pIFace = GBL_IEVENT_HANDLER_GET_CLASS(pSelf);
        if(pIFace && pIFace->pFnEvent) {
            GBL_CTX_CALL(pIFace->pFnEvent(pSelf, pEvent));
        }
    } GBL_CTX_END();
}

GBL_EXPORT GblType GblIEventHandler_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info = {
        .classSize = sizeof(GblIEventHandlerClass)
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        type = GblType_register(GblQuark_internStatic("GblIEventHandler"),
                                GBL_INTERFACE_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}
