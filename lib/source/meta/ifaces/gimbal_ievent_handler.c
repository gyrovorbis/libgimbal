#include <gimbal/meta/ifaces/gimbal_ievent_handler.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/strings/gimbal_quark.h>
#include "../types/gimbal_type_.h"

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

GBL_EXPORT GblType GblIEventHandler_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info = {
        .classSize = sizeof(GblIEventHandlerIFace)
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        GBL_API_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("GblIEventHandler"),
                                      GBL_INTERFACE_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}
