#include <gimbal/meta/instances/gimbal_event.h>
#include <gimbal/core/gimbal_api_frame.h>
#include <gimbal/strings/gimbal_quark.h>

GBL_EXPORT GblType GblEvent_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize = sizeof(GblEventClass),
        .instanceSize = sizeof(GblEvent)
    };

    if(type == GBL_INVALID_TYPE) {
        GBL_API_BEGIN(NULL);
        type = GblType_registerStatic(GblQuark_internStringStatic("GblEvent"),
                                      GBL_BOX_TYPE,
                                      &info,
                                      GBL_TYPE_FLAG_TYPEINFO_STATIC);
        GBL_API_VERIFY_LAST_RECORD();
        GBL_API_END_BLOCK();
    }
    return type;
}
