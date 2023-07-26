#include <gimbal/meta/instances/gimbal_event.h>
#include <gimbal/core/gimbal_ctx.h>
#include <gimbal/strings/gimbal_quark.h>

GBL_EXPORT GblType GblEvent_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    static GblTypeInfo info = {
        .classSize = sizeof(GblEventClass),
        .instanceSize = sizeof(GblEvent)
    };

    if(type == GBL_INVALID_TYPE) GBL_UNLIKELY {
        type = GblType_register(GblQuark_internStringStatic("GblEvent"),
                                GBL_BOX_TYPE,
                                &info,
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
