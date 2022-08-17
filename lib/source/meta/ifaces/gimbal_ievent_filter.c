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

extern GBL_RESULT GblIEventFilter_typeRegister_(GblContext* pCtx) {
    GBL_API_BEGIN(pCtx);
    GblType_registerBuiltin_(GBL_TYPE_BUILTIN_INDEX_IEVENT_FILTER,
                            GBL_INTERFACE_TYPE,
                           GblQuark_internStringStatic("IEventFilter"),
                           &(const GblTypeInfo) {
                               .classSize    = sizeof(GblIEventFilterIFace),
                           },
                           GBL_TYPE_FLAG_ABSTRACT);

    GBL_API_END();
}
