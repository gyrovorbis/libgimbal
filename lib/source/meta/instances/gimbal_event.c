#include <gimbal/meta/instances/gimbal_event.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/strings/gimbal_quark.h>


GBL_EXPORT GblEvent* GblEvent_create(GblType type) {
    return GblType_check(type, GBL_EVENT_TYPE)?
                GBL_EVENT(GblBox_create(type)) :
                NULL;
}

GBL_EXPORT GBL_RESULT GblEvent_construct(GblEvent* pSelf, GblType type) {
    return GblType_check(type, GBL_EVENT_TYPE)?
                GblBox_construct((GblBox*)pSelf, type) :
                GBL_RESULT_ERROR_INVALID_TYPE;
}

GBL_EXPORT GBL_EVENT_STATE GblEvent_state(const GblEvent* pSelf) {
    return GBL_PRIV_REF(pSelf).state;
}

GBL_EXPORT GBL_RESULT GblEvent_accept(GblEvent* pSelf) {
    if(GBL_PRIV_REF(pSelf).state != GBL_EVENT_STATE_PENDING) {
        return GBL_RESULT_ERROR_INVALID_ARG;
    }
    GBL_PRIV_REF(pSelf).state = GBL_EVENT_STATE_ACCEPTED;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT void GblEvent_reset(GblEvent* pSelf) {
    GBL_PRIV_REF(pSelf).state = GBL_EVENT_STATE_PENDING;
}

GBL_EXPORT GblType GblEvent_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize = sizeof(GblEventClass),
        .instanceSize = sizeof(GblEvent)
    };

    if GBL_UNLIKELY(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GblEvent"),
                                GBL_BOX_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
