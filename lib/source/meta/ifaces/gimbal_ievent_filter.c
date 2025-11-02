#include <gimbal/meta/ifaces/gimbal_ievent_filter.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/strings/gimbal_quark.h>
#include "../types/gimbal_type_.h"

GBL_EXPORT GBL_RESULT GblIEventFilter_eventFilter(GblIEventFilter* pSelf, GblIEventHandler* pHandler, GblEvent* pEvent) {
    GBL_CTX_BEGIN(NULL); {
        GBL_CTX_VERIFY_POINTER(pSelf);
        GBL_CTX_VERIFY_POINTER(pHandler);
        GBL_CTX_VERIFY_POINTER(pEvent);
        GblIEventFilterClass* pIFace = GBL_IEVENT_FILTER_GET_CLASS(pSelf);
        GBL_CTX_VERIFY_EXPRESSION(pIFace && pIFace->pFnEventFilter);
        GBL_CTX_CALL(pIFace->pFnEventFilter(pSelf, pHandler, pEvent));
    } GBL_CTX_END();
}

GBL_EXPORT GblType GblIEventFilter_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info = {
        .classSize = sizeof(GblIEventFilterClass)
    };

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GblIEventFilter"),
                                GBL_INTERFACE_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
