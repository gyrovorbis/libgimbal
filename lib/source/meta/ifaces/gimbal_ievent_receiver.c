#include <gimbal/meta/ifaces/gimbal_ievent_receiver.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/strings/gimbal_quark.h>

GBL_EXPORT GBL_RESULT GblIEventReceiver_receiveEvent(GblIEventReceiver* pSelf, GblIEventReceiver* pDest, GblEvent* pEvent) {
    GBL_CTX_BEGIN(NULL);
    GBL_VCALL(GblIEventReceiver, pFnReceiveEvent, pSelf, pDest, pEvent);
    GBL_CTX_END();
}

static GBL_RESULT GblIEventReceiver_receiveEvent_(GblIEventReceiver* pSelf, GblIEventReceiver* pDest, GblEvent* pEvent) {
    GBL_UNUSED(pSelf, pDest, pEvent);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GblIEventReceiverClass_init_(GblClass* pClass, const void* pUd) {
    GBL_UNUSED(pUd);

    GBL_IEVENT_RECEIVER_CLASS(pClass)->pFnReceiveEvent = GblIEventReceiver_receiveEvent_;

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblType GblIEventReceiver_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static const GblTypeInfo info = {
        .pFnClassInit = GblIEventReceiverClass_init_,
        .classSize    = sizeof(GblIEventReceiverClass)
    };

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GblIEventReceiver"),
                                GBL_INTERFACE_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
