#include <gimbal/meta/ifaces/gimbal_ievent_filter.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/strings/gimbal_quark.h>
#include "../types/gimbal_type_.h"

GBL_API GblIEventFilter_eventFilter(GblIEventFilter* pSelf, GblIEventHandler* pHandler, GblEvent* pEvent) {
    GBL_API_BEGIN(NULL); {
        GBL_API_VERIFY_POINTER(pSelf);
        GBL_API_VERIFY_POINTER(pHandler);
        GBL_API_VERIFY_POINTER(pEvent);
        GblIEventFilterIFace* pIFace = GBL_IEVENT_FILTER_GET_IFACE(pSelf);
        GBL_API_VERIFY_EXPRESSION(pIFace && pIFace->pFnEventFilter);
        GBL_API_CALL(pIFace->pFnEventFilter(pSelf, pHandler, pEvent));
    } GBL_API_END();
}

GBL_EXPORT GblType GblIEventFilter_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info = {
        .classSize = sizeof(GblIEventFilterIFace)
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("GblIEventFilter"),
                                      GBL_INTERFACE_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}
